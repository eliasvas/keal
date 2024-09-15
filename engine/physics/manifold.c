#include "manifold.h"

void nmanifold_prestep(nManifold *m, f32 inv_dt) {
    // positional correction
    f32 posc_percent = 0.2;
    vec2 correction = vec2_multf(m->normal, m->penetration * posc_percent);
    if (m->a->inv_mass > 0.0f) {
        m->a->position = vec2_sub(m->a->position, correction);
    }
    if (m->b->inv_mass > 0.0f) {
        m->b->position = vec2_add(m->b->position, correction);
    }
}
b32 nmanifold_gen_circle_circle(nManifold *m) {
    nPhysicsBody *a = m->a;
    nPhysicsBody *b = m->b;
    vec2 n = vec2_sub(b->position,a->position);
    nCircle a_circle= nphysics_body_get_collider(a).circle;
    nCircle b_circle = nphysics_body_get_collider(b).circle;

    f32 r = a_circle.radius + b_circle.radius;
    r *= r;
    if(vec2_dot(n,n) > r){
        return 0;
    }
    f32 d = vec2_len(n);
    if(d != 0) {
        m->penetration = (a_circle.radius+b_circle.radius) - d;
        m->normal = vec2_divf(n,d);
        return 1;
    } else {
        m->penetration = a_circle.radius;
        m->normal = v2(0,0);
        return 1;
    }
    return 0;
}
b32 nmanifold_gen_aabb_circle(nManifold *m) {
    nPhysicsBody *a = m->a;
    nPhysicsBody *b = m->b;

    vec2 n = vec2_sub(b->position, a->position);
    nAABB a_aabb = nphysics_body_get_collider(a).aabb;
    nCircle b_circle = nphysics_body_get_collider(b).circle;

    f32 x_extent = (a_aabb.max.x - a_aabb.min.x) / 2.0f;
    f32 y_extent = (a_aabb.max.y - a_aabb.min.y) / 2.0f;

    vec2 closest = n;
    closest.x = clamp(closest.x, -x_extent, x_extent);
    closest.y = clamp(closest.y, -y_extent, y_extent);

    b32 inside = 0;

    if (equalf(n.x, closest.x, 0.001f) && equalf(n.y, closest.y, 0.001f)) {
        inside = 1;
        // Find the closest edge direction for pushing the circle out
        if (fabs(n.x) > fabs(n.y)) {
            closest.x = (closest.x > 0) ? x_extent : -x_extent;
        } else {
            closest.y = (closest.y > 0) ? y_extent : -y_extent;
        }
    }

    vec2 normal = vec2_sub(n, closest);
    f32 d = vec2_dot(normal, normal);
    f32 r = b_circle.radius;

    if (d > r * r && !inside) {
        return 0;
    }

    d = sqrtf(d);
    if (inside) {
        m->normal = vec2_norm(vec2_multf(normal, -1.0f));
        m->penetration = r - d;
    } else {
        if (d != 0) {
            m->normal = vec2_norm(normal);
        } else {
            m->normal = (vec2){1.0f, 0.0f};
        }
        m->penetration = r - d;
    }

    return 1;
}
b32 nmanifold_gen_aabb_aabb(nManifold *m) {
    nPhysicsBody *a = m->a;
    nPhysicsBody *b = m->b;

    vec2 n = vec2_sub(b->position,a->position);

    nAABB a_box = nphysics_body_get_collider(a).aabb;
    nAABB b_box = nphysics_body_get_collider(b).aabb;

    f32 a_extent = (a_box.max.x - a_box.min.x) / 2;
    f32 b_extent = (b_box.max.x - b_box.min.x) / 2;
    f32 x_overlap = a_extent + b_extent - fabs(n.x);
    if (x_overlap > 0) {
        f32 a_extent = (a_box.max.y - a_box.min.y) / 2;
        f32 b_extent = (b_box.max.y - b_box.min.y) / 2;
        f32 y_overlap = a_extent + b_extent - fabs(n.y);
        if (y_overlap > 0) {
            if (x_overlap < y_overlap) {
                if (n.x < 0) {
                    m->normal = v2(-1,0);
                }else {
                    m->normal = v2(1,0);
                }
                m->penetration = x_overlap;
            } else {
                if (n.y < 0) {
                    m->normal = v2(0,-1);
                }else {
                    m->normal = v2(0,1);
                }
                m->penetration = y_overlap;
            }
            return 1;
        }
    }
    return 0;
}

b32 nmanifold_generate(nManifold *m) {
    if (m->a->c_kind == NCOLLIDER_KIND_AABB && m->b->c_kind == NCOLLIDER_KIND_AABB) {
        return nmanifold_gen_aabb_aabb(m);
    } else if (m->a->c_kind == NCOLLIDER_KIND_CIRCLE && m->b->c_kind == NCOLLIDER_KIND_CIRCLE) {
        return nmanifold_gen_circle_circle(m);
    }else if (m->a->c_kind == NCOLLIDER_KIND_AABB && m->b->c_kind == NCOLLIDER_KIND_CIRCLE) {
        return nmanifold_gen_aabb_circle(m);
    }else if (m->a->c_kind == NCOLLIDER_KIND_CIRCLE && m->b->c_kind == NCOLLIDER_KIND_AABB) {
        // swap 'em
        nPhysicsBody *temp = m->a;
        m->a = m->b;
        m->b = temp;
        return nmanifold_gen_aabb_circle(m);
    }
    return 0;
}

void nmanifold_apply_impulse(nManifold *m) {
    nPhysicsBody *a = m->a;
    nPhysicsBody *b = m->b;

    // Regular impulse
    vec2 rv = vec2_sub(b->velocity, a->velocity);
    f32 vel_along_norm = vec2_dot(rv, m->normal);
    if (vel_along_norm > 0) return;
    f32 e = minimum(a->restitution, b->restitution);
    f32 j = -(1+e)*vel_along_norm;
    j /= (a->inv_mass + b->inv_mass);
    vec2 impulse = vec2_multf(m->normal,j);
    a->velocity = vec2_sub(a->velocity, vec2_multf(impulse, a->inv_mass));
    b->velocity = vec2_add(b->velocity, vec2_multf(impulse, b->inv_mass));

    // Friction
    // rv = vec2_sub(b->velocity, a->velocity);
    // vel_along_norm = vec2_dot(rv, m->normal);
    // vec2 tangent = vec2_sub(rv, vec2_multf(m->normal, vel_along_norm));
    // tangent = vec2_norm(tangent);
    // float vel_along_tangent = vec2_dot(rv, tangent);
    // float jt = -vel_along_tangent;
    // jt /= (a->inv_mass + b->inv_mass);
    // float mu = sqrtf(a->friction * b->friction);
    // vec2 friction_impulse = (fabs(jt) < j * mu) ? vec2_multf(tangent, jt) : vec2_multf(tangent, -j * mu);
    // a->velocity = vec2_sub(a->velocity, vec2_multf(friction_impulse, a->inv_mass));
    // b->velocity = vec2_add(b->velocity, vec2_multf(friction_impulse, b->inv_mass));
}