#include "engine.h"
#include "actor.h"
#include "game_state.h"
#include "tileset4922.inl"

#define NACTOR_CM_MAX_COMPONENTS 1024

void nactor_cm_init(nActorCM *cm, nEntityManager *em) {
    M_ZERO_STRUCT(cm);
    cm->cap = NACTOR_CM_MAX_COMPONENTS;
    cm->size = 0;
    u32 size_of_data = sizeof(nActorComponent) + sizeof(nEntity);
    cm->em_ref = em;
    cm->buf = arena_push_nz(cm->em_ref->arena, size_of_data * cm->cap);

    cm->actors = cm->buf;
    cm->entity = (nEntity*)((u8*)cm->actors + (u64)(sizeof(nActorComponent)*cm->cap));

    cm->lookup_table_size = NACTOR_CM_MAX_COMPONENTS/2;
    cm->lookup_table = push_array(cm->em_ref->arena, nEntityComponentIndexPairHashSlot, cm->lookup_table_size);

}

void nactor_cm_deinit(nActorCM *cm, nEntityManager *em) {
    M_ZERO_STRUCT(cm);
}

nCompIndex nactor_cm_make_new_index(nActorCM *cm) {
    nCompIndex idx = cm->size;
    M_ZERO_STRUCT(&cm->actors[idx]);
    cm->size+=1;
    return idx;
}

nEntityComponentIndexPairNode *nactor_cm_lookup_node(nActorCM *cm, nEntity e) {
    nEntityComponentIndexPairNode *n = 0;
    u32 slot = e%cm->lookup_table_size;
    for (nEntityComponentIndexPairNode *node = cm->lookup_table[slot].hash_first; node != 0; node = node->next) {
        if (node->e == e) {
            n = node;
            break;
        }
    }
    return n;
}


nEntity nactor_cm_lookup_entity_from_ptr(nActorCM *cm, nActorComponent *cmp) {
    nCompIndex idx = (cmp - cm->actors) / sizeof(nActorComponent);
    return cm->entity[idx];
}

nCompIndex nactor_cm_lookup(nActorCM *cm, nEntity e) {
    nEntityComponentIndexPairNode *n = nactor_cm_lookup_node(cm, e);
    return (n) ? n->comp_idx : NCOMPONENT_INDEX_INVALID;
}

nActorComponent *nactor_cm_get(nActorCM *cm, nEntity e) {
    nCompIndex actor_idx = nactor_cm_lookup(cm, e);
    if (actor_idx == NCOMPONENT_INDEX_INVALID){
        return NULL;
    }

    return &cm->actors[actor_idx];
}
nActorComponent *nactor_cm_add(nActorCM *cm, nEntity e) {
    nCompIndex new_idx = nactor_cm_make_new_index(cm);
    cm->entity[new_idx] = e;
    M_ZERO_STRUCT(&cm->actors[new_idx]);
    cm->actors[new_idx].active = 1;

    // insert Pair in lookup table
    u32 slot = e%cm->lookup_table_size;
    nEntityComponentIndexPairNode *node = cm->free_nodes;
    if (node) {
        sll_stack_pop(cm->free_nodes);
    } else {
        node = push_array(cm->em_ref->arena, nEntityComponentIndexPairNode, 1);
    }
    node->e = e;
    node->comp_idx = new_idx;
    dll_push_back(cm->lookup_table[slot].hash_first, cm->lookup_table[slot].hash_last, node);

    return &(cm->actors[new_idx]);
}

// Here we should swap the nCompIndex's while ALSO keeping the links intact
void nactor_move_index(nActorCM *cm, nCompIndex src, nCompIndex dst) {
    cm->entity[dst] = cm->entity[src];
    cm->actors[dst] = cm->actors[src];
}

