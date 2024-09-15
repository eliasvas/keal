#include "map.h"
#include "actor.h"
#include "game_state.h"


void nmap_clear(nMap *map) {
    //M_ZERO(map->tiles, map->height*map->width);
    // TODO -- make this M_SET(..)
    for (u32 i = 0; i < map->width * map->height; i+=1) {
        map->tiles[i] = NTILE_WALL;
    }
    map->last_center = iv2(0,0);
}

typedef enum nDungeonSubdivisionSplitDirection nDungeonSubdivisionSplitDirection;
enum nDungeonSubdivisionSplitDirection {
    NDUNGEON_SUBDIVISION_SPLIT_AXIS_HORIZONTAL,
    NDUNGEON_SUBDIVISION_SPLIT_AXIS_VERTICAL,
    NDUNGEON_SUBDIVISION_SPLIT_AXIS_COUNT,
};

typedef struct nDungeonSubdivision nDungeonSubdivision;
struct nDungeonSubdivision {
	// sub-dungeon hierarchy
	nDungeonSubdivision *first; // first child
	nDungeonSubdivision *last; // last child
	nDungeonSubdivision *next; // next node of parent (meaning in the same 'level' on the n-ary tree)
	nDungeonSubdivision *prev; // prev node of parent (in the same 'level', as in, having the same parent with this)
	nDungeonSubdivision *parent; // parent ref

    u32 child_count;

    // Dungeon Subdivision dimensions
    s32 x,y,w,h;
    ivec2 center;
};

static nDungeonSubdivision g_nil_ds = {
	&g_nil_ds,
	&g_nil_ds,
	&g_nil_ds,
	&g_nil_ds,
	&g_nil_ds,
};

void nmap_subdivide(nMap *map, nDungeonSubdivision *p) {
    // dll_push_back_NPZ(&g_nil_box, parent->first, parent->last, box, next, prev);
    b32 parent_can_be_split = 0;
    p->center = iv2(p->x + p->w/2.0f, p->w + p->h/2.0f);

    nDungeonSubdivisionSplitDirection split_dir = gen_random(0, NDUNGEON_SUBDIVISION_SPLIT_AXIS_COUNT);
    assert(split_dir == NDUNGEON_SUBDIVISION_SPLIT_AXIS_VERTICAL || split_dir == NDUNGEON_SUBDIVISION_SPLIT_AXIS_HORIZONTAL);

    if (map->min_room_factor * p->w < map->min_room_size){
        split_dir = NDUNGEON_SUBDIVISION_SPLIT_AXIS_VERTICAL;
    }
    if (map->min_room_factor * p->h < map->min_room_size){
        split_dir = NDUNGEON_SUBDIVISION_SPLIT_AXIS_HORIZONTAL;
    }
    s32 w1 = p->w;
    s32 w2 = p->w;
    s32 h1 = p->h;
    s32 h2 = p->h;
    // we TRY to split the current parent, if successful, the hierarchy then will have 2 new children A -> +A1 +A2
    if (split_dir == NDUNGEON_SUBDIVISION_SPLIT_AXIS_HORIZONTAL) {
        s32 room_size = map->min_room_factor * p->w;
        if (room_size >= map->min_room_size) {
            parent_can_be_split = 1;
            w1 = gen_random(map->min_room_factor*p->w, map->max_room_factor*p->w+1);
            w2 = p->w - w1;
        }

    } else {
        s32 room_size = map->min_room_factor * p->h;
        if (room_size >= map->min_room_size) {
            parent_can_be_split = 1;
            h1 = gen_random(map->min_room_factor*p->h, map->max_room_factor*p->h+1);
            h2 = p->h - h1;
        }
    }

    // if we can split parent, we make c1 and c2 and insert as children
    if (parent_can_be_split) {

        nDungeonSubdivision *c1 = push_array(get_frame_arena(), nDungeonSubdivision, 1);
        c1->first = c1->last = c1->next = c1->prev = c1->parent = &g_nil_ds;
        c1->x = p->x;
        c1->y = p->y;
        c1->w = w1;
        c1->h = h1;
        dll_push_back_NPZ(&g_nil_ds, p->first, p->last, c1, next, prev);
        p->child_count += 1;
        c1->parent = p;
        nmap_subdivide(map, c1);

        nDungeonSubdivision *c2 = push_array(get_frame_arena(), nDungeonSubdivision, 1);
        c2->first = c2->last = c2->next = c2->prev = c2->parent = &g_nil_ds;
        c2->x = p->x + ((split_dir == NDUNGEON_SUBDIVISION_SPLIT_AXIS_HORIZONTAL) ? w1 : 0);
        c2->y = p->y + ((split_dir == NDUNGEON_SUBDIVISION_SPLIT_AXIS_VERTICAL) ? h1 : 0);
        c2->w = w2;
        c2->h = h2;
        dll_push_back_NPZ(&g_nil_ds, p->first, p->last, c2, next, prev);
        p->child_count += 1;
        c2->parent = p;
        nmap_subdivide(map, c2);

        assert(p->child_count == 2);
    }else {
        assert(p->child_count == 0);
    }
}

