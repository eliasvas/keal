#ifndef ARENA_H
#define ARENA_H
// ref: https://git.mr4th.com/mr4th-public/mr4th/src/branch/main/src/base/base_big_functions.c
#include "base_inc.h"

typedef struct Arena Arena;
struct Arena {
	Arena *curr;
	Arena *prev;
	u64 alignment;
	u64 base_pos;
	u64 chunk_cap; // how many bytes current chunk can save
	u64 chunk_pos; // our offset inside that chunk
	u64 chunk_commit_pos; // how much memory in this chunk is CURRENTLY commited
	// for packing in 64-bit offset
	b32 growable;
	u32 trash;
};

typedef struct ArenaTemp ArenaTemp;
struct ArenaTemp {
	Arena *arena;
	u64 pos;
};

#define M_ARENA_INITIAL_COMMIT_SIZE KB(4)
#define M_ARENA_MAX_ALIGN 64
#define M_ARENA_DEFAULT_RESERVE_SIZE GB(1)
#define M_ARENA_COMMIT_BLOCK_SIZE MB(64)
#define M_ARENA_INTERNAL_MIN_SIZE align_pow2(sizeof(Arena), M_ARENA_MAX_ALIGN)

static Arena* arena_alloc_reserve(u64 reserve_size) {
	Arena *arena = NULL;
	if (reserve_size >= M_ARENA_INITIAL_COMMIT_SIZE) {
		void *mem = M_RESERVE(reserve_size);
		if (M_COMMIT(mem, M_ARENA_INITIAL_COMMIT_SIZE)) {
            AsanPoison(mem, M_ARENA_INITIAL_COMMIT_SIZE);
            AsanUnpoison(mem, M_ARENA_INTERNAL_MIN_SIZE);
			arena = (Arena*)mem;
			arena->curr = arena;
			arena->prev = 0;
			arena->alignment = sizeof(void*);
			arena->base_pos = 0;
			arena->growable = 1;
			arena->chunk_cap = reserve_size;
			arena->chunk_pos = M_ARENA_INTERNAL_MIN_SIZE;
			arena->chunk_commit_pos = M_ARENA_INITIAL_COMMIT_SIZE;
		}
	}
	assert(arena != NULL);
	return arena;
}

static Arena* arena_alloc() {
	return arena_alloc_reserve(M_ARENA_DEFAULT_RESERVE_SIZE);
}

static void arena_release(Arena *arena) {
	Arena *curr = arena->curr;
	for(;curr != NULL;) {
        void *prev = curr->prev;
        // TODO -- for some reason, THIS gives us a use-after-poison
        //AsanPoison(curr, curr->chunk_cap);
		M_RELEASE(curr, curr->chunk_cap);
        curr = prev;
	}
	//M_ZERO_STRUCT(arena);
}

