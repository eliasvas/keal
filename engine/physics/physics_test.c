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

    b1 = nphysics_body_aabb(v2(100,100), 50);
    b1.position = v2(get_nwin()->ww/2, get_nwin()->wh/2);
}

void physics_test_update_and_render() {

    vec2 mouse_pos =ninput_get_mouse_pos(get_nim());
    b2 = nphysics_body_aabb(v2(100,100), 20);
    b2.position = mouse_pos;

    f64 ct = get_current_timestamp_sec();
    mat4 view = m4d(1.0);//ndungeon_cam_get_view_mat(&gs.dcam);
    nbatch2d_rend_set_view_mat(&rend, view);
    nbatch2d_rend_begin(&rend, get_nwin());

    nAABB b1_box = {
        .min = vec2_sub(b1.position, vec2_divf(b1.dim,2)),
        .max = vec2_add(b1.position, vec2_divf(b1.dim,2)),
    };
    nAABB b2_box = {
        .min = vec2_sub(b2.position, vec2_divf(b2.dim,2)),
        .max = vec2_add(b2.position, vec2_divf(b2.dim,2)),
    };
    vec4 color = ntest_aabb(b1_box,b2_box) ? v4(1,0,0,1) : v4(1,1,1,1);

    rend_box(b1_box, color);
    rend_box(b2_box, vec4_divf(color,2));
    nbatch2d_rend_end(&rend);

    nManifold m = {
        .a = &b1,
        .b = &b2,
    };
    nmanifold_aabbs(&m);
    printf("pen_depth: %f\n", m.penetration);
}