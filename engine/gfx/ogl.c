#include "base/base_inc.h"
#include "ogl.h"
//TODO -- multiple vertex buffers not supported https://stackoverflow.com/questions/14249634/opengl-vaos-and-multiple-buffers

////////////////////////////////
// OGL context
////////////////////////////////

b32 ogl_ctx_init(oglContext *ctx) {
    glGenVertexArrays(1, &ctx->vao);
    ctx->initialized = 1;
    return (ctx->vao != 0);
}

b32 ogl_ctx_deinit(oglContext *ctx) {
    glDeleteVertexArrays(1, &ctx->vao);
    return 1;
}

void ogl_clear_all_state(oglContext *ctx) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindVertexArray(0);
    glBindVertexArray(ctx->vao);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    for(u32 i = 0; i < OGL_CTX_MAX_ATTRIBS; i+=1){
        glDisableVertexAttribArray(i);
    }
    for (u32 i = 0; i < OGL_CTX_MAX_TEX_SLOTS; i+=1) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    //...
    //...
    //...
    glBindVertexArray(ctx->vao);
    glEnable(GL_DEPTH_TEST);
    //glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
}



////////////////////////////////
// OGL buffers
////////////////////////////////


oglBuf ogl_buf_make(oglBufKind kind, void *data, u32 data_count, u32 data_size) {
    oglBuf buf;
    buf.kind = kind;
    buf.count = data_count;
    buf.size = data_size * data_count;

    GLuint buffer_kind = (kind == OGL_BUF_KIND_VERTEX) ? GL_ARRAY_BUFFER : GL_ELEMENT_ARRAY_BUFFER;

    glGenBuffers(1, &buf.handle);
    glBindBuffer(buffer_kind, buf.handle);
    ogl_buf_update(&buf, data,data_count,data_size);
    glBindBuffer(buffer_kind, 0);
    return buf;
}

b32 ogl_buf_deinit(oglBuf *b) {
    glDeleteBuffers(1, &b->handle);
    return 1;
}
void ogl_buf_update(oglBuf *buf, void *data, u32 data_count, u32 data_size) {
    ogl_bind_vertex_buffer(buf);
    buf->count = data_count;
    buf->size = data_size * data_count;
    // TODO -- do we need something here for UBO?
    GLuint buffer_kind = (buf->kind == OGL_BUF_KIND_VERTEX) ? GL_ARRAY_BUFFER : GL_ELEMENT_ARRAY_BUFFER;
    if (data) {
        glBufferData(buffer_kind, buf->size, data, GL_STREAM_DRAW); //TODO: make an arg for DYNAMIC/STATIC/STREAM?
    }
}

void ogl_bind_index_buffer(oglBuf *b) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, b->handle);
}

void ogl_bind_vertex_buffer(oglBuf *b) {
    glBindBuffer(GL_ARRAY_BUFFER, b->handle);
}



////////////////////////////////
// OGL shaders 
////////////////////////////////

b32 gl_check_gl_shader_link_errors(GLuint sp_handle) {
    s32 success;
    glGetProgramiv(sp_handle, GL_LINK_STATUS, &success);
    if (!success) {
        s32 info_len;
        glGetProgramiv (sp_handle, GL_INFO_LOG_LENGTH, &info_len);
        //info_data := cast (*u8) alloc (info_len);
        //defer free(info_data);
        u8 info_data[256] = {0};
        glGetProgramInfoLog (sp_handle, info_len, &info_len, info_data);
        printf("Shader link error: %s\n", info_data);
    }
    return (success == 0);
}

b32 gl_check_gl_shader_compile_errors(GLuint sp_handle) {
    s32 success;
    glGetProgramiv(sp_handle, GL_COMPILE_STATUS, &success);
    if (!success) {
        s32 info_len;
        glGetProgramiv (sp_handle, GL_INFO_LOG_LENGTH, &info_len);
        //info_data := cast (*u8) alloc (info_len);
        //defer free(info_data);
        u8 info_data[256] = {0};
        glGetProgramInfoLog (sp_handle, info_len, &info_len, info_data);
        printf("Shader compilation error: %s\n", info_data);
    }
    return (success == 0);
}


GLuint gl_make_sp(const char *vs_source, const char *fs_source) {
    GLuint sp = 0;

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vs_source, NULL);
    glCompileShader(vs);
    assert(vs);
    assert(0 == gl_check_gl_shader_compile_errors(vs));

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fs_source, NULL);
    glCompileShader(fs);
    assert(fs);
    assert(0 == gl_check_gl_shader_compile_errors(fs));

    sp = glCreateProgram();
    glAttachShader(sp, vs);
    glAttachShader(sp, fs);
    glLinkProgram(sp);
    assert(0 == gl_check_gl_shader_compile_errors(sp));

    return sp;
}