void nactor_swap_indices(nActorCM *cm, nCompIndex a, nCompIndex b) {
    nactor_move_index(cm, a, cm->size);
    nactor_move_index(cm, b, a);
    nactor_move_index(cm, cm->size, b);

    // Also swap lookup table entries
    nEntityComponentIndexPairNode * a_node = nactor_cm_lookup_node(cm, cm->entity[a]);
    nCompIndex ai = a_node->comp_idx;
    nEntityComponentIndexPairNode * b_node = nactor_cm_lookup_node(cm, cm->entity[b]);
    nEntity bi = b_node->comp_idx;
    a_node->comp_idx = bi;
    b_node->comp_idx = ai;
}


void nactor_cm_clear(nActorCM *cm) {
    M_ZERO(cm->entity, sizeof(nEntity)*cm->cap);
    M_ZERO(cm->actors, sizeof(nActorComponent)*cm->cap);
    cm->size = 0;

    cm->lookup_table_size = NACTOR_CM_MAX_COMPONENTS/2;
    cm->lookup_table = push_array(cm->em_ref->arena, nEntityComponentIndexPairHashSlot, cm->lookup_table_size);

    // Take all IndexPairs and put to free_node list for later use, then invalidate lookup table
    for (u32 slot = 0 ; slot < cm->lookup_table_size; slot+=1) {
        for (nEntityComponentIndexPairNode *node = cm->lookup_table[slot].hash_first; node != 0; node = node->next) {
            M_ZERO_STRUCT(node);
            sll_stack_push(cm->free_nodes, node);
        }
        cm->lookup_table[slot].hash_first = 0;
        cm->lookup_table[slot].hash_last= 0;
    }

}

void nactor_cm_del(nActorCM *cm, nEntity e) {
    nCompIndex idx = nactor_cm_lookup(cm, e);
    if (cm->size) {
        nactor_swap_indices(cm, idx, cm->size-1);
        cm->size-=1;

        // remove lookup table entry
        nEntityComponentIndexPairNode * e_node = nactor_cm_lookup_node(cm, e);
        u32 slot = e%cm->lookup_table_size;
        dll_remove(cm->lookup_table[slot].hash_first, cm->lookup_table[slot].hash_last, e_node);
        M_ZERO_STRUCT(e_node);
        sll_stack_push(cm->free_nodes, e_node);
    }
}
void nactor_cm_gc(nActorCM *cm) {
    NLOG_DBG("Performing actor CM GC with size: %d\n",cm->size);
    // First count how many actors we should remove
    u32 inactive_count = 0;
    for (u32 i = 0; i < cm->size; i+=1) {
        if (!cm->actors[i].active) {
            inactive_count += 1;
            NLOG_DBG("actor: [%s] inactive\n", cm->actors[i].name);
        }
    }
    if (inactive_count){
        // put inside an array all entitie's whose Actor components will be deleted
        nEntity *actors_to_be_deleted = push_array(get_frame_arena(), nEntity, inactive_count);
        u32 entity_count = 0;
        for (u32 i = 0; i < cm->size; i+=1) {
            if (!cm->actors[i].active) {
                actors_to_be_deleted[entity_count] = cm->entity[i];
                entity_count += 1;
            }
        }
        // finally delete them all 1-by-1
        for (u32 i = 0; i < entity_count; i+=1) {
            nactor_cm_del(cm, actors_to_be_deleted[i]);
        }
    }
    NLOG_DBG("Ending actor CM GC with size: %d\n",cm->size);
}

////////////////////////////////
// Gameplay Code
////////////////////////////////

