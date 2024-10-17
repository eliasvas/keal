#include "comp.h"
#include "game_state.h"
#include "tileset4922.inl"

void nmap_spawn_kealotine(vec2 pos, vec2 dir) {
    nEntityID enemy = nem_make(get_em());
    NENTITY_MANAGER_ADD_COMPONENT(get_em(), enemy, nPhysicsBody);
    NENTITY_MANAGER_ADD_COMPONENT(get_em(), enemy, nSprite);
    NENTITY_MANAGER_ADD_COMPONENT(get_em(), enemy, nEntityTag); // Maybe tag should be instantiated in nem_make(em)
    *NENTITY_MANAGER_GET_COMPONENT(get_em(), enemy, nSprite) = nsprite_make(TILESET_KEALOTINE_OPEN_TILE, 0, 0, v4(0.7,0.3,0.4,1));
    nPhysicsBody *b = NENTITY_MANAGER_GET_COMPONENT(get_em(), enemy, nPhysicsBody);
    *b = nphysics_body_circle(0.5, 20);
    b->position = pos;
    b->gravity_scale = 0;
    b->layer = 5;
    b->mask = 0b101;
    b->velocity = vec2_multf(dir, 30);
    nEntityTag *tag = NENTITY_MANAGER_GET_COMPONENT(get_em(), enemy, nEntityTag);
    *tag = NENTITY_TAG_KEALOTINE;
}

// TODO -- COLLIDER_VISUALIZATION should probably be an option in GameState
//#define COLLIDER_VISUALIZATION 1

nHealthComponent nhealth_component_make(s32 max_health) {
    nHealthComponent n = {
        .max_hlt = max_health,
        .hlt = max_health,
    };
    return n;
}

void nhealth_component_enc(nHealthComponent *hc) {
    hc->hlt = minimum(hc->hlt+1, hc->max_hlt);
}

void nhealth_component_dec(nHealthComponent *hc) {
    hc->hlt = maximum(hc->hlt-1, 0);
}

b32 nhealth_component_alive(nHealthComponent *hc) {
    return (hc->hlt > 0);
}

nAIComponent nai_component_default(void) {
    nAIComponent ai = {
        .state = 0,
        .timestamp= get_current_timestamp_sec(),
        .invinsibility_sec = 0.0,
        .dead = 0,
        .won = 0,
    };
    return ai;
}

void ai_component_enemy_update(nEntityMgr *em, GameState *gs, nEntityID enemy) {
    nEntityID player = gs->player;
    nPhysicsBody *b_player = NENTITY_MANAGER_GET_COMPONENT(em, player, nPhysicsBody);
    nPhysicsBody *b_enemy = NENTITY_MANAGER_GET_COMPONENT(em, enemy, nPhysicsBody);
    nAIComponent *enemy_ai= NENTITY_MANAGER_GET_COMPONENT(em, enemy, nAIComponent);
    if (enemy_ai->dead)return;
    vec2 dist = vec2_sub(b_player->position, b_enemy->position);
    f32 enemy_speed = 20;
    f32 dt = nglobal_state_get_dt_sec();
    // TODO -- maybe this 5 should be part of nAIComponent or some enemy logic
    if (vec2_len(dist) < 5) {
        dist = vec2_norm(dist);
        b_enemy->velocity = vec2_add(vec2_multf(dist, enemy_speed*dt), b_enemy->velocity);
    }
}

