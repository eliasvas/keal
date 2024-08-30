#ifndef NDUNGEON_CAM_H
#define NDUNGEON_CAM_H
#include "base/base_inc.h"

// TODO -- think about how we'll do this for 3D

typedef struct nDungeonCam nDungeonCam;
struct nDungeonCam {
    vec2 pos;
    vec2 max_off;
    f32 zoom;
};

void ndungeon_cam_update(nDungeonCam *cam, vec2 player_pos);
void ndungeon_cam_set(nDungeonCam *cam, vec2 pos, vec2 max_off);
mat4 ndungeon_cam_get_view_mat(nDungeonCam *cam);

#endif