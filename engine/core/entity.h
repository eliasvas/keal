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
nDebugNameComponent *ndebug_name_cm_add_entity(nDebugNameCM *cm, nEntity e);
void ndebug_name_cm_del_entity(nDebugNameCM *cm, nEntity e);
nDebugNameComponent ndebug_name_cm_lookup_entity(nDebugNameCM *cm, nEntity e);
void ndebug_name_cm_prune_destroyed_entities(nDebugNameCM *cm);


////////////////////////////////
// Transform Component manager
////////////////////////////////


typedef struct nTransformComponent nTransformComponent;
struct nTransformComponent {
    mat4 world;
    mat4 local;

    nTransformComponent *parent;
    nTransformComponent *first;
    nTransformComponent *last;
    nTransformComponent *next;
    nTransformComponent *prev;
};
//dll_push_back_NPZ(&g_nil_box, parent->first, parent->last, box, next, prev);

typedef struct nTransformComponentNode nTransformComponentNode;
struct nTransformComponentNode {
    nTransformComponentNode *next;
    nTransformComponentNode *prev;

    nTransformComponent transform;
    // The entity that owns this component (used in hash-map collision resolution)
    nEntity entity;
};

typedef struct nTransformComponentHashSlot nTransformComponentHashSlot;
struct nTransformComponentHashSlot {
    nTransformComponentNode *hash_first;
    nTransformComponentNode *hash_last;
};

// CM = ComponentManager
typedef struct nTransformCM nTransformCM;
struct nTransformCM {
    u32 transform_table_size;
    nTransformComponentHashSlot *transform_table;
    nTransformComponentNode *free_nodes;
    nEntityManager *em_ref;
};

void ntransform_cm_init(nTransformCM *cm, nEntityManager *em);
void ntransform_cm_destroy(nTransformCM *cm, nEntityManager *em);
void ntransform_cm_update(nTransformCM *cm);
nTransformComponent *ntransform_cm_add_entity(nTransformCM *cm, nEntity e);
void ntransform_cm_del_entity(nTransformCM *cm, nEntity e);
nTransformComponent ntransform_cm_lookup_entity(nTransformCM *cm, nEntity e);
void ntransform_cm_prune_destroyed_entities(nTransformCM *cm);



#endif