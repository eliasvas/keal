#define STB_IMAGE_IMPLEMENTATION
#include "game_state.h"
#include "engine.h"
#include "tileset4922.inl"

static GameState gs = {0};

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

GameState *get_game_state() {
    return &gs;
}


void game_state_init_images() {
    get_game_state()->atlas = game_load_rgb_image_from_disk("assets/tileset4922.png");
    u32 white = 0xFFFF;
    ogl_image_init(&get_game_state()->white, &((u8)white), 1, 1, OGL_IMAGE_FORMAT_RGB8U, 1);
}

void game_state_init(nWindow *win) {
    game_state_init_images();
    get_game_state()->win_ref = win;
    M_ZERO_STRUCT(&get_game_state()->batch_rend);
}


void game_state_update_and_render() {
    nbatch2d_rend_begin(&get_game_state()->batch_rend, get_game_state()->win_ref);
    nBatch2DQuad q = {0};
    q.color = v4(1,1,1,1);
    q.pos.x = 0;
    q.pos.y = 0;
    q.dim.x = 32;
    q.dim.y = 32;
    q.tc = TILESET_COW_SKULL_TILE;
    nbatch2d_rend_add_quad(&get_game_state()->batch_rend, q, &get_game_state()->atlas);
    nBatch2DQuad q2 = q;
    q2.pos.x = 32;
    q2.pos.y = 0;
    q2.color = v4(1,0.2,0.2,1);
    nbatch2d_rend_add_quad(&get_game_state()->batch_rend, q2, &get_game_state()->white);
    nBatch2DQuad q3 = q;
    q3.pos.x = 64;
    q3.pos.y = 0;
    q3.color = v4(1,0.0,0.0,1);
    q3.tc = TILESET_SKULL_TILE;
    nbatch2d_rend_add_quad(&get_game_state()->batch_rend, q3, &get_game_state()->atlas);


    nbatch2d_rend_end(&get_game_state()->batch_rend);

}