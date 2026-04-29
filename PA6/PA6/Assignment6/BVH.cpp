#include <algorithm>
#include <cassert>
#include "BVH.hpp"

BVHAccel::BVHAccel(std::vector<Object*> p, int maxPrimsInNode,
                   SplitMethod splitMethod)
    : maxPrimsInNode(std::min(255, maxPrimsInNode)), splitMethod(splitMethod),
      primitives(std::move(p))
{
    time_t start, stop;
    time(&start);
    if (primitives.empty())
        return;
    if (splitMethod == SplitMethod::NAIVE){
        root = recursiveBuild(primitives);
    }
    else if(splitMethod == SplitMethod::SAH){
        root = recursiveBuildSAH(primitives);
    }
    time(&stop);
    double diff = difftime(stop, start);
    int hrs = (int)diff / 3600;
    int mins = ((int)diff / 60) - (hrs * 60);
    int secs = (int)diff - (hrs * 3600) - (mins * 60);

    printf(
        "\rBVH Generation complete: \nTime Taken: %i hrs, %i mins, %i secs\n\n",
        hrs, mins, secs);
}

BVHBuildNode* BVHAccel::recursiveBuild(std::vector<Object*> objects)
{
    BVHBuildNode* node = new BVHBuildNode();

    // Compute bounds of all primitives in BVH node
    Bounds3 bounds;
    for (int i = 0; i < objects.size(); ++i)
        bounds = Union(bounds, objects[i]->getBounds());
    if (objects.size() == 1) {
        // Create leaf _BVHBuildNode_
        node->bounds = objects[0]->getBounds();
        node->object = objects[0];
        node->left = nullptr;
        node->right = nullptr;
        return node;
    }
    else if (objects.size() == 2) {
        node->left = recursiveBuild(std::vector{objects[0]});
        node->right = recursiveBuild(std::vector{objects[1]});

        node->bounds = Union(node->left->bounds, node->right->bounds);
        return node;
    }
    else {
        Bounds3 centroidBounds;
        for (int i = 0; i < objects.size(); ++i)
            centroidBounds =
                Union(centroidBounds, objects[i]->getBounds().Centroid());
        int dim = centroidBounds.maxExtent();
        switch (dim) {
        case 0:
            std::sort(objects.begin(), objects.end(), [](auto f1, auto f2) {
                return f1->getBounds().Centroid().x <
                       f2->getBounds().Centroid().x;
            });
            break;
        case 1:
            std::sort(objects.begin(), objects.end(), [](auto f1, auto f2) {
                return f1->getBounds().Centroid().y <
                       f2->getBounds().Centroid().y;
            });
            break;
        case 2:
            std::sort(objects.begin(), objects.end(), [](auto f1, auto f2) {
                return f1->getBounds().Centroid().z <
                       f2->getBounds().Centroid().z;
            });
            break;
        }

        auto beginning = objects.begin();
        auto middling = objects.begin() + (objects.size() / 2);
        auto ending = objects.end();

        auto leftshapes = std::vector<Object*>(beginning, middling);
        auto rightshapes = std::vector<Object*>(middling, ending);

        assert(objects.size() == (leftshapes.size() + rightshapes.size()));

        node->left = recursiveBuild(leftshapes);
        node->right = recursiveBuild(rightshapes);

        node->bounds = Union(node->left->bounds, node->right->bounds);
    }

    return node;
}


