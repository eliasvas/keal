#include "world.h"

void nphysics_world_init(nPhysicsWorld *world) {
    M_ZERO_STRUCT(world);
    nphysics_world_set_debug_draw(world, 1);
    nphysics_world_set_gravity_scale(world, -10.0);
}

void nphysics_world_set_debug_draw(nPhysicsWorld *world, b32 debug_draw) {
    world->debug_draw = debug_draw;
}

void nphysics_world_set_gravity_scale(nPhysicsWorld *world, f32 gravity_scale) {
    world->gravity_scale = gravity_scale;
}


// This will produce manifolds
void nphysics_world_broadphase(nPhysicsWorld *world) {
}

// This will solve all manifolds and update forces
void nphysics_world_step(nPhysicsWorld *world, f32 dt) {
    nphysics_world_broadphase(world);
}

nPhysicsBody* nphysics_world_add(nPhysicsWorld *world, nPhysicsBody *body) {
    return 0;
}

b32 nphysics_world_del(nPhysicsWorld *world, nPhysicsBody *body) {
    return 0;
}

