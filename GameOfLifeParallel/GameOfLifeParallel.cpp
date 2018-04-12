/* Class definition of parallel game of life */

#include "GameOfLifeParallel.hpp"

/**************************************************************/
/**                Constructor/Deconstructor                 **/
/**************************************************************/
GameOfLifeParallel::GameOfLifeParallel()
{
    //inBoard = NULL;
    //outBoard = NULL;
}

GameOfLifeParallel::~GameOfLifeParallel()
{
    //if(inBoard) delete[] inBoard;
    //if(outBoard) delete[] outBoard;
}

/*************************************************************/
/**  Run the simulation for the given number of generations **/
/*************************************************************/
void GameOfLifeParallel::run(int numGenerations)
{
    int i, curGen;
    //const int SIZE = 80;
    
    /* Have this be sent in from read */
    //const int numCols = 8;
    
    // test array to split between SPEs
    // each SPE should get a row
    /*int in[SIZE] a16 = {
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 1, 0, 0, 0, 0, 0,
        0, 0, 1, 0, 0, 0, 0, 0,
        0, 0, 1, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0
    };*/
    
    /* The output array to write to */
    //int out[SIZE] a16;
    
    /* Fill the array with data */
    //memset(out, 0, SIZE*sizeof(int));
    
    // TODO - error check
    
    /* Calculate how much data to send the SPE */
    //const int amount = 24;
    const int amount = (BOARD_WIDTH+2)*3;
    const int numCols = amount / 3;
    
    /* DEBUG - display input */
    std::cout << "\nIn:\n";
    printBoard(inBoard, BOARD_WIDTH, BOARD_HEIGHT);
    
    ppuThreadArgs pArgs[NUM_THREADS];
    
    /* Init each SPE context */
    for(i=0; i<NUM_THREADS; i++) {
        initThread(&pArgs[i]);
    }
    
    for(curGen=0; curGen<numGenerations; curGen++)
    {
        /* Loop through each thread to create */
        for(i=0; i<NUM_THREADS; i++) {
            
            /* Create a pthread which will spawn a SPE thread */
            createThread(&pArgs[i], i, inBoard, amount, numCols, outBoard);
            
            // DEBUG
            //pthread_join(pArgs[i].pthread, NULL);
        }
        
        /* Wait for each thread to finish */
        for(i=0; i<NUM_THREADS; i++) {
            pthread_join(pArgs[i].pthread, NULL);
        }
        
        /* Display the result */
        std::cout<<"Gen " << curGen+1 << std::endl;
        printBoard(outBoard, BOARD_WIDTH, BOARD_HEIGHT);
        
        /* Copy the result into the current generation */
        memcpy(inBoard, outBoard, ((BOARD_WIDTH+2)*(BOARD_HEIGHT+2)*sizeof(int)));
        
        // DEBUG
        //std::cout << "InBoard:\n";
        //printBoard(inBoard, BOARD_WIDTH, BOARD_HEIGHT);
        //std::cout << "OutBoard:\n";
        //printBoard(outBoard, BOARD_WIDTH, BOARD_HEIGHT);
    }
    
    /* Destroy each thread context */
    for(i=0; i<NUM_THREADS; i++) {
        spe_context_destroy(pArgs[i].speCtx);
    }
    
    /* See if our result was written to */
    // loop through each row
    //std::cout << "\nOut:\n";
    //printBoard(outBoard, BOARD_WIDTH, BOARD_HEIGHT);
}

/* Creates SPE contexts for each ppu arg */
void GameOfLifeParallel::initThread(ppuThreadArgs *pArg)
{
    /* Create the SPE context */
    pArg->speCtx = spe_context_create(0, NULL);
    
    /* Load the program into it */
    spe_program_load(pArg->speCtx, &SpuProg);
}

