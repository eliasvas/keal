#ifndef HASH_H_
#define HASH_H_

#include "core_inc.h"

INLINE u64 djb2_hash(u8 *str) {
	if (!str) return 0;
	u64 hash = 5381;
	int c;
	while ((c = *str++))
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
	return hash;
}

#endif