void nmap_dig_region(nMap *map, s32 x0, s32 y0, s32 x1, s32 y1, nTileKind kind) {
    assert(x0 >= 0 && y0 >= 0 && x1 <= map->width && y1 <= map->height);
    //NLOG_DBG("digging %d %d %d %d\n", x0, y0, x1, y1);
    nTile dest = {.kind = kind, .explored = 0, .color = v4(1,1,1,1)};

    if (kind == NTILE_KIND_WALL) {
        dest.color = v4(1,0,0,1);
    }
    if (x1 < x0){
        s32 temp = x1;
        x1 = x0;
        x0 = temp;
    }
    if (y1 < y0){
        s32 temp = y1;
        y1 = y0;
        y0 = temp;
    }
    for (s32 x = x0; x < x1; x+=1) {
        for (s32 y = y0; y < y1; y+=1) {
            map->tiles[x + y*map->width] = dest;
        }
    }
}

void nmap_tile_set_explored(nMap *map, s32 x, s32 y, b32 state) {
    if (x < 0 || y < 0 || x > map->width || y > map->height)return;
    map->tiles[x + y*map->width].explored = state;
}
b32 nmap_tile_is_explored(nMap *map, s32 x, s32 y) {
    if (x < 0 || y < 0 || x > map->width || y > map->height)return 0;
    return map->tiles[x + y*map->width].explored;
}

b32 nmap_tile_is_wall(nMap *map, s32 x, s32 y) {
    if (x < 0 || y < 0 || x > map->width || y > map->height)return 1;
    return (map->tiles[x + y*map->width].kind == NTILE_KIND_WALL);
}

b32 nmap_tile_is_walkable(nMap *map, s32 x, s32 y) {
    b32 overlapping_actors_movable = 1;
    for (u32 i = 0; i < get_ggs()->acm.size; i+=1) {
        nActorComponent *actor = &get_ggs()->acm.actors[i];
        if (actor->posx == x && actor->posy == y) {
            overlapping_actors_movable &= !(get_ggs()->acm.actors[i].blocks);
        }
    }

    return (!nmap_tile_is_wall(map, x, y) && overlapping_actors_movable);
}



void nmap_compute_fov(nMap *map, s32 px, s32 py, s32 fovRadius) {
    for (s32 i = -fovRadius; i < fovRadius; i+=1) {
        for (s32 j = -fovRadius; j < fovRadius; j+=1) {
            nmap_tile_set_explored(map, px+i, py+j, 1);
        }
    }
}

