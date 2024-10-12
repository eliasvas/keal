#ifndef OGL_H
#define OGL_H
#include "base/base_inc.h"

/*
    ogl: this is a simple abstraction over OpenGLES3 that makes the API much more sane, no more global state.
    The library contains some primitive types for rendering. Namely:

    oglSP - shader program - you pass fragment+vertex shaders to create a shader program
    oglShaderAttrib - shader attrib description - describe whats inside your vertex buffer
    oglBuf - gpu buffer - your index/vertex/uniform buffer
    oglTex - regular texture - you provide a data pointer + dimensions + internal format
    oglRT - render target - contains textures as attachments (4xCOL + 1xDS)
    oglPrimitive - rendering primitive - the primitive you wanna use to draw your draw-call
*/

//#define NO_SDL_GLEXT
#include <SDL2/SDL_opengl.h>
#include "ext/GL/glext.h"

#define OGL_MAX_ATTRIBS 16
#define OGL_MAX_TEX_SLOTS 4

b32 ogl_init(void);
b32 ogl_deinit(void);
// This can be called by the user between draw_arrays, to mitigate global state stuff a bit
void ogl_clear_all_state();

void ogl_set_viewport(f32 x, f32 y, f32 w, f32 h);
void ogl_set_scissor(f32 x, f32 y, f32 w, f32 h);



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
b32 ogl_buf_init(oglBuf *buf, oglBufKind kind, void *data, u32 data_count, u32 data_size);
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
    oglShaderAttrib attribs[OGL_MAX_ATTRIBS];
    u32 attrib_count;

    void *impl_state;
};
oglShaderAttrib ogl_attrib_make(u32 vbo_idx, oglShaderDataType type, u32 stride, u32 offset, b32 instanced);
void ogl_sp_add_attrib(oglSP *shader, oglShaderAttrib attrib);
b32 ogl_sp_init(oglSP *shader, const char *vs_source, const char *fs_source);
b32 ogl_sp_deinit(oglSP *shader);
void ogl_sp_set_uniform(oglSP *sp, const char *uniform_name, oglShaderDataType type, void *val);
void ogl_bind_sp(oglSP *sp);


typedef enum oglDynState oglDynState;
enum oglDynState {
    OGL_DEPTH_STATE,
    OGL_CULL_STATE,
    OGL_STENCIL_STATE,
    OGL_BLEND_STATE,
};
// Todo -- this is kinda bullshit, we need finer grade control, like glBlendFunc(..) and stuff
void ogl_sp_set_dyn_state(oglSP *shader, oglDynState state);


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

typedef enum oglTexFormat oglTexFormat;
enum oglTexFormat {
    OGL_TEX_FORMAT_R8U,
    OGL_TEX_FORMAT_RGBA8U,
    OGL_TEX_FORMAT_RGB8U,
    OGL_TEX_FORMAT_RGBA32F,
};
typedef struct oglTex oglTex;
struct oglTex {
    vec2 dim;
    oglTexFormat format;
    void *impl_state;
};
b32 ogl_tex_init(oglTex *tex, vec2 dim, u8 *data, oglTexFormat format);
void ogl_tex_deinit(oglTex *tex);
void ogl_bind_tex_to_slot(oglTex *tex, u32 slot);

typedef enum oglRTAttachment oglRTAttachment;
enum oglRTAttachment {
    OGL_RT_ATTACHMENT_0,
    OGL_RT_ATTACHMENT_1,
    OGL_RT_ATTACHMENT_2,
    OGL_RT_ATTACHMENT_3,
    OGL_RT_ATTACHMENT_DS,
};
typedef struct oglRT oglRT;
struct oglRT {
    vec2 dim;
    oglTexFormat format; // for color attachments
    void *impl_state;
};
b32 ogl_rt_init(oglRT *rt, vec2 dim, oglTexFormat format);
void ogl_rt_bind(oglRT *rt);
oglTex ogl_rt_get_attachment(oglRT *rt, oglRTAttachment attachment);
void ogl_rt_clear(oglRT *rt);

void ogl_load_gl_functions(void *(*load_func)());

#endif