b32 nactor_cm_check_movement_event(nActorCM *cm) {
    return (ninput_key_pressed(get_nim(),NKEY_SCANCODE_RIGHT) | ninput_key_pressed(get_nim(),NKEY_SCANCODE_LEFT) | ninput_key_pressed(get_nim(),NKEY_SCANCODE_UP) | ninput_key_pressed(get_nim(),NKEY_SCANCODE_DOWN));
}
void nactor_move_or_attack(nActorComponent *ac, nMap *map, ivec2 delta) {
    ivec2 new_pos = iv2(ac->posx + delta.x, ac->posy + delta.y);
    if (nmap_tile_is_walkable(map, new_pos.x, new_pos.y)) {
        ac->posx = new_pos.x;
        ac->posy = new_pos.y;
    }

    // Check to see whether in target position there is an enemy, in which case, we CUT
    for (u32 j = 0; j < get_ggs()->acm.size; j+=1) {
        nActorComponent *actor = &get_ggs()->acm.actors[j];
        if (actor->posx == new_pos.x && actor->posy == new_pos.y && actor->kind != ac->kind && (actor->flags & NACTOR_FEATURE_FLAG_DESTRUCTIBLE)) {
            nactor_attack(ac, actor);
        }
    }

    // Check to see whether in target position there is an item, in which case, we PICK UP
    for (u32 j = 0; j < get_ggs()->acm.size; j+=1) {
        nActorComponent *actor = &get_ggs()->acm.actors[j];
        if (actor->posx == new_pos.x && actor->posy == new_pos.y && actor->kind != ac->kind && (actor->flags & NACTOR_FEATURE_FLAG_PICKABLE) && (ac->flags & NACTOR_FEATURE_FLAG_HAS_CONTAINER)) {
            s32 container_index = nactor_pick_up_item(ac, actor);
            nactor_use_item(ac, container_index);
        }
    }


}

void nactor_update(nActorComponent *ac, nMap *map) {
    if ((ac->flags & NACTOR_FEATURE_FLAG_DESTRUCTIBLE) && ac->d.hp <= 0)return;
    ivec2 delta = iv2(0,0);
    nActorComponent *player_cmp;
    switch (ac->kind) {
        case NACTOR_KIND_PLAYER:
            if (ninput_key_pressed(get_nim(),NKEY_SCANCODE_RIGHT)) {
                delta.x+=1;
            }else if (ninput_key_pressed(get_nim(),NKEY_SCANCODE_LEFT)) {
                delta.x-=1;
            }else if (ninput_key_pressed(get_nim(),NKEY_SCANCODE_UP)) {
                delta.y-=1;
            }else if (ninput_key_pressed(get_nim(),NKEY_SCANCODE_DOWN)) {
                delta.y+=1;
            }
            nactor_move_or_attack(ac, map, delta);
            break;
        case NACTOR_KIND_ENEMY:
            if (gen_random(0,10) == 0) {
                if (gen_random(0,2)) {
                    delta.x = gen_random(1,4)-2;
                } else {
                    delta.y = gen_random(1,4)-2;
                }
            } else {
                player_cmp = nactor_cm_get(&(get_ggs()->acm), get_ggs()->map.player);
                ivec2 move_dir = iv2(player_cmp->posx - ac->posx, player_cmp->posy - ac->posy);
                if (player_cmp && abs(ivec2_len(move_dir)) < 5){
                    if (gen_random(0,2)) {
                        delta.x = signof(move_dir.x);
                    }else {
                        delta.y = signof(move_dir.y);
                    }

                }
            }
            nactor_move_or_attack(ac, map, delta);
            break;
        case NACTOR_KIND_DOOR:
            player_cmp = nactor_cm_get(&(get_ggs()->acm), get_ggs()->map.player);
            if (player_cmp && player_cmp->posx == ac->posx && player_cmp->posy == ac->posy){
                NLOG_DBG("NEXT LEVEL!\n");
                nactor_cm_clear(&(get_ggs()->acm));
                nmap_create(&(get_ggs()->map),64,64);
            }
            break;
        case NACTOR_KIND_ITEM:
            break;
        default:
            NLOG_DBG("Who is dis guy?!\n");
            break;
    }
}


void nactor_cm_simulate(nActorCM *cm, nMap *map, b32 RUNNING) {
    for (u32 i = 0; i < cm->size; i+=1) {
        cm->actors[i].d.shake_duration = maximum(0.0f, cm->actors[i].d.shake_duration - get_ngs()->dt/1000.0);
        if (!RUNNING)continue;
        if (!cm->actors[i].active)continue;
        nactor_update(&(cm->actors[i]), map);
    }
}

vec2 nactor_calc_shake_offset(nActorComponent *ac) {
    f32 off_x = (ac->d.shake_duration > 0.0) ? (gen_random(0,100) / 100.0f - 0.5f) * ac->d.shake_str : 0;
    f32 off_y = (ac->d.shake_duration > 0.0) ? (gen_random(0,100) / 100.0f - 0.5f) * ac->d.shake_str : 0;
    return v2(off_x, off_y);
}

