#include "base/base_inc.h"
#include "gfx/gfx_inc.h"
#include "core/global_state.h"

void nbatch2d_rend_begin(nBatch2DRenderer *rend, nWindow *win) {
    rend->win_ref = win;
    rend->first = rend->last = 0;
    rend->current_bound_image_ref = 0;

    // initialize the Shader Program if its not already
    if (!rend->sp.impl_state) {
        ogl_sp_init(&rend->sp, batch_vert, batch_frag);
        ogl_sp_add_attrib(&rend->sp, ogl_make_attrib(0,OGL_SHADER_DATA_TYPE_VEC2,sizeof(nBatch2DVertex),offsetof(nBatch2DVertex, pos),0));
        ogl_sp_add_attrib(&rend->sp, ogl_make_attrib(1,OGL_SHADER_DATA_TYPE_VEC2,sizeof(nBatch2DVertex),offsetof(nBatch2DVertex, tc),0));
        ogl_sp_add_attrib(&rend->sp, ogl_make_attrib(2,OGL_SHADER_DATA_TYPE_VEC4,sizeof(nBatch2DVertex),offsetof(nBatch2DVertex, col),0));
    }

}

void nbatch2d_rend_flush(nBatch2DRenderer *rend) {
    // alloc the vertex array
    u32 quad_count = nbatch2d_rend_count_quads(rend);
    u32 vertex_count = quad_count * 6;
    nBatch2DVertex *vertices = push_array_nz(get_frame_arena(), nBatch2DVertex, vertex_count);

    // fill the vertex array
    u32 quad_index = 0;
    for (nBatch2DQuadNode *node= rend->first; node != 0; node = node ->next) {
        nBatch2DQuad *quad = &node->quad;
        vertices[quad_index * 6 + 0] = (nBatch2DVertex){v2(quad->pos.x, quad->pos.y),v2(quad->tc.x, quad->tc.y),quad->color};
        vertices[quad_index * 6 + 1] = (nBatch2DVertex){v2(quad->pos.x + quad->dim.x, quad->pos.y),v2(quad->tc.x + quad->tc.z, quad->tc.y),quad->color};
        vertices[quad_index * 6 + 2] = (nBatch2DVertex){v2(quad->pos.x + quad->dim.x, quad->pos.y + quad->dim.y),v2(quad->tc.x + quad->tc.z, quad->tc.y + quad->tc.w),quad->color};
        vertices[quad_index * 6 + 3] = (nBatch2DVertex){v2(quad->pos.x + quad->dim.x, quad->pos.y + quad->dim.y),v2(quad->tc.x + quad->tc.z, quad->tc.y + quad->tc.w),quad->color};
        vertices[quad_index * 6 + 4] = (nBatch2DVertex){v2(quad->pos.x, quad->pos.y + quad->dim.y),v2(quad->tc.x, quad->tc.y + quad->tc.w),quad->color};
        vertices[quad_index * 6 + 5] = (nBatch2DVertex){v2(quad->pos.x, quad->pos.y),v2(quad->tc.x, quad->tc.y),quad->color};
        quad_index += 1;
    }

    // make an immediate vertex buffer
    oglBuf vbo = ogl_buf_make(OGL_BUF_KIND_VERTEX, vertices, vertex_count, sizeof(nBatch2DVertex));

    // render using the batch shader
    ogl_bind_vertex_buffer(&vbo);
    ogl_bind_sp(&rend->sp);

    // FIXME -- this is ULTRA hacky//////////
    ////////////////////////////////////////

    ogl_bind_image_to_texture_slot(rend->current_bound_image_ref, 0, 0);
    vec2 dim = nwindow_get_dim(rend->win_ref);
    ogl_set_viewport(0,0,dim.x,dim.y);
    vec2 camera = v2(0,0);
    mat4 view = mat4_translate(v3(-camera.x,-camera.y,0));
    ogl_sp_set_uniform(&rend->sp, "view", OGL_SHADER_DATA_TYPE_MAT4, &view);
    mat4 proj = mat4_ortho(0,dim.x,0,dim.y,-1.0,1.0);
    ogl_sp_set_uniform(&rend->sp, "proj", OGL_SHADER_DATA_TYPE_MAT4, &proj);

    ogl_draw(OGL_PRIM_TRIANGLES, 0, vertex_count); 

    // delete resources
    ogl_buf_deinit(&vbo);
    // reset the renderer
    rend->first = rend->last = 0;
}


void nbatch2d_rend_end(nBatch2DRenderer *rend) {
    nbatch2d_rend_flush(rend);
}

u32 nbatch2d_rend_count_quads(nBatch2DRenderer *rend) {
    u32 count = 0;
    for (nBatch2DQuadNode *quad = rend->first; quad != 0; quad=quad->next) {
        count += 1;
    }
    return count;
}

void nbatch2d_rend_add_quad(nBatch2DRenderer *rend, nBatch2DQuad quad, oglImage *tex) {
    // TODO -- there should be some safety for when the current bound image is destroyed?? currently we should crash :(
    if (rend->current_bound_image_ref != NULL && tex->impl_state != rend->current_bound_image_ref->impl_state) {
        nbatch2d_rend_flush(rend);
    }
    nBatch2DQuadNode *node = push_array(get_frame_arena(),nBatch2DQuadNode, 1);
    node->quad = quad;
    sll_queue_push(rend->first, rend->last, node);
    rend->current_bound_image_ref = tex;
}
