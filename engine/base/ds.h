#ifndef DS_H_
#define DS_H_

#include "core_inc.h"

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

//typedef struct TestNode TestNode;
//struct TestNode {TestNode*next;TestNode*prev;int data;};
//static void print_ll(TestNode *firstn) {printf("{");for(TestNode*n=firstn;n!=NULL;n=n->next){printf("%i,",n->data);}printf("}\n");}
//static void ll_test() { ArenaTemp temp = arena_get_scratch(NULL); printf("--Linked-List test!--\n"); printf("---------------------\n"); TestNode *ll_first = NULL; printf("SLL_STACK_PUSH: "); for (int i = 0; i < 10; ++i) { TestNode *n = push_array(temp.arena, TestNode, 1); n->data = i; sll_stack_push(ll_first, n); } print_ll(ll_first); printf("SLL_STACK_POP3E_I7:  "); sll_stack_pop(ll_first); sll_stack_pop(ll_first); sll_stack_pop(ll_first); TestNode *g0 = push_array(temp.arena, TestNode, 1); g0->data = 7; sll_stack_push(ll_first,g0); print_ll(ll_first); ll_first = NULL; TestNode *ll_last = NULL; printf("SLL_QUEUE_PUSH: "); for (int i = 0; i < 10; ++i) { TestNode *n = push_array(temp.arena, TestNode, 1); n->data = i; sll_queue_push(ll_first,ll_last, n); } print_ll(ll_first); printf("SLL_QUEUE_POP3E_I7:  "); sll_queue_pop(ll_first,ll_last); sll_queue_pop(ll_first,ll_last); sll_queue_pop(ll_first,ll_last); TestNode *g1 = push_array(temp.arena, TestNode, 1); g1->data = 7; sll_queue_push(ll_first,ll_last, g1); print_ll(ll_first); ll_first = NULL; ll_last = NULL; printf("DLL_PUSH_BACK:  "); for (int i = 0; i < 10; ++i) { TestNode *n = push_array(temp.arena, TestNode, 1); n->data = i; dll_push_back(ll_first,ll_last, n); } print_ll(ll_first); printf("DLL_REMOVE_ODD: "); for (TestNode *n = ll_first;n != NULL;n=n->next){ if (n->data % 2 == 0) { dll_remove(ll_first,ll_last,n); } } print_ll(ll_first); ll_first = NULL; ll_last = NULL; printf("DLL_PUSH_FRONT: "); for (int i = 0; i < 10; ++i) { TestNode *n = push_array(temp.arena, TestNode, 1); n->data = i; dll_push_front(ll_first,ll_last, n); } print_ll(ll_first); printf("DLL_REMOVE_ODD: "); for (TestNode *n = ll_first;n != NULL;n=n->next){ if (n->data % 2 == 0) { dll_remove(ll_first,ll_last,n); } } print_ll(ll_first); printf("---------------------\n"); arena_end_temp(&temp); }


#endif