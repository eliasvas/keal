#ifndef GAME_STATE_H
#define GAME_STATE_H
#include "engine.h"
#include "actor.h"
#include "map.h"
#include "game_gui.h"

typedef enum GameStatus GameStatus;
enum GameStatus {
    GAME_STATUS_STARTUP,
    GAME_STATUS_START_MENU,
    GAME_STATUS_IDLE,
    GAME_STATUS_NEW_TURN,
    GAME_STATUS_VICTORY,
    GAME_STATUS_DEFEAT,
};

typedef struct GameState GameState;
struct GameState {
    GameStatus status;

    nBatch2DRenderer batch_rend;
    oglImage atlas;
    oglImage white;

    nEntityManager em;
    nTransformCM tcm;
    nActorCM acm;
    nMap map;

    f32 zoom_amount;
    f32 animation_speed;
};

void game_state_status_set(GameStatus status);
b32  game_state_status_match(GameStatus status);

void game_state_init();
void game_state_deinit();
void game_state_update_and_render();
void game_state_generate_new_level();

GameState *get_ggs();

#endif