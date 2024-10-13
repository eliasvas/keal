#ifndef MAP_H
#define MAP_H
#include "engine.h"
#include "tileset4922.inl"

typedef enum nTileKind nTileKind;
enum nTileKind {
    NTILE_KIND_WALL = 0,
    NTILE_KIND_GROUND = 1,
    NTILE_KIND_WATER = 2,
};

typedef struct nTile nTile;
struct nTile {
    // TODO -- probably we need a difficult to traverse filed for the A* algorithm (max will be wall??)
    nTileKind kind;
    b32 explored;
    vec4 color;
};
#define NTILE_GROUND (nTile){.kind = NTILE_KIND_GROUND, .color = v4(1,1,1,1)}
#define NTILE_WATER (nTile){.kind = NTILE_KIND_WATER, .color = v4(0,0,1,1)}
#define NTILE_WALL (nTile){.kind = NTILE_KIND_WALL, .color = v4(1,0,0,1)}

#define ROOM_MAX_SIZE 12
#define ROOM_MIN_SIZE 6

typedef struct nMap nMap;
struct nMap {
    u32 width, height;
    nTile *tiles;

    u32 min_room_size;
    f32 min_room_factor;
    f32 max_room_factor;

    f32 max_room_enemies;

    // FIXE -- WHY is this god forsaken thing here???!!
    vec2 player_start_pos;
    vec2 door_start_pos;

    ivec2 last_center;
};

void nmap_create_ex(nMap *map, u32 w, u32 h, s32 min_room_size, f32 min_room_factor, f32 max_room_factor);
void nmap_create(nMap *map, u32 w, u32 h);
void nmap_add_tiles_as_entities(nMap *map);
nTile nmap_tile_at(nMap *map, s32 x, s32 y);
nTile* nmap_tile_ref(nMap *map, s32 x, s32 y);
void nmap_compute_fov(nMap *map, s32 px, s32 py, s32 fovRadius);
b32 nmap_tile_is_walkable(nMap *map, s32 x, s32 y);
void nmap_dig_region(nMap *map, s32 x0, s32 y0, s32 x1, s32 y1, nTileKind kind);

void nmap_generate();

#endif