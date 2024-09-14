#include "engine.h"
nPhysicsBody b1;
nPhysicsBody b2;
nBatch2DRenderer rend;
oglImage white_img;
oglImage circle_img;
nPhysicsWorld pw;

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

void physics_test_init() {
    printf("Hello physics!\n");
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

    nphysics_world_init(&pw);

    b1 = nphysics_body_aabb(v2(40,40), 300);
    b1.position = v2(get_nwin()->ww/2, get_nwin()->wh/2);
    b1.velocity = v2(100,0);
    b1.gravity_scale = 0;
    nphysics_world_add(&pw, &b1);
}

void physics_test_update_and_render() {
    //printf("vel= %f %f\n", pw.bodies[0].velocity.x, pw.bodies[0].velocity.y);
    nphysics_world_step(&pw, nglobal_state_get_dt()/1000.0);

    vec2 mouse_pos = ninput_get_mouse_pos(get_nim());
    if (ninput_mkey_pressed(get_nim(), GUI_LMB)) {
        b2 = nphysics_body_circle(25, 200*gen_rand01());
        b2.position = mouse_pos;
        nphysics_world_add(&pw, &b2);
    }else if (ninput_mkey_pressed(get_nim(), GUI_RMB)) {
        b2 = nphysics_body_aabb(v2(50,50), 200*gen_rand01());
        b2.position = mouse_pos;
        nphysics_world_add(&pw, &b2);
    }

    f64 ct = get_current_timestamp_sec();
    mat4 view = m4d(1.0);//ndungeon_cam_get_view_mat(&gs.dcam);
    nbatch2d_rend_set_view_mat(&rend, view);

    nbatch2d_rend_begin(&rend, get_nwin());
    // render all bodies inside physics engine
    vec4 color = v4(1,1,1,1);
    for (u32 i = 0; i < pw.body_count; i+=1) {
        b32 found = 0;
        for (u32 j = 0; j < pw.body_count; j+=1) {
            if (i == j)continue;
            nManifold m = {
                .a = &pw.bodies[i],
                .b = &pw.bodies[j],
            };
            if (nmanifold_generate(&m)) {
                found = 1;
                break;
            }
        }
        nCollider c = nphysics_body_get_collider(&pw.bodies[i]);
        rend_collider(&c,(found) ? v4(1,0,0,1) : color);
    }
    nbatch2d_rend_end(&rend);
}