void nmap_generate(nMap *map);
void nmap_create_ex(nMap *map, u32 w, u32 h, s32 min_room_size, f32 min_room_factor, f32 max_room_factor) {
    M_ZERO_STRUCT(map);
    map->width = w;
    map->height = h;
    map->min_room_size = min_room_size;
    map->min_room_factor = min_room_factor;
    map->max_room_factor = max_room_factor;
    map->max_room_enemies = 3;
    map->tiles = push_array_nz(get_ngs()->global_arena, nTile, w*h);
    nmap_generate(map);
}

void nmap_create(nMap *map, u32 w, u32 h) {
    nmap_create_ex(map, w, h, 8, 0.4, 0.6);
}

vec4 nmap_calc_tile_tc(nMap *map, s32 x, s32 y) {
    if (nmap_tile_at(map, x,y).kind == NTILE_KIND_GROUND) return TILESET_DOT_TILE;
    ivec2 off = iv2(0,0);

    off.x -= (nmap_tile_is_wall(map, x+1,y));
    off.x += (nmap_tile_is_wall(map, x-1,y));
    off.y += (nmap_tile_is_wall(map, x,y+1));
    off.y -= (nmap_tile_is_wall(map, x,y-1));

    // if (off.x == 0 && off.y == 0) {
    //     return TILESET_WALL_TILE;
    // }

    return TILESET_DUNGEON_TILE(off.x,off.y);
}

void nmap_render(nMap *map, nBatch2DRenderer *rend, oglImage *atlas) {
    for (u32 x = 0; x < map->width; x+=1) {
        for (u32 y = 0; y < map->height; y+=1) {
            nBatch2DQuad q = {
                .color = vec4_divf(nmap_tile_at(map,x,y).color,(nmap_tile_is_explored(map, x, y)) ? 1.0f : 2.0f),
                .pos.x = x*TILESET_DEFAULT_SIZE,
                .pos.y = y*TILESET_DEFAULT_SIZE,
                .dim.x = TILESET_DEFAULT_SIZE,
                .dim.y = TILESET_DEFAULT_SIZE,
                .tc    = nmap_calc_tile_tc(map, x, y),
                .angle_rad = 0,
            };
            nbatch2d_rend_add_quad(rend, q, atlas);
        }

    }
}
nTile* nmap_tile_ref(nMap *map, s32 x, s32 y) {
    if (x < 0 || y < 0 || x > map->width || y > map->height)return NULL;
    return &(map->tiles[x + y*map->width]);
}
nTile nmap_tile_at(nMap *map, s32 x, s32 y) {
    nTile *t = nmap_tile_ref(map,x,y);
    return (t ? (*t) : NTILE_WALL);
}
b32 ndungeon_sub_is_nil(nDungeonSubdivision *s) {
    return (s == 0 || s == &g_nil_ds);
}

b32 ndungeon_sub_is_leaf(nDungeonSubdivision *s) {
    return (s->child_count == 0);
}


nEntity nmap_add_door(nMap *map, s32 x, s32 y) {
    nEntity door = nentity_create(&(get_ggs()->em));
    nActorComponent *ac = nactor_cm_add(&(get_ggs()->acm), door);
    ac->active = 1;
    ac->color = v4(0,0,1,1);
    ac->kind = NACTOR_KIND_DOOR;
    ac->posx = x;
    ac->posy = y;
    ac->tc = TILESET_DOOR_TILE;
    ac->blocks = 0;
    M_ZERO_STRUCT(&ac->c);
    ac->flags = 0;
    sprintf(ac->name, "door");
    return door;
}


