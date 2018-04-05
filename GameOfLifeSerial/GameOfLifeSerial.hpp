#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>

//#define BOARD_WIDTH  10
//#define BOARD_HEIGHT 10

class GameOfLife
{
public:
    /* Constructors */
    GameOfLife();
    ~GameOfLife();
    
    /* Run the simulation for the given number of generations */
    void run(int numGenerations);
    
    /* Set a specified cell on a board */
    void putValue(int y, int x, int val);
    
    /* Read in the initial cell configuration from a text file */
    bool readFromFile(const char *fileName);

private:
    
    /* The Game Board; the current and new version */
    int *board[2];
    
    /* The width and height of the board */
    int BOARD_WIDTH;
    int BOARD_HEIGHT;
    
    /* Calculate the new status of a cell based on the rules */
    int calcNewStatus(int y, int x);
    
    /* Count the number of live neighbors the given cell has */
    int  numNeighbors(int y, int x);
    
    /* Helper Functions */
    void printBoard(void);
    int  getValue(int y, int x);
    void putNextGenValue(int y, int x, int val);
};

#endif // GAME_H_INCLUDED
