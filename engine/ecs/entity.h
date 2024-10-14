#ifndef NENTITY_H
#define NENTITY_H

#include "base/base_inc.h"
#include "core/core_inc.h"

/*
    Pretty much, nEntity entity = nem_make(em) will make a new ID which will be used as index for all the entities components,
    after that you can NENTITY_COMPONENT_MANAGER_ADD_COMPONENT(..) and set appropriate values, also use NENTITYMANAGER_ADD_SYSTEM(..),
    when nem_del(em, entity) happens, the new ID will be added to a stack with .generation+1 to be used in case a new is instantiated 
    and a free ID is needed, if the available id stack is empty a new id is generated. When level done and we no longer need entities,
    just do NENTITY_COMPONENT_MANAGER_CLEAR(em) which will clear everything, except arrays will still be allocated (for now)

    BEWARE -- right now initialization is done ONCE + all allocations (via NENTITY_MANAGER_INIT(..)/NENTITY_MANAGER_REGISTER(..)),
    BEWARE -- and memory is fixed to hold a maximum of NMAX_ENTITIES, we need to make Pool Allocators to bypass this.
    TODO -- maybe instead of pointers we return CompontentHandle's and provide a Component *ch_get(ComponentType, ComponentHandle)
*/

#define NMAX_COMPONENTS 32
#define NMAX_ENTITIES 2048
#define NENTITY_INVALID_ID U32_MAX

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

// nEntityID layout: [ 32b | 32b ] -> [ generation | index ]
typedef u64 nEntityID;
#define NENTITY_GET_INDEX(entity) (entity & U32_MAX)
#define NENTITY_GET_GENERATION(entity) ((entity >> 32) & U32_MAX)
#define NENTITY_INCREMENT_GENERATION(entity) (((NENTITY_GET_GENERATION(entity)+1)<<32) | NENTITY_GET_INDEX(entity))
#define NENTITY_INCREMENT_INDEX(entity) ((NENTITY_GET_GENERATION(entity) << 32) | (NENTITY_GET_INDEX(entity)+1))


// BEWARE this event queue is allocated on frame storage, so no event caching beyong a single frame is currently supported
// TODO -- can we have specific event listeners/queues and not use one BIG global one (this is slow as fuck)
typedef struct nEntityEvent nEntityEvent;
struct nEntityEvent {
    nEntityID entity_a;
    nEntityID entity_b; // this is optional
    u32 flags;
    u32 extra_flags;
};
typedef struct nEntityEventNode nEntityEventNode;
struct nEntityEventNode {
    nEntityEvent e;
    nEntityEventNode *next;
};
typedef struct nEntityEventMgr nEntityEventMgr;
struct nEntityEventMgr {
    nEntityEventNode *first;
    nEntityEventNode *last;
};
void nentity_event_mgr_clear(nEntityEventMgr *ev_mgr);
void nentity_event_mgr_add(nEntityEventMgr *ev_mgr, nEntityEvent e);

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
    void (*func)(nEntityMgr *em, void *ctx);
    u32 priority;
    nEntityMgrSystemNode *next; 
    nEntityMgrSystemNode *prev; 
};

struct nEntityMgr {
    nComponentArray components[NMAX_COMPONENTS];
    nComponentMask  *bitset; // indicates whether nEntityID i has component j
    nEntityID *entity; // current active entity for that ID
    u32 entity_count;
    u32 comp_array_len;
    u32 comp_array_cap;
    // for entity index reuse
    nEntityFreeSlotNode *available_slots; // ready to be reused slots
    nEntityFreeSlotNode *free_slots; // free slot memory we can use
    // for system management
    nEntityMgrSystemNode *systems_first;
    nEntityMgrSystemNode *systems_last;
    nEntityMgrSystemNode *free_system_nodes; // free nodes we can use instead of allocating
    // for events
    nEntityEventMgr event_mgr;
};

nEntityID nem_make(nEntityMgr *em);
void nem_del(nEntityMgr *em, nEntityID entity);
b32 nem_entity_valid(nEntityMgr *em, nEntityID entity);

