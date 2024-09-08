#include "engine.h"
nAABB b1;
nAABB b2;
nBatch2DRenderer rend;
oglImage white_img;

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
}
void physics_test_update_and_render() {
    f64 ct = get_current_timestamp_sec();
    b1.min = v2(get_nwin()->ww/2, get_nwin()->wh/2);
    b1.max= v2(b1.min.x+100, b1.min.y+100);
    b2.min = v2(b1.min.x,b1.min.y + 200 * sin(ct));
    b2.max = v2(b2.min.x+100,b2.min.y+100);

    mat4 view = m4d(1.0);//ndungeon_cam_get_view_mat(&gs.dcam);
    nbatch2d_rend_set_view_mat(&rend, view);
    nbatch2d_rend_begin(&rend, get_nwin());

    vec4 color = ntest_aabb(b1,b2) ? v4(1,0,0,1) : v4(1,1,1,1);
    rend_box(b1, color);
    rend_box(b2, vec4_divf(color,2));
    nbatch2d_rend_end(&rend);
}