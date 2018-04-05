/* Class definition of parallel game of life */

#include <iostream>
#include <cstdlib>

#include <libspe2.h>
#include <pthread.h>

#ifndef GAME_PARALLEL_INCLUDED
#define GAME_PARALLEL_INCLUDED

// easier for typing
#define a16 __attribute__ ((aligned (16)))
#define a128 __attribute__ ((aligned (128)))

/* The maximum amount of SPE threads we can run */
#define MAX_SPE_THREADS 16

/* Our SPE thread program embedded as a library as part
 * of the Make build process */
extern spe_program_handle_t SpuProg;

/* Our argument struct for our local PPU thread */
typedef struct ppuThreadArg {
    spe_context_ptr_t speCtx;  // SPE context
    pthread_t         pthread; // the SPE thread
    void              *speArg; // argument to send to the SPE thread
} ppuThreadArg;

/* Our argument struct for a SPE thread */
typedef struct spuThreadArg {
    int testData;
} spuThreadArg;

class GameOfLifeParallel
{
public:
    /* Constructor/deconstructor */
    GameOfLifeParallel();
    ~GameOfLifeParallel();

    /* Run the simulation for the given number of generations */
    void run(int numGenerations);

private:
    
    // TODO - add args
    /* Create and run a ppu (and hence spu) thread */
    void createThread(ppuThreadArg *pArg);
    
    /* Our local pthread function to create a SPE thread */
    static void *ppuThreadFunc(void *arg);
};

#endif // GAME_PARALLEL_INCLUDED
