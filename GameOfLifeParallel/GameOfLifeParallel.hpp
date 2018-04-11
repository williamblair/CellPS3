/* Class definition of parallel game of life */

#include <iostream>
#include <cstdlib>

#include <libspe2.h>
#include <pthread.h>

#include "ppuThreadArgs.hpp"
#include "speParams.hpp"

#ifndef GAME_PARALLEL_INCLUDED
#define GAME_PARALLEL_INCLUDED

// easier for typing
#define a16 __attribute__ ((aligned (16)))
#define a128 __attribute__ ((aligned (128)))

/* A thread for each SPE */
#define NUM_THREADS 8

/* Our SPE thread program embedded as a library as part
 * of the Make build process */
extern spe_program_handle_t SpuProg;

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
    void createThread(ppuThreadArgs *pArg, int rank, int *arr, int size);
    
    /* Our local pthread function to create a SPE thread */
    static void *ppuThreadFunc(void *arg);
};

#endif // GAME_PARALLEL_INCLUDED
