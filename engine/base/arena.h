#ifndef ARENA_H_
#define ARENA_H_
// ref: https://git.mr4th.com/mr4th-public/mr4th/src/branch/main/src/base/base_big_functions.c
#include "core_inc.h"

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

#define M_ARENA_INITIAL_COMMIT KB(4)
#define M_ARENA_MAX_ALIGN 64
#define M_ARENA_DEFAULT_RESERVE_SIZE GB(1)
#define M_ARENA_COMMIT_BLOCK_SIZE MB(64)
#define M_ARENA_INTERNAL_MIN_SIZE align_pow2(sizeof(Arena), M_ARENA_MAX_ALIGN)

static Arena* arena_alloc_reserve(u64 reserve_size) {
	Arena *arena = NULL;
	if (reserve_size >= M_ARENA_INITIAL_COMMIT) {
		void *mem = M_RESERVE(reserve_size);
		if (M_COMMIT(mem, M_ARENA_INITIAL_COMMIT)) {
			arena = (Arena*)mem;
			arena->curr = arena;
			arena->prev = 0;
			arena->alignment = sizeof(void*);
			arena->base_pos = 0;
			arena->growable = 1;
			arena->chunk_cap = reserve_size;
			arena->chunk_pos = M_ARENA_INTERNAL_MIN_SIZE;
			arena->chunk_commit_pos = M_ARENA_INITIAL_COMMIT;
		}
	}
	assert(arena != NULL);
	return arena;
}
static Arena* arena_alloc() {
	return arena_alloc_reserve(M_ARENA_DEFAULT_RESERVE_SIZE);
}
static void arena_release(Arena *arena) {
	//M_RELEASE(arena, arena->chunk_cap);
	//Arena *curr = arena->curr;
	for(Arena *curr = arena->curr; curr != NULL; curr = curr->prev) {
		M_RELEASE(curr, curr->chunk_cap);
	}
	M_ZERO_STRUCT(arena);
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
				// because 4KB is recommended page size for most currect Architectures
				new_reserved_size = align_pow2(least_size, MB(4));
			}
			void *mem = M_RESERVE(new_reserved_size);
			if (M_COMMIT(mem, M_ARENA_INITIAL_COMMIT)) {
				Arena *new_chunk_arena = (Arena*)mem;
				new_chunk_arena->curr = new_chunk_arena;
				new_chunk_arena->prev = curr;
				new_chunk_arena->base_pos = curr->base_pos + curr->chunk_cap;
				new_chunk_arena->chunk_cap = new_reserved_size;
				new_chunk_arena->chunk_pos = M_ARENA_INTERNAL_MIN_SIZE;
				new_chunk_arena->chunk_commit_pos = M_ARENA_INITIAL_COMMIT;
				curr = new_chunk_arena;
				arena->curr = new_chunk_arena;
			}
		}
	}

	// assumming we have enough free space ( < chunk_cap)
	u64 result_pos = align_pow2(curr->chunk_pos, arena->alignment);
	u64 next_chunk_pos = result_pos + size;
	if (next_chunk_pos <= curr->chunk_cap) {
		if (next_chunk_pos > curr->chunk_commit_pos) {
			u64 next_commit_pos_aligned = align_pow2(next_chunk_pos, M_ARENA_COMMIT_BLOCK_SIZE);
			u64 next_commit_pos = minimum(next_commit_pos_aligned,curr->chunk_cap);
			u64 commit_size = next_commit_pos - curr->chunk_commit_pos;
			if (M_COMMIT((u8*)curr + curr->chunk_commit_pos, commit_size)) {
				curr->chunk_commit_pos = next_commit_pos;
			}
		}
	}

	// if allocation successful, return the pointer
	if (next_chunk_pos <= curr->chunk_commit_pos) {
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

static void* arena_pop_to_pos(Arena *arena, u64 pos) {
	Arena *curr = arena->curr;
	u64 total_pos = arena_current_pos(curr);
	// release chunks that BEGIN after this pos
	if (pos < total_pos) {
		// We need at least M_ARENA_INTERNAL_MIN_SIZE of allocation in our arena (for the Arena* at least)
		u64 clamped_total_pos = maximum(pos, M_ARENA_INTERNAL_MIN_SIZE);
		for(;clamped_total_pos < pos;) {
			Arena *prev = curr->prev;
			M_RELEASE(curr, curr->chunk_cap);
			curr = prev;
		}
		arena->curr = curr;
		u64 chunk_pos = clamped_total_pos - curr->base_pos;
		u64 clamped_chunk_pos = maximum(chunk_pos, M_ARENA_INTERNAL_MIN_SIZE);
		curr->chunk_pos = clamped_chunk_pos;
	}
	return NULL;
}
static void* arena_pop_amount(Arena *arena, u64 amount) {
	Arena *curr = arena->curr;
	u64 total_pos = arena_current_pos(curr);
	if (amount <= total_pos) {
		u64 new_pos = total_pos - amount;
		arena_pop_to_pos(arena, new_pos);
	}
	// FIXME -- WHY do we return NULL here?????????
	return NULL;
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

// static thread_loc Arena *m__scratch_pool[2] = {0};

// static ArenaTemp arena_get_scratch(Arena *conflict) {

// 	// init the scratch pool at the first time
// 	if (m__scratch_pool[0] == 0) {
// 		Arena **scratch_slot = m__scratch_pool;
// 		for (u32 i = 0; i < 2; i+=1, scratch_slot+=1) {
// 			*scratch_slot = arena_alloc();
// 		}
// 	}

// 	// return the non conflicting arena from pool
// 	ArenaTemp res = {0};
// 	Arena **scratch_slot = m__scratch_pool;
// 	for (u32 i = 0; i < 2; i+=1, scratch_slot+=1) {
// 		if (*scratch_slot == conflict){
// 			continue;
// 		}
// 		res = arena_begin_temp(*scratch_slot);
// 	}

// 	return res;
// }

//static void arena_test() { printf("------Arena test!-----\n"); printf("---------------------\n"); ArenaTemp temp = arena_get_scratch(NULL); Arena *arena = arena_alloc(); u8 arr[5560]; u8 *mem = arena_push_nz(arena, kilobytes(1)); memcpy(mem, arr, 2560); mem = arena_push(arena, gigabytes(0.1)); printf("arena_current_pos=[%lud]", arena_current_pos(arena)); ArenaTemp t = arena_begin_temp(arena); void *large_mem = arena_push(arena, gigabytes(1)); printf("\nafter [10GB] arena_current_pos=[%lud]", arena_current_pos(arena)); printf("\nafter [10GB] temp_arena_current_pos=[%lud]", arena_current_pos(t.arena)); arena_end_temp(&t); printf("\nafter [POP] arena_current_pos=[%lud]\n", arena_current_pos(arena)); for (int i = 0; i < 9; ++i) { mem[i] = '0'+(9 - i); } printf("%s\n", &mem[0]); arena_end_temp(&temp); printf("---------------------\n"); }
/*
// WHY we need to pass conflict arena in arena_get_scratch(..);
void* bar(Arena *arena){
    // this should be arena_get_scratch(arena) to get the other scratch arena, and not foo's
    ArenaTemp temp = arena_get_scratch(0);
    // we allocate memory on 'arena' allocator which is the same as foo's
    u8 *mem = arena_push(arena, kilobytes(1));
    memcpy(mem, "Hello bar\n", strlen("Hello bar\n"));
    // some BS allocation we need to do with temp
    void *bs_allocation = arena_push(temp.arena, megabytes(2));
    // This will free temp, but ALSO, because temp == arena, arena will be freed and our data (Hello bar) will be invalid
    arena_end_temp(&temp);
    return mem;
}
void foo(){
    ArenaTemp temp = arena_get_scratch(0);
    void *mem_old = bar(temp.arena);

    u8 *mem = arena_push(temp.arena, kilobytes(1));
    memcpy(mem, "Hello foo\n", strlen("Hello foo\n"));

    // Because our bar() function popped temp allocator, 'Hello foo' will OVERWRITE 'Hello bar'
    // And it will be the output of this printf, thats why we need to pass conflict arenas in bar
    printf("%s", mem_old);

    arena_end_temp(&temp);
}
*/

b32 arena_test(void) {
    Arena *a = arena_alloc();
    u32 *mem = push_array(a,u32,100000);
    assert(mem);
    AsanPoison(mem, 100);
    //This will cause a crash I think
    mem[0] = 5;

    return 1;
}

#endif