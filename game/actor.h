#ifndef ACTOR_H
#define ACTOR_H
#include "engine.h"
#include "map.h"

////////////////////////////////
// Composition-based nActor attributes
////////////////////////////////

typedef struct nDestructibleData nDestructibleData;
struct nDestructibleData {
    s32 max_hp;
    s32 hp;
    s32 def;

    // Should these really be here??
    f32 shake_duration;
    f32 shake_str;
};
nDestructibleData ndestructible_data_make(s32 max_hp, s32 def, f32 shake_str);

typedef struct nAttackData nAttackData;
struct nAttackData {
    s32 powa;
};
nAttackData nattack_data_make(s32 powa);

#define NMAX_ITEMS 6
typedef struct nActorContainer nActorContainer;
typedef struct nActorComponent nActorComponent; // this is UGLY AF, basically mutated struct declaration
struct nActorContainer {
    nActorComponent *items[NMAX_ITEMS];
    u8 item_count;
    u8 current_selected_item_index;
};

////////////////////////////////
// nActor component
////////////////////////////////

typedef enum nActorFeatureFlags nActorFeatureFlags;
enum nActorFeatureFlags {
    NACTOR_FEATURE_FLAG_DESTRUCTIBLE  = (1 << 0), // Can take damage and break/die
    NACTOR_FEATURE_FLAG_ATTACKER      = (1 << 1), // Can damage destructibles
    NACTOR_FEATURE_FLAG_AI            = (1 << 2), // It's self updating
    NACTOR_FEATURE_FLAG_SHAKEABLE     = (1 << 3), // It will Shake!
    NACTOR_FEATURE_FLAG_PICKABLE      = (1 << 4), // Actor is pickable
    NACTOR_FEATURE_FLAG_HAS_CONTAINER = (1 << 5), // Actor can pick up things
};

typedef enum nActorKind nActorKind;
enum nActorKind {
    NACTOR_KIND_PLAYER,
    NACTOR_KIND_ENEMY,
    NACTOR_KIND_NPC,
    NACTOR_KIND_OTHER,
    NACTOR_KIND_ITEM,
    NACTOR_KIND_DOOR,
    NACTOR_KIND_COUNT,
};

typedef struct nActorComponent nActorComponent;
struct nActorComponent {
    s32 posx, posy;
    vec4 tc;
    vec4 color;
    nActorKind kind;
    nActorFeatureFlags flags;
    nDestructibleData d;
    nAttackData a;
    nActorContainer c;
    b32 blocks; // Is this actor blocking movement??
    u8 name[64];
    b32 active;
};

////////////////////////////////
// nActor component manager
////////////////////////////////

// CM = ComponentManager
#define NACTOR_CM_MAX_COMPONENTS 1024
typedef struct nActorCM nActorCM;
struct nActorCM {
    // The internal, allocated space
    u32 size, cap;
    void *buf;

    // The arrays assigned
    nActorComponent *actors;
    nEntity *entity;

    // lookup table
    u32 lookup_table_size;
    nEntityComponentIndexPairHashSlot *lookup_table;
    // To store deleted IndexPairs
    nEntityComponentIndexPairNode *free_nodes;

    // ref to parent nEntityManager (who does allocations for now)
    nEntityManager *em_ref;
};

void nactor_cm_init(nActorCM *cm, nEntityManager *em);
void nactor_cm_deinit(nActorCM *cm, nEntityManager *em);
nCompIndex nactor_cm_lookup(nActorCM *cm, nEntity e);
nEntity nactor_cm_lookup_entity_from_ptr(nActorCM *cm, nActorComponent *cmp);
nActorComponent *nactor_cm_add(nActorCM *cm, nEntity e);
nActorComponent *nactor_cm_get(nActorCM *cm, nEntity e);
void nactor_cm_del(nActorCM *cm, nEntity e);
void nactor_cm_render(nActorCM *cm, nBatch2DRenderer *rend, oglImage *atlas);
void nactor_cm_simulate(nActorCM *cm, nMap *map, b32 new_turn);
void nactor_cm_clear(nActorCM *cm);
b32 nactor_cm_check_movement_event(nActorCM *cm);
void nactor_attack(nActorComponent *attacker, nActorComponent *victim);
s32 nactor_pick_up_item(nActorComponent *ac, nActorComponent *item);
void nactor_use_item(nActorComponent *ac, u8 item_index);


#endif