#include "engine.h"
oglTex sample_tex;
oglSP fsp = {0};
oglBuf fvbo = {0};

oglSP img_copy_to_rt_shader = {0};
oglRT offscreen_rt = {0};

oglTex game_load_rgba_image_from_disk2(const char *path) {
    oglTex tex;
    s32 w,h,comp;
    stbi_set_flip_vertically_on_load(1);
    unsigned char* image = stbi_load(path, &w, &h, &comp, STBI_rgb_alpha);
    if(stbi_failure_reason()) {
        NLOG_ERR("Failed reading image: %s\n", stbi_failure_reason());
    }
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    assert(ogl_tex_init(&tex, v2(w,h), image, OGL_TEX_FORMAT_RGBA8U));
    stbi_image_free(image);
    return tex;
}

void gfx_test_init() {
    NLOG_INFO("Hello gfx!");
    ogl_sp_init(&fsp, fullscreen_col_vert, fullscreen_tex_frag);
    ogl_sp_add_attrib(&fsp, ogl_make_attrib(0,OGL_SHADER_DATA_TYPE_VEC2,sizeof(vec2),offsetof(vec2, x),0));
    vec2 vdata[4] = {
        [0] = v2(+1.0,-1.0),
        [1] = v2(+1.0,+1.0),
        [2] = v2(-1.0,+1.0),
        [3] = v2(-1.0,-1.0),
    };
    fvbo = ogl_buf_make(OGL_BUF_KIND_VERTEX, vdata, 4, sizeof(vec2));
    vec2 win_dim = v2(get_nwin()->ww, get_nwin()->wh);
    ogl_rt_init(&offscreen_rt, v2(256, 256) ,OGL_TEX_FORMAT_RGBA32F);
    sample_tex = game_load_rgba_image_from_disk2("assets/tileset4922.png");
}

void gfx_test_update_and_render() {
    ogl_clear_all_state();
    vec2 win_dim = v2(get_nwin()->ww, get_nwin()->wh);

    // 1. We write a subtexture in an offscreen RT (256x256)
    ogl_rt_bind(&offscreen_rt);
    ogl_bind_vertex_buffer(&fvbo);
    ogl_bind_sp(&fsp);
    ogl_set_viewport(0,0,offscreen_rt.dim.x, offscreen_rt.dim.y);
    ogl_bind_tex_to_slot(&sample_tex, 0);
    ogl_draw(OGL_PRIM_TRIANGLE_FAN, 0, 4);

    // 2. We bind the offscreen's 1st attachment and render to our swapchain, so we should see it pixelated
    ogl_rt_bind(0);
    ogl_bind_vertex_buffer(&fvbo);
    ogl_bind_sp(&fsp);
    ogl_set_viewport(0,0,win_dim.x, win_dim.y);
    oglTex att0 = ogl_rt_get_attachment(&offscreen_rt, 0);
    ogl_bind_tex_to_slot(&att0, 0);
    ogl_draw(OGL_PRIM_TRIANGLE_FAN, 0, 4);
}