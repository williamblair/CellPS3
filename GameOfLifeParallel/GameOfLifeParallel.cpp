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
    ppuThreadArg pArg;
    
    // DEBUG HERE
    
    /* Create the thread */
    //createThread(&pArg);
    createThread(&pArg);
    
    /* Wait for it to finish */
    pthread_join(pArg.pthread, NULL);
    
    /* Destroy its spe context */
    spe_context_destroy(pArg.speCtx);
}

/*************************************************************/
/**      Create and run a ppu (and hence spu) thread        **/
/*************************************************************/
void GameOfLifeParallel::createThread(ppuThreadArg *pArg)
{
    /* Argument structs to send the ppu->then spu */
    /* aligned 16 property is important - get a bus error without it! */
    spuThreadArg sArg a16;
    
    /* Specify our SPE thread args */
    sArg.testData = 120;
    
    // TODO - error check below!
    
    /* Specify our ppu thread args */
    // create an SPE context
    if((pArg->speCtx = spe_context_create(0,NULL)) == NULL) {
        perror("Failed creating context!\n");
        exit(1);
    }
    // load the SPE program
    if(spe_program_load(pArg->speCtx, &SpuProg)) {
        perror("Failed loading program\n");
        exit(1);
    }
    
    // specify argument pointer for the SPE thread
    pArg->speArg = &sArg; 
    
    /* Create the local PPU thread 
     * (which will send the prog to the SPE) */
    if(pthread_create(&pArg->pthread, NULL, &ppuThreadFunc, pArg)) {
        perror("Failed creating thread\n");
        exit(1);
    }
}

/*************************************************************/
/**              Our local pthread function                 **/
/*************************************************************/
void *GameOfLifeParallel::ppuThreadFunc(void *arg)
{
    /* Cast our given argument as the thread arg struct */
    ppuThreadArg *argp = (ppuThreadArg*)arg;
    
    /* Get the entry point to the SPE */
    unsigned int entry = SPE_DEFAULT_ENTRY;
    
    /* Run the SPE context */
    if( spe_context_run(argp->speCtx, &entry, 0, argp->speArg, 
            NULL,NULL) < 0) {
        perror("Failed running context\n");
        exit(1);
    }
    
    return NULL;
}
