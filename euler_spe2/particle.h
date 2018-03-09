#ifndef PARTICLE_H_INCLUDED
#define PARTICLE_H_INCLUDED

#include "euler.h"

// data to be sent to the spe
typedef struct {
    int particles; // number of particles to process
    vector float *pos_v; // pointer to array of position vectors
    vector float *vel_v; // pointer to array of velocity vectors
    float *inv_mass; // pointer to array of mass vectors
    vector float force_v; // force vector
    float dt; // current step in time
} parm_context; // parameter context?

#endif // PARTICLE_H_INCLUDED
