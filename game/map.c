#include "map.h"

void nmap_create(nMap *map, u32 w, u32 h) {
    M_ZERO_STRUCT(map);
    map->width = w;
    map->height = h;
    map->tiles = push_array(get_ngs()->global_arena, nTile, w*h);
    map->tiles[0] = NTILE_WALKABLE; 
    map->tiles[1] = NTILE_WALKABLE; 
    map->tiles[2] = NTILE_WALKABLE;
    map->tiles[w] = NTILE_WALKABLE; 
    map->tiles[w+1] = NTILE_WALKABLE; 
}

void nmap_render(nMap *map, nBatch2DRenderer *rend, oglImage *atlas) {
    for (u32 x = 0; x < map->width; x+=1) {
        for (u32 y = 0; y < map->height; y+=1) {
            nBatch2DQuad q = {
                .color = v4(1,1,1,1),
                .pos.x = x*32,
                .pos.y = y*32,
                .dim.x = 32,
                .dim.y = 32,
                .tc    = (nmap_tile_at(map,x,y).can_walk == 1) ? TILESET_EMPTY_TILE : TILESET_WALL_TILE,
                .angle_rad = 0,
            };
            nbatch2d_rend_add_quad(rend, q, atlas);
        }

    }
}

nTile nmap_tile_at(nMap *map, u32 x, u32 y) {
    assert(x < map->width);
    assert(y < map->height);
    return map->tiles[x + y*map->width];
}
