/* SPU code for euler particle simulation, taken
 * from Cell SDK tutorial code */

#include <spu_intrinsics.h>
#include <spu_mfcio.h>
#include "../particle.h"

#define PARTICLES_PER_BLOCK 1024 // number of particles to process at a time

// easier for typing
#define a16 __attribute__ ((aligned (16)))
#define a128 __attribute__ ((aligned (128)))
#define aunused __attribute__ ((unused))

// local store structures and buffers
volatile parm_context ctx;
volatile vector float pos[PARTICLES_PER_BLOCK] a128;
volatile vector float vel[PARTICLES_PER_BLOCK] a128;
volatile float inv_mass[PARTICLES_PER_BLOCK] a128;

int main(unsigned long long spu_id aunused, unsigned long long parm)
{
    int i, j;
    int left, cnt;
    float time;
    unsigned int tag_id;
    vector float dt_v, dt_inv_mass_v;

    // reserve a tag id
    tag_id = mfc_tag_reserve();

    spu_writech(MFC_WrTagMask, -1);

    // input parameter parm is a pointer to the particle parameter ctx
    // fetch the ctx, waiting for it to complete

    spu_mfcdma32((void*)&ctx, (unsigned int)parm, sizeof(parm_context), tag_id, MFC_GET_CMD);
    (void)spu_mfcstat(MFC_TAG_UPDATE_ALL);

    dt_v = spu_splats(ctx.dt);

    // for each step in time
    for(time=0; time<END_OF_TIME; time += ctx.dt)
    {
        // for each block of particles
        for(i=0; i<ctx.particles; i+=PARTICLES_PER_BLOCK)
        {
            // determine the number of particles in this block
            left = ctx.particles - i;
            cnt = (left < PARTICLES_PER_BLOCK) ? left : PARTICLES_PER_BLOCK;

            // fetch the data - positon, velocity, and inverse_mass. wait for
            // the DMA to complete before performing computation
            spu_mfcdma32((void*)(pos), (unsigned int)(ctx.pos_v+i), cnt*sizeof(vector float), tag_id, MFC_GETB_CMD);
            spu_mfcdma32((void*)(vel), (unsigned int)(ctx.vel_v+i), cnt*sizeof(vector float), tag_id, MFC_GET_CMD);
            spu_mfcdma32((void*)(inv_mass), (unsigned int)(ctx.inv_mass+i), cnt*sizeof(float), tag_id, MFC_GET_CMD);
            (void)spu_mfcstat(MFC_TAG_UPDATE_ALL);

            // compute the step in time for the block of particles
            for(j=0; j<cnt; j++)
            {
                pos[j] = spu_madd(vel[j], dt_v, pos[j]);
                dt_inv_mass_v = spu_mul(dt_v, spu_splats(inv_mass[j]));
                vel[j] = spu_madd(dt_inv_mass_v, ctx.force_v, vel[j]);
            }

            // put the position and velocity back into system memory
            spu_mfcdma32((void*)(pos), (unsigned int)(ctx.pos_v+i), cnt*sizeof(vector float), tag_id, MFC_PUT_CMD);
            spu_mfcdma32((void*)(vel), (unsigned int)(ctx.vel_v+i), cnt*sizeof(vector float), tag_id, MFC_PUT_CMD);

        }
    }

    // wait for final DMAs to complete before terminating SPU thread
    (void)spu_mfcstat(MFC_TAG_UPDATE_ALL);
    return 0;
}
