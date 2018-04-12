/* SPU program for Game of Life */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <spu_intrinsics.h>
#include <spu_mfcio.h>

#include "../speParams.hpp"

// easier for typing
#define a16 __attribute__ ((aligned (16)))
#define a128 __attribute__ ((aligned (128)))
#define aunused __attribute__ ((unused))

//#define MAX_BUFSIZE 128
#define MAX_BUFSIZE 2048

// test input/output data
int in_spe[MAX_BUFSIZE] a16;
int out_spe[MAX_BUFSIZE] a16;

// the width of the board, calculated based on the total amount
// of data this SPE has divided by 3
int BOARD_WIDTH;

// the parameters sent from the PPU
speParams sArg a16;

/*********************************************************/
/**                 Function Prototypes                 **/
/*********************************************************/
void computeNextGen(void);
int calcNewStatus(int y, int x);
int numNeighbors(int y, int x);
void putValue(int y, int x, int val);
void putNextGenValue(int y, int x, int val);
void putNextGenValue(int y, int x, int val);
int getValue(int y, int x);

/* SPE program entry point */
int main(unsigned long long spe, unsigned long long argp, unsigned long long envp)
{
    unsigned int i, j;
    int tag = 1;
    
    /* Get input/output parameters  */
    spu_mfcdma64(&sArg, mfc_ea2h(argp), mfc_ea2l(argp),
                sizeof(speParams), tag, MFC_GET_CMD);
    spu_writech(MFC_WrTagMask, 1 << tag);
    spu_mfcstat(MFC_TAG_UPDATE_ALL);
    
    /* Get the rank for this thread */
    int myRank = (int)envp;
    
    /* Get input data */
    spu_mfcdma64(in_spe, mfc_ea2h(sArg.ea_in), mfc_ea2l(sArg.ea_in),
                sArg.size * sizeof(int), tag, MFC_GET_CMD);
    spu_writech(MFC_WrTagMask, 1 << tag);
    spu_mfcstat(MFC_TAG_UPDATE_ALL);
    
    /* Calculate the width of the board by removing the 2 padding rows
     * and the padding cell on the left and right */
    BOARD_WIDTH = (sArg.size / 3)-2;
    
    /* Figure out the next generation */
    computeNextGen();
    
    /* Calculate where to write our output data to based
     * on the padding previous and after rows */
    int *outloc = out_spe + (BOARD_WIDTH)+2;
    
    /* Put output data */
    spu_mfcdma64(outloc, mfc_ea2h(sArg.ea_out), mfc_ea2l(sArg.ea_out),
                (sArg.size/3) * sizeof(int), tag, MFC_PUT_CMD);
    spu_writech(MFC_WrTagMask, 1 << tag);
    spu_mfcstat(MFC_TAG_UPDATE_ALL);
    
    return 0;
}

/* Scan and apply rules to each cell 
 * This code modified from the serial version */
void computeNextGen(void)
{
    int y=0;
    int x;
    
    /* Compute the width of the board based on our total data,
     * which contains a row plus the row before and after it,
     * and then subtract the left and right padding edges */
    //const int BOARD_WIDTH = (sArg->size / 3)-2;
    
    for(x=0; x < BOARD_WIDTH; x++)
    {
        /* figure out the next generation value */
        int newStatus = calcNewStatus(y,x);
        
        /* Apply the value to the next generation */
        putNextGenValue(y,x,newStatus);
    }
}

/* Scan the current cell for neigbors and apply rules */
int calcNewStatus(int y, int x)
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
int numNeighbors(int y, int x)
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

void putValue(int y, int x, int val)
{
    x+=1; y+=1; // add 1 for padding
    
    int loc = y * (BOARD_WIDTH + 2) + x;
    in_spe[loc] = val;
    //std::cout << "Putting " << val << "at " << loc << std::endl;
}

void putNextGenValue(int y, int x, int val)
{
    x+=1; y+=1; // add 1 for padding
    
    int loc = y * (BOARD_WIDTH + 2) + x;
    out_spe[loc] = val;
    //std::cout << "Putting " << val << "at " << loc << std::endl;
}

int getValue(int y, int x)
{
    x+=1; y+=1;
    int loc = y * (BOARD_WIDTH + 2) + x;
    return in_spe[loc];
}
