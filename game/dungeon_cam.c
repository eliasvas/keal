#include "dungeon_cam.h"
#include "core/core_inc.h"
#include "tileset4922.inl"

void ndungeon_cam_update(nDungeonCam *cam, vec2 player_pos) {
    if (player_pos.x - cam->pos.x > cam->max_off.x) cam->pos.x +=0.1;
    if (player_pos.x + cam->max_off.x < cam->pos.x) cam->pos.x -=0.1;
    if (player_pos.y - cam->pos.y > cam->max_off.y) cam->pos.y +=0.1;
    if (player_pos.y + cam->max_off.y < cam->pos.y) cam->pos.y -=0.1;
}
void ndungeon_cam_set(nDungeonCam *cam, vec2 pos, vec2 max_off, f32 zoom) {
    cam->pos = pos;
    cam->max_off = max_off;
    cam->zoom = zoom;
}
mat4 ndungeon_cam_get_view_mat(nDungeonCam *cam) {
    ivec2 dist_to_view_pos = iv2(-(cam->pos.x *TILESET_DEFAULT_SIZE*cam->zoom - get_nwin()->ww/2.0+TILESET_DEFAULT_SIZE*cam->zoom/2.0), -(cam->pos.y *TILESET_DEFAULT_SIZE*cam->zoom - get_nwin()->wh/2+TILESET_DEFAULT_SIZE*cam->zoom/2));
    return mat4_mult(mat4_translate(v3(dist_to_view_pos.x, dist_to_view_pos.y,0)),mat4_scale(v3(cam->zoom, cam->zoom, 1)));
}

// TODO -- maybe the inverse for the view matrix should be calculated and stored,
// one mat4 mult is slow enough already!!
vec2 ndungeon_cam_screen_to_world(nDungeonCam *cam, vec2 sc) {
    vec4 m = mat4_multv(mat4_inv(ndungeon_cam_get_view_mat(cam)), v4(sc.x,sc.y,0,1));
    return v2(m.x/TILESET_DEFAULT_SIZE,m.y/TILESET_DEFAULT_SIZE);
}
