#ifndef GAME_STATE_H
#define GAME_STATE_H
#include "engine.h"
#include "dungeon_cam.h"

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

    // resources
    nBatch2DRenderer batch_rend;
    oglTex atlas;
    oglTex white;
    nDungeonCam dcam;

    // scene transition stuff
    f32 fade_timer;
    vec4 fade_color;
    oglSP fade_sp;
    oglBuf full_vbo;

    // gameplay options (persistent)
    ivec2 panel_dim; // for all menus, TODO -- we should make this configurable to screen width/height?
    b32 music_enabled;
    b32 effects_enabled;
    b32 fullscreen_enabled;
    b32 endless_mode_enabled;
    b32 hud_enabled;
    b32 col_vis;

    // cached player entity
    nEntityID player;
};

void game_state_status_set(GameState *gs, GameStatus status);
b32  game_state_status_match(GameState *gs, GameStatus status);

void game_state_init(GameState *gs);
void game_state_deinit(GameState *gs);
void game_state_update_and_render(GameState *gs);
void game_state_generate_new_level(GameState *gs);

GameState *get_ggs();

#endif