/* this is an absolute value converter using the SPEs and DMA transfers, taken from
 * here: https://www.kernel.org/pub/linux/kernel/people/geoff/cell/ps3-linux-docs/CellProgrammingTutorial/BasicsOfSPEProgramming.html */

#include <stdio.h>
#include <spu_intrinsics.h>
#include <spu_mfcio.h>

// easier typing
#define a16 __attribute__((aligned(16)))

#define MAX_BUFSIZE (128)

float  in_spe[MAX_BUFSIZE] a16;
float out_spe[MAX_BUFSIZE] a16;

// same struct in dma_tut.c - defines our parameters
typedef struct {
    unsigned long long ea_in;
    unsigned long long ea_out;
    unsigned int       size;
    int                pad[3];
} abs_params_t;

abs_params_t abs_params a16;

int main(unsigned long long spe_id, unsigned long long argp, unsigned long long envp)
{
    unsigned int i;
    int tag = 1;
    
    /* New variables to use SIMD programming on the SPE */
    vector float *vin = (vector float *) in_spe;
    vector float *vout = (vector float *) out_spe;
    vector float vin_negative;
    vector unsigned int vpat;
    
    /* zeros to check if the input is greater than 0, -1 to multiply by
     * if necessary */
    vector float vzero = (vector float){0,0,0,0};
    vector float vminus = (vector float){-1,-1,-1,-1};
    
    /* DMA transfer 1: GET input/output parameters */
    spu_mfcdma64(&abs_params, mfc_ea2h(argp), mfc_ea2l(argp), 
                 sizeof(abs_params_t), tag, MFC_GET_CMD);
    spu_writech(MFC_WrTagMask, 1 << tag);
    spu_mfcstat(MFC_TAG_UPDATE_ALL);
    
    /* DMA transfer 2: GET input data */
    spu_mfcdma64(in_spe, mfc_ea2h(abs_params.ea_in), mfc_ea2l(abs_params.ea_in),
                 abs_params.size*sizeof(float), tag, MFC_GET_CMD);
    spu_writech(MFC_WrTagMask, 1 << tag);
    spu_mfcstat(MFC_TAG_UPDATE_ALL);
    
    /* Calculate absolute values */
    //for(i=0; i<abs_params.size; i++) {
    for(i=0; i<abs_params.size/4; i++) {
        
        /*--- Previous code from last tut (dma_tut) ----*/
        
        //if(in_spe[i] > 0) out_spe[i] = in_spe[i];
        //else              out_spe[i] = -1*in_spe[i];
        
        /*------------ New code --------------------*/
        
        // insert 1 or 0 into the pattern vector if the input number
        // is greater than 0 or not
        vpat = spu_cmpgt(vin[i], vzero);
        
        // multiply all input numbers in the current vector
        // by negative 1 and we'll select which ones to use from
        // it next
        vin_negative = spu_mul(vin[i], vminus);
        
        // select from either the input vector or negated input 
        // vector based on what was put in the pattern vector
        vout[i] = spu_sel(vin_negative, vin[i], vpat);
    }
    
    /* DMA transfer 3: PUT output data */
    spu_mfcdma64(out_spe, mfc_ea2h(abs_params.ea_out), mfc_ea2l(abs_params.ea_out),
                 abs_params.size * sizeof(float), tag, MFC_PUT_CMD);
    spu_writech(MFC_WrTagMask, 1 << tag);
    spu_mfcstat(MFC_TAG_UPDATE_ALL);
    
    return 0;
}





