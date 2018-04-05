/* SPU program for Game of Life */

#include <stdio.h>
#include <spu_intrinsics.h>
#include <spu_mfcio.h>

// easier for typing
#define a16 __attribute__ ((aligned (16)))
#define a128 __attribute__ ((aligned (128)))
#define aunused __attribute__ ((unused))

// TODO - put this is a header
/* Our argument struct for a SPE thread */
typedef struct spuThreadArg {
    int testData;
} spuThreadArg;

volatile spuThreadArg sArg;

/* SPE program entry point */
int main(unsigned long long spe, unsigned long long argp)
{
    
    /* Get the arg data from the PPU based off our argp, an argument pointer */
    unsigned int tagID = mfc_tag_reserve();
    spu_writech(MFC_WrTagMask, -1);
    
    spu_mfcdma32((void*)&sArg, (unsigned int)argp, sizeof(spuThreadArg), 
        tagID, MFC_GET_CMD);
    (void)spu_mfcstat(MFC_TAG_UPDATE_ALL);
    
    printf("Message from SPE 0x%llx: %d\n", spe, sArg.testData);
    
    return 0;
}