void ai_component_player_update(nEntityMgr *em, nEntityID player, GameState *gs) {
    nPhysicsBody *b = NENTITY_MANAGER_GET_COMPONENT(em, player, nPhysicsBody);
    nSprite *s = NENTITY_MANAGER_GET_COMPONENT(em, player, nSprite);
    nAIComponent *ai = NENTITY_MANAGER_GET_COMPONENT(get_em(), player, nAIComponent);
    vec2 keal_dir;
    vec2 mp = ninput_get_mouse_pos(get_nim());
    mp = ndungeon_cam_screen_to_world(&gs->dcam, mp);
    mp = vec2_sub(mp, b->position);
    keal_dir = vec2_norm(mp);
    if (!ai->dead && ninput_mkey_down(get_nim(),NKEY_LMB)) {nmap_spawn_kealotine(b->position, keal_dir);}
    f32 dt = nglobal_state_get_dt_sec();
    f32 player_speed = 50.0;
    b32 face_right = 1;
    b32 hmov = 0;
    if (!ai->dead && ninput_key_down(get_nim(),NKEY_SCANCODE_D)) { b->velocity.x+=player_speed*dt; face_right = 1; hmov=1; }
    if (!ai->dead && ninput_key_down(get_nim(),NKEY_SCANCODE_A))  { b->velocity.x-=player_speed*dt; face_right = 0; hmov=1; }
    if (!ai->dead && ninput_key_down(get_nim(),NKEY_SCANCODE_W))    { b->velocity.y-=player_speed*dt; }
    if (!ai->dead && ninput_key_down(get_nim(),NKEY_SCANCODE_S))  { b->velocity.y+=player_speed*dt; }
    if (hmov) { s->vflip = !face_right; }
    if (equalf(b->velocity.x,0.0,0.05) && equalf(b->velocity.y,0.0,0.05)){
        s->repeat=0;
    } else {
        if (b->inv_mass != 0) {
            s->repeat=1;
        }
    }
    if (ai->invinsibility_sec == 0.0 && ninput_key_pressed(get_nim(), NKEY_SCANCODE_SPACE)) {
        //b->velocity = vec2_add(b->velocity, vec2_multf(vec2_norm(b->velocity), 10*player_speed*dt));
        vec2 dash_vec = v2(0.0,0.0);
        if (!ai->dead && ninput_key_down(get_nim(),NKEY_SCANCODE_D)) { dash_vec.x += 1;}
        if (!ai->dead && ninput_key_down(get_nim(),NKEY_SCANCODE_A))  { dash_vec.x -= 1;}
        if (!ai->dead && ninput_key_down(get_nim(),NKEY_SCANCODE_W))    { dash_vec.y -= 1;}
        if (!ai->dead && ninput_key_down(get_nim(),NKEY_SCANCODE_S))  { dash_vec.y += 1;}
        if (!(dash_vec.x == 0.0f && dash_vec.y == 0.0f)){
            dash_vec = vec2_norm(dash_vec);
            ai->dash_sec = 0.5;
            b->velocity = vec2_add(b->velocity, vec2_multf(dash_vec, player_speed*dt*20));
        }
    }
    ai->invinsibility_sec = maximum(0.0, ai->invinsibility_sec - dt);
    ai->dash_sec = maximum(0.0, ai->dash_sec - dt);
}

void render_dir_arrow(GameState *gs) {
    nPhysicsBody *b = NENTITY_MANAGER_GET_COMPONENT(get_em(), gs->player, nPhysicsBody);
    nBatch2DQuad q = {0};
    vec2 mp = ninput_get_mouse_pos(get_nim());
    mp = ndungeon_cam_screen_to_world(&gs->dcam, mp);
    mp = vec2_sub(mp, b->position);
    mp = vec2_norm(mp);
    f32 dist = 0.5;
    f32 x_off = dist * mp.x;
    f32 y_off = dist * mp.y;
    q.color = v4(1,1,1,0.5);
    q.pos = vec2_add(vec2_sub(b->position, v2(0.25,0.0)), v2(x_off,y_off));
    vec2 sprite_dim = (b->c_kind == NCOLLIDER_KIND_CIRCLE) ? v2(b->radius, b->radius) : b->hdim;
    q.dim = sprite_dim;
    q.tc = TILESET_RARROW_TILE;
    q.angle_rad = atan2(y_off, x_off);
    nbatch2d_rend_add_quad(&gs->batch_rend, q, &gs->atlas);
}
void game_ai_system(nEntityMgr *em, void *ctx) {
    GameState *gs = (GameState*)ctx;
    // do default game AI

    // branch of to logic for certain AI kinds (player/enemy/projectile)
    for (s64 i = 0; i < em->comp_array_len; i+=1) {
        nEntityID entity = NENTITY_MANAGER_GET_ENTITY_FOR_INDEX(get_em(), i);
        nEntityTag tag = *(NENTITY_MANAGER_GET_COMPONENT(em, entity, nEntityTag));
        switch (tag) {
            case NENTITY_TAG_PLAYER: 
                ai_component_player_update(em, entity, gs);
                break;
            case NENTITY_TAG_ENEMY: 
                ai_component_enemy_update(em, gs, entity);
                break;
            // case NENTITY_TAG_PROJECTILE: 
            // case NENTITY_TAG_DOOR: 
            default:
                break;
        } 
    }
}


