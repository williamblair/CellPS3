/* Scalar version of euler particle simulation
 * taken from Cell SDK tutorial code */

#include "euler.h"
#include <stdio.h>

vec4D pos[PARTICLES]; // particle positions
vec4D vel[PARTICLES]; // particle velocities
vec4D force; // current force being applied to the particles
float inv_mass[PARTICLES]; // inverse mass of the particles
float dt = 1.0f; // step in time

int main(void)
{
    int i;
    float time;
    float dt_inv_mass;

    // for each step in time
    for(time=0; time<END_OF_TIME; time+=dt)
    {
        // for each particle
        for(i=0; i<PARTICLES; i++)
        {
            // compute the new position and velocity
            pos[i].x = vel[i].x * dt + pos[i].x;
            pos[i].y = vel[i].y * dt + pos[i].y;
            pos[i].z = vel[i].z * dt + pos[i].z;

            dt_inv_mass = dt * inv_mass[i];

            vel[i].x = dt_inv_mass * force.x + vel[i].x;
            vel[i].y = dt_inv_mass * force.y + vel[i].y;
            vel[i].z = dt_inv_mass * force.z + vel[i].z;
        }
    }

    return 0;
}
