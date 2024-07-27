#include <stdio.h>
#define STB_IMAGE_IMPLEMENTATION
#include "engine.h"
#include "tileset4922.inl"

oglImage game_load_rgb_image_from_disk(const char *path) {
    oglImage img;
    s32 w,h,comp;
    stbi_set_flip_vertically_on_load(1);
    unsigned char* image = stbi_load(path, &w, &h, &comp, STBI_rgb);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
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
    u32 white = 0xFFFF;
    oglImage img_white = game_load_rgb_image_from_disk("assets/tileset4922.png");
    ogl_image_init(&img_white, &((u8)white), 1, 1, OGL_IMAGE_FORMAT_RGB8U, 1);

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
        nBatch2DQuad q = {0};
        q.color = v4(1,1,1,1);
        q.pos.x = 0;
        q.pos.y = 0;
        q.dim.x = 32;
        q.dim.y = 32;
        q.tc = TILESET_COW_SKULL_TILE;
        nbatch2d_rend_add_quad(&batch_rend, q, &img);
        nBatch2DQuad q2 = q;
        q2.pos.x = 32;
        q2.pos.y = 0;
        q2.color = v4(1,0.2,0.2,1);
        nbatch2d_rend_add_quad(&batch_rend, q2, &img_white);
        nBatch2DQuad q3 = q;
        q3.pos.x = 64;
        q3.pos.y = 0;
        q3.color = v4(1,0.0,0.0,1);
        q3.tc = TILESET_SKULL_TILE;
        nbatch2d_rend_add_quad(&batch_rend, q3, &img);


        nbatch2d_rend_end(&batch_rend);
        /////////////////////

        nwindow_swap(&win);
        engine_global_state_frame_end();
    }
    nwindow_deinit(&win);
}