void nactor_cm_render(nActorCM *cm, nBatch2DRenderer *rend, oglImage *atlas) {

    for (u32 i = 0; i < cm->size; i+=1) {
        nActorComponent *actor = &(cm->actors[i]);
        if (!actor->active)continue;
        vec2 shake_offset = nactor_calc_shake_offset(actor);
        nBatch2DQuad q = {
            .color = actor->color,
            .pos.x = (actor->posx+shake_offset.x)*TILESET_DEFAULT_SIZE,
            .pos.y = (actor->posy+shake_offset.y)*TILESET_DEFAULT_SIZE,
            .dim.x = TILESET_DEFAULT_SIZE,
            .dim.y = TILESET_DEFAULT_SIZE,
            .tc    = actor->tc,
            .angle_rad = 0,
        };
        nbatch2d_rend_add_quad(rend, q, atlas);
    }
}

// also shift this component to begining, so its drawn before HERO
void nactor_die(nActorComponent *ac) {
    NLOG_DBG("actor: %s died!\n", ac->name);
    ac->tc = TILESET_SKULL_TILE;
    ac->blocks = 0;
    if (strcmp(ac->name,"player") == 0) {
        NLOG_DBG("YOU DIED\n");
        nactor_cm_clear(&(get_ggs()->acm));
        nmap_create(&(get_ggs()->map),64,64);
    }
}
s32 nactor_pick_up_item(nActorComponent *ac, nActorComponent *item) {
    if (ac->flags & NACTOR_FEATURE_FLAG_HAS_CONTAINER) {
        if (ac->c.item_count < NMAX_ITEMS) {
            for (s32 i = 0; i < NMAX_ITEMS; i+=1) {
                if (ac->c.items[i] == 0) {
                    ac->c.items[i] = item;
                    ac->c.item_count+=1;
                    item->active = 0;
                    return i;
                }
            }
        }else {
            NLOG_DBG("inventory full!\n");
        }
    }
    return 0;
}

s32 nactor_heal(nActorComponent *ac, s32 heal_amount) {
    ac->d.hp += heal_amount;
    ac->d.hp = minimum(ac->d.hp, ac->d.max_hp);
    return ac->d.hp;
}

s32 nactor_take_damage(nActorComponent *ac, s32 damage) {
    damage -= ac->d.def;
    if (damage > 0) {
        ac->d.hp -= damage;
        if (ac->d.hp <= 0) {
            nactor_die(ac);
        }
        ac->d.shake_duration = 0.3;
    }else {
        damage = 0;
    }
    return damage;
}

void nactor_use_item(nActorComponent *ac, u8 item_index) {
    nActorComponent *item = ac->c.items[item_index];

    // ITEM LOGIC
    NLOG_DBG("item: %s used!\n", item->name);
    nEntity item_entity = nactor_cm_lookup_entity_from_ptr(&(get_ggs()->acm), item);
    if (strcmp(item->name, "hlt-potion") == 0) {
        nactor_heal(ac, 10);
    }else {
        ac->a.powa*=2;
    }

    // remove item from container
    ac->c.item_count-=1;
    ac->c.items[item_index] = 0;
    //nactor_cm_del(&(get_ggs()->acm), item_entity);
}


void nactor_attack(nActorComponent *attacker, nActorComponent *victim) {
    if (victim->flags & NACTOR_FEATURE_FLAG_DESTRUCTIBLE && (victim->d.hp > 0)) {
        nactor_take_damage(victim, attacker->a.powa);
    }
}

nDestructibleData ndestructible_data_make(s32 max_hp, s32 def, f32 shake_str) {
    return (nDestructibleData){.max_hp = max_hp, .hp = max_hp, .def = def, .shake_duration = 0.0, .shake_str = shake_str};
}

nAttackData nattack_data_make(s32 powa) {
    return (nAttackData){.powa = powa};
}
