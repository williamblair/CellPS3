#include "GameOfLifeSerial.hpp"

/*********************************************************/
/**             Constructor/Deconstructor               **/
/*********************************************************/
GameOfLife::GameOfLife()
{
    board[0] = NULL;
    board[1] = NULL;
    
    BOARD_WIDTH=0; BOARD_HEIGHT=0;
    
    /* Create the Board */
    // include a border padding in the array, so we don't have to check for edge conditions
    //int boardSize = (BOARD_WIDTH+2) * (BOARD_HEIGHT+2);
    //std::cout << "Initializing board of size " << boardSize << std::endl;
    //board[0] = new int[boardSize];
    //board[1] = new int[boardSize];
    
    /* Fill with a default value */
    //memset(board[0], 0, boardSize*sizeof(int));
    //memset(board[1], 0, boardSize*sizeof(int));
}

GameOfLife::~GameOfLife()
{
    /* Free memory */
    if(board[0]) delete[] board[0];
    if(board[1]) delete[] board[1];
}

/*********************************************************/
/**Read the initial cell configuration from a text file **/
/*********************************************************/
bool GameOfLife::readFromFile(const char *fileName)
{
    int y,x;
    std::ifstream myFile;
    
    /* Clear the board if already initialized */
    if(board[0]) delete[] board[0];
    if(board[1]) delete[] board[1];
    
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
    //std::cout << "Initializing board of size " << boardSize << std::endl;
    board[0] = new int[boardSize];
    board[1] = new int[boardSize];
    
    /* Fill with a default value (including the padding) */
    memset(board[0], 0, boardSize*sizeof(int));
    memset(board[1], 0, boardSize*sizeof(int));
    
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

/*********************************************************/
/**                  Run the Simulation                 **/
/*********************************************************/
void GameOfLife::run(int numGenerations)
{
    // TODO - actually run for specified number of generations 
    
    int y,x;
    int curGen;
    
    
    for(curGen=0; curGen<numGenerations; curGen++)
    {
        /* Display the generation status */
        std::cout << "Gen " << curGen+1 << ":\n";
        printBoard();
        
        for(y=0; y<BOARD_HEIGHT; y++)
        {
            for(x=0; x<BOARD_WIDTH; x++)
            {
                /* Figure out the next generation value */
                int newStatus = calcNewStatus(y,x);
                
                /* apply the next generation value to the
                 * new generation */
                putNextGenValue(y,x,newStatus);            
            }
        }
        
        /* Copy the new generation into the current generation */
        memcpy(board[0], board[1], (BOARD_WIDTH+2)*(BOARD_HEIGHT+2)*sizeof(int));
    }
}

/*********************************************************/
/**  Apply the Rules of Life and Death to a Given Cell  **/
/*********************************************************/
int GameOfLife::calcNewStatus(int y, int x)
{
    /* Is the cell currently alive or dead? */
    int cellStatus = getValue(y,x);
    
    /* The new status of the cell based on the rules 
     * by default leave it as it was in the prev. gen. */
    int finalStatus=cellStatus;
    
    /* If alive... */
    if(cellStatus == 1)
    {
        //std::cout << "Found alive cell!\n";
        
        int n = numNeighbors(y,x);
        //std::cout << "num neighbors: " << n << std::endl;
        
        /* The cell dies if it has < 2 neighbors (loneliness)
         * or > 3 neighbors (starved for food) */
        if(n < 2 || n > 3) {
            //std::cout << "Starving/Lonely Cell! Killing: " << y 
            //    << ", " << x << "\n";
            finalStatus = 0;
        }
        
        /* Otherwise leave the cell alive */
    }
    
    /* If dead... */
    else
    {
        int n = numNeighbors(y,x);
        
        /* A dead cell becomes alive if it has exactly
         * 3 neighbors */
        if(n == 3) {
            //std::cout << "Found 3 neighbor on dead cell!\n";
            finalStatus = 1;
        }
        
        /* Otherwise leave the cell dead */
    }
    
    return finalStatus;
}

/*********************************************************/
/**    Count the Surrounding Neighbors of a Given Cell  **/
/*********************************************************/
int GameOfLife::numNeighbors(int y, int x)
{
    int numNeighbors=0;
    
    //x+=1; y+=1;
    
    /* Count all of the surrounding neighbors*/
    int i,j; // the location relative to the given cell
    for(i = -1; i < 2; i++)
    {
        for(j = -1; j < 2; j++)
        {
            /* If the neighboring cell is 1 and we aren't 
             * at the same location as the given cell... 
             * (Since we don't count ourself as a neighbor) */
            if( getValue(y+i, x+j) && !(i==0 && j==0) ) {
                /* Then increase the number of neighbors */
                numNeighbors++;
            }
        }
    }
    
    return numNeighbors;
}

/*********************************************************/
/**                   Helper Functions                  **/
/*********************************************************/
void GameOfLife::printBoard(void)
{
    int i,j;
    std::cout << std::endl;
    for(i=1; i<BOARD_HEIGHT+1; i++)
    {
        for(j=1; j<BOARD_WIDTH+1; j++)
        {
            /* Compiler gives warning otherwise */
            std::string space = " ";
            
            /* Get the current value from the board entry */
            int val = board[0][i*(BOARD_WIDTH+2)+j];
            
            /* If the entry is a one print it otherwise a space */
            std::cout << (val == 1) ? "1" : space;
        }
        
        /* Move to the next line */
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void GameOfLife::putValue(int y, int x, int val)
{
    x+=1; y+=1; // add 1 for padding
    
    if(x > BOARD_WIDTH + 1 || y > BOARD_HEIGHT + 1) {
        std::cerr << "putValue: Invalid board location: " << x 
            << ", " << y << std::endl;
        return;
    }
    
    int loc = y * (BOARD_WIDTH + 2) + x;
    board[0][loc] = val;
    //std::cout << "Putting " << val << "at " << loc << std::endl;
}

void GameOfLife::putNextGenValue(int y, int x, int val)
{
    x+=1; y+=1; // add 1 for padding
    
    if(x > BOARD_WIDTH + 1 || y > BOARD_HEIGHT + 1) {
        std::cerr << "putValue: Invalid board location: " << x 
            << ", " << y << std::endl;
        return;
    }
    
    int loc = y * (BOARD_WIDTH + 2) + x;
    board[1][loc] = val;
    //std::cout << "Putting " << val << "at " << loc << std::endl;
}

int GameOfLife::getValue(int y, int x)
{
    x+=1; y+=1;
    int loc = y * (BOARD_WIDTH + 2) + x;
    return board[0][loc];
}

