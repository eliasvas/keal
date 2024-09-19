#include "entity.h"

nEntityMgr em = {0};

nEntityMgr* get_em() {
    return &em;
}

nEntityFreeSlotNode* nem_get_available_entity_slot(nEntityMgr *em) {
    nEntityFreeSlotNode *node = em->available_slots;
    if (node) {
        sll_stack_pop(em->available_slots);
    }
    return node;
}
nEntityFreeSlotNode* nem_get_free_entity_slot(nEntityMgr *em) {
    nEntityFreeSlotNode *node = em->free_slots;
    if (node) {
        sll_stack_pop(em->free_slots);
    } else {
        node = push_array(get_global_arena(), nEntityFreeSlotNode, 1);
    }
    return node;
}

// Create an entity, we first try to get an empty slot, if not found we make a new one
nEntityID nem_make(nEntityMgr *em) {
    static int entity_count = 0;
    // reuse an entity slot if possible
    nEntityFreeSlotNode *slot = nem_get_available_entity_slot(em);
    if (slot) {
        nEntityID id = slot->id;
        M_ZERO_STRUCT(slot);
        sll_stack_push(em->free_slots, slot);
        return id;
    }
    return entity_count++;
}

// Delete an entity, we insert the ID as a free slot so we can reuse it later
void nem_del(nEntityMgr *em, nEntityID entity) {
    // clear the bitset (entity has no components now)
    em->bitset[NENTITY_GET_INDEX(entity)] = 0;
    nEntityFreeSlotNode *next_free_slot = nem_get_free_entity_slot(em);
    next_free_slot->id = NENTITY_INCREMENT_GENERATION(entity);
    sll_stack_push(em->available_slots, next_free_slot);
}

void entity_test() {
    NENTITY_MANAGER_INIT(get_em());

    NENTITY_MANAGER_COMPONENT_REGISTER(get_em(), Position);
    NENTITY_MANAGER_COMPONENT_REGISTER(get_em(), Health);
    nEntityID id = nem_make(get_em()); // create entity
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

    for (u32 i = 0; i < 10; i+=1) {
        nEntityID entity = nem_make(get_em());
        assert(NENTITY_GET_INDEX(entity) == 1);
        assert(NENTITY_GET_GENERATION(entity) == i);
        nem_del(get_em(), entity);
    }
}