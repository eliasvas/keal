#include "dungeon_cam.h"
#include "core/core_inc.h"
#include "tileset4922.inl"

void ndungeon_cam_update(nDungeonCam *cam, vec2 player_pos) {
    if (player_pos.x - cam->pos.x > cam->max_off.x) cam->pos.x +=1;
    if (player_pos.x + cam->max_off.x < cam->pos.x) cam->pos.x -=1;
    if (player_pos.y - cam->pos.y > cam->max_off.y) cam->pos.y +=1;
    if (player_pos.y + cam->max_off.y < cam->pos.y) cam->pos.y -=1;
}
void ndungeon_cam_set(nDungeonCam *cam, vec2 pos, vec2 max_off) {
    cam->pos = pos;
    cam->max_off = max_off;
}
mat4 ndungeon_cam_get_view_mat(nDungeonCam *cam) {
    ivec2 dist_to_view_pos = iv2(-(cam->pos.x *TILESET_DEFAULT_SIZE*cam->zoom - get_nwin()->ww/2.0+TILESET_DEFAULT_SIZE*cam->zoom/2.0), -(cam->pos.y *TILESET_DEFAULT_SIZE*cam->zoom - get_nwin()->wh/2+TILESET_DEFAULT_SIZE*cam->zoom/2));
    //mat4 view = mat4_mult(mat4_translate(v3(dist_to_mp.x,dist_to_mp.y,0)),mat4_scale(v3(gs.zoom_amount, gs.zoom_amount, 1)));
    return mat4_mult(mat4_translate(v3(dist_to_view_pos.x, dist_to_view_pos.y,0)),mat4_scale(v3(cam->zoom, cam->zoom, 1)));
}