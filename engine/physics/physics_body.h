#ifndef NPHYSICS_BODY_H
#define NPHYSICS_BODY_H
#include "base/base_inc.h"
#include "collider.h"

typedef struct nPhysicsBody nPhysicsBody;
struct nPhysicsBody {
    //collider data (can this be a different Thing?)
    union {
        struct AABB_Data {
            vec2 half_dim;
        };
        struct Circle_Data {
            f32 radius;
        };
    };
    nColliderKind c_kind;
    //-------------------------------------------------
    // mass data
    f32 mass;
    f32 inv_mass;
    // material data
    f32 density;
    f32 restitution;
    f32 friction;
    // physics stuff
    vec2 position;
    f32 angle;
    vec2 velocity;
    vec2 force;
    f32 gravity_scale;

    // HACKY
    b32 collider_off;
};
nPhysicsBody nphysics_body_default(void);
nPhysicsBody nphysics_body_aabb(vec2 half_dim, f32 m);
nPhysicsBody nphysics_body_circle(f32 radius, f32 m);

nCollider nphysics_body_get_collider(nPhysicsBody *b);

#endif