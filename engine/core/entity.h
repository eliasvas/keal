#ifndef ENTITY_H
#define ENTITY_H
#include "engine.h"
// TODO -- We probably need an arena per component manager.. to have eveything nice and packed

////////////////////////////////
// Entity Manager
////////////////////////////////

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


////////////////////////////////
// Debug-Name Component manager
////////////////////////////////


// TODO -- Should all this type stuff be macros (prb YES)
typedef struct nDebugNameComponent nDebugNameComponent;
struct nDebugNameComponent {
    u8 name[64];
};

typedef struct nDebugNameComponentNode nDebugNameComponentNode;
struct nDebugNameComponentNode {
    nDebugNameComponentNode *next;
    nDebugNameComponentNode *prev;

    nDebugNameComponent debug_name;
    // The entity that owns this component (used in hash-map collision resolution)
    nEntity entity;
};

typedef struct nDebugNameComponentHashSlot nDebugNameComponentHashSlot;
struct nDebugNameComponentHashSlot {
    nDebugNameComponentNode *hash_first;
    nDebugNameComponentNode *hash_last;
};

// CM = ComponentManager
typedef struct nDebugNameCM nDebugNameCM;
struct nDebugNameCM {
    u32 debug_table_size;
    nDebugNameComponentHashSlot *debug_table;
    nDebugNameComponentNode *free_nodes;
    nEntityManager *em_ref;
};

void ndebug_name_cm_init(nDebugNameCM *cm, nEntityManager *em);
void ndebug_name_cm_destroy(nDebugNameCM *cm, nEntityManager *em);
void ndebug_name_cm_update(nDebugNameCM *cm);
nDebugNameComponent *ndebug_name_cm_add(nDebugNameCM *cm, nEntity e);
void ndebug_name_cm_del_entity(nDebugNameCM *cm, nEntity e);
nDebugNameComponentNode *ndebug_name_cm_lookup(nDebugNameCM *cm, nEntity e);
void ndebug_name_cm_prune_destroyed_entities(nDebugNameCM *cm);


////////////////////////////////
// Transform Component manager
////////////////////////////////

typedef u32 nCompIndex;
#define NCOMPONENT_INDEX_INVALID (U32_MAX)
#define NCOMPONENT_INDEX_VALID(idx) (idx != NCOMPONENT_INDEX_INVALID)

typedef struct nEntityComponentIndexPairNode nEntityComponentIndexPairNode;
struct nEntityComponentIndexPairNode {
    nEntity e;
    nCompIndex comp_idx;
    nEntityComponentIndexPairNode *next;
    nEntityComponentIndexPairNode *prev;
};

typedef struct nEntityComponentIndexPairHashSlot nEntityComponentIndexPairHashSlot;
struct nEntityComponentIndexPairHashSlot {
    nEntityComponentIndexPairNode *hash_first;
    nEntityComponentIndexPairNode *hash_last;
};

typedef struct nTransformComponent nTransformComponent;
struct nTransformComponent {
    mat4 world;
    mat4 local;
};

// CM = ComponentManager
#define NTRANSFORM_CM_MAX_COMPONENTS 1024
typedef struct nTransformCM nTransformCM;
struct nTransformCM {
    // The internal, allocated space
    u32 size, cap;
    void *buf;

    // The arrays assigned
    nTransformComponent *transform;
    nEntity *entity;
    u32 *parent;
    u32 *first;
    u32 *next;
    u32 *prev;

    // lookup table
    u32 lookup_table_size;
    nEntityComponentIndexPairHashSlot *lookup_table;
 
    // ref to parent nEntityManager (who does allocations for now)
    nEntityManager *em_ref;
};

void ntransform_cm_init(nTransformCM *cm, nEntityManager *em);
void ntransform_cm_deinit(nTransformCM *cm, nEntityManager *em);
void ntransform_cm_simulate(nTransformCM *cm);
nCompIndex ntransform_cm_lookup(nTransformCM *cm, nEntity e);
nTransformComponent *ntransform_cm_add(nTransformCM *cm, nEntity e, nEntity p);
void ntransform_cm_set_local(nTransformCM *cm, nCompIndex idx, mat4 local);
void ntransform_cm_del(nTransformCM *cm, nEntity e);




#endif