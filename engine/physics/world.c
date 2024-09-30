#include "phys_inc.h"

void nphysics_world_init(nEntityMgr *em, nPhysicsWorld *world) {
    M_ZERO_STRUCT(world);
    world->body_cap = em->comp_array_len;
    world->body_count = em->comp_array_len;
    world->bodies = em->components[GetTypeID_nPhysicsBody()].data;
    world->gravity_scale = 10;
    world->iterations = 2;
}

// This will produce manifolds for all collisions
void nphysics_world_broadphase(nEntityMgr *em, nPhysicsWorld *world) {
    world->manifolds = 0;
    for (u64 i = 0; i < world->body_count; i+=1) {
        if (!NENTITY_MANAGER_HAS_COMPONENT(em, i, nPhysicsBody))continue;
        for (u32 j = i+1; j < world->body_count; j+=1) {
            if (!NENTITY_MANAGER_HAS_COMPONENT(em, j, nPhysicsBody))continue;
            nManifold m = {
                .a = &world->bodies[i],
                .b = &world->bodies[j],
            };
            if (m.a->collider_off || m.b->collider_off)continue;
            if (m.a->inv_mass == 0.0 && m.b->inv_mass == 0.0)continue;
            // broadphase narrow-down 
            {
                nCollider a_col = nphysics_body_get_collider(m.a);
                nCollider b_col = nphysics_body_get_collider(m.b);
                nAABB a_box = ncollider_to_aabb(&a_col);
                nAABB b_box = ncollider_to_aabb(&b_col);
                if (!ntest_aabb(a_box,b_box))continue;
            }

            if (nmanifold_generate(&m)) {
                nManifoldNode *node = push_array(get_frame_arena(), nManifoldNode, 1);
                node->m = m;
                sll_stack_push(world->manifolds, node);

                nEntityEvent e = {
                    // TODO -- make an api to get entity from 'index'
                    .entity_a = NENTITY_MANAGER_GET_ENTITY_FOR_INDEX(em, i),
                    .entity_b = NENTITY_MANAGER_GET_ENTITY_FOR_INDEX(em, j),
                    .flags = NENTITY_EVENT_KIND_COLLISION,
                    .extra_flags = 0,
                };
                nentity_event_mgr_add(&em->event_mgr, e);
            }
        }
    }
}

// This will solve all manifolds and update forces
void nphysics_world_step(nEntityMgr *em, nPhysicsWorld *world, f32 dt) {
    f32 inv_dt = dt > 0.0f ? 1.0f / dt : 0.0f;
    // 1. perform broadphase/calculate the manifolds
    nphysics_world_broadphase(em, world);
    // 2. integrate forces
    for (u64 i = 0; i < world->body_count; i+=1) {
        nEntityID entity = NENTITY_MANAGER_GET_ENTITY_FOR_INDEX(em,i);
        if (!NENTITY_MANAGER_HAS_COMPONENT(em, entity, nPhysicsBody))continue;
		nPhysicsBody* b = &world->bodies[i];
		if (b->inv_mass == 0.0f) continue;
		b->velocity = vec2_add(b->velocity, vec2_multf(vec2_add(v2(0,world->gravity_scale * b->gravity_scale), vec2_multf(b->force, b->inv_mass)),dt));
	}
    // 3. do presteps (mainly just positional correction)
    for (u64 i = 0; i < world->iterations; i+=1) {
        for (nManifoldNode *m = world->manifolds; m != 0; m = m->next) {
            nmanifold_prestep(&m->m, inv_dt);
        }
    }
    // 4. do impulse iterations
    for (u64 i = 0; i < world->iterations; i+=1) {
        for (nManifoldNode *m = world->manifolds; m != 0; m = m->next) {
            nmanifold_apply_impulse(&m->m);
        }
    }
    // 5. integrate velocities
    for (u64 i = 0; i < world->body_count; i+=1) {
        if (!NENTITY_MANAGER_HAS_COMPONENT(em, i, nPhysicsBody))continue;
		nPhysicsBody* b = &world->bodies[i];

		b->position = vec2_add(b->position, vec2_multf(b->velocity, dt));
		b->force = v2(0.0f, 0.0f);
	}
    // 6. Do FAKE friction (TODO -- remove this ASAP)
    for (u64 i = 0; i < world->body_count; i+=1) {
        if (!NENTITY_MANAGER_HAS_COMPONENT(em, i, nPhysicsBody))continue;
		nPhysicsBody* b = &world->bodies[i];
        b->velocity = vec2_divf(b->velocity, 1.05);
    }
}

// We make a world from the EntityManager's nPhysicsBody components and 'step' to simulate
void nphysics_world_update_func(nEntityMgr *em) {
    nPhysicsWorld world = {0};
    nphysics_world_init(em, &world);
    nphysics_world_step(em, &world, nglobal_state_get_dt_sec());
}


