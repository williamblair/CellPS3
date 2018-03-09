/* SIMD version 2 - ppu vecs only
 * taken from Cell SDK tutorial code
 *
 * the method used here is called Struct of Arrays as
 * each element of the previous struct is split up into separate
 * arrays */

#include "euler.h"
#include <altivec.h>
#include <stdio.h>

// easier for typing
#define a16 __attribute__ ((aligned (16)))

// Seperate arrays for each component of the vector
vector float pos_x[PARTICLES/4], pos_y[PARTICLES/4], pos_z[PARTICLES/4];
vector float vel_x[PARTICLES/4], vel_y[PARTICLES/4], vel_z[PARTICLES/4];
vec4D force a16;
float inv_mass[PARTICLES] a16;
float dt = 1.0f;

int main(void)
{
    int i;
    float time;
    float dt_inv_mass a16;

    // vector versions so we can do 4 SIMD at once
    vector float dt_v, dt_inv_mass_v;
    vector float force_v, force_x, force_y, force_z;

    // cast force as a vec for force_v and copy the x,y,and z
    // components into force_x,y,and z respectively
    force_v = *((vector float *)&force);
    force_x = vec_splat(force_v, 0); // index 0 for force_v for x
    force_y = vec_splat(force_v, 1); // index 1 for force_v for y
    force_z = vec_splat(force_v, 2); // index 2 for force_v for z

    // copy time step into all 4 elements instead of x,y,z then 0
    dt_v = vec_splat(vec_lde(0, &dt), 0);

    // for each step in time
    for(time=0; time<END_OF_TIME; time+=dt)
    {
        // for each particle (split for 4 element vecs)
        for(i=0; i<PARTICLES/4; i++)
        {
            // ------------ previous code ------------------
            // vec_madd - vector multiply add (does same as
            // previous code)
            //pos_v[i] = vec_madd(vel_v[i], dt_v, pos_v[i]);

            // ----------- new code -----------------------
            // calculates each component individually
            pos_x[i] = vec_madd(vel_x[i], dt_v, pos_x[i]);
            pos_y[i] = vec_madd(vel_y[i], dt_v, pos_y[i]);
            pos_z[i] = vec_madd(vel_z[i], dt_v, pos_z[i]);


            // calculate the inv_mass like before, but then
            // copy it into a vector for the vec_* funcs
            dt_inv_mass = dt * inv_mass[i];
            dt_inv_mass_v = vec_splat(vec_lde(0, &dt_inv_mass), 0);

            // ------------- previous code -------------------
            //vel_v[i] = vec_madd(dt_inv_mass_v, force_v, vel_v[i]);

            // ------------- new code -------------------------
            vel_x[i] = vec_madd(dt_inv_mass_v, force_x, vel_x[i]);
            vel_y[i] = vec_madd(dt_inv_mass_v, force_y, vel_y[i]);
            vel_z[i] = vec_madd(dt_inv_mass_v, force_z, vel_z[i]);

        }
    }

    return 0;
}
