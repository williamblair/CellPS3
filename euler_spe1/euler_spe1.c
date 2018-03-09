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
    void *argp;
} ppu_pthread_data_t;

// function for each spe thread to run
void *ppu_pthread_function(void *arg) {
    // cast our argument as our defined struct above
    ppu_pthread_data_t *datap = (ppu_pthread_data_t *)arg;

    unsigned int entry = SPE_DEFAULT_ENTRY;

    if(spe_context_run(datap->spe_ctx, &entry, 0, datap->argp,
                       NULL, NULL) < 0) {
        perror("Failed running context\n");
        exit(1);
    }

    pthread_exit(NULL);
}

int main(void)
{
    ppu_pthread_data_t data;
    parm_context ctx a16;

    // specify our thread context parameters
    ctx.particles = PARTICLES;
    ctx.pos_v = (vector float *)pos;
    ctx.vel_v = (vector float *)vel;
    ctx.force_v = *((vector float *)&force);
    ctx.inv_mass = inv_mass;
    ctx.dt = dt;

    // create a SPE context
    if((data.spe_ctx = spe_context_create(0, NULL)) == NULL) {
        perror("Failed creating context\n");
        exit(1);
    }

    // load SPE program into the SPE context
    if(spe_program_load(data.spe_ctx, &particle)) {
        perror("Failed loading program\n");
        exit(1);
    }

    // initialize the context run data
    data.argp = &ctx;
    // create a pthread for each of the spe contexts
    if(pthread_create(&data.pthread, NULL, &ppu_pthread_function, &data)) {
        perror("Failed creating thread");
        exit(1);
    }

    // wait for threads to complete
    if(pthread_join(data.pthread, NULL)) {
        perror("Failed joining thread\n");
        exit(1);
    }

    // destroy spe context
    if(spe_context_destroy(data.spe_ctx) != 0) {
        perror("Failed destroying context");
        exit(1);
    }

    return 0;
}
