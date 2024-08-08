#ifndef MAP_H
#define MAP_H
#include "engine.h"
#include "tileset4922.inl"

typedef struct nTile nTile;
struct nTile {
    // TODO -- probably we need a difficult to traverse filed for the A* algorithm (max will be wall??)
    b32 can_walk;
};
#define NTILE_WALKABLE (nTile){.can_walk = 1};
#define NTILE_NOT_WALKABLE (nTile){.can_walk = 0};

typedef struct nMap nMap;
struct nMap {
    u32 width, height;
    nTile *tiles;
};

void nmap_create(nMap *map, u32 w, u32 h);
void nmap_render(nMap *map, nBatch2DRenderer *rend, oglImage *atlas);
nTile nmap_tile_at(nMap *map, u32 x, u32 y);

#endif