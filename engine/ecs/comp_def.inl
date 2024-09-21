// Due to some problems with component definitions, we just define them here and product the type info

// Should this be here?? Maybe we should use this just in gameplay code
typedef enum NENTITY_TAG NENTITY_TAG; 
enum NENTITY_TAG{
    NENTITY_TAG_PLAYER = (1 << 0),
    NENTITY_TAG_ENEMY = (1 << 1),
    NENTITY_TAG_MAP = (1 << 2),
};
typedef u64 nEntityTag;
REGISTER_TYPE(nEntityTag)
// to make a component you can just define its struct and REGISTER_TYPE like here:
typedef struct nPhysicsBody nPhysicsBody;
REGISTER_TYPE(nPhysicsBody)

typedef struct nSprite nSprite;
REGISTER_TYPE(nSprite)