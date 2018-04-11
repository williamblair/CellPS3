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
    int i,j;
    const int SIZE = 80;
    
    /* Have this be sent in from read */
    //const int numCols = 8;
    
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
        9, 9, 9, 9, 9, 9, 9, 9,
        10, 10, 10, 10, 10, 10, 10, 10
    };
    
    int out[SIZE] a16;
    
    /* Fill the array with data */
    memset(out, 0, SIZE*sizeof(int));
    
    // TODO - error check
    
    /* Calculate how much data to send the SPE */
    //const int amount = (SIZE / NUM_THREADS)*3; // need the previous and next row for computation
    const int amount = 24;
    const int numCols = amount / 3;
    std::cout << "Amount: " << amount << std::endl;
    
    //int i;
    ppuThreadArgs pArgs[NUM_THREADS];
    
    /* Loop through each thread to create */
    for(i=0; i<NUM_THREADS; i++) {
        
        /* Create a pthread which will spawn a SPE thread */
        createThread(&pArgs[i], i, in, amount, numCols, out);
        
        // DEBUG
        pthread_join(pArgs[i].pthread, NULL);
    }
    
    /* Wait for each thread to finish */
    //for(i=0; i<NUM_THREADS; i++) {
    //    pthread_join(pArgs[i].pthread, NULL);
   // }
    
    /* Destroy each thread context */
    for(i=0; i<NUM_THREADS; i++) {
        spe_context_destroy(pArgs[i].speCtx);
    }
    
    /* See if our result was written to */
    // loop through each row
    std::cout << "\nOut:\n";
    for(i=0; i<10; i++) {
        // loop through each column
        for(j=0; j<numCols;j++){
            std::cout << out[i*numCols+j] <<", ";
        }
        std::cout << std::endl;
    }
}

/*************************************************************/
/**      Create and run a ppu (and hence spu) thread        **/
/*************************************************************/
void GameOfLifeParallel::createThread(ppuThreadArgs *pArg, int rank, int *arr, int size, int numCols, int *out)
{
    const int SIZE = 80; // test 64 numbers
    
    /* Set the rank of the thread */
    pArg->rank = rank;
    
    /* Arguments to be sent to the SPE */
    pArg->arr = arr;
    pArg->size = size;
    pArg->start = rank*numCols; // each thread needs the previous and next row,
                                // so the data sent will overlap and hence
                                // the need to know the width of the array
                                
    pArg->out = out;
    
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
    mySarg.ea_in = (unsigned long) (pArg->arr+pArg->start);
    
    /* Starting point for the thread to write its data to */
    mySarg.ea_out = (unsigned long) (pArg->out+pArg->start);
    
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
