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
b32 ntest_circle(nCircle a, nCircle b);
b32 ntest_aabb(nAABB a, nAABB b);
#endif