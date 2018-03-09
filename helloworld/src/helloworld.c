#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <libspe2.h>
#include <pthread.h>

/* Handle to the SPE program */
extern spe_program_handle_t hello_spu;

/* 2 threads per SPU? */
#define MAX_SPU_THREADS 16

/* Function for each thread to run on the PPU */
void *ppu_thread_func(void *arg) 
{
    spe_context_ptr_t ctx;
    unsigned int entry = SPE_DEFAULT_ENTRY;

    /* Get our pointer to the SPE context */
    ctx = *((spe_context_ptr_t *)arg);

    /* Run our SPE context */
    if(spe_context_run(ctx,&entry,0,NULL,NULL,NULL) < 0) {
        perror("Failed running context\n");
        exit(1);
    }
    pthread_exit(NULL);
}

int main(void)
{
    int i;
    int spu_threads; // how many SPEs are available

    spe_context_ptr_t ctxs[MAX_SPU_THREADS];
    pthread_t threads[MAX_SPU_THREADS];

    /* Determine the number of SPE threads to create */
    spu_threads = spe_cpu_info_get(SPE_COUNT_USABLE_SPES, -1);
    if(spu_threads > MAX_SPU_THREADS) spu_threads = MAX_SPU_THREADS;

    /* Create several SPE threads */
    for(i=0; i<spu_threads; i++)
    {
        /* Create SPE context */
        if((ctxs[i] = spe_context_create(0,NULL)) == NULL) {
            perror("Failed creating context!\n");
            exit(1);
        }

        /* Load program into context */
        if(spe_program_load(ctxs[i], &hello_spu)) {
            perror("Failed loading program!\n");
            exit(1);
        }

        /* Create thread for each SPE context */
        if(pthread_create(&threads[i], NULL, &ppu_thread_func, &ctxs[i])) {
            perror("Failed creating thread!\n");
            exit(1);
        }
    }

    /* Wait for SPU-thread to complete execution */
    for(i=0; i<spu_threads; i++) {
        if(pthread_join(threads[i], NULL)) {
            perror("Failed pthread_join!\n");
            exit(1);
        }
    }

    printf("PPU Program successfully completed!\n");
    return 0;

    return 0;
}




