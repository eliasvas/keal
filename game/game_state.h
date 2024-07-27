#ifndef GAME_STATE_H
#define GAME_STATE_H
#include "engine.h"

typedef struct GameState GameState;
struct GameState {
    nBatch2DRenderer batch_rend;
    oglImage atlas;
    oglImage white;
    nWindow *win_ref;
};

void game_state_init();
void game_state_update_and_render();
GameState *get_game_state();


#endif