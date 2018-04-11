/* Class definition of parallel game of life */

#include "GameOfLifeParallel.hpp"

/**************************************************************/
/**                Constructor/Deconstructor                 **/
/**************************************************************/
GameOfLifeParallel::GameOfLifeParallel()
{
}

GameOfLifeParallel::~GameOfLifeParallel()
{
}

/*************************************************************/
/**  Run the simulation for the given number of generations **/
/*************************************************************/
void GameOfLifeParallel::run(int numGenerations)
{
    const int SIZE = 64;
    
    // test array to split between SPEs
    // each SPE should get a row
    int in[SIZE] a16 = {
        1, 1, 1, 1, 1, 1, 1, 1,
        2, 2, 2, 2, 2, 2, 2, 2,
        3, 3, 3, 3, 3, 3, 3, 3,
        4, 4, 4, 4, 4, 4, 4, 4,
        5, 5, 5, 5, 5, 5, 5, 5,
        6, 6, 6, 6, 6, 6, 6, 6,
        7, 7, 7, 7, 7, 7, 7, 7,
        8, 8, 8, 8, 8, 8, 8, 8,
    };
    
    int out[SIZE] a16;
    
    // TODO - error check
    
    /* Calculate how much data to send the SPE */
    const int amount = SIZE / NUM_THREADS;
    
    int i;
    ppuThreadArgs pArgs[NUM_THREADS];
    
    /* Loop through each thread to create */
    for(i=0; i<NUM_THREADS; i++) {
        
        /* Create a pthread which will spawn a SPE thread */
        createThread(&pArgs[i], i, in, amount);
    }
    
    /* Wait for each thread to finish */
    for(i=0; i<NUM_THREADS; i++) {
        pthread_join(pArgs[i].pthread, NULL);
    }
    
    /* Destroy each thread context */
    for(i=0; i<NUM_THREADS; i++) {
        spe_context_destroy(pArgs[i].speCtx);
    }
    
    /* Create the thread */
    //createThread(&pArg);
    
    /* Wait for it to finish */
    //pthread_join(pArg.pthread, NULL);
    
    /* Destroy its spe context */
    //spe_context_destroy(pArg.speCtx);
    //if(inP) delete[] inP;
    
    //if(out) delete[] out;
}

/*************************************************************/
/**      Create and run a ppu (and hence spu) thread        **/
/*************************************************************/
void GameOfLifeParallel::createThread(ppuThreadArgs *pArg, int rank, int *arr, int size)
{
    const int SIZE = 64; // test 64 numbers
    int out[SIZE] a16; // test output
    
    /* Set the rank of the thread */
    pArg->rank = rank;
    
    /* Arguments to be sent to the SPE */
    pArg->arr = arr;
    pArg->size = size;
    
    /* Create SPE context */
    pArg->speCtx = spe_context_create(0,NULL);
    
    /* Load program into context */
    spe_program_load(pArg->speCtx, &SpuProg);
    
    /* Run the local PPU thread that will run the SPU thread */
    pthread_create(&pArg->pthread, NULL, &ppuThreadFunc, pArg);
    
    // DEBUG --
    /* Wait for the SPU thread to finish */
    //pthread_join(pArg->pthread, NULL);
    
    /* Destroy the program context */
    //spe_context_destroy(pArg.speCtx);
}

/*************************************************************/
/**              Our local pthread function                 **/
/*************************************************************/
void *GameOfLifeParallel::ppuThreadFunc(void *arg)
{
    /* SPE thread arguments 
     * Aligned 16 attribute is very important as we'll get a 
     * bus error without it! */
    speParams mySarg a16;
    
    /* Cast the function arg as our ppu thread arg */
    ppuThreadArgs *pArg = (ppuThreadArgs*)arg;
    
    /* Calculate the starting point in the array for the thread */
    mySarg.ea_in = (unsigned long) (pArg->arr+(pArg->size*pArg->rank));
    
    // NULL for now
    mySarg.ea_out = (unsigned long) 0;
    
    /* The amount of data for the SPE to read */
    mySarg.size = pArg->size;
    
    /* Get the entry point to the SPE */
    unsigned int entry = SPE_DEFAULT_ENTRY;
    
    /* Run the SPE context
     * rank is sent as the envp */
    if( spe_context_run(pArg->speCtx, &entry, 0, &mySarg, 
            (void*)pArg->rank,NULL) < 0) {
        perror("Failed running context\n");
        exit(1);
    }
    
    return NULL;
}
