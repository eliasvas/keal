#include "engine.h"
nPhysicsBody b1;
nPhysicsBody b2;
nBatch2DRenderer rend;
oglImage white_img;
oglImage circle_img;


void rend_collider(nCollider *collider, vec4 color) {
    nAABB box = ncollider_to_aabb(collider);
    nBatch2DQuad q = {0};
    q.color = color;
    q.pos.x = box.min.x;
    q.pos.y = box.min.y;
    q.dim.x = box.max.x - box.min.x;
    q.dim.y = box.max.y - box.min.y;
    q.tc = v4(0,0,16,15);
    q.angle_rad = 0;
    nbatch2d_rend_add_quad(&rend, q, (collider->kind == NCOLLIDER_KIND_CIRCLE) ? &circle_img : &white_img);
}

void nphysics_world_update_func(nEntityMgr *em, void *ctx);

void nphysics_world_render_func(nEntityMgr *em, void *ctx) {
    nPhysicsWorld world = {0};
    u32 rendered_objects_count = 0;
    nphysics_world_init(em, &world);
    vec4 color = v4(1,1,1,1);
    for (u32 i = 0; i < world.body_count; i+=1) {
        if (!NENTITY_MANAGER_HAS_COMPONENT(get_em(), i, nPhysicsBody))continue;
        b32 found = 0;
        for (u32 j = 0; j < world.body_count; j+=1) {
            if (!NENTITY_MANAGER_HAS_COMPONENT(get_em(), j, nPhysicsBody))continue;
            if (i == j)continue;
            nManifold m = {
                .a = &world.bodies[i],
                .b = &world.bodies[j],
            };
            if (nmanifold_generate(&m)) {
                found = 1;
                break;
            }
        }
        nCollider c = nphysics_body_get_collider(&world.bodies[i]);
        rend_collider(&c,(found) ? v4(1,0,0,1) : color);
        rendered_objects_count+=1;
    }
    NLOG_INFO("Objects rendered: %d", rendered_objects_count);
}

void nphysics_world_far_away_delete_entities_func(nEntityMgr *em, void *ctx) {
    nPhysicsWorld world = {0};
    u32 rendered_objects_count = 0;
    nphysics_world_init(em, &world);
    vec4 color = v4(1,1,1,1);
    for (u32 i = 0; i < world.body_count; i+=1) {
        if (!NENTITY_MANAGER_HAS_COMPONENT(get_em(), i, nPhysicsBody))continue;
        if (fabsf(world.bodies[i].position.y) > 1000 || fabsf(world.bodies[i].position.y) > 1000) {
            nem_del(get_em(), i);
        }
    }
}

void physics_test_init_textures() {
    u8 white[] = { 0xFF,0xFF,0xFF,0xFF };
    ogl_image_init(&white_img, (u8*)white, 1, 1, OGL_IMAGE_FORMAT_RGBA8U);
    #define WC 0xFF,0xFF,0xFF,0xFF
    #define BC 0x00,0x00,0x00,0x00
    u8 circle[] = {
        BC,BC,BC,BC,BC,WC,WC,WC,WC,WC,WC,BC,BC,BC,BC,BC,
        BC,BC,BC,WC,WC,WC,WC,WC,WC,WC,WC,WC,WC,BC,BC,BC,
        BC,BC,WC,WC,WC,WC,WC,WC,WC,WC,WC,WC,WC,WC,BC,BC,
        BC,BC,WC,WC,WC,WC,WC,WC,WC,WC,WC,WC,WC,WC,BC,BC,
        BC,WC,WC,WC,WC,WC,WC,WC,WC,WC,WC,WC,WC,WC,WC,BC,
        BC,WC,WC,WC,WC,WC,WC,WC,WC,WC,WC,WC,WC,WC,WC,BC,
        WC,WC,WC,WC,WC,WC,WC,WC,WC,WC,WC,WC,WC,WC,WC,WC,
        WC,WC,WC,WC,WC,WC,WC,WC,WC,WC,WC,WC,WC,WC,WC,WC,
        WC,WC,WC,WC,WC,WC,WC,WC,WC,WC,WC,WC,WC,WC,WC,WC,
        BC,WC,WC,WC,WC,WC,WC,WC,WC,WC,WC,WC,WC,WC,WC,BC,
        BC,WC,WC,WC,WC,WC,WC,WC,WC,WC,WC,WC,WC,WC,WC,BC,
        BC,BC,WC,WC,WC,WC,WC,WC,WC,WC,WC,WC,WC,WC,BC,BC,
        BC,BC,WC,WC,WC,WC,WC,WC,WC,WC,WC,WC,WC,WC,BC,BC,
        BC,BC,BC,WC,WC,WC,WC,WC,WC,WC,WC,WC,WC,BC,BC,BC,
        BC,BC,BC,BC,BC,WC,WC,WC,WC,WC,WC,BC,BC,BC,BC,BC,
    };
    ogl_image_init(&circle_img, (u8*)circle, 16, 16, OGL_IMAGE_FORMAT_RGBA8U);


}

