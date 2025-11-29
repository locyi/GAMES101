def calculate_line(start, end):
    x1, y1 = start
    x2, y2 = end
    dx = x2 - x1
    dy = y2 - y1
    points = []  # 存储经过的格子

    if abs(dx) >= abs(dy):  # 水平步长优先
        sx = 1 if dx > 0 else -1
        sy = dy / abs(dx) if dx != 0 else 0
        for i in range(abs(dx) + 1):
            points.append((int(x1), int(y1)))  # 添加坐标
            x1 += sx
            y1 += sy
    else:  # 垂直步长优先
        sy = 1 if dy > 0 else -1
        sx = dx / abs(dy) if dy != 0 else 0
        for i in range(abs(dy) + 1):
            points.append((int(x1), int(y1)))  # 添加坐标
            x1 += sx
            y1 += sy

    return points  # 返回经过的格子列表


"""
static bool line_edge(int x, int y, const Vector3f* _v){
    float x0 = _v[0].x();
    float x1 = _v[1].x();
    float x2 = _v[2].x();
    float y0 = _v[0].y();
    float y1 = _v[1].y();
    float y2 = _v[2].y();
    float dx0 = x1-x0;
    float dx1 = x2-x1;
    float dx2 = x0-x2;
    float dy0 = y1-y0;
    float dy1 = y2-y0;
    float dy2 = y0-y2;
    typedef struct{
        float x,y;
        float z = 1.0f;
    }Vec3;
    std::vector<Vec3>points;

    if (std::abs(dx0) >= std::abs(dy0)){
        float sx0 = dx0>0?1:-1;
        float sy0 = dx0!=0?dy0/std::abs(dx0):0;
        for (int i = 0; i < std::abs(dx0); i++){
            points.emplace_back(x0,y0,1.0f);
            x0+=sx0;
            y0+=sy0;
        }
    }
    else{
        float sy0 = dy0>0?1:-1;
        float sx0 = dy0!=0?dx0/std::abs(dy0):0;
        for (int i = 0; i < std::abs(dy0); i++){
            points.emplace_back(x0,y0,1.0f);
            x0+=sx0;
            y0+=sy0;
        }
    }


    if (std::abs(dx1) >= std::abs(dy1)){
        float sx1 = dx1>0?1:-1;
        float sy1 = dx1!=0?dy1/std::abs(dx1):0;
        for (int i = 0; i < std::abs(dx1); i++){
            points.emplace_back(x1,y1,1.0f);
            x1+=sx1;
            y1+=sy1;
        }
    }
    else{
        float sy1 = dy1>0?1:-1;
        float sx1 = dy1!=0?dx1/std::abs(dy1):0;
        for (int i = 0; i < std::abs(dy1); i++){
            points.emplace_back(x1,y1,1.0f);
            x1+=sx1;
            y1+=sy1;
        }
    }

    if (std::abs(dx2) >= std::abs(dy2)){
        float sx2 = dx2>0?1:-1;
        float sy2 = dx2!=0?dy2/std::abs(dx2):0;
        for (int i = 0; i < std::abs(dx2); i++){
            points.emplace_back(x2,y2,1.0f);
            x2+=sx2;
            y2+=sy2;
        }
    }
    else{
        float sy2 = dy2>0?1:-1;
        float sx2 = dy2!=0?dx2/std::abs(dy2):0;
        for (int i = 0; i < std::abs(dy2); i++){
            points.emplace_back(x2,y2,1.0f);
            x2+=sx2;
            y2+=sy2;
        }
    }
    
    Vec3 target{x, y};
    auto it = std::find(points.begin(), points.end(), target);

    if (it != points.end()) {
    return 1;
    } 
    else {
    return 0;
    }
}
"""