#include "engine.h"
//oglImage white_img;
oglSP fsp = {0};
oglBuf fvbo = {0};
oglSP offscreen_shader = {0};
oglSP img_copy_to_rt_shader = {0};
oglImage offscreen_rt = {0};
f32 zoom_factor = 1.0;

void gfx_test_init() {
    NLOG_INFO("Hello gfx!");
    ogl_sp_init(&fsp, fullscreen_col_vert, fullscreen_col_frag);
    ogl_sp_add_attrib(&fsp, ogl_make_attrib(0,OGL_SHADER_DATA_TYPE_VEC2,sizeof(vec2),offsetof(vec2, x),0));
    vec2 vdata[4] = {
        [0] = v2(+1.0,-1.0),
        [1] = v2(+1.0,+1.0),
        [2] = v2(-1.0,+1.0),
        [3] = v2(-1.0,-1.0),
    };
    fvbo = ogl_buf_make(OGL_BUF_KIND_VERTEX, vdata, 4, sizeof(vec2));
    vec2 win_dim = v2(get_nwin()->ww, get_nwin()->wh);
    ogl_image_init(&offscreen_rt, NULL, 512, 512, OGL_IMAGE_FORMAT_RGBA32F);
}

void gfx_test_update_and_render() {
    ogl_clear_all_state();
    vec2 win_dim = v2(get_nwin()->ww, get_nwin()->wh);
    ogl_set_viewport(0,0,win_dim.x, win_dim.y);

    ogl_bind_vertex_buffer(&fvbo);
    ogl_bind_sp(&fsp);

    vec4 color = vec4_multf(v4(0.93,0.25,0.25,1.0), (sin(10*get_current_timestamp_sec())+ 1.0) / 2.0);
    ogl_sp_set_uniform(&fsp, "color", OGL_SHADER_DATA_TYPE_VEC4, &color);

    nScrollAmount sa = ninput_get_scroll_amount_delta(get_nim());
    zoom_factor = (sa) ? zoom_factor * (1.0 + 0.10 * sa) : zoom_factor;
    ogl_sp_set_uniform(&fsp, "zoom_factor", OGL_SHADER_DATA_TYPE_FLOAT, &zoom_factor);

    //ogl_bind_image_to_texture_slot(&sample_tex, 0, 0);

    ogl_draw(OGL_PRIM_TRIANGLE_FAN, 0, 4);
}