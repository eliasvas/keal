#ifndef NWORLD_H
#define NWORLD_H
#include "base/base_inc.h"
#include "physics_body.h"
#include "collider.h"
#include "ecs/ecs_inc.h"

// Maybe this could be the low-level physics engine and
// for gameplay stuff we could have PhysicsManager which
// has a nPhysicsWorld but does also entity stuff

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
void nphysics_world_init(nPhysicsWorld *world);
void nphysics_world_init_from_em(nPhysicsWorld *world);
void nphysics_world_step(nPhysicsWorld *world, f32 dt);
void nphysics_world_set_debug_draw(nPhysicsWorld *world, b32 debug_draw);

nPhysicsBody* nphysics_world_add(nPhysicsWorld *world, nPhysicsBody *body);
b32 nphysics_world_del(nPhysicsWorld *world, nPhysicsBody *body);

#endif