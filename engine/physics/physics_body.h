#ifndef NPHYSICS_BODY_H
#define NPHYSICS_BODY_H
#include "base/base_inc.h"
#include "collider.h"

typedef struct nPhysicsBody nPhysicsBody;
struct nPhysicsBody {
    //collider data (can this be a different Thing?)
    union {
        struct AABB_Data {
            vec2 dim;
        };
        struct Circle_Data {
            f32 radius;
        };
    };
    nColliderKind kind;
    //-------------------------------------------------
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
nPhysicsBody nphysics_body_aabb(vec2 dim, f32 m);
nPhysicsBody nphysics_body_circle(f32 radius, f32 m);

#endif