#include "comp_def.inl"

// TODO -- why do we crash with the line below uncommented??!
//#define NENTITY_MANAGER_GET_ENTITY_FOR_INDEX(em, index) ((em)->entity[NENTITY_GET_INDEX(index)])
#define NENTITY_MANAGER_GET_ENTITY_FOR_INDEX(em, index) index

#define NENTITY_MANAGER_INIT(em) \
    do { \
        (em)->entity_count = 0; \
        (em)->comp_array_len = 0; \
        (em)->comp_array_cap = NMAX_ENTITIES; \
        (em)->bitset = push_array(get_global_arena(), u32, NMAX_ENTITIES); \
        (em)->entity = push_array(get_global_arena(), u64, NMAX_ENTITIES); \
    } while (0)

#define NENTITY_MANAGER_CLEAR(em) \
    do { \
        (em)->entity_count = 0; \
        for (u64 i = 0; i < (em)->comp_array_len; i+=1) { \
            nEntityID entity = NENTITY_MANAGER_GET_ENTITY_FOR_INDEX(em,i); \
            nem_del(em, entity); \
        } \
    } while (0)

#define NENTITY_MANAGER_COMPONENT_REGISTER(em, comp_type) \
    do { \
        int index = GetTypeID_##comp_type(); \
        if (index >= NMAX_COMPONENTS) { \
            assert(0 && "Component index out of bounds"); \
        } \
        u32 elem_size = sizeof(comp_type); \
        (em)->components[index].elem_size = elem_size; \
        (em)->components[index].data = push_array_nz(get_global_arena(), comp_type, elem_size*NMAX_ENTITIES); \
        if (!(em)->components[index].data) { \
            assert(0 && "Cant allocate component memory"); \
        } \
    } while (0)
    
#define NENTITY_MANAGER_HAS_COMPONENT(em, entity, comp_type) NCOMPONENT_MASK_GET_SLOT((em)->bitset[NENTITY_GET_INDEX(entity)],GetTypeID_##comp_type())    

#define NENTITY_MANAGER_ADD_COMPONENT(em, entity, comp_type) \
    do { \
        int index = GetTypeID_##comp_type(); \
        if (index >= NMAX_COMPONENTS || (em)->components[index].data == NULL) { \
            assert(0 && "Component type not registered"); \
        } \
        if (!nem_entity_valid(em, entity)) { \
            assert(0 && "entity is not valid anymore! also this check should be in more ops"); \
        } \
        if (NENTITY_GET_INDEX(entity) >= NMAX_ENTITIES) { \
            assert(0 && "invalid entity id"); \
        } \
        if (NENTITY_MANAGER_HAS_COMPONENT(em,entity,comp_type)) { \
            NLOG_ERR("Component <%s> already set for entity %ld", #comp_type, entity); \
        } else { \
            void *dest = (char *)(em)->components[index].data + NENTITY_GET_INDEX(entity) * (em)->components[index].elem_size; \
            memset(dest, 0, (em)->components[index].elem_size); \
            NCOMPONENT_MASK_SET_SLOT((em)->bitset[NENTITY_GET_INDEX(entity)],index); \
        } \
    } while (0)

#define NENTITY_MANAGER_GET_COMPONENT(em, entity, comp_type) (comp_type *)((char *)(em)->components[GetTypeID_##comp_type()].data + NENTITY_GET_INDEX(entity) * (em)->components[GetTypeID_##comp_type()].elem_size)
#define NENTITY_MANAGER_DEL_COMPONENT(em, entity, comp_type) NCOMPONENT_MASK_DEL_SLOT((em)->bitset[NENTITY_GET_INDEX(entity)],GetTypeID_##comp_type())    

#define NENTITY_MANAGER_ADD_SYSTEM(em, cb, prio) \
    do { \
        nEntityMgrSystemNode *node = (em)->free_system_nodes; \
        if (node) { \
            sll_stack_pop((em)->free_system_nodes); \
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

// this ctx is probably just the GameState (we cant expose it here though since we're inside the engine)
void nem_update(nEntityMgr *em, void *ctx);

#endif