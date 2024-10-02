#include "engine.h"
nBatch2DRenderer rend;
oglImage white_img;


void rend_box(nAABB box, vec4 color) {
    nBatch2DQuad q = {0};
    q.color = color;
    q.pos.x = box.min.x;
    q.pos.y = box.min.y;
    q.dim.x = box.max.x - box.min.x;
    q.dim.y = box.max.y - box.min.y;
    q.tc = v4(0,0,1,1);
    q.angle_rad = 0;
    nbatch2d_rend_add_quad(&rend, q, &white_img);
}

void gfx_test_init() {
    NLOG_INFO("Hello gfx!\n");
    u8 white[] = { 0xFF,0xFF,0xFF,0xFF };
    ogl_image_init(&white_img, (u8*)white, 1, 1, OGL_IMAGE_FORMAT_RGBA8U);
}

void gfx_test_update_and_render() {

    mat4 view = m4d(1.0);
    nbatch2d_rend_set_view_mat(&rend, view);
    nbatch2d_rend_begin(&rend, get_nwin());
    nAABB box = {
        .min = v2(100,100),
        .max = v2(200,400),
    };
    rend_box(box, v4(1,1,1,1));

    nbatch2d_rend_end(&rend);
}