void render_sprites_system(nEntityMgr *em, void *ctx) {
    GameState *gs = (GameState*)ctx;

    nbatch2d_rend_begin(&gs->batch_rend, get_nwin());
    mat4 view = ndungeon_cam_get_view_mat(&gs->dcam);
    nbatch2d_rend_set_view_mat(&gs->batch_rend, view);
    for (u32 layer = 0; layer <= 5; ++layer) {
        for (s64 i = em->comp_array_len; i>=0; i-=1) {
            nEntityID entity = NENTITY_MANAGER_GET_ENTITY_FOR_INDEX(get_em(), i);
            // render the sprites with physics body
            if (NENTITY_MANAGER_HAS_COMPONENT(em, entity, nSprite) && NENTITY_MANAGER_HAS_COMPONENT(em, entity, nPhysicsBody)) {
                nSprite *s = NENTITY_MANAGER_GET_COMPONENT(em, entity, nSprite);
                nPhysicsBody *b = NENTITY_MANAGER_GET_COMPONENT(em, entity, nPhysicsBody);
                if (b->layer != layer)continue;
                nAIComponent *ai = NENTITY_MANAGER_GET_COMPONENT(em, entity, nAIComponent);
                nsprite_update(s, nglobal_state_get_dt_sec());
                vec4 tc = nsprite_get_current_tc(s);
                nBatch2DQuad q = {0};
                q.color = s->color;
                if (ai && ai->invinsibility_sec > 0.0) {
                    q.color = vec4_multf(q.color, 0.7 +0.3 * (sin(10*get_current_timestamp_sec())+1/2));
                    b->mask |= (0b100);
                } else if (ai && ai->dash_sec > 0.0) {
                    q.color = vec4_multf(q.color, 0.7);
                    b->mask &= ~(0b100);
                } else {
                    b->mask |= (0b100);
                }
                // we convert our collider to a bounding box (which is used for rendering dimensions)
                q.tc = tc;
                vec2 sprite_dim = (b->c_kind == NCOLLIDER_KIND_CIRCLE) ? v2(b->radius, b->radius) : b->hdim;
                sprite_dim = vec2_multf(sprite_dim, 2);
                q.pos.x = b->position.x - sprite_dim.x/2.0;
                q.pos.y = b->position.y - sprite_dim.y/2.0;
                q.dim.x = sprite_dim.x;
                q.dim.y = sprite_dim.y;
                q.angle_rad = 0;
                nbatch2d_rend_add_quad(&gs->batch_rend, q, &gs->atlas);
                // if entity has health, draw that as well
                if (NENTITY_MANAGER_HAS_COMPONENT(em,entity, nHealthComponent)) {
                    nHealthComponent *h = NENTITY_MANAGER_GET_COMPONENT(em, entity, nHealthComponent);
                    u32 heart_count = h->hlt;
                    f32 heart_sprite_dim = (sprite_dim.x/2.0);
                    f32 total_width = heart_sprite_dim * heart_count;
                    f32 start_y = q.pos.y - heart_sprite_dim;
                    f32 start_x = b->position.x - total_width/2.0;
                    for (u32 heart = 0; heart < heart_count; heart+=1) {
                        nBatch2DQuad q = {0};
                        q.color = s->color;
                        q.tc = TILESET_HEART_TILE;
                        q.pos.x = start_x + heart*heart_sprite_dim;
                        q.pos.y = start_y;
                        q.dim.x = heart_sprite_dim;
                        q.dim.y = heart_sprite_dim;
                        q.angle_rad = 0;
                        nbatch2d_rend_add_quad(&gs->batch_rend, q, &gs->atlas);
                    }
                }
            }

            if (gs->col_vis) {
                    // render the stuff
                    if (NENTITY_MANAGER_HAS_COMPONENT(em, entity, nSprite) && NENTITY_MANAGER_HAS_COMPONENT(em, entity, nPhysicsBody)) {
                        if (((nPhysicsBody*)NENTITY_MANAGER_GET_COMPONENT(em, entity, nPhysicsBody))->layer == 0)continue;
                        nSprite *s = NENTITY_MANAGER_GET_COMPONENT(em, entity, nSprite);
                        nPhysicsBody *b = NENTITY_MANAGER_GET_COMPONENT(em, entity, nPhysicsBody);
                        nsprite_update(s, nglobal_state_get_dt_sec());
                        vec4 tc = nsprite_get_current_tc(s);
                        nBatch2DQuad q = {0};
                        q.color = v4(1,0,0,0.3);
                        q.tc = (b->c_kind == NCOLLIDER_KIND_CIRCLE) ? TILESET_CIRCLE_TILE : TILESET_SOLID_TILE;
                        vec2 sprite_dim = (b->c_kind == NCOLLIDER_KIND_CIRCLE) ? v2(b->radius, b->radius) : b->hdim;
                        sprite_dim = vec2_multf(sprite_dim, 2);
                        q.pos.x = b->position.x - sprite_dim.x/2.0;
                        q.pos.y = b->position.y - sprite_dim.y/2.0;
                        q.dim.x = sprite_dim.x;
                        q.dim.y = sprite_dim.y;
                        q.angle_rad = 0;
                        nbatch2d_rend_add_quad(&gs->batch_rend, q, &gs->atlas);
                    }
            }
        }
    }
    nAIComponent *ai = NENTITY_MANAGER_GET_COMPONENT(get_em(), gs->player, nAIComponent);
    if (!ai->won) {
        render_dir_arrow(gs);
    }

    nbatch2d_rend_end(&gs->batch_rend);
}