static void* arena_push_nz(Arena *arena, u64 size) {
	void *res = NULL;
	Arena *curr = arena->curr;

	// alloc a new chunk if not enough space
	if (arena->growable) {
		u64 next_chunk_pos = align_pow2(curr->chunk_pos, arena->alignment);
		if (next_chunk_pos + size > curr->chunk_cap) {
			u64 new_reserved_size = M_ARENA_DEFAULT_RESERVE_SIZE;
			u64 least_size = M_ARENA_INTERNAL_MIN_SIZE + size;
			if (new_reserved_size < least_size) {
				// because 4KB is recommended page size for most current Architectures
				new_reserved_size = align_pow2(least_size, KB(4));
			}
			void *mem = M_RESERVE(new_reserved_size);
			if (M_COMMIT(mem, M_ARENA_INITIAL_COMMIT_SIZE)) {
                AsanPoison(mem, new_reserved_size);
                AsanUnpoison(mem, M_ARENA_INTERNAL_MIN_SIZE);
				Arena *new_chunk_arena = (Arena*)mem;
				new_chunk_arena->curr = new_chunk_arena;
				new_chunk_arena->prev = curr;
				new_chunk_arena->base_pos = curr->base_pos + curr->chunk_cap;
				new_chunk_arena->chunk_cap = new_reserved_size;
				new_chunk_arena->chunk_pos = M_ARENA_INTERNAL_MIN_SIZE;
				new_chunk_arena->chunk_commit_pos = M_ARENA_INITIAL_COMMIT_SIZE;
				curr = new_chunk_arena;
				arena->curr = new_chunk_arena;
			}
		}
	}

	// assumming we have enough free space ( < chunk_cap)
	u64 result_pos = align_pow2(curr->chunk_pos, arena->alignment);
	u64 next_chunk_pos = result_pos + size;
	if (next_chunk_pos <= curr->chunk_cap) {
        // if we need memory for next_chunk_pos that isn't already commited, commit it
		if (next_chunk_pos > curr->chunk_commit_pos) {
			u64 next_commit_pos_aligned = align_pow2(next_chunk_pos, M_ARENA_COMMIT_BLOCK_SIZE);
			u64 next_commit_pos = minimum(next_commit_pos_aligned,curr->chunk_cap);
			u64 commit_size = next_commit_pos - curr->chunk_commit_pos;
			if (M_COMMIT((u8*)curr + curr->chunk_commit_pos, commit_size)) {
				curr->chunk_commit_pos = next_commit_pos;
			}
		}
	}

	// if allocation succesful, return the pointer
	if (next_chunk_pos <= curr->chunk_commit_pos) {
        //unpoison the memory before returning it
        AsanUnpoison((u8*)curr + curr->chunk_pos, next_chunk_pos - curr->chunk_pos);
		res = (u8*)curr + result_pos;
		curr->chunk_pos = next_chunk_pos;
	}

	return res;
}

static void* arena_push(Arena *arena, u64 size) {
	Arena *curr = arena->curr;
	void *res = arena_push_nz(curr, size);
	M_ZERO(res, size);
	return res;
}

static void arena_align(Arena *arena, u64 p)
{
	assert(is_pow2(p) && p < M_ARENA_MAX_ALIGN);
	Arena *curr = arena->curr;
	u64 current_chunk_pos = curr->chunk_pos;
	u64 current_chunk_pos_aligned = align_pow2(curr->chunk_pos, p);
	u64 needed_space = current_chunk_pos_aligned - current_chunk_pos;
	// This 'if' might not be needed
	if (needed_space > 0) {
		arena_push(curr, needed_space);
	}
}

static u64 arena_current_pos(Arena *arena){
	Arena *curr = arena->curr;
	u64 pos = curr->base_pos + curr->chunk_pos;
	return(pos);
}

static void arena_pop_to_pos(Arena *arena, u64 pos) {
	Arena *curr = arena->curr;
	u64 total_pos = arena_current_pos(curr);
	// release chunks that BEGIN after this pos
	if (pos < total_pos) {
		// We need at least M_ARENA_INTERNAL_MIN_SIZE of allocation in our arena (for the Arena* at least)
		u64 clamped_total_pos = maximum(pos, M_ARENA_INTERNAL_MIN_SIZE);
		for(;clamped_total_pos < curr->base_pos;) {
			Arena *prev = curr->prev;
            AsanPoison(curr, curr->chunk_cap);
			M_RELEASE(curr, curr->chunk_cap);
			curr = prev;
		}
        // arena's curr will become the last arena to have its memory released
		arena->curr = curr;

        // update arena's chunk_pos to only contain up to pop pos
		u64 chunk_pos = clamped_total_pos - curr->base_pos;
		u64 clamped_chunk_pos = maximum(chunk_pos, M_ARENA_INTERNAL_MIN_SIZE);
        AsanPoison((u8*)curr + clamped_chunk_pos, curr->chunk_pos - clamped_chunk_pos);
		curr->chunk_pos = clamped_chunk_pos;
	}
}

