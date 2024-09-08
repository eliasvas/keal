#include "collider.h"

// Perfrom SAT for the two AABBs
b32 ntest_aabb(nAABB a, nAABB b) {
    if(a.max.x < b.min.x || a.min.x > b.max.x) return 0;
    if(a.max.y < b.min.y || a.min.y > b.max.y) return 0;
    return 1;
}

b32 ntest_circle(nCircle a, nCircle b) {
    f32 r = a.radius + b.radius;
    return r >= vec2_len(vec2_sub(a.pos, b.pos));
}


void ntest_colliders() {
    nAABB a =  {
        .min = v2(100,100),
        .max = v2(200,200),
    };
    nAABB b =  {
        .min = v2(102,100),
        .max = v2(200,200),
    };
    assert(ntest_aabb(a,b));

    nCircle ca = {
        .pos = v2(6,6),
        .radius = 3,
    };
    nCircle cb = {
        .pos = v2(5,5),
        .radius = 3,
    };
    assert(ntest_circle(ca,cb));
}
