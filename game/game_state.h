#ifndef GAME_STATE_H
#define GAME_STATE_H
#include "engine.h"
#include "dungeon_cam.h"
#include "game_gui.h"

typedef enum GameStatus GameStatus;
enum GameStatus {
    GAME_STATUS_STARTUP,
    GAME_STATUS_START_MENU,
    GAME_STATUS_OPTIONS_MENU,
    GAME_STATUS_PAUSED,
    GAME_STATUS_RUNNING,
};

typedef struct GameState GameState;
struct GameState {
    GameStatus status;

    nBatch2DRenderer batch_rend;
    oglImage atlas;
    oglImage white;

    nDungeonCam dcam;
    nEntityID player;
};

void game_state_status_set(GameStatus status);
b32  game_state_status_match(GameStatus status);

void game_state_init();
void game_state_deinit();
void game_state_update_and_render();
void game_state_generate_new_level();

GameState *get_ggs();

#endif