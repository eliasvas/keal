#include <stdio.h>
#include "engine.h"

#define SDL_MAIN_NOIMPL

#include <SDL.h>
#include <SDL_main.h>

#if defined(OS_WINDOWS)
    #include <GL/glew.h>
    #include <GL/wglew.h>
#elif defined(OS_LINUX)
    #include <SDL_syswm.h>
    #include <GLES3/gl3.h>
    #include <GLES/egl.h>
#elif defined(__EMSCRIPTEN)
    #include <emscripten.h>
	#include <emscripten/html5.h>
	#include <GLES3/gl3.h>
#endif

int main() {
    nWindow win;
    engine_global_state_init();
    nwindow_init(&win, "gudGame", 600, 400, N_WINDOW_OPT_RESIZABLE | N_WINDOW_OPT_BORDERLESS);
    engine_global_state_set_target_fps(60.0);
    f32 vertices[] = {
        -0.5f, -0.5f, 0.0f,0.0,0.0,
        -0.5f, 0.5f, 0.0f,0.0,1.0,
        0.5f, 0.5f, 0.0f,1.0,1.0,
        0.5f,  -0.5f, 0.0f,1.0,0.0,
    };
    u32 indices[] = {0,1,2,2,3,0};
    oglContext ogl_ctx;
    ogl_ctx_init(&ogl_ctx);

    u8 checkers[] = {
        0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00,
        0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF,
        0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00,
        0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF,
        0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00,
        0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF,
        0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00,
        0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF,
    };
    oglImage img;
    assert(ogl_image_init(&img, checkers, 8, 8, OGL_IMAGE_FORMAT_RGBA8U, 1));
    oglBuf vbo = ogl_buf_make(OGL_BUF_KIND_VERTEX, vertices, 4, sizeof(vec3)+sizeof(vec2));
    oglBuf ibo = ogl_buf_make(OGL_BUF_KIND_INDEX, indices, 6, sizeof(u32));
    oglSP sp;
    ogl_sp_init(&sp, vs_ogl, fs_ogl);
    ogl_sp_add_attrib(&sp, ogl_make_attrib(0,OGL_SHADER_DATA_TYPE_VEC3,sizeof(vec3)+sizeof(vec2),0,0));
    ogl_sp_add_attrib(&sp, ogl_make_attrib(1,OGL_SHADER_DATA_TYPE_VEC2,sizeof(vec3)+sizeof(vec2),sizeof(vec3),0));
    while(1) {
        nwindow_capture_events(&win);
        ogl_clear_all_state(&ogl_ctx);
        engine_global_state_frame_begin();
        ogl_image_clear(NULL);
        ogl_bind_vertex_buffer(&vbo);
        ogl_bind_index_buffer(&ibo);
        ogl_bind_image_to_texture_slot(&img, 0, 0);
        ogl_bind_sp(&sp);
        vec3 c = v3(1.0,0.0,1.0);
        ogl_sp_set_uniform(&sp, "color", OGL_SHADER_DATA_TYPE_VEC3, &c);

        vec2 dim = nwindow_get_dim(&win);
        ogl_set_viewport(0,0,dim.x,dim.y);

        ogl_draw_indexed(OGL_PRIM_TRIANGLES, 6); 
        nwindow_swap(&win);

        engine_global_state_frame_end();
    }
    nwindow_deinit(&win);
}

