// Due to some problems with component definitions, we just define them here and product the type info
typedef u64 nEntityTag;
REGISTER_TYPE(nEntityTag)
typedef struct nPhysicsBody nPhysicsBody;
REGISTER_TYPE(nPhysicsBody)
typedef struct nSprite nSprite;
REGISTER_TYPE(nSprite)
typedef struct nHealthComponent nHealthComponent;
REGISTER_TYPE(nHealthComponent)
typedef struct nAIComponent nAIComponent;
REGISTER_TYPE(nAIComponent)
typedef struct nKealotineData nKealotineData;
REGISTER_TYPE(nKealotineData)


// Should these be here?? Maybe we should use this just in gameplay code
typedef enum NENTITY_TAG NENTITY_TAG; 
enum NENTITY_TAG{
    NENTITY_TAG_PLAYER    = (1 << 0),
    NENTITY_TAG_ENEMY     = (1 << 1),
    NENTITY_TAG_DOOR      = (1 << 2),
    NENTITY_TAG_MAP       = (1 << 3),
    NENTITY_TAG_KEALOTINE = (1 << 4),
};

typedef enum nEntityEventKind nEntityEventKind;
enum nEntityEventKind {
    NENTITY_EVENT_KIND_COLLISION,
    NENTITY_EVENT_KIND_SPAWN,
    NENTITY_EVENT_KIND_DEATH,
};