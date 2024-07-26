#include <stdio.h>
#define STB_IMAGE_IMPLEMENTATION
#include "engine.h"
#include "tileset4922.inl"

oglImage game_load_rgb_image_from_disk(const char *path) {
    oglImage img;
    s32 w,h,comp;
    stbi_set_flip_vertically_on_load(1);
    unsigned char* image = stbi_load(path, &w, &h, &comp, STBI_rgb);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    assert(ogl_image_init(&img, image, w, h, OGL_IMAGE_FORMAT_RGB8U, 0));
    stbi_image_free(image);
    return img;
}

int main(int argc, char **argv) {
    nWindow win = {0};
    nBatch2DRenderer batch_rend = {0};
    engine_global_state_init();
    nwindow_init(&win, "gudGame", 800, 600, N_WINDOW_OPT_RESIZABLE | N_WINDOW_OPT_BORDERLESS);
    engine_global_state_set_target_fps(60.0);
    oglContext ogl_ctx;
    ogl_ctx_init(&ogl_ctx);
    oglImage img = game_load_rgb_image_from_disk("assets/tileset4922.png");

    while(1) {
        ninput_manager_consume_events_from_window(&win);
        if (ninput_mkey_down(NKEY_MMB)){
            printf("MMB down!\n");
        }
        ogl_clear_all_state(&ogl_ctx);
        engine_global_state_frame_begin();
        ogl_image_clear(NULL);

        // Batch rendering ///
        nbatch2d_rend_begin(&batch_rend, &win);
        nBatch2DQuadNode node = {0};
        node.color = v4(1,1,1,1);
        node.pos.x = 0;
        node.pos.y = 0;
        node.dim.x = 100;
        node.dim.y = 100;
        node.tc = TILESET_BONES_TILE;
        nbatch2d_rend_add_quad(&batch_rend, node, &img);
        nBatch2DQuadNode node2 = node;
        node2.pos.x = 100;
        node2.pos.y = 0;
        node2.color = v4(1,0.2,0.2,1);
        nbatch2d_rend_add_quad(&batch_rend, node2, &img);
        nBatch2DQuadNode node3 = node;
        node3.pos.x = 200;
        node3.pos.y = 0;
        node3.color = v4(1,0.0,0.0,1);
        nbatch2d_rend_add_quad(&batch_rend, node3, &img);


        nbatch2d_rend_end(&batch_rend);
        /////////////////////

        nwindow_swap(&win);
        engine_global_state_frame_end();
    }
    nwindow_deinit(&win);
}

