#include "ecs/entity.h"

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
        NLOG_ERR("entity %llu created/reused", id);
        em->entity[NENTITY_GET_INDEX(id)] = id;
        return id;
    }
    NLOG_ERR("entity %llu created", entity_count);
    em->entity[entity_count] = entity_count;
    em->comp_array_len = entity_count+1;
    return entity_count++;
}

// Delete an entity, we insert the ID as a free slot so we can reuse it later
void nem_del(nEntityMgr *em, nEntityID entity) {
    // clear the bitset (entity has no components now)
    em->bitset[NENTITY_GET_INDEX(entity)] = 0;
    em->entity[NENTITY_GET_INDEX(entity)] = NENTITY_INVALID_ID;
    nEntityFreeSlotNode *next_free_slot = nem_get_free_entity_slot(em);
    next_free_slot->id = NENTITY_INCREMENT_GENERATION(entity);
    sll_stack_push(em->available_slots, next_free_slot);
    NLOG_ERR("entity %llu destroyed", entity);
    NLOG_ERR("entity %llu added to cache", next_free_slot->id);
}

void nem_update(nEntityMgr *em) {
    nentity_event_mgr_clear(&em->event_mgr);
    for (u32 prio = NENTITY_MANAGER_TOP_PRIORITY; prio <= NENTITY_MANAGER_BOTTOM_PRIORITY; prio+=1) {
        for (nEntityMgrSystemNode *node = em->systems_first; node != 0; node = node->next) {
            if (prio == node->priority) {
                node->func(em);
            }
        }
    }
}

void update_first(nEntityMgr *em) {
    // for every entity
    for (u32 i = 0; i < em->comp_array_len; i+=1) {
        nEntityID entity = i;
        if (NENTITY_MANAGER_HAS_COMPONENT(em, entity, nSprite)) {
            NLOG_INFO("Entity <%d> has Sprite component!", entity);
        } else {
            NLOG_INFO("Entity <%d> doesnt have Sprite component!", entity);
        }
    }
}

void update_second(nEntityMgr *em) {
    NLOG_INFO("I came seconde!");
}

void entity_test() {
    NENTITY_MANAGER_INIT(get_em());

    NENTITY_MANAGER_COMPONENT_REGISTER(get_em(), nPhysicsBody);
    NENTITY_MANAGER_COMPONENT_REGISTER(get_em(), nSprite);
    nEntityID id = nem_make(get_em()); // create entity
    NENTITY_MANAGER_ADD_COMPONENT(get_em(), id, nSprite); // add component
    NENTITY_MANAGER_ADD_COMPONENT(get_em(), id, nSprite); // add component (this will fail)
    nSprite *p = (nSprite*)NENTITY_MANAGER_GET_COMPONENT(get_em(), id, nSprite); //modify component data
    p->color.x = 1.0;
    p = NENTITY_MANAGER_GET_COMPONENT(get_em(), id, nSprite);
    printf("Sprite color : (%f, %f, %f, %f)\n", p->color.x, p->color.y, p->color.z, p->color.w);
    assert(p->color.x == 1.0);

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
    NENTITY_MANAGER_ADD_SYSTEM(get_em(), update_second, 2);
    NENTITY_MANAGER_ADD_SYSTEM(get_em(), update_first, 1);
    nem_update(get_em());
    NENTITY_MANAGER_DEL_SYSTEM(get_em(), update_first);
    nem_update(get_em());
}

b32 nem_entity_valid(nEntityMgr *em, nEntityID entity) {
    return (em->entity[NENTITY_GET_INDEX(entity)] == entity);
}

void nentity_event_mgr_clear(nEntityEventMgr *ev_mgr) {
    ev_mgr->first = 0;
    ev_mgr->last = 0;
}

void nentity_event_mgr_add(nEntityEventMgr *ev_mgr, nEntityEvent e) {
    nEntityEventNode *node = push_array(get_frame_arena(), nEntityEventNode, 1);
    node->e = e;
    sll_queue_push(ev_mgr->first, ev_mgr->last, node);
}