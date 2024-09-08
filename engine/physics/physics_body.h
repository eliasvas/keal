#ifndef NPHYSICS_BODY_H
#define NPHYSICS_BODY_H
#include "base/base_inc.h"

typedef struct nPhysicsBody nPhysicsBody;
struct nPhysicsBody {
    // mass data
    f32 mass;
    f32 inv_mass;
    // material data
    f32 density;
    f32 restitution;
    // physics stuff
    vec2 position;
    f32 angle;
    vec2 velocity;
    vec2 force;
    f32 gravity_scale;
};
nPhysicsBody nphysics_body_default(void);

#endif