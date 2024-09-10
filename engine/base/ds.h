#ifndef DS_H
#define DS_H

#include "base_inc.h"

#define sll_stack_push_N(f,n,next) ((n)->next=(f), (f)=(n))
#define sll_stack_pop_N(f,next) ((f==0)?((f)=(f)):((f)=(f)->next))
#define sll_stack_push(f,n) sll_stack_push_N(f,n,next)
#define sll_stack_pop(f) sll_stack_pop_N(f,next)

#define sll_queue_push_N(f,l,n,next) (((f)==0)?((f)=(l)=(n)):((l)->next=(n),(l)=(n),(n)->next=0))
#define sll_queue_pop_N(f,l,next) (((f)==(l))?((f)=(l)=0):((f)=(f)->next))
#define sll_queue_push(f,l,n) sll_queue_push_N(f,l,n,next)
#define sll_queue_pop(f,l) sll_queue_pop_N(f,l,next)

#define check_zero(z,p) ((p) == 0 || (p) == z)
#define set_zero(z,p) ((p) = z)
#define dll_insert_NPZ(z,f,l,p,n,next,prev) (check_zero(z,f) ? ((f) = (l) = (n), set_zero(z,(n)->next), set_zero(z,(n)->prev)) : check_zero(z,p) ? ((n)->next = (f), (f)->prev = (n), (f) = (n), set_zero(z,(n)->prev)) : ((p)==(l)) ? ((l)->next = (n), (n)->prev = (l), (l) = (n), set_zero(z, (n)->next)) : (((!check_zero(z,p) && check_zero(z,(p)->next)) ? (0) : ((p)->next->prev = (n))), ((n)->next = (p)->next), ((p)->next = (n)), ((n)->prev = (p))))
#define dll_push_back_NPZ(z,f,l,n,next,prev) dll_insert_NPZ(z,f,l,l,n,next,prev)
#define dll_push_back_NP(f,l,n,next,prev) dll_push_back_NPZ(0,f,l,n,next,prev)
#define dll_push_back(f,l,n) dll_push_back_NP(f,l,n,next,prev)
#define dll_push_front_NPZ(z,f,l,n,next,prev) dll_insert_NPZ(z,l,f,f,n,prev,next)
#define dll_push_front_NP(f,l,n,next,prev) dll_push_front_NPZ(0,f,l,n,next,prev)
#define dll_push_front(f,l,n) dll_push_back_NP(l,f,n,prev,next)
#define dll_remove_NPZ(z,f,l,n,next,prev) (((n) == (f) ? (f) = (n)->next : (0)), ((n) == (l) ? (l) = (l)->prev : (0)), (check_zero(z,(n)->prev) ? (0) : ((n)->prev->next = (n)->next)), (check_zero(z,(n)->next) ? (0) : ((n)->next->prev = (n)->prev)))
#define dll_remove_NP(f,l,n,next,prev) dll_remove_NPZ(0,f,l,n,next,prev)
#define dll_remove(f,l,n) dll_remove_NP(f,l,n,next,prev)

typedef struct TestNode TestNode;
struct TestNode {TestNode*next;TestNode*prev;int data;};
static u64 get_ll_sum(TestNode *node) { u64 sum = 0;for (;node!=NULL;node=node->next){sum+=node->data;} return sum; }

#if ENABLE_TESTS
static void sll_stack_test() {
    ArenaTemp temp = arena_get_scratch(0);
	u32 sum = 0;
	u32 last_prime=0;

	TestNode *head=NULL;

	for (u32 i = 0; i < 10; ++i){
		TestNode *node = push_array(temp.arena, TestNode, 1);
		node->data = i;
		if (i%2==0){
			sll_stack_push(head,node);
			sum+=i;
			last_prime=i;
		}
	}
	assert(get_ll_sum(head) == sum);
	sll_stack_pop(head);
	assert(get_ll_sum(head) == sum-last_prime);

	printf("SLL stack test finished succesfully\n");
    arena_end_temp(&temp);
}

static void sll_queue_test() {
    ArenaTemp temp = arena_get_scratch(0);
	u32 sum = 0;
	u32 first_prime = 0;

	TestNode *first=NULL;
	TestNode *last=NULL;

	for (u32 i = 0; i < 10; ++i){
		TestNode *node = push_array(temp.arena, TestNode, 1);
		node->data = i;
		if (i%2==0){
			sll_queue_push(first,last,node);
			sum+=i;
		}
	}
	assert(get_ll_sum(first) == sum);
	sll_queue_pop(first,last);
	assert(get_ll_sum(first) == (sum-first_prime));

	printf("SLL queue test finished succesfully\n");
    arena_end_temp(&temp);
}

static void dll_test() {
    ArenaTemp temp = arena_get_scratch(0);
	u32 sum = 0;

	u32 prime_to_delete = 4;
	TestNode *prime_to_delete_node=NULL;

	TestNode *first=NULL;
	TestNode *last=NULL;

	for (u32 i = 0; i < 10; ++i){
		TestNode *node = push_array(temp.arena, TestNode, 1);
		node->data = i;
		if (i%2==0){
			dll_push_back(first,last,node);
			sum+=i;
			if (i == 4){
				prime_to_delete_node = node;
			}
		}
	}
	assert(get_ll_sum(first) == sum);
	dll_remove(prime_to_delete_node->prev,prime_to_delete_node->next,prime_to_delete_node);
	assert(get_ll_sum(first) == (sum-prime_to_delete));

	printf("DLL test finished succesfully\n");
    arena_end_temp(&temp);
}
#endif



#endif