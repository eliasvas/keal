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
    // arena_test();
    // arena_scratch_test();
    // sll_stack_test();
    // sll_queue_test();
    // dll_test();

    platform_init();
    engine_global_state_init();
    engine_global_state_set_target_fps(60.0);
    f32 vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f,  0.5f, 0.0f,
    };
    oglContext ogl_ctx;
    ogl_ctx_init(&ogl_ctx);
    oglBuf vbo = ogl_buf_make(OGL_BUF_KIND_VERTEX, vertices, 3, sizeof(vec3));
    oglSP sp;
    ogl_sp_init(&sp, vs_ogl, fs_ogl);
    ogl_sp_add_attrib(&sp, ogl_make_attrib(0,OGL_SHADER_DATA_TYPE_VEC3,0,0,0));
    while(1) {
        ogl_clear_all_state(&ogl_ctx);
        glDisable(GL_DEPTH_TEST);
        engine_global_state_frame_begin();
        glClearColor(0.2 * cos(get_current_timestamp()/4000.0),0.2 * sin(get_current_timestamp()/3000.0),0.3 * cos(get_current_timestamp()/1000.0),1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        platform_update();
        ogl_bind_vertex_buffer(&vbo);
        ogl_bind_sp(&sp);
        vec3 c = v3(1.0,0.0,0.0);
        ogl_sp_set_uniform(&sp, "color", OGL_SHADER_DATA_TYPE_VEC3, &c);
        glViewport(0,0,800,600);
        ogl_draw(OGL_PRIM_TRIANGLES, 0, 3);
        platform_swap();

        engine_global_state_frame_end();
    }
}