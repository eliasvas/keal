#include "physics_body.h"
nPhysicsBody nphysics_body_default(void) {
    nPhysicsBody b = {
        .c_kind = NCOLLIDER_KIND_AABB,
        .hdim = v2(1,1),
        .mass = 0,
        .friction = 0.8,
        .inv_mass = 0,
        .density = 0,
        .restitution = 0.1,
        .position = v2(0,0),
        .angle = 0,
        .velocity = v2(0,0),
        .force = v2(0,0),
        .gravity_scale = 10,
        .layer = 1,
        .mask = U32_MAX,
    };
    return b;
}
nPhysicsBody nphysics_body_aabb(vec2 hdim, f32 m) {
    nPhysicsBody body = nphysics_body_default();
    body.mass = m;
    body.hdim = hdim;
    body.c_kind = NCOLLIDER_KIND_AABB;
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
    body.c_kind = NCOLLIDER_KIND_CIRCLE;
    if (m < F32_MAX) {
        body.inv_mass = 1.0/m;
    } else {
        body.inv_mass = 0;
    }
    return body;
}

nCollider nphysics_body_get_collider(nPhysicsBody *b) {
    nCollider c = {
        .kind = b->c_kind,
    };
    switch (b->c_kind) {
        case NCOLLIDER_KIND_AABB:
            c.aabb.min = vec2_sub(b->position, b->hdim);
            c.aabb.max = vec2_add(b->position, b->hdim);
            break;
        case NCOLLIDER_KIND_CIRCLE:
            c.circle.radius = b->radius;
            c.circle.pos    = b->position;
            break;
        case NCOLLIDER_KIND_OBB:
        default:
            assert(0 && "Unknown Collider type!");
    }
    return c;
}