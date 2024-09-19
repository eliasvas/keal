// Due to some problems with component definitions, we just define them here and product the type info
typedef struct Position Position; struct Position { int x; int y; };
REGISTER_TYPE(Position)
typedef struct Health Health; struct Health { int hlt; };
REGISTER_TYPE(Health)