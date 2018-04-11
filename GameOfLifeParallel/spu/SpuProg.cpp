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

#define MAX_BUFSIZE 128

// test input/output data
int in_spe[MAX_BUFSIZE] a16;
int out_spe[MAX_BUFSIZE] a16;

speParams sArg a16;

/* SPE program entry point */
int main(unsigned long long spe, unsigned long long argp, unsigned long long envp)
{
    unsigned int i;
    int tag = 1;
    
    
    // string to write the entire line at once to avoid confusion
    //char output[256];
    //char temp[16]; // the current data entry as a str
    //sprintf(output, "Data from SPE 0x%llX:\n", spe);
    
    //printf("Hello World From SPE: 0x%llX\n", spe);
    
    /* Get input/output parameters  */
    spu_mfcdma64(&sArg, mfc_ea2h(argp), mfc_ea2l(argp),
                sizeof(speParams), tag, MFC_GET_CMD);
    spu_writech(MFC_WrTagMask, 1 << tag);
    spu_mfcstat(MFC_TAG_UPDATE_ALL);
    
    /* DEBUG - get rank from envp 
     * IT WORKS!!! */
    int myRank = (int)envp;
    printf("SPE 0x%llX rank: %d\n", spe, myRank);
    
    //printf("sArg size: %d\n", sArg.size);
    
    //unsigned long long myDataP = (unsigned long long)(((int*)sArg.ea_in)+(sArg.size*myRank));
    unsigned long long myDataP = sArg.ea_in+((unsigned long long)sArg.size*(unsigned long long)myRank);
    
    /* Get input data */
    spu_mfcdma64(in_spe, mfc_ea2h(sArg.ea_in), mfc_ea2l(sArg.ea_in),
                sArg.size * sizeof(int), tag, MFC_GET_CMD);
    //spu_mfcdma64(in_spe, mfc_ea2h(myDataP), mfc_ea2l(myDataP),
    //            sArg.size * sizeof(int), tag, MFC_GET_CMD);
    spu_writech(MFC_WrTagMask, 1 << tag);
    spu_mfcstat(MFC_TAG_UPDATE_ALL);
    
    // TEST for now
    //for(i=0; i<sArg.size; i++) {
    //    printf("in[%d]: %f\n", i, in_spe[i]);
   // }
    for(i=0; i<sArg.size; i++) {
        
        printf("SPE 0x%llX[i]: %d\n", spe, in_spe[i]);
        //sprintf(temp, "%d, ", in_spe[i]);
        //strcat(output, temp);
    } //strcat(output, "\n");
    
    //printf("%s", output);
    
    /* Put output data */
    
    // TODO
    
    return 0;
}
