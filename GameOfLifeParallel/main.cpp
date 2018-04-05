/* Game of life in parallel */

#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <libspe2.h>

#include "GameOfLifeParallel.hpp"

int main(int argc, char *argv[])
{
    GameOfLifeParallel game;
    
    game.run(5);
    
    return 0;
}
