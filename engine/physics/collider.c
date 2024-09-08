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

b32 ntest_collider(nCollider a, nCollider b) {
    if (a.kind == NCOLLIDER_KIND_AABB && a.kind == b.kind) {
        return ntest_aabb(a.aabb, b.aabb);
    }
    if (a.kind == NCOLLIDER_KIND_CIRCLE && a.kind == b.kind) {
        return ntest_circle(a.circle, b.circle);
    }
    return 0;
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

    nCollider cola = {
        .aabb = a,
        .kind = NCOLLIDER_KIND_AABB,
    };
    nCollider colb = {
        .aabb = b,
        .kind = NCOLLIDER_KIND_AABB,
    };
    assert(ntest_collider(cola,colb));
}