static void arena_pop_amount(Arena *arena, u64 amount) {
	Arena *curr = arena->curr;
	u64 total_pos = arena_current_pos(curr);
	if (amount <= total_pos) {
		u64 new_pos = total_pos - amount;
		arena_pop_to_pos(arena, new_pos);
	}
}

static void arena_clear(Arena *arena) {
  arena_pop_to_pos(arena, 0);
}

static ArenaTemp arena_begin_temp(Arena *arena) {
	u64 pos = arena_current_pos(arena);
	ArenaTemp t = {arena, pos};
	return t;
}

static void arena_end_temp(ArenaTemp *t) {
	arena_pop_to_pos(t->arena, t->pos);
}

#define push_array_nz(arena, type, count) (type *)arena_push_nz((arena), sizeof(type)*(count))
#define push_array(arena, type, count) (type *)arena_push((arena), sizeof(type)*(count))

static thread_loc Arena *m__scratch_pool[2] = {0};

static ArenaTemp arena_get_scratch(Arena *conflict) {

	// init the scratch pool at the first time
	if (m__scratch_pool[0] == 0) {
		Arena **scratch_slot = m__scratch_pool;
		for (u32 i = 0; i < 2; i+=1, scratch_slot+=1) {
			*scratch_slot = arena_alloc();
		}
	}

	// return the non conflicting arena from pool
	ArenaTemp res = {0};
	Arena **scratch_slot = m__scratch_pool;
	for (u32 i = 0; i < 2; i+=1, scratch_slot+=1) {
		if (*scratch_slot == conflict){
			continue;
		}
		res = arena_begin_temp(*scratch_slot);
	}

	return res;
}

static void arena_test(void) {
    Arena *a = arena_alloc();
    // allocate small amount of elements and try to access (hopefully no Asan use-after-poison)
    u32 *mem = push_array(a,u32,10);
    u32 *mem2 = mem;
    assert(mem);
    // set LAST u32 to 666
    mem[9] = 666;
    // allocate medium amount of elements and try to access > INITIAL_COMMIT
    mem = push_array(a,u32,M_ARENA_INITIAL_COMMIT_SIZE * 2);
    assert(mem);
    mem[M_ARENA_INITIAL_COMMIT_SIZE*2-1] = 555;
    arena_pop_amount(a, M_ARENA_INITIAL_COMMIT_SIZE*2);
    // assert that previous element is actually the 666 we written earlier
    mem = push_array_nz(a,u32,M_ARENA_DEFAULT_RESERVE_SIZE*2);
    assert(mem);
    arena_pop_amount(a, M_ARENA_DEFAULT_RESERVE_SIZE*2);
    // assert that the first allocation hasn't moved
    assert(mem2[9] == 666);
    // release our arena
    arena_release(a);
    printf("arena test finished succesfully\n");
}

// This!! is why we need to pass a conflict arena to arena_get_scratch,
// problem is that arena is the same as temp.arena! so all helper's memory will be cleared
static u8 *arena_scratch_test_helper(Arena *arena) {
    ArenaTemp temp = arena_get_scratch(arena);
    u8 *mem_to_return = arena_push(arena, KB(1));
    sprintf(mem_to_return, "Hello1234");
    u8 *extra_mem = arena_push(temp.arena, KB(1));
    sprintf(extra_mem, "test_helper");
    arena_end_temp(&temp);
    return mem_to_return;
}
static void arena_scratch_test() {
    ArenaTemp temp = arena_get_scratch(0);
    u8 *mem_from_helper = arena_scratch_test_helper(temp.arena);
    u8 *mem = arena_push(temp.arena, KB(1));
    sprintf(mem, "Hello scratch_test");
    assert(strcmp(mem_from_helper, mem) != 0);
    arena_end_temp(&temp);
    printf("arena scratch test finished succesfully\n");
}

#endif