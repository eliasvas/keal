#include "physics_body.h"
nPhysicsBody nphysics_body_default(void) {
    nPhysicsBody b = {
        .kind = NCOLLIDER_KIND_AABB,
        .dim = v2(1,1),
        .mass = 0,
        .inv_mass = 0,
        .density = 0,
        .restitution = 0.1,
        .position = v2(0,0),
        .angle = 0,
        .velocity = v2(0,0),
        .force = v2(0,0),
        .gravity_scale = 10,
    };
    return b;
}
nPhysicsBody nphysics_body_aabb(vec2 dim, f32 m) {
    nPhysicsBody body = nphysics_body_default();
    body.mass = m;
    body.dim = dim;
    if (m < F32_MAX) {
        body.inv_mass = 1.0/m;
    } else {
        body.inv_mass = 0;
    }
    return body;
}

nPhysicsBody nphysics_body_circle(f32 radius, f32 m) {
    nPhysicsBody body = nphysics_body_default();
    body.mass = m;
    body.radius = radius;
    if (m < F32_MAX) {
        body.inv_mass = 1.0/m;
    } else {
        body.inv_mass = 0;
    }
    return body;
}

nCollider nphysics_body_get_collider(nPhysicsBody *body) {
    nCollider c = {0};
    c.kind = body->kind;
    switch (body->kind) {
        case NCOLLIDER_KIND_AABB:
            c.aabb.min = vec2_sub(body->position, vec2_divf(body->dim,2));
            c.aabb.max = vec2_add(body->position, vec2_divf(body->dim,2));
            break;
        case NCOLLIDER_KIND_CIRCLE:
            c.circle.radius = body->radius;
            c.circle.pos = body->position;
            break;
        case NCOLLIDER_KIND_OBB:
        default:
            assert(0 && "OBBs not implemented yet!");
    }
    return c;
}