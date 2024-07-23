#include "engine.h"
#include "shaders.inl"
#include "glb.h"

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

// globals
GLuint vao,vbo,sp,atlas_tex,atlas_sampler;
SDL_Window *window;
SDL_GLContext glcontext;


GLuint compile_shader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    GLint isCompiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE) {
        GLint maxLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

        char errorLog[256];
        glGetShaderInfoLog(shader, maxLength, &maxLength, errorLog);

        SDL_Log("Shader compilation error: %s",errorLog);

        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

GLuint create_sp(const char* gui_vs, const char* gui_fs) {
    GLuint vertexShader = compile_shader(GL_VERTEX_SHADER, gui_vs);
    if (vertexShader == 0) {
        return 0;
    }

    GLuint fragmentShader = compile_shader(GL_FRAGMENT_SHADER, gui_fs);
    if (fragmentShader == 0) {
        glDeleteShader(vertexShader);
        return 0;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    GLint isLinked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, (int *)&isLinked);
    if (isLinked == GL_FALSE) {
        GLint maxLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

        char errorLog[512];
        glGetProgramInfoLog(program, maxLength, &maxLength, errorLog);
        SDL_Log("Shader compilation error: %s",errorLog);



        glDeleteProgram(program);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return 0;
    }

    glDetachShader(program, vertexShader);
    glDetachShader(program, fragmentShader);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}
#include <stddef.h> // for offsetof

void fill_instance_vbo(GLuint instanceVBO, const InstanceData* instances, GLsizei instance_count) {
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, instance_count * sizeof(InstanceData), instances, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

GLuint create_instance_vbo(const InstanceData* instances, GLsizei instance_count) {
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, instance_count * sizeof(InstanceData), instances, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return vbo;
}

GLuint create_atlas_tex_and_sampler(const GLubyte* pixels, GLsizei width, GLsizei height, GLuint* sampler) {
    GLuint texture;

    // Generate and bind texture
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Specify texture parameters
    #ifdef __EMSCRIPTEN__
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, pixels);
    #else
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, pixels);
    #endif
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Generate mipmaps
    glGenerateMipmap(GL_TEXTURE_2D);

    // Create and configure sampler
    // glGenSamplers(1, sampler);
    // glSamplerParameteri(*sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    // glSamplerParameteri(*sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // glSamplerParameteri(*sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
    // glSamplerParameteri(*sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Unbind texture and sampler to clean state
    glBindTexture(GL_TEXTURE_2D, 0);
    return texture;
}

GLuint setup_instance_vao(GLuint vbo) {
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glEnableVertexAttribArray(0); // inPos0
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, pos0));
    glVertexAttribDivisor(0, 1);

    glEnableVertexAttribArray(1); // inPos1
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, pos1));
    glVertexAttribDivisor(1, 1);

    glEnableVertexAttribArray(2); // inUV0
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, uv0));
    glVertexAttribDivisor(2, 1);

    glEnableVertexAttribArray(3); // inUV1
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, uv1));
    glVertexAttribDivisor(3, 1);

    glEnableVertexAttribArray(4); // inColor
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, color));
    glVertexAttribDivisor(4, 1);

    glEnableVertexAttribArray(5); // inCornerRadius
    glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, corner_radius));
    glVertexAttribDivisor(5, 1);

    glEnableVertexAttribArray(6); // inEdgeSoftness
    glVertexAttribPointer(6, 1, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, edge_softness));
    glVertexAttribDivisor(6, 1);

    glEnableVertexAttribArray(7); // inBorderThickness
    glVertexAttribPointer(7, 1, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, border_thickness));
    glVertexAttribDivisor(7, 1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return vao;
}

void platform_init(void) {
    // init the library, here we make a window so we only need the Video capabilities.
    if (SDL_Init(SDL_INIT_VIDEO)) {
        exit(1);
    }
    #ifdef __EMSCRIPTEN__
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    #elif defined(OS_WINDOWS)
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);

        // SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
        // SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
        // SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
        // SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
        // SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
        // SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        // SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 8);
        // SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, 1);
    #else
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    #endif
    
    window = SDL_CreateWindow("sample",SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    glcontext = SDL_GL_CreateContext(window);
    if (!glcontext) {
        SDL_Log("Error creating GL context! exiting");
        exit(1);
    }
    SDL_GL_MakeCurrent(window,glcontext);

    #if (OS_WINDOWS)
        glewInit();
        assert(GLEW_ARB_ES3_compatibility);
        glEnable(GL_FRAMEBUFFER_SRGB);
    #endif

    {
        int majorv, minorv, profilem;
        SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &majorv);
        SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &minorv);
        SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, &profilem);
        SDL_Log("OpenGL ES [%s] [%d.%d]\n", (SDL_GL_CONTEXT_PROFILE_ES == profilem) ? "true" : "false", majorv, minorv);
    }

    SDL_ShowWindow(window);
    {
        int width, height;//, bbwidth, bbheight;
        SDL_GetWindowSize(window, &width, &height);
        //SDL_GetWindowSizeInPixels(window, &bbwidth, &bbheight);
        SDL_Log("Window size: %ix%i", width, height);
        //SDL_Log("Backbuffer size: %ix%i", bbwidth, bbheight);
        // if (width != bbwidth){
        //     SDL_Log("this is a highdpi environment.");
        // }
    }
    SDL_Log("Application started successfully!");
    vbo = create_instance_vbo(NULL, 0);
    fill_instance_vbo(vbo, NULL, 0);
    SDL_Log("vbo OK!");
    vao = setup_instance_vao(vbo);
    SDL_Log("vao OK!");
    sp = create_sp(gui_vs, gui_fs);
    SDL_Log("Shader program OK!");
    f32 *font_atlas_data = (f32*)ALLOC(1024*1024*sizeof(u8));
    font_atlas_data[0] = 0xFF;
    atlas_tex = create_atlas_tex_and_sampler((GLubyte*)font_atlas_data, 1024, 1024,&atlas_sampler);
    //SDL_Log("Atlas texture OK!");
    glDisable(GL_BLEND);
    FREE(font_atlas_data);
}

vec2 platform_get_windim(void) {
    int ww,wh;
    SDL_GetWindowSize(window, &ww, &wh);
    return v2(ww,wh);
}

void platform_update() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                exit(1);
            case SDL_MOUSEMOTION:
                break;
            case SDL_MOUSEBUTTONDOWN:
                break;
            case SDL_MOUSEBUTTONUP:
                break;
            default:
                break;
        }
    }
    vec2 windim = platform_get_windim();
    //glViewport(0,0,windim.x,windim.y);
}

void platform_swap(){
    SDL_GL_SwapWindow(window);
}
void platform_render(InstanceData *data, u32 instance_count)
{
    //printf("instance count: %d\n", command_count);
    //glClearColor(1.f, 1.f, 1.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);
    fill_instance_vbo(vbo, data, instance_count);
    glUseProgram(sp);
    glBindVertexArray(vao);
    glUniform2f(glGetUniformLocation(sp, "winDim"), platform_get_windim().x, platform_get_windim().y);
    glBindTexture(GL_TEXTURE_2D, atlas_tex);
    //glBindSampler(0, atlas_sampler);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, instance_count);
    glBindTexture(GL_TEXTURE_2D, 0);
    //glBindSampler(0, 0);
    glBindVertexArray(0);
    platform_swap();
}


void platform_deinit() {}