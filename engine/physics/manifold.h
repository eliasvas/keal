#ifndef NMANIFOLD_H
#define NMANIFOLD_H
#include "collider.h"
#include "physics_body.h"
typedef struct nManifold nManifold;
struct nManifold {
    nPhysicsBody *a;
    nPhysicsBody *b;
    f32 penetration;
    vec2 normal;
};

b32 nmanifold_aabbs(nManifold *m);
void nmanifold_apply_impulse(nManifold *m);
void nmanifold_prestep(nManifold *m, f32 inv_dt);

#endif