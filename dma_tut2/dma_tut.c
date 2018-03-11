/* this is an absolute value converter using the SPEs and DMA transfers, taken from
 * here: https://www.kernel.org/pub/linux/kernel/people/geoff/cell/ps3-linux-docs/CellProgrammingTutorial/BasicsOfSPEProgramming.html */
#include <stdio.h>
#include <stdlib.h>
#include <libspe2.h>

// easier typing
#define a16 __attribute__((aligned(16)))

// the amount of numbers to convert to absolute value
#define SIZE (64)

// the numbers to be converted
float in[SIZE] a16 = { 1,  -2,  3,  -4,  5,  -6,  7,  -8,
                       9, -10, 11, -12, 13, -14, 15, -16,
                       17, -18, 19, -20, 21, -22, 23, -24,
                       25, -26, 27, -28, 29, -30, 31, -32,
                       33, -34, 35, -36, 37, -38, 39, -40,
                       41, -42, 43, -44, 45, -46, 47, -48,
                       49, -50, 51, -52, 53, -54, 55, -56,
                       57, -58, 59, -60, 61, -62, 63, -64 
};

// the results
float out[SIZE] a16;

// define our parameters for the SPE
typedef struct {
    unsigned long long ea_in;
    unsigned long long ea_out;
    unsigned int       size;
    int                pad[3];
} abs_params_t;

abs_params_t abs_params a16;

int main(int argc, char *argv[])
{
    int i, ret;
    
    spe_context_ptr_t     spe;
    spe_program_handle_t *prog;
    unsigned int          entry;
    spe_stop_info_t       stop_info;
    
    /* Load the SPE program into main memory */
    prog = spe_image_open("dma_tut_spu");
    if(!prog) {
        perror("SPE image open\n");
        exit(1);
    }
    
    /* Create the SPE context */
    spe = spe_context_create(0, NULL);
    if(!spe) {
        perror("SPE context create\n");
        exit(1);
    }
    
    /* Upload the program to the SPE */
    ret = spe_program_load(spe, prog);
    if(ret) {
        perror("SPE program load");
        exit(1);
    }
    
    /* Define SPE prog parameters */
    abs_params.ea_in  = (unsigned long) in;
    abs_params.ea_out = (unsigned long) out;
    abs_params.size   = SIZE;
    
    /* define the entry point for the SPE program */
    entry = SPE_DEFAULT_ENTRY;
    
    /* Run the SPE program */
    ret = spe_context_run(spe, &entry, 0, &abs_params, NULL, &stop_info);
    if(ret < 0) {
        perror("SPE context run\n");
        exit(1);
    }
    
    /* Destroy the SPE context */
    ret = spe_context_destroy(spe);
    if(ret) {
        perror("SPE context destroy\n");
        exit(1);
    }
    
    /* Close the SPE program from main memory */
    ret = spe_image_close(prog);
    if(ret) {
        perror("SPE image close\n");
        exit(1);
    }
    
    /* View our results of the abs program */
    for(i=0; i<SIZE; i++) {
        printf("out[%02d]=%0.0f\n", i, out[i]);
    }
    
    return 0;
}



