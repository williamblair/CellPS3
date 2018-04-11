#include <libspe2.h>
#include <pthread.h>

#include "speParams.hpp"

#ifndef PPU_THREAD_ARGS_H_INCLUDED
#define PPU_THREAD_ARGS_H_INCLUDED

// easier for typing
#define a16 __attribute__ ((aligned (16)))
#define a128 __attribute__ ((aligned (128)))

/* PPU thread parameters */
typedef struct {
    int               rank;    // testing sending this as environment pointer
    int               *arr;    // base array
    int               *out;    // where the SPE should write it's computed data
    int               size;    // amount for this thread
    int               start;   // where the thread's area starts in the array
    spe_context_ptr_t speCtx;  // SPE context
    pthread_t         pthread; // the PPU thread
} ppuThreadArgs;

#endif
