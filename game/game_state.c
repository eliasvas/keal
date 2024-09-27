#define STB_IMAGE_IMPLEMENTATION
#include "game_state.h"
#include "map.h"
#include "engine.h"
#include "tileset4922.inl"

static GameState gs = {0};

GameState *get_ggs() {
    return &gs;
}

#define COLLIDER_VISUALIZATION 1

void update_and_render_sprites(nEntityMgr *em) {
    nbatch2d_rend_begin(&gs.batch_rend, get_nwin());
    mat4 view = ndungeon_cam_get_view_mat(&gs.dcam);
    nbatch2d_rend_set_view_mat(&gs.batch_rend, view);
    for (s32 i = em->comp_array_len-1; i >= 0; i-=1) {
        nEntityID entity = (u32)i;

        // simulate movement input stuff only for player
        if (NENTITY_MANAGER_HAS_COMPONENT(em, entity, nPhysicsBody)) {
            nPhysicsBody *b = NENTITY_MANAGER_GET_COMPONENT(em, entity, nPhysicsBody);
            f32 dt = nglobal_state_get_dt()/1000.0;
            if (*(NENTITY_MANAGER_GET_COMPONENT(em, entity, nEntityTag)) == NENTITY_TAG_PLAYER) {
                b32 face_right = 1;
                b32 vmov= 0;
                if (ninput_key_down(get_nim(),NKEY_SCANCODE_RIGHT)) {
                    b->velocity.x+=50*dt; // speed * dt
                    face_right = 1;
                    vmov=1;
                }
                if (ninput_key_down(get_nim(),NKEY_SCANCODE_LEFT)) {
                    b->velocity.x-=50*dt; // speed * dt
                    face_right = 0;
                    vmov=1;
                }
                if (ninput_key_down(get_nim(),NKEY_SCANCODE_UP)) {
                    b->velocity.y-=50*dt; // speed * dt
                }
                if (ninput_key_down(get_nim(),NKEY_SCANCODE_DOWN)) {
                    b->velocity.y+=50*dt; // speed * dt
                }
                if (vmov && NENTITY_MANAGER_HAS_COMPONENT(em, entity, nSprite)) {
                    nSprite *s = NENTITY_MANAGER_GET_COMPONENT(em, entity, nSprite);
                    s->vflip = !face_right;
                    //s->hflip = (direction.y == 1);
                }
            }
        }

        // render the stuff
        if (NENTITY_MANAGER_HAS_COMPONENT(em, entity, nSprite) && NENTITY_MANAGER_HAS_COMPONENT(em, entity, nPhysicsBody)) {
            nSprite *s = NENTITY_MANAGER_GET_COMPONENT(em, entity, nSprite);
            nPhysicsBody *b = NENTITY_MANAGER_GET_COMPONENT(em, entity, nPhysicsBody);
            nsprite_update(s, nglobal_state_get_dt() / 1000.0);
            vec4 tc = nsprite_get_current_tc(s);
            nBatch2DQuad q = {0};
            q.color = s->color;
            // we convert our collider to a bounding box (which is used for rendering dimensions)
            q.tc = tc;
            vec2 sprite_dim = (b->c_kind == NCOLLIDER_KIND_CIRCLE) ? v2(b->radius, b->radius) : b->half_dim;
            sprite_dim = vec2_multf(sprite_dim, 2);
            q.pos.x = b->position.x - sprite_dim.x/2.0;
            q.pos.y = b->position.y - sprite_dim.y/2.0;
            q.dim.x = sprite_dim.x;
            q.dim.y = sprite_dim.y;
            q.angle_rad = 0;
            nbatch2d_rend_add_quad(&gs.batch_rend, q, &gs.atlas);
        }

#if COLLIDER_VISUALIZATION
        // render the stuff
        if (NENTITY_MANAGER_HAS_COMPONENT(em, entity, nSprite) && NENTITY_MANAGER_HAS_COMPONENT(em, entity, nPhysicsBody)) {
            if (((nPhysicsBody*)NENTITY_MANAGER_GET_COMPONENT(em, entity, nPhysicsBody))->collider_off)continue;
            nSprite *s = NENTITY_MANAGER_GET_COMPONENT(em, entity, nSprite);
            nPhysicsBody *b = NENTITY_MANAGER_GET_COMPONENT(em, entity, nPhysicsBody);
            nsprite_update(s, nglobal_state_get_dt() / 1000.0);
            vec4 tc = nsprite_get_current_tc(s);
            nBatch2DQuad q = {0};
            q.color = v4(1,0,0,0.3);
            q.tc = (b->c_kind == NCOLLIDER_KIND_CIRCLE) ? TILESET_CIRCLE_TILE : TILESET_SOLID_TILE;
            vec2 sprite_dim = (b->c_kind == NCOLLIDER_KIND_CIRCLE) ? v2(b->radius, b->radius) : b->half_dim;
            sprite_dim = vec2_multf(sprite_dim, 2);
            q.pos.x = b->position.x - sprite_dim.x/2.0;
            q.pos.y = b->position.y - sprite_dim.y/2.0;
            q.dim.x = sprite_dim.x;
            q.dim.y = sprite_dim.y;
            q.angle_rad = 0;
            nbatch2d_rend_add_quad(&gs.batch_rend, q, &gs.atlas);
        }
#endif

    }
    nbatch2d_rend_end(&gs.batch_rend);
}

oglImage game_load_rgba_image_from_disk(const char *path) {
    oglImage img;
    s32 w,h,comp;
    stbi_set_flip_vertically_on_load(1);
    unsigned char* image = stbi_load(path, &w, &h, &comp, STBI_rgb_alpha);
    if(stbi_failure_reason()) {
        NLOG_ERR("Failed reading image: %s\n", stbi_failure_reason());
    }
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    assert(ogl_image_init(&img, image, w, h, OGL_IMAGE_FORMAT_RGBA8U));
    stbi_image_free(image);
    return img;
}


