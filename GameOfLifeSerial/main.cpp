/* Serial version of Game of Life on PPU */
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <sys/time.h>

#include "GameOfLifeSerial.hpp"

unsigned long int getTimeMicroSeconds(void) {
    struct timeval tv;
    
    gettimeofday(&tv, NULL);
    return 1000000 * tv.tv_sec + tv.tv_usec;
}

int main(int argc, char **argv)
{
    unsigned long int start, stop;
    float elapsed;
    
    int numGens=0;
    GameOfLife game;
    
    /* Get command line args */
    if(argc != 3) {
        std::cout << "Usage: "<< argv[0] << " <boardfile> <num generations>\n";
        std::cout << "Example: " << argv[0] << " board.txt 10\n";
        return 0;
    }
    
    /* Read in the board */
    if(!game.readFromFile(argv[1])) {
        return -1;
    }
    
    /* Get the number of generations */
    numGens = atoi(argv[2]);
    //numGens = 2;
    
    /* 'Blinker' Example */
    //game.putValue(4,4, 1);
    //game.putValue(4,5, 1);
    //game.putValue(4,6, 1);
    
    //std::cout << "Gen 1:\n";
    //game.printBoard();
    
    /* Time our program */
    start = getTimeMicroSeconds();
    
    game.run(numGens);
    
    /* Time the end result */
    stop = getTimeMicroSeconds();
    elapsed = stop - start;
    
    std::cout << "\nResult time (microseconds): " << elapsed << std::endl;
    
    //std::cout << "End Gen:\n";
    //game.printBoard();
    
    return 0;
}

