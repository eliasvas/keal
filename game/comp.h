#ifndef COMPONENTS_H
#define COMPONENTS_H 
#include "engine.h"

typedef struct nHealthComponent nHealthComponent;
struct nHealthComponent {
    s32 hlt;
    s32 max_hlt;
};
nHealthComponent nhealth_component_make(s32 max_health);
b32 nhealth_component_alive(nHealthComponent *hc);
void nhealth_component_enc(nHealthComponent *hc);
void nhealth_component_dec(nHealthComponent *hc);

typedef struct nAIComponent nAIComponent;
struct nAIComponent {
    u32 state;
    u64 timestamp;
    u64 invinsibility_sec;
    b32 dead;
};
#define NAI_COMPONENT_DEFAULT 0

nAIComponent nai_component_default(void);

void game_ai_system(nEntityMgr *em, void *ctx);
void render_sprites_system(nEntityMgr *em, void *ctx);
void resolve_collision_events(nEntityMgr *em, void *ctx);

#endif