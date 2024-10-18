#ifndef NDUNGEON_CAM_H
#define NDUNGEON_CAM_H
#include "base/base_inc.h"

// TODO -- think about how we'll do this for 3D

typedef struct nDungeonCam nDungeonCam;
struct nDungeonCam {
    vec2 pos;
    vec2 max_off; // after this offset the camera is gonna follow
    f32 zoom;

    f32 shake_amount;
    f32 shake_sec;
};

void ndungeon_cam_update(nDungeonCam *cam, vec2 player_pos);
void ndungeon_cam_set(nDungeonCam *cam, vec2 pos, vec2 max_off, f32 zoom);
mat4 ndungeon_cam_get_view_mat(nDungeonCam *cam);
vec2 ndungeon_cam_screen_to_world(nDungeonCam *cam, vec2 sc);
void ndungeon_cam_start_shake(nDungeonCam *cam, f32 shake_amount, f32 shake_sec);

#endif