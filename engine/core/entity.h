#ifndef ENTITY_H
#define ENTITY_H
#include "engine.h"
// ref: https://bitsquid.blogspot.com/2014/08/building-data-oriented-entity-system.html
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

typedef struct nEntityManager nEntityManager;
struct nEntityManager {
    Arena *arena;
    // Allocated in EntityManager's arena?
    u8 *generation;
    u32 generation_count;
    // Allocated in persistent storage?
    nFreeIndexNode *free_indices_first;
    nFreeIndexNode *free_indices_last;
    u32 free_indices_count;
};

void nentity_manager_init(nEntityManager *em);
void nentity_manager_destroy(nEntityManager *em);

nEntity nentity_create(nEntityManager *em);
b32 nentity_alive(nEntityManager *em, nEntity e);
void nentity_destroy(nEntityManager *em, nEntity e);

#endif