void resolve_collision_events(nEntityMgr *em, void *ctx) {
    GameState *gs = (GameState*)ctx;

    for (nEntityEventNode *en = em->event_mgr.first; en != 0; en = en->next) {
        nEntityTag tag_a = *(NENTITY_MANAGER_GET_COMPONENT(em, en->e.entity_a, nEntityTag));
        nEntityTag tag_b = *(NENTITY_MANAGER_GET_COMPONENT(em, en->e.entity_b, nEntityTag));
        // player collides with enemy
        if (tag_a == NENTITY_TAG_PLAYER && tag_b == NENTITY_TAG_ENEMY) {
            nHealthComponent *h = NENTITY_MANAGER_GET_COMPONENT(get_em(), en->e.entity_a, nHealthComponent);
            nAIComponent *ai = NENTITY_MANAGER_GET_COMPONENT(get_em(), en->e.entity_a, nAIComponent);
            if (ai->invinsibility_sec == 0.0) {
                ai->invinsibility_sec = 1.0;
                nhealth_component_dec(h);
                if (!nhealth_component_alive(h)) {
                    // set sprite so that death animation will play
                    nSprite *s = NENTITY_MANAGER_GET_COMPONENT(get_em(), en->e.entity_a, nSprite);
                    s->frame_count = 6;
                    s->repeat = 0;
                    // make collider static
                    nPhysicsBody *b = NENTITY_MANAGER_GET_COMPONENT(get_em(), en->e.entity_a, nPhysicsBody);
                    b->mass = F32_MAX;
                    b->inv_mass = 0;
                    b->velocity = v2(0,0);
                    b->force= v2(0,0);
                    ai->dead = 1;
                }
            }
        }
        // player collides with enemy
        if (tag_a == NENTITY_TAG_PLAYER && tag_b == NENTITY_TAG_DOOR) {
            nSprite *s = NENTITY_MANAGER_GET_COMPONENT(get_em(), en->e.entity_a, nSprite);
            nAIComponent *ai = NENTITY_MANAGER_GET_COMPONENT(get_em(), en->e.entity_a, nAIComponent);
            nPhysicsBody *b = NENTITY_MANAGER_GET_COMPONENT(get_em(), en->e.entity_a, nPhysicsBody);
            b->mass = F32_MAX;
            b->inv_mass = 0;
            b->velocity = v2(0,0);
            b->force= v2(0,0);
            b->layer= 0;
            // win
            ai->won = 1;
            ai->dead = 1;
            s->color = v4(0.0,0.0,0.0,0.0);
        }
        if (tag_a == NENTITY_TAG_ENEMY && tag_b == NENTITY_TAG_KEALOTINE) {
            nSprite *s = NENTITY_MANAGER_GET_COMPONENT(get_em(), en->e.entity_a, nSprite);
            nAIComponent *ai = NENTITY_MANAGER_GET_COMPONENT(get_em(), en->e.entity_a, nAIComponent);
            nPhysicsBody *b = NENTITY_MANAGER_GET_COMPONENT(get_em(), en->e.entity_a, nPhysicsBody);
            b->mass = F32_MAX;
            b->inv_mass = 0;
            b->velocity = v2(0,0);
            b->force= v2(0,0);
            b->layer= 0;
            ai->dead = 1;
            s->start_tc = TILESET_SKULL_TILE;
        }
    }
}

void fade_system(nEntityMgr *em, void *ctx) {
    GameState *gs = (GameState*)ctx;
    // do Fade if needed
    nAIComponent *ai = NENTITY_MANAGER_GET_COMPONENT(get_em(), gs->player, nAIComponent);
    nSprite *s = NENTITY_MANAGER_GET_COMPONENT(get_em(), gs->player, nSprite);
    f32 dt = nglobal_state_get_dt_sec();
    gs->fade_timer = (ai->dead || ai->won) ? minimum(gs->fade_timer+dt, 1.0) : maximum(gs->fade_timer-dt, 0.0);
    vec4 color = vec4_multf(gs->fade_color, gs->fade_timer);
    ogl_clear_all_state();
    ogl_rt_bind(0);
    ogl_bind_vertex_buffer(&gs->full_vbo);
    ogl_bind_sp(&gs->fade_sp);
    ogl_sp_set_uniform(&gs->fade_sp, "color", OGL_SHADER_DATA_TYPE_VEC4, &color);
    ogl_set_viewport(0,0,get_nwin()->ww, get_nwin()->wh);
    ogl_sp_set_dyn_state(&gs->fade_sp, OGL_BLEND_STATE);
    ogl_draw(OGL_PRIM_TRIANGLE_FAN, 0, 4);
    if (gs->fade_timer == 1.0) {
        game_state_generate_new_level(gs);
        game_state_status_set(gs, GAME_STATUS_RUNNING);
    }
}

