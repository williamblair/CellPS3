/* Serial version of Game of Life on PPU */
#include <iostream>
#include <cstring>
#include <cstdlib>

#include "GameOfLifeSerial.hpp"

int main(int argc, char **argv)
{
    GameOfLife game;
    
    game.putValue(5,5, 1);
    game.putValue(5,6, 1);
    //game.putValue(6,6, 1);
    //game.putValue(6,5, 1);
    
    game.run(5);
    
    game.printBoard();
    
    return 0;
}

