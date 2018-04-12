/* Game of life in parallel */

#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <libspe2.h>
#include <sys/time.h>

#include "GameOfLifeParallel.hpp"

unsigned long int getTimeMicroSeconds(void) {
    struct timeval tv;
    
    gettimeofday(&tv, NULL);
    return 1000000 * tv.tv_sec + tv.tv_usec;
}

int main(int argc, char *argv[])
{
    int numGens;
    unsigned long int start, stop;
    float elapsed;
    
    GameOfLifeParallel game;
    
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

    /* Time our program */
    start = getTimeMicroSeconds();
   
    game.run(numGens);
    
    /* Time the end result */
    stop = getTimeMicroSeconds();
    elapsed = stop - start;
    
    std::cout << "\nResult time (microseconds): " << elapsed << std::endl;
    
    return 0;
}
