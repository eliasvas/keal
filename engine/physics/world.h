#ifndef NWORLD_H
#define NWORLD_H
#include "base/base_inc.h"
#include "physics_body.h"
#include "collider.h"
#include "ecs/ecs_inc.h"

/*
    This kindof IS the physics engine, right now we are just
    doing an update through the ECS via the update_func!
*/

typedef struct nManifoldNode nManifoldNode;
struct nManifoldNode {
    nManifold m;
    nManifoldNode *next;
};

typedef struct nPhysicsWorld nPhysicsWorld;
struct nPhysicsWorld {
    nPhysicsBody *bodies;
    u32 body_count;
    u32 body_cap;

    nManifoldNode *manifolds;

    b32 debug_draw;
    f32 gravity_scale;
    u32 iterations;
};

typedef struct nEntityMgr nEntityMgr;
void nphysics_world_init(nEntityMgr *em, nPhysicsWorld *world);
void nphysics_world_step(nEntityMgr *em, nPhysicsWorld *world, f32 dt);
void nphysics_world_update_func(nEntityMgr *em);

#endif