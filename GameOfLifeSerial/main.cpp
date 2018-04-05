/* Serial version of Game of Life on PPU */
#include <iostream>
#include <cstring>
#include <cstdlib>

#include "GameOfLifeSerial.hpp"

int main(int argc, char **argv)
{
    GameOfLife game;
    
    game.putValue(4,5, 1);
    game.putValue(5,5, 1);
    game.putValue(5,6, 1);
    
    //std::cout << "Gen 1:\n";
    //game.printBoard();
    
    game.run(5);
    
    //std::cout << "End Gen:\n";
    //game.printBoard();
    
    return 0;
}

