#ifndef NCOLLIDER_H
#define NCOLLIDER_H
#include "base/base_inc.h"

typedef struct nAABB nAABB;
struct nAABB{
    vec2 min;
    vec2 max;
};
typedef struct nCircle nCircle;
struct nCircle {
    vec2 pos;
    f32 radius;
};

typedef enum nColliderKind nColliderKind;
enum nColliderKind {
    NCOLLIDER_KIND_AABB,
    NCOLLIDER_KIND_CIRCLE,
    NCOLLIDER_KIND_OBB,
};

// Tagged Union used for generic nCollider type
typedef struct nCollider nCollider;
struct nCollider {
    union {
        nAABB aabb;
        nCircle circle;
    };
    nColliderKind kind;
};
b32 ntest_aabb(nAABB a, nAABB b);
nAABB ncollider_to_aabb(nCollider *c);

#endif