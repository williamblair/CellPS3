/* Class definition of parallel game of life */

#include <iostream>
//#include <cstdlib>
#include <stdlib.h>
#include <malloc.h>
#include <fstream>

#include <libspe2.h>
#include <pthread.h>

#include "ppuThreadArgs.hpp"
#include "speParams.hpp"

#ifndef GAME_PARALLEL_INCLUDED
#define GAME_PARALLEL_INCLUDED

// max size you can send through the dma at once
//#define MAX_BUFSIZE 128
#define MAX_BUFSIZE 2048

// max size of a row based on padding (prev and next row)
#define MAX_ROWSIZE (MAX_BUFSIZE/3)

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

    bool readFromFile(const char *fileName);

    /* Run the simulation for the given number of generations */
    void run(int numGenerations);

private:

    /* Our input generation and output generation */
    //int *inBoard a16;
    //int *outBoard a16;
    int inBoard[MAX_ROWSIZE*NUM_THREADS] a16;
    int outBoard[MAX_ROWSIZE*NUM_THREADS] a16;
    
    /* The size of our board */
    int BOARD_WIDTH, BOARD_HEIGHT;
    
    /* Create and send an SPE context */
    void initThread(ppuThreadArgs *pArg);
    
    /* Create and run a ppu (and hence spu) thread */
    void createThread(ppuThreadArgs *pArg, int rank, int *arr, int size, int numCols, int *out);
    
    /* Our local pthread function to create a SPE thread */
    static void *ppuThreadFunc(void *arg);
    
    void printBoard(int *arr, int width, int height);
    int  getValue(int y, int x);
    void putNextGenValue(int y, int x, int val);
    void putValue(int y, int x, int val);
    
};

#endif // GAME_PARALLEL_INCLUDED
