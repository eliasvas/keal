#include "engine.h"
nPhysicsBody b1;
nPhysicsBody b2;
nBatch2DRenderer rend;
oglImage white_img;
nPhysicsWorld pw;

void rend_box(nAABB box, vec4 color) {
    nBatch2DQuad q = {0};
    q.color = color;
    q.pos.x = box.min.x;
    q.pos.y = box.min.y;
    q.dim.x = box.max.x - box.min.x;
    q.dim.y = box.max.y - box.min.y;
    q.tc = v4(0,0,0,0);
    q.angle_rad = 0;
    nbatch2d_rend_add_quad(&rend, q, &white_img);
}

void physics_test_init() {
    printf("Hello physics!\n");
    u8 white[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    ogl_image_init(&white_img, (u8*)white, 1, 1, OGL_IMAGE_FORMAT_RGBA8U);
    nphysics_world_init(&pw);

    b1 = nphysics_body_aabb(v2(50,50), 500);
    b1.position = v2(get_nwin()->ww/2, get_nwin()->wh/2);
    b1.gravity_scale = 0;
    nphysics_world_add(&pw, &b1);
}

void physics_test_update_and_render() {
    nphysics_world_step(&pw, 1/60.0);

    vec2 mouse_pos =ninput_get_mouse_pos(get_nim());
    b2 = nphysics_body_aabb(v2(50,50), 200);
    b2.position = mouse_pos;
    if (ninput_mkey_pressed(get_nim(), GUI_LMB)) {
        nphysics_world_add(&pw, &b2);
    }

    f64 ct = get_current_timestamp_sec();
    mat4 view = m4d(1.0);//ndungeon_cam_get_view_mat(&gs.dcam);
    nbatch2d_rend_set_view_mat(&rend, view);

    nbatch2d_rend_begin(&rend, get_nwin());
    // render all bodies inside physics engine
    vec4 color = v4(1,1,1,1);
    for (u32 i = 0; i < pw.body_count; i+=1) {
        nAABB box = {
            .min = vec2_sub(pw.bodies[i].position, vec2_divf(pw.bodies[i].dim,2)),
            .max = vec2_add(pw.bodies[i].position, vec2_divf(pw.bodies[i].dim,2)),
        };
        b32 found = 0;
        for (u32 j = 0; j < pw.body_count; j+=1) {
            if (i == j)continue;
            nManifold m = {
                .a = &pw.bodies[i],
                .b = &pw.bodies[j],
            };
            if (nmanifold_aabbs(&m)) {
                found = 1;
                rend_box(box, v4(1,0,0,1));
                break;
            }
        }
        if (!found) {
            rend_box(box, color);
        }
    }
    nbatch2d_rend_end(&rend);

    nManifold m = {
        .a = &b1,
        .b = &b2,
    };
    nmanifold_aabbs(&m);
    //printf("pen_depth: %f\n", m.penetration);
}