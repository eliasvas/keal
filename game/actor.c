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
    M_ZERO_STRUCT(&cm->actors[new_idx]);
    cm->entity[new_idx] = e;
    //cm->actors[new_idx] = e;

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

////////////////////////////////
// Gameplay Code
////////////////////////////////

b32 nactor_cm_check_movement_event(nActorCM *cm) {
    return (ninput_key_pressed(NKEY_SCANCODE_RIGHT) | ninput_key_pressed(NKEY_SCANCODE_LEFT) | ninput_key_pressed(NKEY_SCANCODE_UP) | ninput_key_pressed(NKEY_SCANCODE_DOWN));
}

void nactor_cm_simulate(nActorCM *cm, nMap *map) {
    for (u32 i = 0; i < cm->size; i+=1) {
        ivec2 delta = iv2(0,0);
        switch (cm->actors[i].kind) {
            case NACTOR_KIND_PLAYER:
                if (ninput_key_pressed(NKEY_SCANCODE_RIGHT)) {
                    delta.x+=1;
                }else if (ninput_key_pressed(NKEY_SCANCODE_LEFT)) {
                    delta.x-=1;
                }else if (ninput_key_pressed(NKEY_SCANCODE_UP)) {
                    delta.y-=1;
                }else if (ninput_key_pressed(NKEY_SCANCODE_DOWN)) {
                    delta.y+=1;
                }
                // Should this logic remain here??
                {
                    ivec2 new_pos = iv2(cm->actors[i].posx + delta.x, cm->actors[i].posy + delta.y);

                    if (nmap_tile_is_walkable(map, new_pos.x, new_pos.y) || !nmap_tile_is_walkable(map, cm->actors[i].posx, cm->actors[i].posy) ) {
                        cm->actors[i].posx = new_pos.x;
                        cm->actors[i].posy = new_pos.y;
                    }

                    // Check to see whether in target position there is an enemy, in which case, we CUT
                    for (u32 j = 0; j < get_ggs()->acm.size; j+=1) {
                        nActorComponent *actor = &get_ggs()->acm.actors[j];
                        if (actor->posx == new_pos.x && actor->posy == new_pos.y && actor->kind != NACTOR_KIND_PLAYER) {
                            nactor_attack(&(cm->actors[i]), &(cm->actors[j]));
                        }
                    }
                }
                break;
            case NACTOR_KIND_ENEMY:
                break;
            default:
                printf("Who is dis guy?!\n");
                break;
        }
        

    }
}

void nactor_cm_render(nActorCM *cm, nBatch2DRenderer *rend, oglImage *atlas) {

    for (u32 i = 0; i < cm->size; i+=1) {
        nActorComponent *actor = &(cm->actors[i]);
        nBatch2DQuad q = {
            .color = actor->color,
            .pos.x = actor->posx*TILESET_DEFAULT_SIZE,
            .pos.y = actor->posy*TILESET_DEFAULT_SIZE,
            .dim.x = TILESET_DEFAULT_SIZE,
            .dim.y = TILESET_DEFAULT_SIZE,
            .tc    = actor->tc,
            .angle_rad = 0,
        };
        nbatch2d_rend_add_quad(rend, q, atlas);
    }
}

s32 nactor_die(nActorComponent *ac) {
    printf("actor: %s died!\n", ac->name);
    ac->tc = TILESET_SKULL_TILE;
    ac->blocks = 0;
    //also shift this component to begining, so its drawn before HERO
}

s32 nactor_take_damage(nActorComponent *ac, s32 damage) {
    damage -= ac->d.def;
    if (damage > 0) {
        ac->d.hp -= damage;
        if (ac->d.hp <= 0) {
            nactor_die(ac);
        }
    }else {
        damage = 0;
    }
    return damage;
}

void nactor_attack(nActorComponent *attacker, nActorComponent *victim) {
    if (victim->flags & NACTOR_FEATURE_FLAG_DESTRUCTIBLE && (victim->d.hp > 0)) {
        nactor_take_damage(victim, attacker->a.powa);
    }
}

nDestructibleData ndestructible_data_make(s32 max_hp, s32 def) {
    return (nDestructibleData){.max_hp = max_hp, .hp = max_hp, .def = def};
}
nAttackData nattack_data_make(s32 powa) {
    return (nAttackData){.powa = powa};
}