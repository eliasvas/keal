#ifndef ENTITY_H
#define ENTITY_H
#include "engine.h"
// TODO -- we have an allocation problem here.. can we bypass having ANOTHER arena somehow?

typedef u32 nEntity;

#define NENTITY_INDEX_BITS (22)
#define NENTITY_INDEX_MASK ((1<<NENTITY_INDEX_BITS)-1)
#define NENTITY_GENERATION_BITS (8)
#define NENTITY_GENERATION_MASK ((1<<NENTITY_GENERATION_BITS)-1)

typedef struct nFreeIndexNode nFreeIndexNode;
struct nFreeIndexNode {
    nFreeIndexNode *next;
    u8 index;
};

#define NGEN_CHUNK_COUNT 2048
typedef struct nGenArrayChunk nGenArrayChunk;
struct nGenArrayChunk {
    u8 *generation; 
    u32 gen_count; // when gen_count == NGEN_CHUNK_COUNT, we add another chunk to the list here

    nGenArrayChunk *next;
};

typedef struct nEntityManager nEntityManager;
struct nEntityManager {
    Arena *arena;

    nGenArrayChunk *first;
    nGenArrayChunk *last;
    //nGenArrayChunk *free_gen_chunks;

    nFreeIndexNode *free_indices_first;
    nFreeIndexNode *free_indices_last;
    u32 free_indices_count;
};

void nentity_manager_init(nEntityManager *em);
void nentity_manager_destroy(nEntityManager *em);

u32 nentity_manager_make_new_index(nEntityManager *em);
u8 *nentity_manager_get_generation_for_index(nEntityManager *em, u32 index);
u8 nentity_manager_get_generation_val_for_index(nEntityManager *em, u32 index);
void nentity_manager_increment_generation_for_index(nEntityManager *em, u32 index);

nEntity nentity_create(nEntityManager *em);
b32 nentity_alive(nEntityManager *em, nEntity e);
void nentity_destroy(nEntityManager *em, nEntity e);

#endif