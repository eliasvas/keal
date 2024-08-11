#ifndef RAND_H
#define RAND_H

#include "time.h"

#include "base_inc.h"

static void rand_init() {
    srand(time(NULL));
}

static f64 gen_rand01(void)
{
    return (f64)rand() / (f64)RAND_MAX;
}

static u32 gen_random(u32 min, u32 max) {
    assert(min < max);
    return (rand() % (max - min) + min);
}

#endif