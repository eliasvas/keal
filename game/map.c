#include "map.h"

void nmap_clear(nMap *map) {
    M_ZERO(map->tiles, map->height*map->width);
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

    nDungeonSubdivisionSplitDirection split_dir = gen_random(0, NDUNGEON_SUBDIVISION_SPLIT_AXIS_COUNT); 
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
            //w1 = gen_random(room_size, p->w);
            w1 = room_size;
            w2 = p->w - w1;
        }

    } else {
        s32 room_size = map->min_room_factor * p->h;
        if (room_size >= map->min_room_size) {
            parent_can_be_split = 1;
            //h1 = gen_random(room_size, p->h);
            h1 = room_size;
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
        //printf("c1: %d %d %d %d\n", c1->x, c1->y, c1->w, c1->h);
        nmap_subdivide(map, c1);
 
        nDungeonSubdivision *c2 = push_array(get_frame_arena(), nDungeonSubdivision, 1);
        c2->first = c2->last = c2->next = c2->prev = c2->parent = &g_nil_ds;
        c2->x = p->x + ((w1!=w2) ? w1 : 0);
        c2->y = p->y + ((h1!=h2) ? h1 : 0);
        c2->w = w2;
        c2->h = h2;
        dll_push_back_NPZ(&g_nil_ds, p->first, p->last, c2, next, prev);
        p->child_count += 1;
        c2->parent = p;
        //printf("c2: %d %d %d %d\n", c2->x, c2->y, c2->w, c2->h);
        nmap_subdivide(map, c2);
 


    }
}

void nmap_dig_region(nMap *map, s32 x0, s32 y0, s32 x1, s32 y1) {
    assert(x0 >= 0 && y0 >= 0 && x1 <= map->width && y1 <- map->height);
    nTile dest = NTILE_GROUND;
    dest.color = v4(gen_rand01(), gen_rand01(), gen_rand01(), 1);
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

void nmap_generate(nMap *map);

void nmap_create_ex(nMap *map, u32 w, u32 h, s32 min_room_size, f32 min_room_factor) {
    M_ZERO_STRUCT(map);
    map->width = w;
    map->height = h;
    map->min_room_size = min_room_size;
    map->min_room_factor = min_room_factor;
    map->tiles = push_array_nz(get_ngs()->global_arena, nTile, w*h);
    nmap_generate(map);
}

void nmap_create(nMap *map, u32 w, u32 h) {
    nmap_create_ex(map, w, h, 8, 0.4);
}


void nmap_render(nMap *map, nBatch2DRenderer *rend, oglImage *atlas) {
    for (u32 x = 0; x < map->width; x+=1) {
        for (u32 y = 0; y < map->height; y+=1) {
            nBatch2DQuad q = {
                .color = nmap_tile_at(map,x,y).color,
                .pos.x = x*TILESET_DEFAULT_SIZE,
                .pos.y = y*TILESET_DEFAULT_SIZE,
                .dim.x = TILESET_DEFAULT_SIZE,
                .dim.y = TILESET_DEFAULT_SIZE,
                .tc    = TILESET_WALL_TILE,
                .angle_rad = 0,
            };
            nbatch2d_rend_add_quad(rend, q, atlas);
        }

    }
}

nTile nmap_tile_at(nMap *map, s32 x, s32 y) {
    if (x < 0 || y < 0 || x > map->width || y > map->height)return NTILE_WALL;
    return map->tiles[x + y*map->width];
}


b32 ndungeon_sub_is_nil(nDungeonSubdivision *s) {
    return (s == 0 || s == &g_nil_ds);
}

void nmap_populate(nMap *map, nDungeonSubdivision *p) {
    f32 child_count = p->child_count;
    for (nDungeonSubdivision *child = p->first; !ndungeon_sub_is_nil(child); child = child->next) {
        //printf("testing for child %d %d %d %d!\n", child->x, child->y, child->w, child->h);
        if (child->child_count == 0) {
            printf("child %d %d %d %d!\n", child->x, child->y, child->w, child->h);
            nmap_dig_region(map, child->x, child->y, child->x + child->w, child->y + child->h);
        }
        nmap_populate(map, child);
    }

}

void nmap_generate(nMap *map) {
    nmap_clear(map);

    nDungeonSubdivision *dungeon = push_array(get_frame_arena(), nDungeonSubdivision, 1);
    dungeon->first = dungeon->last = dungeon->next = dungeon->prev = dungeon->parent = &g_nil_ds;
    dungeon->x = 0;
    dungeon->y = 0;
    dungeon->w = map->width;
    dungeon->h = map->height;
    nmap_subdivide(map, dungeon);
    nmap_populate(map, dungeon);
}