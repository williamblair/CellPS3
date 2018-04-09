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
    spe_context_ptr_t speCtx;  // SPE context
    pthread_t         pthread; // the PPU thread
    speParams         *sArg; // argument to send the SPE thread
} ppuThreadArgs;

#endif
