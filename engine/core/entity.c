#include "entity.h"

void nentity_manager_init(nEntityManager *em) {
    M_ZERO_STRUCT(em);
    em->arena = arena_alloc();
}

void nentity_manager_destroy(nEntityManager *em){
    arena_clear(em->arena);
}

#define NENTITY_MIN_FREE_INDICES 1024

u32 nentity_index(nEntity e) {
    return (e & NENTITY_INDEX_MASK);
}

u32 nentity_generation(nEntity e) {
    return ((e >> NENTITY_INDEX_BITS) & NENTITY_GENERATION_MASK);
}


nEntity nentity_make(u32 idx, u32 generation) {
    return ((generation & NENTITY_GENERATION_MASK) << NENTITY_INDEX_BITS) | (idx & NENTITY_INDEX_MASK);
}

nEntity nentity_create(nEntityManager *em) {
    u32 idx = 0;
    if (em->free_indices_count > NENTITY_MIN_FREE_INDICES) {
        nFreeIndexNode *n = sll_queue_pop(em->free_indices_first, em->free_indices_last);
        idx = n->index;
    }else {
        u8 *new_gen = push_array(em->arena, u8, 1);
        // FIXME -- this is HACKY

        if (!em->generation)em->generation = new_gen;
        idx = em->generation_count;
        em->generation_count+=1;
    }
    return nentity_make(idx, em->generation[idx]);
}

b32 nentity_alive(nEntityManager *em, nEntity e) {
    return (em->generation[nentity_index(e)] == nentity_generation(e));
}

void nentity_destroy(nEntityManager *em, nEntity e) {
    u32 idx = nentity_index(e);
    em->generation[idx]+=1;
    // push current index in free_index list for reuse

    // TODO -- there is a problem here because this list node will be POPED, we should
    // have a free_list that gets these popped nodes to reuse them! (like we do in GUI for guiBoxes)
    nFreeIndexNode *free_idx = push_array(get_global_arena(), nFreeIndexNode, 1); 
    free_idx->index = idx;
    sll_queue_push(em->free_indices_first, em->free_indices_last, free_idx);
    em->free_indices_count+=1;
}