#include "entity.h"
// ref: https://bitsquid.blogspot.com/2014/08/building-data-oriented-entity-system.html

void nentity_manager_init(nEntityManager *em) {
    M_ZERO_STRUCT(em);
    em->arena = arena_alloc();

    nGenArrayChunk *new_chunk = push_array(em->arena, nGenArrayChunk, 1);
    new_chunk->generation = push_array(em->arena, u8, NGEN_CHUNK_COUNT);
    // This is because id == 0 is a special case, it is the INVALID entity
    new_chunk->gen_count = 1;
    sll_queue_push(em->first, em->last, new_chunk);
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

// TODO -- this code isn't very explanatory! fix dis
u32 nentity_manager_make_new_index(nEntityManager *em) {
    u32 idx;
    u32 running_index = 0;
    b32 index_created = 0;
    for (nGenArrayChunk *chunk = em->first; chunk != 0; chunk = chunk->next) {
        if (chunk->gen_count < NGEN_CHUNK_COUNT) {
            idx = running_index + chunk->gen_count;
            chunk->gen_count+=1;
            index_created = 1;
            break;
        }
        running_index += NGEN_CHUNK_COUNT;
    }
    // if index was not created, there wasn't enough space,
    // we should alloc another chunk and insert the index
    if (!index_created) {
        nGenArrayChunk *new_chunk = push_array(em->arena, nGenArrayChunk, 1);
        new_chunk->generation = push_array(em->arena, u8, NGEN_CHUNK_COUNT);
        new_chunk->gen_count = 1;
        idx = running_index;
        sll_queue_push(em->first, em->last, new_chunk);
    }
    return idx;
}

u8 *nentity_manager_get_generation_for_index(nEntityManager *em, u32 index) {
    u32 running_index = 0;
    for (nGenArrayChunk *chunk = em->first; chunk != 0; chunk = chunk->next) {
        if (index < running_index + chunk->gen_count) {
            return &(chunk->generation[index%NGEN_CHUNK_COUNT]);
        }
        running_index += NGEN_CHUNK_COUNT;
    }
    return 0;
}
u8 nentity_manager_get_generation_val_for_index(nEntityManager *em, u32 index) {
    u8 *gen = nentity_manager_get_generation_for_index(em, index);
    return *gen;
}
void nentity_manager_increment_generation_for_index(nEntityManager *em, u32 index) {
    u8 *gen = nentity_manager_get_generation_for_index(em, index);
    if (gen) {
        *(gen)+=1;
    }
}


nEntity nentity_create(nEntityManager *em) {
    u32 idx;
    if (em->free_indices_count > NENTITY_MIN_FREE_INDICES) {
        nFreeIndexNode *n = sll_queue_pop(em->free_indices_first, em->free_indices_last);
        idx = n->index;
    }else {
        idx = nentity_manager_make_new_index(em);
    }
    return nentity_make(idx, nentity_manager_get_generation_val_for_index(em, idx));
}

b32 nentity_alive(nEntityManager *em, nEntity e) {
    u8 *generation = nentity_manager_get_generation_for_index(em, nentity_index(e));
    return ((generation != 0) && (*generation == nentity_generation(e)));
}

void nentity_destroy(nEntityManager *em, nEntity e) {
    u32 idx = nentity_index(e);
    nentity_manager_increment_generation_for_index(em, idx);

    // push current index in free_index list for reuse
    nFreeIndexNode *free_idx = push_array(em->arena, nFreeIndexNode, 1); 
    free_idx->index = idx;
    sll_queue_push(em->free_indices_first, em->free_indices_last, free_idx);
    em->free_indices_count+=1;
}



void ndebug_name_cm_init(nDebugNameCM *cm, nEntityManager *em) {
    M_ZERO_STRUCT(cm);
    cm->debug_table_size = 4096;
    cm->debug_table = push_array(em->arena, nDebugNameComponentHashSlot, cm->debug_table_size);
    cm->em_ref = em;
}

void ndebug_name_cm_destroy(nDebugNameCM *cm, nEntityManager *em) {
    // Nothing?
}

nDebugNameComponentNode *ndebug_name_cm_lookup(nDebugNameCM *cm, nEntity e) {
    nDebugNameComponentNode *c = 0;
    u32 slot = e%cm->debug_table_size;
    for (nDebugNameComponentNode *node = cm->debug_table[slot].hash_first; node!=0; node = node->next) {
        if (node->entity == e){
            c = node;
            break;
        }
    }
    return c;
}

nDebugNameComponent *ndebug_name_cm_add(nDebugNameCM *cm, nEntity e) {
    //if(ndebug_name_cm_lookup_entity(e).entity)
    u32 slot = e%cm->debug_table_size;
    nDebugNameComponentNode *node = cm->free_nodes;
    if (node) {
        sll_stack_pop(cm->free_nodes);
    } else {
        node = push_array(cm->em_ref->arena, nDebugNameComponentNode, 1);
    }
    M_ZERO_STRUCT(node);
    node->entity = e;
    dll_push_back(cm->debug_table[slot].hash_first, cm->debug_table[slot].hash_last, node);
    return &(node->debug_name);
}

void ndebug_name_cm_del_entity(nDebugNameCM *cm, nEntity e) {
    u32 slot = e%cm->debug_table_size;
    for (nDebugNameComponentNode *node = cm->debug_table[slot].hash_first; node != 0; node = node->next) {
        if (node->entity == e) {
            dll_remove(cm->debug_table[slot].hash_first, cm->debug_table[slot].hash_last, node);
            M_ZERO_STRUCT(node);
            sll_stack_push(cm->free_nodes, node);
            break;
        }
    }
}

void ndebug_name_cm_prune_destroyed_entities(nDebugNameCM *cm) {
    for (u32 slot = 0; slot < cm->debug_table_size; slot+=1){
        for (nDebugNameComponentNode *node = cm->debug_table[slot].hash_first; node!=0; ) {
            nDebugNameComponentNode *next_node = node->next;
            if (!nentity_alive(cm->em_ref, node->entity)) {
                dll_remove(cm->debug_table[slot].hash_first, cm->debug_table[slot].hash_last, node);
                M_ZERO_STRUCT(node);
                sll_stack_push(cm->free_nodes, node);
            }
            node = next_node;
        }
    }
}

void ndebug_name_cm_update(nDebugNameCM *cm) {
    ndebug_name_cm_prune_destroyed_entities(cm);
    // For now we just print stuff
    nDebugNameComponent c = {0};
    for (u32 slot = 0; slot < cm->debug_table_size; slot+=1){
        for (nDebugNameComponentNode *node = cm->debug_table[slot].hash_first; node!=0; node = node->next) {
            printf("entity [%d] has debug name [%s]\n", node->entity, node->debug_name.name);
        }
    }
}









void ntransform_cm_init(nTransformCM *cm, nEntityManager *em) {
    M_ZERO_STRUCT(cm);
    cm->cap = NTRANSFORM_CM_MAX_COMPONENTS;
    cm->size = 0;
    u32 size_of_data = sizeof(nTransformComponent) + sizeof(nEntity) + sizeof(u32)*4;
    cm->em_ref = em;
    cm->buf = arena_push_nz(cm->em_ref->arena, size_of_data * cm->cap); 

    cm->transform = cm->buf;
    cm->entity = (nEntity*)((u8*)cm->transform + (u64)(sizeof(nTransformComponent)*cm->cap));
    cm->parent = (u32*)((u8*)cm->entity + (u64)(sizeof(nEntity)*cm->cap));
    cm->first = (u32*)((u8*)cm->parent + (u64)(sizeof(u32)*cm->cap));
    cm->next = (u32*)((u8*)cm->first + (u64)(sizeof(u32)*cm->cap));
    cm->prev = (u32*)((u8*)cm->next + (u64)(sizeof(u32)*cm->cap));

    cm->lookup_table_size = NTRANSFORM_CM_MAX_COMPONENTS/2;
    cm->lookup_table = push_array(cm->em_ref->arena, nEntityComponentIndexPairHashSlot, cm->lookup_table_size);
}

void ntransform_cm_deinit(nTransformCM *cm, nEntityManager *em) {
    M_ZERO_STRUCT(cm);
}

void ntransform_cm_gc(nTransformCM *cm) {
    // TBA
}

void ntransform_cm_simulate(nTransformCM *cm) {
    // maybe lazily delete destroyed components/entities??
    ntransform_cm_gc(cm);
}

nCompIndex ntransform_cm_make_new_index(nTransformCM *cm) {
    // TODO -- MUTEX stuff
    nCompIndex idx = cm->size;
    M_ZERO_STRUCT(&cm->transform[idx]);
    cm->size+=1;
    return idx;
}

nEntityComponentIndexPairNode *ntransform_cm_lookup_node(nTransformCM *cm, nEntity e) {
    nEntityComponentIndexPairNode *n = 0;
    u32 slot = e%cm->lookup_table_size;
    for (nEntityComponentIndexPairNode *node = cm->lookup_table[slot].hash_first; node != 0; node = node->next) {
        if (node->e == e) {
            n = node;
            break;
        }
    }
    return n;
}

nCompIndex ntransform_cm_lookup(nTransformCM *cm, nEntity e) {
    nEntityComponentIndexPairNode *n = ntransform_cm_lookup_node(cm, e);
    return (n) ? n->comp_idx : NCOMPONENT_INDEX_INVALID;
}


nTransformComponent *ntransform_cm_add(nTransformCM *cm, nEntity e, nEntity p) {
    nCompIndex new_idx = ntransform_cm_make_new_index(cm);
    cm->entity[new_idx] = e;
    cm->parent[new_idx] = NCOMPONENT_INDEX_INVALID;
    cm->next[new_idx] = NCOMPONENT_INDEX_INVALID;
    cm->prev[new_idx] = NCOMPONENT_INDEX_INVALID;
    cm->first[new_idx] = NCOMPONENT_INDEX_INVALID;
    cm->transform[new_idx].local = m4d(1.0);
    cm->transform[new_idx].world = m4d(1.0);

    nCompIndex parent_idx = ntransform_cm_lookup(cm, p);
    if (NCOMPONENT_INDEX_VALID(parent_idx)) {
        cm->parent[new_idx] = parent_idx;
        nCompIndex child_index = cm->first[parent_idx];
        if (!NCOMPONENT_INDEX_VALID(child_index)) {
            cm->first[parent_idx] = new_idx;
        } else {
            for(;NCOMPONENT_INDEX_VALID(cm->next[child_index]);child_index = cm->next[child_index]);
            cm->next[child_index] = new_idx;
            cm->prev[new_idx] = child_index;
        }
    }

    // insert Pair in lookup table
    u32 slot = e%cm->lookup_table_size;
    nEntityComponentIndexPairNode *node = push_array(cm->em_ref->arena, nEntityComponentIndexPairNode, 1);
    node->e = e;
    node->comp_idx = new_idx;
    dll_push_back(cm->lookup_table[slot].hash_first, cm->lookup_table[slot].hash_last, node);

    return &(cm->transform[new_idx]);
}

void ntransform_cm_transform(nTransformCM *cm, mat4 pm, nCompIndex idx) {
    assert(NCOMPONENT_INDEX_VALID(idx));
    cm->transform[idx].world = mat4_mult(pm, cm->transform[idx].local);

    for (nCompIndex child_idx = cm->first[idx]; NCOMPONENT_INDEX_VALID(child_idx); child_idx = cm->next[child_idx]) {
        ntransform_cm_transform(cm, cm->transform[idx].world, child_idx);
    }
}

nTransformComponent *ntransform_cm_set_local(nTransformCM *cm, nCompIndex idx, mat4 local) {
    assert(NCOMPONENT_INDEX_VALID(idx));
    cm->transform[idx].local = local;
    nCompIndex parent_idx = cm->parent[idx];
    mat4 parent_wm = NCOMPONENT_INDEX_VALID(parent_idx) ? cm->transform[parent_idx].world : m4d(1.0); 

    ntransform_cm_transform(cm, parent_wm, idx);
    return &(cm->transform[idx]);
}

nTransformComponent *ntransform_cm_get(nTransformCM *cm, nCompIndex idx) {
    return &(cm->transform[idx]);
}

// Here we should swap the nCompIndex's while ALSO keeping the links intact
void ntransform_move_index(nTransformCM *cm, nCompIndex src, nCompIndex dst) {
    cm->transform[dst] = cm->transform[src];
    cm->entity[dst] = cm->entity[src];
    cm->parent[dst] = cm->parent[src];
    cm->first[dst] = cm->first[src];
    cm->next[dst] = cm->next[src];
    cm->prev[dst] = cm->prev[src];

    // Make prev sibling to point to dst
    nCompIndex prev_sibling_index = cm->prev[dst];
    if (NCOMPONENT_INDEX_VALID(prev_sibling_index)) {
        cm->next[prev_sibling_index] = dst;
    }

    // Make children's parents dst
    for (nCompIndex child_idx = cm->first[dst]; NCOMPONENT_INDEX_VALID(child_idx); child_idx = cm->next[child_idx]) {
        cm->parent[child_idx] = dst;
    }
}
void ntransform_swap_indices(nTransformCM *cm, nCompIndex a, nCompIndex b) {
    // Move element at A (and references to it) to size.
    //[size] <--- [A]
    ntransform_move_index(cm, a, cm->size);

    // Now nothing refers to A, so we can safely move element at B (and references
    // to it) to A.
    //[A] <--- [B]
    ntransform_move_index(cm, b, a);

    // And finally move the element at size to B.
    //[B] <-- [size]
    ntransform_move_index(cm, cm->size, b);

    // Also swap lookup table entries
    nEntityComponentIndexPairNode * a_node = ntransform_cm_lookup_node(cm, cm->entity[a]);
    nCompIndex ai = a_node->comp_idx;
    nEntityComponentIndexPairNode * b_node = ntransform_cm_lookup_node(cm, cm->entity[b]);
    nEntity bi = b_node->comp_idx;
    a_node->comp_idx = bi;
    b_node->comp_idx = ai;
}

// TODO -- see which calls need nEntity and which the nCompIndex
void ntransform_cm_del(nTransformCM *cm, nEntity e) {
    nCompIndex idx = ntransform_cm_lookup(cm, e);
    // first delete all children (post-order)
    for (nCompIndex child_idx = cm->first[idx]; NCOMPONENT_INDEX_VALID(child_idx);) {
        nCompIndex next_idx = cm->next[child_idx];
        ntransform_cm_del(cm, cm->entity[child_idx]);
        child_idx = next_idx;
    }

    // then delete entity e (the parent)
    // swap with last index
    if (cm->size) {
        ntransform_swap_indices(cm, idx, cm->size-1); 
        cm->size-=1;
        // remove lookup table entry
        nEntityComponentIndexPairNode * e_node = ntransform_cm_lookup_node(cm, e);
        u32 slot = e%cm->lookup_table_size;
        dll_remove(cm->lookup_table[slot].hash_first, cm->lookup_table[slot].hash_last, e_node);
    }
}

    // nentity_manager_init(&gs.em);
    // ntransform_cm_init(&gs.tcm, &gs.em);
    // // ECS test
    // {
    //     nEntity parent = nentity_create(&gs.em);
    //     nEntity child = nentity_create(&gs.em);
    //     ntransform_cm_add(&gs.tcm, parent, 0);
    //     ntransform_cm_set_local(&gs.tcm, ntransform_cm_lookup(&gs.tcm, parent), m4d(6.0));
    //     nTransformComponent *c = ntransform_cm_add(&gs.tcm, child, parent);
    //     ntransform_cm_set_local(&gs.tcm, ntransform_cm_lookup(&gs.tcm, child), m4d(2.0));
    //     ntransform_cm_simulate(&gs.tcm);

    //     ntransform_cm_del(&gs.tcm, parent);
    //     assert(!NCOMPONENT_INDEX_VALID(ntransform_cm_lookup(&gs.tcm, parent)));
    //     assert(!NCOMPONENT_INDEX_VALID(ntransform_cm_lookup(&gs.tcm, child)));
    // }
    