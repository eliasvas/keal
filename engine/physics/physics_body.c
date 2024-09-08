#include "physics_body.h"
nPhysicsBody nphysics_body_default(void) {
    nPhysicsBody b = {
        .mass = 0,
        .inv_mass = 0,
        .density = 0,
        .restitution = 0,
        .position = v2(0,0),
        .angle = 0,
        .velocity = v2(0,0),
        .force = v2(0,0),
        .gravity_scale = 0,
    };
    return b;
}