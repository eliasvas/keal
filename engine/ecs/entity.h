#ifndef NENTITY2_H
#define NENTITY2_H

#include "base/base_inc.h"
#include "core/core_inc.h"


#define NMAX_COMPONENTS 32
// TODO -- this should be deleted and space should be allocated via pool allocators on top of our regular arenas
#define NMAX_ENTITIES 512

// type registry to hold <Type,Id>
static int type_registry[NMAX_COMPONENTS];
static int type_count = 0;
static int RegisterType() {
    if (type_count >= NMAX_COMPONENTS) {
        fprintf(stderr, "Type registry is full\n");
        exit(EXIT_FAILURE);
    } 
    int id = type_count++;
    type_registry[id] = id;
    return id;
}
#define REGISTER_TYPE(type) \
    static int type_id_##type = __COUNTER__; \
    static int GetTypeID_##type() { return type_id_##type; } \

typedef u32 nComponentMask;
#define NCOMPONENT_MASK_GET_SLOT(mask, slot) ((mask >> slot) & 0x1)
#define NCOMPONENT_MASK_SET_SLOT(mask, slot) (mask = (mask | (0x1 << slot)))
#define NCOMPONENT_MASK_DEL_SLOT(mask, slot) (mask = mask & (~(0x1 << slot)))

//  [ 32b | 32b ] -> [ generation | index ]
typedef u64 nEntityID;
#define NENTITY_GET_INDEX(entity) (entity & U32_MAX)
#define NENTITY_GET_GENERATION(entity) ((entity >> 32) & U32_MAX)
#define NENTITY_INCREMENT_GENERATION(entity) (((NENTITY_GET_GENERATION(entity)+1)<<32) | NENTITY_GET_INDEX(entity))
#define NENTITY_INCREMENT_INDEX(entity) ((NENTITY_GET_GENERATION(entity) << 32) | (NENTITY_GET_INDEX(entity)+1))

typedef struct nComponentArray nComponentArray;
struct nComponentArray {
    int elem_size;  // total size of each component
    void *data;     // allocated data
};

typedef struct nEntityFreeSlotNode nEntityFreeSlotNode;
struct nEntityFreeSlotNode {
    nEntityID id;
    nEntityFreeSlotNode *next;
};

typedef struct nEntityMgr nEntityMgr;
#define NENTITY_MANAGER_TOP_PRIORITY 0
#define NENTITY_MANAGER_BOTTOM_PRIORITY 10
typedef struct nEntityMgrSystemNode nEntityMgrSystemNode;
struct nEntityMgrSystemNode {
    void (*func)(nEntityMgr *em);
    u32 priority;
    nEntityMgrSystemNode *next; 
    nEntityMgrSystemNode *prev; 
};

struct nEntityMgr {
    nComponentArray components[NMAX_COMPONENTS];
    nComponentMask  *bitset; // indicates whether nEntityID i has component j
    // TODO -- this is no updated
    u32 comp_array_len;
    u32 comp_array_cap;
    // entity index reuse
    nEntityFreeSlotNode *available_slots; // ready to be reused slots
    nEntityFreeSlotNode *free_slots; // free slot memory we can use
    // system management
    nEntityMgrSystemNode *systems_first;
    nEntityMgrSystemNode *systems_last;
    nEntityMgrSystemNode *free_system_nodes; // free nodes we can use instead of allocating
};

nEntityID nem_make();
void nem_del(nEntityMgr *em, nEntityID entity);

#include "comp_def.inl"

#define NENTITY_MANAGER_INIT(em)        \
    do {                                \
        (em)->comp_array_len = 0;           \
        (em)->comp_array_cap = NMAX_ENTITIES;\
        (em)->bitset = push_array(get_global_arena(), u32, NMAX_ENTITIES);\
    } while (0)

#define NENTITY_MANAGER_COMPONENT_REGISTER(em, comp_type)       \
    do {                                                                \
        int index = GetTypeID_##comp_type();                            \
        if (index >= NMAX_COMPONENTS) {                                 \
            assert(0 && "Component index out of bounds");               \
        }                                                               \
        u32 elem_size = sizeof(comp_type);                              \
        (em)->components[index].elem_size = elem_size;                  \
        (em)->components[index].data = push_array_nz(get_global_arena(), comp_type, elem_size*NMAX_ENTITIES);\
        if (!(em)->components[index].data) {                             \
            assert(0 && "Cant allocate component memory");              \
        }                                                               \
    } while (0)
    
#define NENTITY_MANAGER_HAS_COMPONENT(em, entity, comp_type) NCOMPONENT_MASK_GET_SLOT((em)->bitset[NENTITY_GET_INDEX(entity)],GetTypeID_##comp_type())    

#define NENTITY_MANAGER_ADD_COMPONENT(em, entity, comp_type)    \
    do {                                                                \
        int index = GetTypeID_##comp_type();                            \
        if (index >= NMAX_COMPONENTS || (em)->components[index].data == NULL) { \
            assert(0 && "Component type not registered");               \
        }                                                               \
        if (NENTITY_GET_INDEX(entity) >= NMAX_ENTITIES) {                                  \
            assert(0 && "invalid entity id");                           \
        }                                                               \
        if (NENTITY_MANAGER_HAS_COMPONENT(em,entity,comp_type)) {      \
            NLOG_ERR("Component <%s> already set for entity %d", #comp_type, entity);\
        } else {                                                               \
            void *dest = (char *)(em)->components[index].data + NENTITY_GET_INDEX(entity) * (em)->components[index].elem_size; \
            memset(dest, 0, (em)->components[index].elem_size);                  \
            NCOMPONENT_MASK_SET_SLOT((em)->bitset[NENTITY_GET_INDEX(entity)],index); \
        } \
    } while (0)

#define NENTITY_MANAGER_GET_COMPONENT(em, entity, comp_type) (comp_type *)((char *)(em)->components[GetTypeID_##comp_type()].data + NENTITY_GET_INDEX(entity) * (em)->components[GetTypeID_##comp_type()].elem_size)
#define NENTITY_MANAGER_DEL_COMPONENT(em, entity, comp_type) NCOMPONENT_MASK_DEL_SLOT((em)->bitset[NENTITY_GET_INDEX(entity)],GetTypeID_##comp_type())    

#define NENTITY_MANAGER_ADD_SYSTEM(em, cb, prio) \
    do {                                                     \
        nEntityMgrSystemNode *node = (em)->free_system_nodes; \
        if (node) {   \
            sll_stack_pop((em)->free_system_nodes);   \
        } else { \
            node = push_array(get_global_arena(), nEntityMgrSystemNode, 1); \
        } \
        node->func = cb; \
        node->priority = prio; \
        dll_push_back((em)->systems_first, (em)->systems_last, node); \
    } while (0)

#define NENTITY_MANAGER_DEL_SYSTEM(em, cb) \
    do { \
        for (nEntityMgrSystemNode *node = (em)->systems_first; node != 0; node = node->next) { \
            if (node->func == cb){ \
                dll_remove((em)->systems_first, (em)->systems_last, node); \
                M_ZERO_STRUCT(node); \
                sll_stack_push((em)->free_system_nodes, node); \
                break; \
            } \
        } \
    } while (0)

void entity_test();
nEntityMgr* get_em();
void nem_update(nEntityMgr *em);

#endif