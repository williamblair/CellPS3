/* SIMD version 1 - ppu vecs only
 * taken from Cell SDK tutorial code
 *
 * the method used here is called Array of Structs (since we
 * have the struct defined in euler.h and pos/vel array of them) */

#include "euler.h"
#include <altivec.h>
#include <stdio.h>

// easier for typing
#define a16 __attribute__ ((aligned (16)))

vec4D pos[PARTICLES] a16; // particle positions
vec4D vel[PARTICLES] a16; // particle velocities
vec4D force a16; // current force being applied to the particles
float inv_mass[PARTICLES] a16; // inverse mass of the particles
float dt a16 = 1.0f; // step in time

int main(void)
{
    int i;
    float time;
    float dt_inv_mass a16;

    // vector versions so we can do 4 SIMD at once
    vector float dt_v, dt_inv_mass_v;
    vector float *pos_v, *vel_v, force_v;

    // zero vector
    vector float zero = (vector float){0.0f, 0.0f, 0.0f, 0.0f};

    // cast vector versions
    // possible since vec4D is just a struct with x,y,z,w, same
    // as a vector
    pos_v   = (vector float *)pos;
    vel_v   = (vector float *)vel;
    force_v = *((vector float *)&force);

    // copy the time step variable into the x,y,z of a vector
    // and make the last element 0
    // vec_sld - shift left double
    // vec_splat - vector splat (returns a vec filled with given value(s))
    // vec_lde - vector load element indexed
    dt_v = vec_sld(vec_splat(vec_lde(0,&dt), 0), zero, 4);

    // for each step in time
    for(time=0; time<END_OF_TIME; time+=dt)
    {
        // for each particle
        for(i=0; i<PARTICLES; i++)
        {
            // ------------ previous code ------------
            // compute the new position and velocity
            //pos[i].x = vel[i].x * dt + pos[i].x;
            //pos[i].y = vel[i].y * dt + pos[i].y;
            //pos[i].z = vel[i].z * dt + pos[i].z;

            // ------------ new code ------------------
            // vec_madd - vector multiply add (does same as
            // previous code)
            pos_v[i] = vec_madd(vel_v[i], dt_v, pos_v[i]);

            // calculate the inv_mass like before, but then
            // copy it into a vector for the vec_* funcs
            dt_inv_mass = dt * inv_mass[i];
            dt_inv_mass_v = vec_splat(vec_lde(0, &dt_inv_mass), 0);

            // -------------- previous code-------------
            //vel[i].x = dt_inv_mass * force.x + vel[i].x;
            //vel[i].y = dt_inv_mass * force.y + vel[i].y;
            //vel[i].z = dt_inv_mass * force.z + vel[i].z;

            // ------------- new code -------------------
            vel_v[i] = vec_madd(dt_inv_mass_v, force_v, vel_v[i]);
        }
    }

    return 0;
}