nEntity nmap_add_player(nMap *map, s32 x, s32 y) {
    nEntity player = nentity_create(&(get_ggs()->em));
    nActorComponent *ac = nactor_cm_add(&(get_ggs()->acm), player);
    ac->active = 1;
    ac->color = v4(0.5,0.3,0.7,1);
    ac->kind = NACTOR_KIND_PLAYER;
    ac->posx = x;
    ac->posy = y;
    ac->tc = TILESET_PLAYER_TILE;
    ac->blocks = 1;
    ac->d = ndestructible_data_make(10,2,0.3);
    ac->a = nattack_data_make(3);
    M_ZERO_STRUCT(&ac->c);
    ac->flags = NACTOR_FEATURE_FLAG_ATTACKER | NACTOR_FEATURE_FLAG_DESTRUCTIBLE | NACTOR_FEATURE_FLAG_SHAKEABLE | NACTOR_FEATURE_FLAG_HAS_CONTAINER;
    sprintf(ac->name, "player");
    return player;
}

nEntity nmap_add_enemy(nMap *map, s32 x, s32 y) {
    nEntity enemy = nentity_create(&(get_ggs()->em));
    nActorComponent *ac = nactor_cm_add(&(get_ggs()->acm), enemy);
    ac->kind = NACTOR_KIND_ENEMY;
    ac->posx = x;
    ac->posy = y;
    ac->blocks = 1;
    ac->d = ndestructible_data_make(10,1,0.3);
    ac->a = nattack_data_make(5);
    ac->active = 1;
    M_ZERO_STRUCT(&ac->c);
    ac->flags = NACTOR_FEATURE_FLAG_ATTACKER | NACTOR_FEATURE_FLAG_DESTRUCTIBLE | NACTOR_FEATURE_FLAG_SHAKEABLE;
    if (gen_random(0,100) < 70) {
        sprintf(ac->name, "skelly");
        ac->color = v4(0.9,0.9,0.9,1);
        ac->tc = TILESET_SKELLY_TILE;
    }else {
        sprintf(ac->name, "troll");
        ac->color = v4(0.8,0.5,0.7,1);
        ac->tc = TILESET_TROLL_TILE;
    }
    return enemy;
}

nEntity nmap_add_item(nMap *map, s32 x, s32 y) {
    nEntity enemy = nentity_create(&(get_ggs()->em));
    nActorComponent *ac = nactor_cm_add(&(get_ggs()->acm), enemy);
    ac->active = 1;
    ac->kind = NACTOR_KIND_ITEM;
    ac->posx = x;
    ac->posy = y;
    ac->blocks = 0;
    ac->d = ndestructible_data_make(10,1,0.3);
    ac->a = nattack_data_make(10);
    M_ZERO_STRUCT(&ac->c);
    ac->flags = NACTOR_FEATURE_FLAG_PICKABLE;
    if (gen_random(0,100) < 70) {
        sprintf(ac->name, "hlt-potion");
        ac->color = v4(0.9,0.2,0.2,1);
        ac->tc = TILESET_POTION_TILE;
    }else {
        sprintf(ac->name, "str-potion");
        ac->color = v4(0.3,0.9,0.8,1);
        ac->tc = TILESET_POTION_TILE;
    }
    return enemy;
}


// TODO -- there is a bug here when we are making corridors, sometimes a corridor is off by 1 (right corners only)
void nmap_gen_corridors(nMap *map, nDungeonSubdivision *p) {
    for (nDungeonSubdivision *child = p->first; !ndungeon_sub_is_nil(child); child = child->next) {
        if (ndungeon_sub_is_leaf(child)) {
            if (map->last_center.x && map->last_center.y) {
                nmap_dig_region(map, map->last_center.x, map->last_center.y, child->center.x+1, map->last_center.y+1, NTILE_KIND_GROUND);
                nmap_dig_region(map, child->center.x, map->last_center.y, child->center.x+1, child->center.y+1, NTILE_KIND_GROUND);
            }
            map->last_center = child->center;
        }
        nmap_gen_corridors(map, child);
    }

}


