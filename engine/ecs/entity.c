#include "entity.h"

nEntityMgr em = {0};

nEntityMgr* get_em() {
    return &em;
}

nEntityID nem_make() {
    static int entity_count = 0;
    return entity_count++;
}

void entity_test() {
    NENTITY_MANAGER_INIT(get_em());

    NENTITY_MANAGER_COMPONENT_REGISTER(get_em(), Position);
    NENTITY_MANAGER_COMPONENT_REGISTER(get_em(), Health);
    nEntityID id = nem_make(); // create entity
    NENTITY_MANAGER_ADD_COMPONENT(get_em(), id, Position); // add component
    NENTITY_MANAGER_ADD_COMPONENT(get_em(), id, Position); // add component (this will fail)
    Position *p = (Position*)NENTITY_MANAGER_GET_COMPONENT(get_em(), id, Position); //modify component data
    p->x = 5;
    p->y = 10;
    p = NENTITY_MANAGER_GET_COMPONENT(get_em(), id, Position);
    printf("Position: (%d, %d)\n", p->x, p->y);
    assert(p->x == 5);

    u32 mask = 0;
    NCOMPONENT_MASK_SET_SLOT(mask, 0);
    assert(mask == 1);
    NCOMPONENT_MASK_SET_SLOT(mask, 1);
    assert(mask == 3);
    assert(NCOMPONENT_MASK_GET_SLOT(mask, 1) == 1);
    NCOMPONENT_MASK_DEL_SLOT(mask, 1);
    assert(NCOMPONENT_MASK_GET_SLOT(mask, 1) == 0);
    assert(mask == 1);
}