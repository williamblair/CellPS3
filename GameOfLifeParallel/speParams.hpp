/* Arguments to send to the SPE */

#ifndef SPE_PARAMS_H_INCLUDED
#define SPE_PARAMS_H_INCLUDED

/* SPE Parameters */
typedef struct {
    unsigned long long ea_in;  // effective address in (pointer)
    unsigned long long ea_out; // effective address out (pointer)
    unsigned int       size;   // the amount of data we're working with
    int                pad[3]; // pad data to make the size of this struct 32 bytes
} speParams;

#endif
