// Due to some problems with component definitions, we just define them here and product the type info
typedef struct Position Position; struct Position { int x; int y; };
REGISTER_TYPE(Position)
typedef struct Health Health; struct Health { int hlt; };
REGISTER_TYPE(Health)

// to make a component you can just define its struct and REGISTER_TYPE like here:
typedef struct nPhysicsBody nPhysicsBody;
REGISTER_TYPE(nPhysicsBody)