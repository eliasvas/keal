#include "phys_inc.h"

void nphysics_world_init_from_em(nPhysicsWorld *world) {
    M_ZERO_STRUCT(world);
    world->body_cap = get_em()->comp_array_len;
    world->body_count = get_em()->comp_array_len;
    world->bodies = get_em()->components[GetTypeID_nPhysicsBody()].data;
    world->gravity_scale = 10;
    world->iterations = 2;
}

void nphysics_world_init(nPhysicsWorld *world) {
    M_ZERO_STRUCT(world);
    nphysics_world_set_debug_draw(world, 1);
    world->body_cap = 1024;
    world->bodies = push_array(get_global_arena(), nPhysicsBody, world->body_cap);
    world->body_count = 0;
    world->gravity_scale = 10;
    // we do many iterations to solve interactions that wouldn't necessarily happen
    world->iterations = 2;
}

void nphysics_world_set_debug_draw(nPhysicsWorld *world, b32 debug_draw) {
    world->debug_draw = debug_draw;
}

// This will produce manifolds for all collisions
void nphysics_world_broadphase(nPhysicsWorld *world) {
    world->manifolds = 0;
    for (u32 i = 0; i < world->body_count; i+=1) {
        if (!NENTITY_MANAGER_HAS_COMPONENT(get_em(), i, nPhysicsBody))continue;
        for (u32 j = i+1; j < world->body_count; j+=1) {
            if (!NENTITY_MANAGER_HAS_COMPONENT(get_em(), j, nPhysicsBody))continue;
            nManifold m = {
                .a = &world->bodies[i],
                .b = &world->bodies[j],
            };
            if (nmanifold_generate(&m)) {
                nManifoldNode *node = push_array(get_frame_arena(), nManifoldNode, 1);
                node->m = m;
                sll_stack_push(world->manifolds, node);
            }
        }
    }
}

// This will solve all manifolds and update forces
void nphysics_world_step(nPhysicsWorld *world, f32 dt) {
    f32 inv_dt = dt > 0.0f ? 1.0f / dt : 0.0f;
    // 1. perform broadphase/calculate the manifolds
    nphysics_world_broadphase(world);
    // 2. integrate forces
    for (u32 i = 0; i < world->body_count; i+=1) {
        if (!NENTITY_MANAGER_HAS_COMPONENT(get_em(), i, nPhysicsBody))continue;
		nPhysicsBody* b = &world->bodies[i];
		if (b->inv_mass == 0.0f) continue;
		b->velocity = vec2_add(b->velocity, vec2_multf(vec2_add(v2(0,world->gravity_scale * b->gravity_scale), vec2_multf(b->force, b->inv_mass)),dt));
	}
    // 3. do presteps (mainly just positional correction)
    for (u32 i = 0; i < world->iterations; i+=1) {
        for (nManifoldNode *m = world->manifolds; m != 0; m = m->next) {
            nmanifold_prestep(&m->m, inv_dt);
        }
    }
    // 4. do impulse iterations
    for (u32 i = 0; i < world->iterations; i+=1) {
        for (nManifoldNode *m = world->manifolds; m != 0; m = m->next) {
            nmanifold_apply_impulse(&m->m);
        }
    }
    // 5. integrate velocities
    for (u32 i = 0; i < world->body_count; i+=1) {
        if (!NENTITY_MANAGER_HAS_COMPONENT(get_em(), i, nPhysicsBody))continue;
		nPhysicsBody* b = &world->bodies[i];

		b->position = vec2_add(b->position, vec2_multf(b->velocity, dt));
		b->force = v2(0.0f, 0.0f);
	}
    // 6. Do FAKE friction (TODO -- remove this ASAP)
    for (u32 i = 0; i < world->body_count; i+=1) {
        if (!NENTITY_MANAGER_HAS_COMPONENT(get_em(), i, nPhysicsBody))continue;
		nPhysicsBody* b = &world->bodies[i];
        b->velocity = vec2_divf(b->velocity, 1.05);
    }
}

nPhysicsBody* nphysics_world_add(nPhysicsWorld *world, nPhysicsBody *body) {
    assert(world->body_count+1 < world->body_cap);
    world->bodies[world->body_count] = *body;
    world->body_count+=1;
    return &world->bodies[world->body_count];
}

b32 nphysics_world_del(nPhysicsWorld *world, nPhysicsBody *body) {
    // TBA
    return 0;
}


// For ECS
void nphysics_world_update_func(nEntityMgr *em) {
    nPhysicsWorld world = {0};
    nphysics_world_init_from_em(&world);
    nphysics_world_step(&world, nglobal_state_get_dt()/1000.0);
}


