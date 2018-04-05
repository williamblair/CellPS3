/* Serial version of Game of Life on PPU */
#include <iostream>
#include <cstring>
#include <cstdlib>

#include "GameOfLifeSerial.hpp"

int main(int argc, char **argv)
{
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
    
    /* 'Blinker' Example */
    //game.putValue(4,4, 1);
    //game.putValue(4,5, 1);
    //game.putValue(4,6, 1);
    
    //std::cout << "Gen 1:\n";
    //game.printBoard();
    
    game.run(numGens);
    
    //std::cout << "End Gen:\n";
    //game.printBoard();
    
    return 0;
}