/*************************************************************/
/**      Create and run a ppu (and hence spu) thread        **/
/*************************************************************/
void GameOfLifeParallel::createThread(ppuThreadArgs *pArg, int rank, int *arr, int size, int numCols, int *out)
{
    
    /* Set the rank of the thread */
    pArg->rank = rank;
    
    /* Arguments to be sent to the SPE */
    pArg->arr = arr;
    pArg->size = size;
    pArg->start = rank*numCols; // each thread needs the previous and next row,
                                // so the data sent will overlap and hence
                                // the need to know the width of the array
        
    /* We only want to write to the middle/main row, not the padding
     * previous and after rows */
    pArg->out = out+(size/3);
    
    /* Create SPE context */
    //pArg->speCtx = spe_context_create(0,NULL);
    
    /* Load program into context */
    //spe_program_load(pArg->speCtx, &SpuProg);
    
    /* Run the local PPU thread that will run the SPU thread */
    pthread_create(&pArg->pthread, NULL, &ppuThreadFunc, pArg);
    
    // DEBUG --
    /* Wait for the SPU thread to finish */
    //pthread_join(pArg->pthread, NULL);
    
    /* Destroy the program context */
    //spe_context_destroy(pArg.speCtx);
}

/*********************************************************/
/**Read the initial cell configuration from a text file **/
/*********************************************************/
bool GameOfLifeParallel::readFromFile(const char *fileName)
{
    int y,x;
    std::ifstream myFile;
    
    /* Open the file */
    myFile.open(fileName);
    if(!myFile.is_open()) {
        std::cerr << "Failed to open " << fileName << std::endl;
        return false;
    }
    
    /* Read in the file dimensions */
    myFile >> BOARD_WIDTH >> BOARD_HEIGHT;
    
     /* Create the Board */
    // include a border padding in the array, so we don't have to check for edge conditions
    int boardSize = (BOARD_WIDTH+2) * (BOARD_HEIGHT+2);
    
    /* Fill with a default value (including the padding) */
    memset(inBoard, 0, boardSize*sizeof(int));
    memset(outBoard, 0, boardSize*sizeof(int));
    
    /* Read in the board */
    for(y=0; y<BOARD_HEIGHT; y++)
    {
        for(x=0; x<BOARD_WIDTH; x++)
        {
            int val;
            myFile >> val;
            putValue(y,x,val);
        }
    }
    
    /* close the file */
    myFile.close();
    
    return true;
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

/*********************************************************/
/**                   Helper Functions                  **/
/*********************************************************/
void GameOfLifeParallel::printBoard(int *arr, int width, int height)
{
    int i,j;
    std::cout << std::endl;
    for(i=1; i<height+1; i++)
    {
        for(j=1; j<width+1; j++)
        {
            /* Compiler gives warning otherwise */
            std::string space = " ";
            
            /* Get the current value from the board entry */
            int val = arr[i*(width+2)+j];
            
            /* If the entry is a one print it otherwise a space */
            std::cout << (val == 1) ? "1" : space;
        }
        
        /* Move to the next line */
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void GameOfLifeParallel::putValue(int y, int x, int val)
{
    x+=1; y+=1; // add 1 for padding
    
    if(x > BOARD_WIDTH + 1 || y > BOARD_HEIGHT + 1) {
        std::cerr << "putValue: Invalid board location: " << x 
            << ", " << y << std::endl;
        return;
    }
    
    int loc = y * (BOARD_WIDTH + 2) + x;
    inBoard[loc] = val;
    //std::cout << "Putting " << val << "at " << loc << std::endl;
}

void GameOfLifeParallel::putNextGenValue(int y, int x, int val)
{
    x+=1; y+=1; // add 1 for padding
    
    if(x > BOARD_WIDTH + 1 || y > BOARD_HEIGHT + 1) {
        std::cerr << "putValue: Invalid board location: " << x 
            << ", " << y << std::endl;
        return;
    }
    
    int loc = y * (BOARD_WIDTH + 2) + x;
    outBoard[loc] = val;
    //std::cout << "Putting " << val << "at " << loc << std::endl;
}

int GameOfLifeParallel::getValue(int y, int x)
{
    x+=1; y+=1;
    int loc = y * (BOARD_WIDTH + 2) + x;
    return inBoard[loc];
}