BVHBuildNode* BVHAccel::recursiveBuildSAH(std::vector<Object*>objects){
    std::vector<BVHPrimitiveInfo> primitiveInfo(objects.size());
    for (int i = 0; i < objects.size(); i++){
        primitiveInfo[i].primieNum = i;
        primitiveInfo[i].bounds = objects[i]->getBounds();
        primitiveInfo[i].centriod = primitiveInfo[i].bounds.Centroid();
    };
    BVHBuildNode* node = new BVHBuildNode();
    Bounds3 bounds;
    for (int i = 0; i < objects.size(); ++i)
        bounds = Union(bounds, objects[i]->getBounds());
    if (objects.size() == 1) {
        // Create leaf _BVHBuildNode_
        node->bounds = primitiveInfo[0].bounds;
        node->object = objects[0];
        node->left = nullptr;
        node->right = nullptr;
        return node;
    }
    else if (objects.size() == 2) {
        node->left = recursiveBuildSAH(std::vector{objects[0]});
        node->right = recursiveBuildSAH(std::vector{objects[1]});

        node->bounds = Union(node->left->bounds, node->right->bounds);
        return node;
    }
    else{
        float xLength = bounds.pMax.x-bounds.pMin.x;
        float yLength = bounds.pMax.y-bounds.pMin.y;
        float zLength = bounds.pMax.z-bounds.pMin.z;
        float lengths[3] = {xLength, yLength, zLength};
        node->splitAxis = std::max_element(lengths, lengths+3) - lengths;
        std::vector<BVHPrimitiveInfo> buckets[12];
        float minCost = std::numeric_limits<float>::max();
        int splitPos = 0;
        std::vector<int> index(objects.size());
        for (int i = 0; i < objects.size(); i++){
            index[i] = int((primitiveInfo[i].centriod[node->splitAxis]-bounds.pMin[node->splitAxis])/(bounds.pMax[node->splitAxis]-bounds.pMin[node->splitAxis])*12);
            if (index[i] == 12) index[i] = 11;
            buckets[index[i]].push_back(primitiveInfo[i]);
        }
        float surfaceLeft;
        int numLeft = 0;
        float surfaceRight;
        int numRight = 0;
        for (int i = 0; i < 11; i++){
            Bounds3 tempBoundsLeft;
            Bounds3 tempBoundsRight;
            for (int j = 0; j <= i; j++){
                for (int k = 0; k < buckets[j].size(); k++){
                    tempBoundsLeft = Union(tempBoundsLeft, buckets[j][k].bounds);
                    numLeft++;
                    surfaceLeft = tempBoundsLeft.SurfaceArea();
                }
                
            }
            for (int j = i+1; j < 12; j++){
                for (int k = 0; k < buckets[j].size(); k++){
                    tempBoundsRight = Union(tempBoundsRight, buckets[j][k].bounds);
                    numRight++;
                    surfaceRight = tempBoundsRight.SurfaceArea();
                }
            }
            float tempCost = surfaceLeft/bounds.SurfaceArea()*numLeft+surfaceRight/bounds.SurfaceArea()*numRight+0.125f;
            if (tempCost < minCost){
                minCost = tempCost;
                splitPos = i;
            }
            numLeft = 0;
            numRight = 0;
        }
    
        std::vector<Object*>leftObjects;
        std::vector<Object*>rightObjects;
        for (int i = 0; i < objects.size(); i++){
            if (index[i] <= splitPos){
                leftObjects.push_back(objects[i]);
            }
            else{
                rightObjects.push_back(objects[i]);
            }
        }
        if (leftObjects.size() == 0 || rightObjects.size() == 0){
            return recursiveBuild(objects);
        }
        node->left = recursiveBuildSAH(leftObjects);
        node->right = recursiveBuildSAH(rightObjects);
        node->bounds = Union(node->left->bounds, node->right->bounds);
    };
    return node;
};


Intersection BVHAccel::Intersect(const Ray& ray) const
{
    Intersection isect;
    if (!root)
        return isect;
    isect = BVHAccel::getIntersection(root, ray);
    return isect;
}

Intersection BVHAccel::getIntersection(BVHBuildNode* node, const Ray& ray) const
{
    // TODO Traverse the BVH to find intersection
    Vector3f invDir = ray.direction_inv;
    std::array<int, 3> dirIsNeg = {(int)(ray.direction.x>0), (int)(ray.direction.y>0), (int)(ray.direction.z>0)};
    if ((node->bounds.IntersectP(ray, invDir, dirIsNeg) == false)) return Intersection();
    if (node->left == nullptr && node->right == nullptr){
        return node->object->getIntersection(ray);
    } 
    auto hit1 = BVHAccel::getIntersection(node->left, ray);
    auto hit2 = BVHAccel::getIntersection(node->right, ray);
    if (hit1.happened && hit2.happened){
        return (hit1.distance > hit2.distance)?hit2:hit1;
    }
    if (!hit1.happened && !hit2.happened) return Intersection();
    if (!hit1.happened) return hit2;
    if (!hit2.happened) return hit1;
}