#include "dungeon_cam.h"
#include "core/core_inc.h"
#include "tileset4922.inl"

void ndungeon_cam_update(nDungeonCam *cam, vec2 player_pos) {
    cam->pos = player_pos;
    if (cam->shake_sec > 0.0) {
        vec2 rand_in_unit_circle = vec2_norm(v2(gen_rand01()*2-1,gen_rand01()*2-1));
        cam->pos = vec2_add(cam->pos, vec2_multf(rand_in_unit_circle, cam->shake_amount));
    }
    cam->shake_sec = maximum(cam->shake_sec - nglobal_state_get_dt_sec(), 0.0);
    // if (player_pos.x - cam->pos.x > cam->max_off.x) {cam->pos.x +=0.5;}
    // if (player_pos.x + cam->max_off.x < cam->pos.x) cam->pos.x -=0.5;
    // if (player_pos.y - cam->pos.y > cam->max_off.y) cam->pos.y +=0.5;
    // if (player_pos.y + cam->max_off.y < cam->pos.y) cam->pos.y -=0.5;
}
void ndungeon_cam_set(nDungeonCam *cam, vec2 pos, vec2 max_off, f32 zoom) {
    cam->pos = pos;
    cam->max_off = max_off;
    cam->zoom = zoom;
}

void ndungeon_cam_start_shake(nDungeonCam *cam, f32 shake_amount, f32 shake_sec) {
    cam->shake_amount = shake_amount;
    cam->shake_sec = shake_sec;
}

mat4 ndungeon_cam_get_view_mat(nDungeonCam *cam) {
    vec2 win_hs = v2(get_nwin()->ww/2.0, get_nwin()->wh/2.0);
    mat4 view = mat4_translate(v3(-cam->pos.x, -cam->pos.y, 0));
    view = mat4_mult(mat4_scale(v3(cam->zoom, cam->zoom, 1)), view);
    view = mat4_mult(mat4_translate(v3(win_hs.x, win_hs.y, 0)), view);
    return view;
}

vec2 ndungeon_cam_screen_to_world(nDungeonCam *cam, vec2 sc) {
    vec4 m = mat4_multv(mat4_inv(ndungeon_cam_get_view_mat(cam)), v4(sc.x,sc.y,0,1));
    return v2(m.x,m.y);
}
