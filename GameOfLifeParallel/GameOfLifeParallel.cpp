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
    //int *out = new int[SIZE];
    
    // TODO - error check
    
    /* Calculate how much data to send the SPE */
    const int amount = SIZE / NUM_THREADS;
    
    // DEBUG HERE
    int i, start;
    ppuThreadArgs pArgs[NUM_THREADS];

    //std::cout << "PPU Amount: " << amount << std::endl;
    
    /* Loop through each thread to create */
    for(i=0; i<NUM_THREADS; i++) {
        
        /* Calculate where in the array to start for the SPE */
        start = i*amount;
        createThread(&pArgs[i], in+(i*amount), amount);
        
        //threads[i].create()
        //threads[i].copyArr((int*)(in+(i*amount)), amount);
        //threads[i].create(out);
    }
    
    /* Wait for each thread to finish */
    for(i=0; i<NUM_THREADS; i++) {
        pthread_join(pArgs[i].pthread, NULL);
    }
    
    /* Destroy each thread context */
    //for(i=0; i<NUM_THREADS; i++) {
    //    spe_context_destroy(pArgs[i].speCtx);
   // }
    
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
void GameOfLifeParallel::createThread(ppuThreadArgs *pArg, int *arr, int size)
{
    const int SIZE = 64; // test 64 numbers
    int out[SIZE] a16; // test output
    
    //const int localStart = start;
    
    // test data to send to the SPE
    
    /* Aligned 16 attribute is VERY important as you will
     * get a 'Bus Error' without it ! */
    speParams sArg a16;
    //ppuThreadArgs pArg;
   
    //std::cout << "createThread amount: " << amount << std::endl;
    
    /* Define parameters for the SPE */
    //sArg.ea_in = (unsigned long ) in;
    //sArg.ea_in = (unsigned long ) (arr+start);
    //std::cout << "CreateThread start: " << start << std::endl;
    //std::cout << "CreateThread local start: " << localStart << std::endl;
    //sArg.ea_in = (unsigned long) (&arr[localStart]);
    sArg.ea_in = (unsigned long) arr;
    sArg.ea_out = (unsigned long) out;
    //sArg.size = SIZE;
    sArg.size = size;
    
    /* Set the pointer for the ppu to send the spu for args */
    pArg->sArg = &sArg;
    
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
    /* Cast the function arg as our ppu thread arg */
    ppuThreadArgs *pArg = (ppuThreadArgs*)arg;
    
    /* Get the entry point to the SPE */
    unsigned int entry = SPE_DEFAULT_ENTRY;
    
    /* Run the SPE context */
    if( spe_context_run(pArg->speCtx, &entry, 0, pArg->sArg, 
            NULL,NULL) < 0) {
        perror("Failed running context\n");
        exit(1);
    }
    
    return NULL;
}
