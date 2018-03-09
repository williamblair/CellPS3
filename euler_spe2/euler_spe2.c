/* SPE thread version 1 - runs on a single SPE
 * taken from Cell SDK tutorial code
 * */

#include "particle.h"
#include <stdlib.h>
#include <libspe2.h>
#include <pthread.h>
#include <stdio.h>

// easier for typing
#define a16 __attribute__ ((aligned (16)))
#define a128 __attribute__ ((aligned (128)))

#define MAX_SPE_THREADS 16

// Seperate arrays for each component of the vector
vec4D pos[PARTICLES] a128;
vec4D vel[PARTICLES] a128;
vec4D force a16;
float inv_mass[PARTICLES] a128;
float dt = 1.0f;

// SPE program handle
extern spe_program_handle_t particle;

// arg for each thread
typedef struct ppu_pthread_data {
    spe_context_ptr_t spe_ctx;
    pthread_t pthread;
    unsigned int entry;   // new for this code - unique entry for each thread
    void *argp;
} ppu_pthread_data_t;

// function for each spe thread to run
void *ppu_pthread_function(void *arg) {
    // cast our argument as our defined struct above
    ppu_pthread_data_t *datap = (ppu_pthread_data_t *)arg;

    // this time we use the entry unique to this thread intead
    // of the default SPE entry
    if(spe_context_run(datap->spe_ctx, &datap->entry, 0, datap->argp,
                       NULL, NULL) < 0) {
        perror("Failed running context\n");
        exit(1);
    }

    pthread_exit(NULL);
}

int main(void)
{
    int i, offset, count, spe_threads;
    // this time we have a data struct and context
    // for each thread
    ppu_pthread_data_t datas[MAX_SPE_THREADS];
    parm_context ctxs[MAX_SPE_THREADS] a16;

    // determine the number of SPE threads to create
    spe_threads = spe_cpu_info_get(SPE_COUNT_USABLE_SPES, -1);
    if(spe_threads > MAX_SPE_THREADS) spe_threads = MAX_SPE_THREADS;

    for(i=0, offset=0; i<spe_threads; i++, offset+=count)
    {
        // calculate the number of particles to simulate
        count = (PARTICLES / spe_threads + 3) & ~3;

        // specify our thread context parameters
        ctxs[i].particles = (i==(spe_threads-1)) ? PARTICLES-offset : count;
        ctxs[i].pos_v = (vector float *)&pos[offset];
        ctxs[i].vel_v = (vector float *)&vel[offset];
        ctxs[i].force_v = *((vector float *)&force);
        ctxs[i].inv_mass = &inv_mass[offset];
        ctxs[i].dt = dt;

        // create a SPE context
        if((datas[i].spe_ctx = spe_context_create(0, NULL)) == NULL) {
            perror("Failed creating context\n");
            exit(1);
        }

        // load SPE program into the SPE context
        if(spe_program_load(datas[i].spe_ctx, &particle)) {
            perror("Failed loading program\n");
            exit(1);
        }

        // initialize the context run data
        datas[i].entry = SPE_DEFAULT_ENTRY;
        datas[i].argp = &ctxs[i];
        // create a pthread for each of the spe contexts
        if(pthread_create(&datas[i].pthread, NULL, &ppu_pthread_function, &datas[i])) {
            perror("Failed creating thread");
            exit(1);
        }
    }

    // wait for threads to complete
    for(i=0; i<spe_threads; i++) {
        if(pthread_join(datas[i].pthread, NULL)) {
            perror("Failed joining thread\n");
            exit(1);
        }
    }

    // destroy spe context
    for(i=0; i<spe_threads; i++) {
        if(spe_context_destroy(datas[i].spe_ctx) != 0) {
            perror("Failed destroying context");
            exit(1);
        }
    }

    return 0;
}