void physics_test_init() {
    NLOG_INFO("Hello physics!\n");

    physics_test_init_textures();
    
    NENTITY_MANAGER_INIT(get_em());
    NENTITY_MANAGER_COMPONENT_REGISTER(get_em(), nEntityTag);
    NENTITY_MANAGER_COMPONENT_REGISTER(get_em(), nPhysicsBody);
    NENTITY_MANAGER_COMPONENT_REGISTER(get_em(), nSprite);

    NENTITY_MANAGER_ADD_SYSTEM(get_em(), nphysics_world_update_func, 1);
    NENTITY_MANAGER_ADD_SYSTEM(get_em(), nphysics_world_far_away_delete_entities_func, 2);
    NENTITY_MANAGER_ADD_SYSTEM(get_em(), nphysics_world_render_func, 3);

    // innit a square in the middle with positive velocity on X axis (it goes right) 
    nEntityID middle_square = nem_make(get_em());
    NENTITY_MANAGER_ADD_COMPONENT(get_em(), middle_square, nPhysicsBody);
    nPhysicsBody *b = NENTITY_MANAGER_GET_COMPONENT(get_em(), middle_square, nPhysicsBody);
    *b = nphysics_body_aabb(v2(40,40), 300);
    b->position = v2(get_nwin()->ww/2, get_nwin()->wh/2);
    b->velocity = v2(100,0);
    b->gravity_scale = 0;
}

void physics_test_update_and_render() {

    vec2 mouse_pos = ninput_get_mouse_pos(get_nim());
    if (ninput_mkey_pressed(get_nim(), GUI_LMB)) {
        nEntityID new_entity = nem_make(get_em());
        NENTITY_MANAGER_ADD_COMPONENT(get_em(), new_entity, nPhysicsBody);
        nPhysicsBody *b = NENTITY_MANAGER_GET_COMPONENT(get_em(), new_entity, nPhysicsBody);
        *b = nphysics_body_circle(25, 300*gen_rand01());
        b->position = mouse_pos;
    }else if (ninput_mkey_pressed(get_nim(), GUI_RMB)) {
        nEntityID new_entity = nem_make(get_em());
        NENTITY_MANAGER_ADD_COMPONENT(get_em(), new_entity, nPhysicsBody);
        nPhysicsBody *b = NENTITY_MANAGER_GET_COMPONENT(get_em(), new_entity, nPhysicsBody);
        *b = nphysics_body_aabb(v2(50,50), 200*gen_rand01());
        b->position = mouse_pos;
    }

    f64 ct = get_current_timestamp_sec();
    mat4 view = m4d(1.0);
    nbatch2d_rend_set_view_mat(&rend, view);
    nbatch2d_rend_begin(&rend, get_nwin());

    nem_update(get_em(), 0);

    nbatch2d_rend_end(&rend);
}