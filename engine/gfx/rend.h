#ifndef REND_H
#define REND_H
#include "base/base_inc.h"
#include "ogl.h"
// TODO -- do we NEED to provide the window? Right now we need it for the viewport / projection matrix
// TODO -- can we add a model matrix for encoding rotations maybe? something?
#include "core/window.h"

typedef struct nBatch2DVertex nBatch2DVertex;
struct nBatch2DVertex {
    vec2 pos;
    vec2 tc;
    vec4 col;
};

typedef struct nBatch2DQuad nBatch2DQuad;
struct nBatch2DQuad {
    vec2 pos; // in pixels
    vec2 dim; // in pixels
    vec4 tc; // in pixels
    vec4 color;
    f32 angle_rad;
};


typedef struct nBatch2DQuadNode nBatch2DQuadNode;
struct nBatch2DQuadNode {
    nBatch2DQuadNode *next;
    nBatch2DQuad quad;
};

typedef struct nBatch2DRenderer nBatch2DRenderer;
struct nBatch2DRenderer {
    nBatch2DQuadNode *first;
    nBatch2DQuadNode *last;

    mat4 view;

    oglSP sp;
    nWindow *win_ref;
    oglTex *current_bound_image_ref;
};

void nbatch2d_rend_begin(nBatch2DRenderer *rend, nWindow *win);
void nbatch2d_rend_add_quad(nBatch2DRenderer *rend, nBatch2DQuad quad, oglTex *tex);
void nbatch2d_rend_end(nBatch2DRenderer *rend);
void nbatch2d_rend_set_view_mat(nBatch2DRenderer *rend, mat4 view);

#endif