/* this is an absolute value converter using the SPEs and DMA transfers, taken from
 * here: https://www.kernel.org/pub/linux/kernel/people/geoff/cell/ps3-linux-docs/CellProgrammingTutorial/BasicsOfSPEProgramming.html */
#include <stdio.h>
#include <stdlib.h>
#include <libspe2.h>
#include <pthread.h>

// easier typing
#define a16 __attribute__((aligned(16)))

// the amount of numbers to convert to absolute value
#define SIZE (64)

// the number of SPU/SPEs to use
#define NUM_SPE 4

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

// now we need a parameter struct for each SPU
//abs_params_t abs_params a16;
abs_params_t abs_params[NUM_SPE];

// arguments for our local pthreads
typedef struct {
    spe_context_ptr_t spe;
    abs_params_t      *abs_params;
} thread_arg_t;

// the function for our local pthread to run (starts execution on a SPE)
void *run_abs_spe(void *thread_arg)
{
    int ret;
    thread_arg_t *arg = (thread_arg_t *) thread_arg;
    
    unsigned int entry;
    spe_stop_info_t stop_info;
    
    /* Define the entry point for our SPE program */
    entry = SPE_DEFAULT_ENTRY;
    
    /* Run our SPE program */
    ret = spe_context_run(arg->spe, &entry, 0, arg->abs_params, NULL, &stop_info);
    if(ret < 0) {
        perror("SPE context run\n");
        return NULL;
    }
    
    return NULL;
}

int main(int argc, char *argv[])
{
    int i, ret, size;
    
    // we need a spe context for each thread this time
    //spe_context_ptr_t     spe;
    spe_context_ptr_t spe[NUM_SPE];
    
    // still only need 1 prog since its the same program for each thread
    spe_program_handle_t *prog;
    
    /* Local thread and thread arguments */
    pthread_t thread[NUM_SPE];
    thread_arg_t arg[NUM_SPE];
    
    // these are used in *run_abs_spe() now instead
    //unsigned int          entry;
    //spe_stop_info_t       stop_info;
    
    /* Load the SPE program into main memory */
    prog = spe_image_open("dma_tut_spu");
    if(!prog) {
        perror("SPE image open\n");
        exit(1);
    }
    
    
    /* For each thread */
    for(i=0; i<NUM_SPE; i++) {
        
        /* Create the SPE context */
        spe[i] = spe_context_create(0, NULL);
        if(!spe[i]) {
            perror("SPE context create\n");
            exit(1);
        }
        
        /* Upload the program to the SPE */
        ret = spe_program_load(spe[i], prog);
        if(ret) {
            perror("SPE program load");
            exit(1);
        }
    }
    
    /* Calculate how much data to send each thread */
    size = SIZE / NUM_SPE;
    
    for(i=0; i<NUM_SPE; i++) {
    
        /* Define SPE prog parameters */
        abs_params[i].ea_in  = (unsigned long) in;
        abs_params[i].ea_out = (unsigned long) out;
        abs_params[i].size   = SIZE;
        
        /* Define pthread function parameters */
        arg[i].spe = spe[i];
        arg[i].abs_params = &abs_params[i];
        
        // this is done in the pthread function (*run_abs_spe) now
        /* define the entry point for the SPE program */
        //entry = SPE_DEFAULT_ENTRY;
        
        // this is done in the pthread function (*run_abs_spe) now
        /* Run the SPE program */
        /*ret = spe_context_run(spe, &entry, 0, &abs_params, NULL, &stop_info);
        if(ret < 0) {
            perror("SPE context run\n");
            exit(1);
        }*/
    
        /* Create our local thread to run the SPE program */
        ret = pthread_create(&thread[i], NULL, run_abs_spe, &arg[i]);
        if(ret) {
            perror("pthread create\n");
            exit(1);
        }
    
    }
    
    for(i=0; i<NUM_SPE; i++) {
        
        /* Wait for the local pthread, and the SPE program relative to that
         * thread, to finish */
        pthread_join(thread[i], NULL);
    
        /* Destroy the SPE context */
        ret = spe_context_destroy(spe[i]);
        if(ret) {
            perror("SPE context destroy\n");
            exit(1);
        }
    
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



