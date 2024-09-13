#ifndef NWORLD_H
#define NWORLD_H
#include "base/base_inc.h"
#include "physics_body.h"
#include "collider.h"

// Maybe this could be the low-level physics engine and
// for gameplay stuff we could have PhysicsManager which
// has a nPhysicsWorld but does also entity stuff

typedef struct nPhysicsWorld nPhysicsWorld;
struct nPhysicsWorld {
    nPhysicsBody *bodies;
    u32 body_count;

    b32 debug_draw;
    f32 gravity_scale;
};
void nphysics_world_init(nPhysicsWorld *world);
void nphysics_world_step(nPhysicsWorld *world, f32 dt);
void nphysics_world_set_debug_draw(nPhysicsWorld *world, b32 debug_draw);
void nphysics_world_set_gravity_scale(nPhysicsWorld *world, f32 gravity_scale);

nPhysicsBody* nphysics_world_add(nPhysicsWorld *world, nPhysicsBody *body);
b32 nphysics_world_del(nPhysicsWorld *world, nPhysicsBody *body);




#endif