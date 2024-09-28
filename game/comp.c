#include "comp.h"
#include "game_state.h"
#include "tileset4922.inl"

extern GameState gs;
// TODO -- COLLIDER_VISUALIZATION should probably be an option in GameState
//#define COLLIDER_VISUALIZATION 1

nHealthComponent nhealth_component_make(s32 max_health) {
    nHealthComponent n = {
        .max_hlt = max_health,
        .hlt = max_health,
    };
    return n;
}

b32 nhealth_component_alive(nHealthComponent *hc) {
    return (hc->hlt > 0);
}

nAIComponent nai_component_default(void) {
    nAIComponent ai = {
        .state = 0,
        .timestamp= get_current_timestamp_sec(),
        .dead = 0,
    };
    return ai;
}

void ai_component_enemy_update(nEntityMgr *em, nEntityID enemy) {
    nEntityID player = gs.player;
    nPhysicsBody *b_player = NENTITY_MANAGER_GET_COMPONENT(em, player, nPhysicsBody);
    nPhysicsBody *b_enemy = NENTITY_MANAGER_GET_COMPONENT(em, enemy, nPhysicsBody);
    vec2 dist = vec2_sub(b_player->position, b_enemy->position);
    f32 enemy_speed = 20;
    f32 dt = nglobal_state_get_dt()/1000.0;
    // TODO -- maybe this 5 should be part of nAIComponent or some enemy logic
    if (vec2_len(dist) < 5) {
        dist = vec2_norm(dist);
        b_enemy->velocity = vec2_add(vec2_multf(dist, enemy_speed*dt), b_enemy->velocity);
    }
}

void ai_component_player_update(nEntityMgr *em, nEntityID player) {
    nPhysicsBody *b = NENTITY_MANAGER_GET_COMPONENT(em, player, nPhysicsBody);
    nSprite *s = NENTITY_MANAGER_GET_COMPONENT(em, player, nSprite);
    f32 dt = nglobal_state_get_dt()/1000.0;
    f32 player_speed = 50.0;
    b32 face_right = 1;
    b32 hmov = 0;
    if (ninput_key_down(get_nim(),NKEY_SCANCODE_RIGHT)) { b->velocity.x+=player_speed*dt; face_right = 1; hmov=1; }
    if (ninput_key_down(get_nim(),NKEY_SCANCODE_LEFT))  { b->velocity.x-=player_speed*dt; face_right = 0; hmov=1; }
    if (ninput_key_down(get_nim(),NKEY_SCANCODE_UP))    { b->velocity.y-=player_speed*dt; }
    if (ninput_key_down(get_nim(),NKEY_SCANCODE_DOWN))  { b->velocity.y+=player_speed*dt; }
    if (hmov) { s->vflip = !face_right; }
    if (equalf(b->velocity.x,0.0,1.0)){
        s->repeat=0;
    } else {
        if (b->inv_mass != 0) {
            s->repeat=1;
        }
    }
}

void game_ai_system(nEntityMgr *em) {
    for (s64 i = 0; i < em->comp_array_len; i+=1) {
        nEntityID entity = NENTITY_MANAGER_GET_ENTITY_FOR_INDEX(get_em(), i);
        nEntityTag tag = *(NENTITY_MANAGER_GET_COMPONENT(em, entity, nEntityTag));
        switch (tag) {
            case NENTITY_TAG_PLAYER: 
                ai_component_player_update(em, entity);
                break;
            case NENTITY_TAG_ENEMY: 
                ai_component_enemy_update(em, entity);
                break;
            default:
                break;
        } 
    }
}


void render_sprites_system(nEntityMgr *em) {
    nbatch2d_rend_begin(&gs.batch_rend, get_nwin());
    mat4 view = ndungeon_cam_get_view_mat(&gs.dcam);
    nbatch2d_rend_set_view_mat(&gs.batch_rend, view);
    for (s64 i = em->comp_array_len-1; i >= 0; i-=1) {
        nEntityID entity = NENTITY_MANAGER_GET_ENTITY_FOR_INDEX(get_em(), i);

        // render the sprites with physics body
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

void resolve_collision_events(nEntityMgr *em) {
    for (nEntityEventNode *en = em->event_mgr.first; en != 0; en = en->next) {
        nEntityTag tag_a = *(NENTITY_MANAGER_GET_COMPONENT(em, en->e.entity_a, nEntityTag));
        nEntityTag tag_b = *(NENTITY_MANAGER_GET_COMPONENT(em, en->e.entity_b, nEntityTag));
        if (tag_a == NENTITY_TAG_PLAYER && tag_b == NENTITY_TAG_ENEMY) {
            // set sprite so that death animation will play
            nSprite *s = NENTITY_MANAGER_GET_COMPONENT(get_em(), en->e.entity_a, nSprite);
            s->frame_count = 6;
            s->repeat = 0;
            // make collider static
            nPhysicsBody *b = NENTITY_MANAGER_GET_COMPONENT(get_em(), en->e.entity_a, nPhysicsBody);
            b->mass = F32_MAX;
            b->inv_mass = 0;
            b->velocity = v2(0,0);
        }
        if (tag_b == NENTITY_TAG_PLAYER && tag_a == NENTITY_TAG_ENEMY) {
            // set sprite so that death animation will play
            nSprite *s = NENTITY_MANAGER_GET_COMPONENT(get_em(), en->e.entity_b, nSprite);
            s->frame_count = 6;
            s->repeat = 0;
            // make collider static
            nPhysicsBody *b = NENTITY_MANAGER_GET_COMPONENT(get_em(), en->e.entity_b, nPhysicsBody);
            b->mass = F32_MAX;
            b->inv_mass = 0;
        }
    }
}
