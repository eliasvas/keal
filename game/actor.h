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
};
nDestructibleData ndestructible_data_make(s32 max_hp, s32 def);

typedef struct nAttackData nAttackData;
struct nAttackData {
    s32 powa;
};
nAttackData nattack_data_make(s32 powa);

typedef struct nShakeData nShakeData;
struct nShakeData {
    f32 shake_duration;
    f32 shake_str;
};
nShakeData nshake_data_make(f32 shake_duration, f32 shake_str);





////////////////////////////////
// nActor component
////////////////////////////////

typedef enum nActorFeatureFlags nActorFeatureFlags;
enum nActorFeatureFlags {
    NACTOR_FEATURE_FLAG_DESTRUCTIBLE = (1 << 0), // Can take damage and break/die
    NACTOR_FEATURE_FLAG_ATTACKER     = (1 << 1), // Can damage destructibles
    NACTOR_FEATURE_FLAG_AI           = (1 << 2), // It's self updating
    NACTOR_FEATURE_FLAG_SHAKEABLE     = (1 << 3), // It will Shake!
};

typedef enum nActorKind nActorKind;
enum nActorKind {
    NACTOR_KIND_PLAYER,
    NACTOR_KIND_ENEMY,
    NACTOR_KIND_NPC,
    NACTOR_KIND_OTHER,
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
    nShakeData s;
    b32 blocks; // Is this actor blocking movement??
    u8 name[64];
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
nActorComponent *nactor_cm_add(nActorCM *cm, nEntity e);
nActorComponent *nactor_cm_get(nActorCM *cm, nEntity e);
void nactor_cm_del(nActorCM *cm, nEntity e);
void nactor_cm_render(nActorCM *cm, nBatch2DRenderer *rend, oglImage *atlas);
void nactor_cm_simulate(nActorCM *cm, nMap *map, b32 new_turn);
void nactor_cm_clear(nActorCM *cm);
b32 nactor_cm_check_movement_event(nActorCM *cm);
void nactor_attack(nActorComponent *attacker, nActorComponent *victim);


#endif