void game_state_init_images() {
    gs.atlas = game_load_rgba_image_from_disk("assets/tileset4922.png");
    u32 white = 0xFFFF;
    ogl_image_init(&gs.white, (u8*)(&white), 1, 1, OGL_IMAGE_FORMAT_R8U);
}

extern void nphysics_world_update_func(nEntityMgr *em);

void game_state_init() {
    game_state_status_set(GAME_STATUS_STARTUP);
    game_state_init_images();

    NENTITY_MANAGER_INIT(get_em());
    NENTITY_MANAGER_COMPONENT_REGISTER(get_em(), nEntityTag);
    NENTITY_MANAGER_COMPONENT_REGISTER(get_em(), nPhysicsBody);
    NENTITY_MANAGER_COMPONENT_REGISTER(get_em(), nSprite);
    NENTITY_MANAGER_ADD_SYSTEM(get_em(), nphysics_world_update_func, 1);
    NENTITY_MANAGER_ADD_SYSTEM(get_em(), update_and_render_sprites, 2);

    game_state_status_set(GAME_STATUS_START_MENU);
}

void game_state_deinit() {
    // TBA
}

void game_state_update_and_render() {
    vec2 screen_coords =  ndungeon_cam_screen_to_world(&gs.dcam, ninput_get_mouse_pos(get_nim()));
    do_game_gui();
    if (ninput_key_pressed(get_nim(), NKEY_SCANCODE_ESCAPE)) {game_state_status_set(GAME_STATUS_START_MENU);}

    if (game_state_status_match(GAME_STATUS_RUNNING)) {
        nPhysicsBody *b = NENTITY_MANAGER_GET_COMPONENT(get_em(), gs.player, nPhysicsBody);
        vec2 player_pos = v2(b->position.x, b->position.y);
        // NLOG_ERR("camera coords: %f %f", gs.dcam.pos.x, gs.dcam.pos.y);
        // NLOG_ERR("world coords: %f %f", screen_coords.x, screen_coords.y);
        //NLOG_ERR("player pos : %f %f", player_pos.x, player_pos.y);


        nScrollAmount scroll_y = ninput_get_scroll_amount_delta(get_nim());
        if (scroll_y) { gs.dcam.zoom += signof(scroll_y) * 4; }
        ndungeon_cam_update(&gs.dcam, v2(player_pos.x, player_pos.y));
        // ----
        nem_update(get_em());
    }

    if (ninput_key_pressed(get_nim(), NKEY_SCANCODE_SPACE)) {
        game_state_status_set(game_state_status_match(GAME_STATUS_PAUSED) ? GAME_STATUS_RUNNING : GAME_STATUS_PAUSED);
    }

    // // If game is running, we can simulate!
    // if (game_state_status_match(GAME_STATUS_RUNNING)) {
    //     nactor_cm_simulate(&(gs.acm), &(gs.map), game_state_status_match(GAME_STATUS_RUNNING));
    // }
    // // If game paused, do editor stuff (TODO -- we should have some form of tween-animation for this transition)
    // if (game_state_status_match(GAME_STATUS_PAUSED)) {
    //     vec2 mp = ninput_get_mouse_pos(get_nim());
    //     mp = ndungeon_screen_to_world(&gs.dcam, mp);
    //     ivec2 tile = iv2(floor(mp.x), floor(mp.y));
    //     nTile *t = nmap_tile_ref(&gs.map, tile.x, tile.y);
    //     if (t) {
    //         if (gui_input_mb_down(GUI_LMB)) {
    //             nmap_dig_region(&gs.map, tile.x, tile.y, tile.x+1, tile.y+1, NTILE_KIND_WALL);
    //         }
    //         if (gui_input_mb_down(GUI_RMB)) {
    //             nmap_dig_region(&gs.map, tile.x, tile.y, tile.x+1, tile.y+1, NTILE_KIND_GROUND);
    //         }
    //     }
    // }
}

void game_state_status_set(GameStatus status) {
    gs.status = status;
}

b32  game_state_status_match(GameStatus status) {
    return (gs.status == status);
}

void game_state_generate_new_level() {
    NENTITY_MANAGER_CLEAR(get_em());
    ndungeon_cam_set(&gs.dcam, v2(0,0), v2(10,10), 50);
    // init player entity
    gs.player = nem_make(get_em());
    NENTITY_MANAGER_ADD_COMPONENT(get_em(), gs.player, nPhysicsBody);
    NENTITY_MANAGER_ADD_COMPONENT(get_em(), gs.player, nSprite);
    NENTITY_MANAGER_ADD_COMPONENT(get_em(), gs.player, nEntityTag); // Maybe tag should be instantiated in nem_make(em)
    *NENTITY_MANAGER_GET_COMPONENT(get_em(), gs.player, nSprite) = nsprite_make(TILESET_ANIM_PLAYER_TILE, 5, 2, v4(1,0.3,0.3,1));
    nPhysicsBody *b = NENTITY_MANAGER_GET_COMPONENT(get_em(), gs.player, nPhysicsBody);
    //*b = nphysics_body_aabb(v2(10,10), 200*gen_rand01());
    *b = nphysics_body_circle(0.5, 20);
    b->position = v2(0,0);
    b->gravity_scale = 0;
    nEntityTag *player_tag = NENTITY_MANAGER_GET_COMPONENT(get_em(), gs.player, nEntityTag);
    *player_tag = NENTITY_TAG_PLAYER;

    // generate a NEW map
    nMap map = {0};
    nmap_create(&map, 32, 32);
}