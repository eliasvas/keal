#include "manifold.h"

void nmanifold_prestep(nManifold *m, f32 inv_dt) {}

b32 nmanifold_aabbs(nManifold *m) {
    nPhysicsBody *a = m->a;
    nPhysicsBody *b = m->b;

    vec2 n = vec2_sub(b->position,a->position);

    nAABB a_box = {
        .min = vec2_sub(a->position, vec2_divf(a->dim,2)),
        .max = vec2_add(a->position, vec2_divf(a->dim,2)),
    };
    nAABB b_box = {
        .min = vec2_sub(b->position, vec2_divf(b->dim,2)),
        .max = vec2_add(b->position, vec2_divf(b->dim,2)),
    };

    // calc half extents along x axis
    f32 a_extent = (a_box.max.x - a_box.min.x) / 2;
    f32 b_extent = (b_box.max.x - b_box.min.x) / 2;
    //find overlap on x axis
    f32 x_overlap = a_extent + b_extent - fabs(n.x);
    // Perform SAT on x axis
    if (x_overlap > 0) {
        //calc the extents on y axis
        f32 a_extent = (a_box.max.y - a_box.min.y) / 2;
        f32 b_extent = (b_box.max.y - b_box.min.y) / 2;
        //calc overlap on y axis
        f32 y_overlap = a_extent + b_extent - fabs(n.y);

        if (y_overlap > 0) {
            //find axis of LEAST penetration
            if (x_overlap > y_overlap) {
                // point towards B if n goes A->B
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

void nmanifold_apply_impulse(nManifold *m) {
    nPhysicsBody *a = m->a;
    nPhysicsBody *b = m->b;

    vec2 rv = vec2_sub(b->velocity, a->velocity);
    f32 vel_along_norm = vec2_dot(rv, m->normal);
    if (vel_along_norm > 0) return;
    f32 e = minimum(a->restitution, b->restitution);
    f32 j = -(1+e)*vel_along_norm;
    j /= (a->inv_mass + b->inv_mass);
    vec2 impulse = vec2_multf(m->normal,j);

    a->velocity = vec2_add(a->velocity, vec2_multf(impulse, a->inv_mass));
    b->velocity = vec2_sub(b->velocity, vec2_multf(impulse, b->inv_mass));
}