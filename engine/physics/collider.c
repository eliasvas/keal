#include "collider.h"

// I have left this here so we can use it for broadphase calculations
b32 ntest_aabb(nAABB a, nAABB b) {
    if(a.max.x < b.min.x || a.min.x > b.max.x) return 0;
    if(a.max.y < b.min.y || a.min.y > b.max.y) return 0;
    return 1;
}

nAABB ncollider_to_aabb(nCollider *c) {
    nAABB box = {0};
    switch (c->kind) {
        case NCOLLIDER_KIND_AABB:
            box = c->aabb;
            break;
        case NCOLLIDER_KIND_CIRCLE:
            box.min = vec2_sub(c->circle.pos, v2(c->circle.radius, c->circle.radius));
            box.max = vec2_add(c->circle.pos, v2(c->circle.radius, c->circle.radius));
            break;
        case NCOLLIDER_KIND_OBB:
        default:
            assert(0 && "TRANSITION TO OBB NOT IMPLEMENTED");
            break;
    }
    return box;
}