#ifndef GAME_STATE_H
#define GAME_STATE_H
#include "engine.h"
#include "actor.h"
#include "map.h"

typedef enum GameStatus GameStatus;
enum GameStatus {
    GAME_STATUS_STARTUP,
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
};


void game_state_init();
void game_state_deinit();
void game_state_update_and_render();

GameState *get_ggs();

#endif