u32 ogl_get_component_num_for_data_type(oglShaderDataType type) {
    u32 comp_count = 0;
    switch(type) {
        case (OGL_SHADER_DATA_TYPE_FLOAT):
            comp_count = 1;
            break;
        case (OGL_SHADER_DATA_TYPE_INT):
            comp_count = 1;
            break;
        case (OGL_SHADER_DATA_TYPE_VEC2):
            comp_count = 2;
            break;
        case (OGL_SHADER_DATA_TYPE_VEC3):
            comp_count = 3;
            break;
        case (OGL_SHADER_DATA_TYPE_VEC4):
            comp_count = 4;
            break;
         case (OGL_SHADER_DATA_TYPE_MAT4):
            comp_count = 16;
            break;
        default:
            comp_count = 0;
            break;
    }
    return comp_count;
}

oglShaderAttrib ogl_make_attrib(u32 vbo_idx, oglShaderDataType type, u32 stride, u32 offset, b32 instanced) {
    oglShaderAttrib sa;
    sa.vbo_idx = vbo_idx;
    sa.stride = stride;
    sa.offset = offset;
    sa.type = type;
    sa.instanced = instanced;
    return sa;
}



void ogl_bind_attrib(oglShaderAttrib *attrib) {
    glEnableVertexAttribArray(attrib->vbo_idx);
    if (attrib->type == OGL_SHADER_DATA_TYPE_INT) {
        glVertexAttribIPointer(attrib->vbo_idx, ogl_get_component_num_for_data_type(attrib->type), GL_FLOAT, attrib->stride, (void*)((u64)attrib->offset));
    } else {
        glVertexAttribPointer(attrib->vbo_idx, ogl_get_component_num_for_data_type(attrib->type), GL_FLOAT, GL_FALSE, attrib->stride, (void*)((u64)attrib->offset));
    }
    glVertexAttribDivisor(attrib->vbo_idx, attrib->instanced);
}

b32 ogl_sp_init(oglSP *shader, const char *vs_source, const char *fs_source) {
    shader->sp = gl_make_sp(vs_source, fs_source);
    shader->attrib_count = 0;
    return (shader->sp != 0);
}

b32 ogl_sp_deinit(oglSP *shader) {
    glDeleteProgram(shader->sp);
    return 1;
}

void ogl_sp_add_attrib(oglSP *shader, oglShaderAttrib attrib) {
    shader->attribs[shader->attrib_count] = attrib;
    shader->attrib_count += 1;
    assert(shader->attrib_count < OGL_CTX_MAX_ATTRIBS);
}

void ogl_sp_bind_attribs(oglSP *sp){
    for (u32 i = 0; i < sp->attrib_count; i+=1) {
        ogl_bind_attrib(&sp->attribs[i]);
    }
}

void ogl_bind_sp(oglSP *sp){
    glUseProgram(sp->sp);
    ogl_sp_bind_attribs(sp);
}

void ogl_sp_set_uniform(oglSP *sp, const char *uniform_name, oglShaderDataType type, void * val){
    switch (type) {
        case(OGL_SHADER_DATA_TYPE_FLOAT):
            glUniform1fv(glGetUniformLocation(sp->sp, uniform_name), 1, (f32*)val);
            break;
        case(OGL_SHADER_DATA_TYPE_VEC2):
            glUniform2fv(glGetUniformLocation(sp->sp, uniform_name), 1, (f32*)val);
            break;
        case(OGL_SHADER_DATA_TYPE_VEC3):
            glUniform3fv(glGetUniformLocation(sp->sp, uniform_name), 1, (f32*)val);
            break;
        case(OGL_SHADER_DATA_TYPE_VEC4):
            glUniform4fv(glGetUniformLocation(sp->sp, uniform_name), 1, (f32*)val);
            break;
        case(OGL_SHADER_DATA_TYPE_MAT4):
            glUniformMatrix4fv(glGetUniformLocation(sp->sp, uniform_name), 1, GL_FALSE, (f32*)val);
            break;
        default:
            assert(0);
            break;
    }
}


////////////////////////////////
// OGL draw API
////////////////////////////////

GLuint ogl_prim_to_gl_prim(oglPrimitive prim) {
    GLuint gl_prim = GL_TRIANGLES;
    switch (prim) {
        case OGL_PRIM_TRIANGLES:
            gl_prim = GL_TRIANGLES;
            break;
        case OGL_PRIM_TRIANGLE_STRIP:
            gl_prim = GL_TRIANGLE_STRIP;
            break;
        case OGL_PRIM_TRIANGLE_FAN:
            gl_prim = GL_TRIANGLE_FAN;
            break;
        default:
            gl_prim = GL_TRIANGLES;
            break;
    }
    return gl_prim;
}

void ogl_draw(oglPrimitive prim, u32 first, u32 count) {
    glDrawArrays(ogl_prim_to_gl_prim(prim), first, count);
}

void ogl_draw_instanced(oglPrimitive prim, u32 first, s32 count, u32 instance_count) {
    glDrawArraysInstanced(ogl_prim_to_gl_prim(prim), first, count, instance_count);
}

void ogl_draw_indexed(oglPrimitive prim, u32 count) {
    glDrawElements(ogl_prim_to_gl_prim(prim), count, GL_UNSIGNED_INT, 0);
}
