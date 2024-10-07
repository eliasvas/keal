#ifndef OGL_H
#define OGL_H
#include "base/base_inc.h"

/*
    ogl: this is a simple abstraction over OpenGLES3 that makes the API much more sane, no more global state.
    The library contains some primitive types for rendering. Namely:

    oglSP - shader program: you use fragment+vertex shaders to create a shader program
    oglShaderAttrib - shader attrib description: describe whats inside your vertex buffer 
    oglShaderBindings - shader state: describes all the 'options' for rendering (sets them just before rendering)
    oglBuf - gpu buffer: your index/vertex/uniform buffer
    oglImage - texture/fbo: any type of image, can be just a texture or a whole framebuffer!
    oglPrimitive - rendering primitive: the primitive you wanna use to draw your drawcall
*/


// for the bug with multiple pointers from framebuffer attachments and stuff https://stackoverflow.com/questions/15089703/how-to-get-the-textures-attached-to-a-framebuffer

//#define NO_SDL_GLEXT
#include <SDL2/SDL_opengl.h>
#include "ext/GL/glext.h"

#define OGL_CTX_MAX_ATTRIBS 16
#define OGL_CTX_MAX_TEX_SLOTS 4

typedef struct oglContext oglContext;
struct oglContext {
    GLuint vao;
    b32 initialized;
};
b32 ogl_ctx_init(oglContext *ctx);
b32 ogl_ctx_deinit(oglContext *ctx);
void ogl_set_viewport(f32 x, f32 y, f32 w, f32 h);
void ogl_set_scissor(f32 x, f32 y, f32 w, f32 h);
// This can be called by the user between draw_arrays, to mitigate global state stuff a bit
void ogl_clear_all_state(oglContext *ctx);



typedef enum oglBufKind oglBufKind;
enum oglBufKind {
    OGL_BUF_KIND_VERTEX,
    OGL_BUF_KIND_INDEX,
    OGL_BUF_KIND_UBO,
};
typedef struct oglBuf oglBuf;
struct oglBuf {
    s64 count;
    s64 size; //in bytes
    oglBufKind kind;

    void *impl_state;
};
oglBuf ogl_buf_make(oglBufKind kind, void *data, u32 data_count, u32 data_size);
void ogl_buf_update(oglBuf *buf, void *data, u32 data_count, u32 data_size);
b32 ogl_buf_deinit(oglBuf *b);
void ogl_bind_index_buffer(oglBuf *b);
void ogl_bind_vertex_buffer(oglBuf *b);



typedef enum oglShaderDataType oglShaderDataType;
enum oglShaderDataType {
    OGL_SHADER_DATA_TYPE_FLOAT,
    OGL_SHADER_DATA_TYPE_INT,
    OGL_SHADER_DATA_TYPE_VEC2,
    OGL_SHADER_DATA_TYPE_VEC3,
    OGL_SHADER_DATA_TYPE_VEC4,
    OGL_SHADER_DATA_TYPE_MAT4,
};
typedef struct oglShaderAttrib oglShaderAttrib;
struct oglShaderAttrib {
    u32 vbo_idx;
    u32 stride;
    u32 offset;
    oglShaderDataType type;
    b32 instanced; // whether the attribute is instanced
};
typedef struct oglSP oglSP;
struct oglSP {
    // TODO -- maybe attribs need to be managed somehow (dynamic array)
    oglShaderAttrib attribs[OGL_CTX_MAX_ATTRIBS];
    u32 attrib_count;

    void *impl_state; // the shader program
};
oglShaderAttrib ogl_make_attrib(u32 vbo_idx, oglShaderDataType type, u32 stride, u32 offset, b32 instanced);
void ogl_sp_add_attrib(oglSP *shader, oglShaderAttrib attrib);
b32 ogl_sp_init(oglSP *shader, const char *vs_source, const char *fs_source);
b32 ogl_sp_deinit(oglSP *shader);
void ogl_sp_set_uniform(oglSP *sp, const char *uniform_name, oglShaderDataType type, void *val);
void ogl_bind_sp(oglSP *sp);


typedef enum oglPrimitive oglPrimitive;
enum oglPrimitive {
    OGL_PRIM_TRIANGLES,
    OGL_PRIM_TRIANGLE_STRIP,
    OGL_PRIM_TRIANGLE_FAN,
    OGL_PRIM_LINES,
    OGL_PRIM_LINE_STRIP,
};
void ogl_draw(oglPrimitive prim, u32 first, u32 count);
void ogl_draw_instanced(oglPrimitive prim, u32 first, s32 count, u32 instance_count);
void ogl_draw_indexed(oglPrimitive prim, u32 count);

typedef enum oglImageKind oglImageKind;
enum oglImageKind {
    OGL_IMAGE_KIND_TEXTURE,
    OGL_IMAGE_KIND_RT,
};

typedef enum oglImageFormat oglImageFormat;
enum oglImageFormat {
    OGL_IMAGE_FORMAT_R8U,
    OGL_IMAGE_FORMAT_RGBA8U,
    OGL_IMAGE_FORMAT_RGB8U,
    OGL_IMAGE_FORMAT_RGBA32F,
};

typedef struct oglImage oglImage;
struct oglImage {
    u32 width;
    u32 height;
    oglImageFormat format;
    oglImageKind kind;

    void *impl_state;
    //optional: only configured for RTs (RT_COL -> colors) (RT_DS -> rbo)
    GLuint attachments[4];
};

void ogl_rt_bind(oglImage *img);
void ogl_image_clear(oglImage *img);
b32 ogl_image_init(oglImage *img, u8 *tex_data, u32 tex_w, u32 tex_h, oglImageFormat fmt);
void ogl_image_deinit(oglImage *img);
void ogl_bind_image_to_texture_slot(oglImage *img, u32 tex_slot, u32 attachment);

void ogl_load_gl_functions(void *(*load_func)());

#endif