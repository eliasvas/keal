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

typedef struct nBatch2DQuadNode nBatch2DQuadNode;
struct nBatch2DQuadNode {
    nBatch2DQuadNode *next;
    vec2 pos; // in pixels
    vec2 dim; // in pixels
    vec4 tc; // in pixels
    vec4 color;
};

typedef struct nBatch2DRenderer nBatch2DRenderer;
struct nBatch2DRenderer {
    nBatch2DQuadNode *first;
    nBatch2DQuadNode *last;

    oglSP sp;
    nWindow *win_ref;
    u64 current_tex_id;
};

void nbatch2d_rend_begin(nBatch2DRenderer *rend, nWindow *win);
void nbatch2d_rend_end(nBatch2DRenderer *rend);
void nbatch2d_rend_flush(nBatch2DRenderer *rend);
// TODO -- maybe in the API we expose a Quad type and make the QuadNode internally!
void nbatch2d_rend_add_quad(nBatch2DRenderer *rend, nBatch2DQuadNode quad, oglImage *tex);

#endif