void nmap_gen_rooms(nMap *map, nDungeonSubdivision *p) {
    for (nDungeonSubdivision *child = p->first; !ndungeon_sub_is_nil(child); child = child->next) {
        if (ndungeon_sub_is_leaf(child)) {
            //NLOG_DBG("child %d %d %d %d!\n", child->x, child->y, child->w, child->h);
            if (gen_rand01() < 0.75) {
                s32 w = child->w;
                s32 h = child->h;
                // Generate randomized room inside sub-dungeon bounds
                child->w = gen_random(map->min_room_size, child->w+1)-1;
                child->h = gen_random(map->min_room_size, child->h+1)-1;
                child->x = child->x + (w/child->w)/2.0 + 1;
                child->y = child->y + (h/child->h)/2.0 + 1;
                child->center = iv2(child->x + child->w/2.0, child->y + child->h/2.0);
                nmap_dig_region(map, child->x, child->y, child->x + child->w, child->y + child->h, NTILE_KIND_GROUND);

                // Add player to first dungeon's first block
                //if (map->player ==)
                nActorComponent *player_cmp = nactor_cm_get(&(get_ggs()->acm), map->player);
                if (!player_cmp) {
                    map->player = nmap_add_player(map, child->x, child->y);
                }


                // Generate randomized enemies
                s32 enemy_num = gen_random(0, map->max_room_enemies+1);
                while (enemy_num) {
                    s32 x = gen_random(child->x, child->x + child->w+1);
                    s32 y = gen_random(child->y, child->y + child->h+1);
                    if (nmap_tile_is_walkable(map, x,y)) {
                        nmap_add_enemy(map,x,y);
                    }
                    enemy_num-=1;
                }

                // Generate randomized items
                s32 item_num = gen_random(0, map->max_room_enemies+1);
                while (item_num) {
                    s32 x = gen_random(child->x, child->x + child->w+1);
                    s32 y = gen_random(child->y, child->y + child->h+1);
                    if (nmap_tile_is_walkable(map, x,y)) {
                        nmap_add_item(map,x,y);
                    }
                    item_num-=1;
                }
            }else {
                nmap_dig_region(map, child->x, child->y, child->x + child->w, child->y + child->h, NTILE_KIND_WALL);
            }
        }
        nmap_gen_rooms(map, child);
    }

}

void nmap_print_dungeon_bsp(nDungeonSubdivision *ds, u32 depth) {
	if (ndungeon_sub_is_nil(ds))return;

	if (depth == 0) {
		NLOG_DBG("[%d:%d:%d:%d : %d]\n",ds->x, ds->y, ds->w, ds->h, depth);
	}else {
		for (u32 i = 1; i < depth; ++i) {
			NLOG_DBG("\t");
		}
		NLOG_DBG("+----[%d:%d:%d:%d : %d]\n",ds->x, ds->y, ds->w, ds->h, depth);
	}
    for (nDungeonSubdivision *child = ds->first; !ndungeon_sub_is_nil(child); child = child->next) {
		nmap_print_dungeon_bsp(child, depth+1);
	}
}

void nmap_generate(nMap *map) {
    nmap_clear(map);

    nDungeonSubdivision *dungeon = push_array(get_frame_arena(), nDungeonSubdivision, 1);
    dungeon->first = dungeon->last = dungeon->next = dungeon->prev = dungeon->parent = &g_nil_ds;
    dungeon->x = 1;
    dungeon->y = 1;
    dungeon->w = map->width-2;
    dungeon->h = map->height-2;
    nmap_subdivide(map, dungeon);
    nmap_gen_rooms(map, dungeon);
    nmap_gen_corridors(map, dungeon);

    // Generate randomized door for end of level
    s32 door_x= gen_random(0, map->width);
    s32 door_y = gen_random(0, map->height);
    while (!nmap_tile_is_walkable(map, door_x, door_y)) {
        door_x= gen_random(0, map->width);
        door_y = gen_random(0, map->height);
    }
    nmap_add_door(map, door_x, door_y);


    //nmap_print_dungeon_bsp(dungeon, 0);
}
