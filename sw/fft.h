/*
 * fft.h
 *
 *  Created on: May 11, 2015
 *      Author: bwiec
 */

#ifndef FFT_H_
#define FFT_H_

// Includes
#include "cplx.h"
#include "dma.h"
#include "xgpio.h"

// Hardware-specific parameters
#define FFT_MAX_NUM_PTS      8192
#define FFT_NUM_PTS_MASK     0x0000001F // Bits [4:0]
#define FFT_FWD              0x00000001
#define FFT_INV              0x00000000
#define FFT_FWD_INV_MASK     0x00000100 // Bit 8
#define FFT_SCALE_SCH_MASK   0x007FFE00 // Bits [22:9]
#define FFT_XK_RE_MASK       0x0000FFFF
#define FFT_XK_IM_MASK       0xFFFF0000
#define FFT_SIGN_TEST_MASK   0x00008000 // Testing for signedness. Used for sign extension
#define FFT_SIGN_EXTEND_MASK 0xFFFF0000

// Return types
#define FFT_SUCCESS          0
#define FFT_ILLEGAL_NUM_PTS -1
#define FFT_DMA_FAIL        -2

// Enumerated data types
typedef enum
{
	FFT_INVERSE = 0,
	FFT_FORWARD = 1
} fft_fwd_inv_t;

// Private data
typedef struct fft fft_t;


// Public functions

//
// dma_accel_create - Create a FFT object
//
fft_t* fft_create(int gpio_device_id, int dma_device_id, int intc_device_id, int s2mm_intr_id, int mm2s_intr_id);

//
// fft_destroy - Destroy FFT object
//
void fft_destroy(fft_t* p_fft_inst);

//
// fft_set_num_pts - Set the number of points to use for the FFT
//
//  Arguments
//    - p_fft_config: Pointer to the FFT driver instance
//    - num_pts:      Number of points to use for the FFT
//
//  Return
//    - XST_SUCCESS:  Number of points in the FFT is legal and the object is set
//    - XST_FAILURE:  Number of points in the FFT is illegal and the object is not set
//
int fft_set_num_pts(fft_t* p_fft_inst, int num_pts);

//
// fft_get_num_pts - Get the current setting for the number of points to use for the FFT
//
//  Arguments
//    - p_fft_config: Pointer to the FFT driver instance
//
//  Return
//    - int:  Number of points to be used for the FFT
//
int fft_get_num_pts(fft_t* p_fft_inst);

//
// fft_set_fwd_inv - Set the whether to do forward or inverse FFT
//
//  Arguments
//    - p_fft_config: Pointer to the FFT driver instance
//    - fwd_inv:      Whether to do forward or inverse FFT
//
//  Return
//    - XST_SUCCESS:  Setting for whether to do forward or inverse FFT is legal and the object is set
//    - XST_FAILURE:  Setting for whether to do forward or inverse FFT is illegal and the object is not set
//
void fft_set_fwd_inv(fft_t* p_fft_inst, fft_fwd_inv_t fwd_inv);

//
// fft_get_fwd_inv - Get the current setting for whether to do forward or inverse FFT
//
//  Arguments
//    - p_fft_config: Pointer to the FFT driver instance
//
//  Return
//    - int:  Whether to do forward or inverse FFT
//
fft_fwd_inv_t fft_get_fwd_inv(fft_t* p_fft_inst);

//
// fft_set_scale_sch - Set the scaling schedule for the FFT
//
//  Arguments
//    - p_fft_config: Pointer to the FFT driver instance
//    - scale_sch:    Scaling schedule for the FFT
//
//  Return
//    - XST_SUCCESS:  Setting for scaling schedule for the FFT is legal and the object is set
//    - XST_FAILURE:  Setting for scaling schedule for the FFT is illegal and the object is not set
//
void fft_set_scale_sch(fft_t* p_fft_inst, int scale_sch);

//
// fft_get_scale_sch - Get the current setting for the scaling schedule
//
//  Arguments
//    - p_fft_config: Pointer to the FFT driver instance
//
//  Return
//    - int:  Scaling schedule for the FFT
//
int fft_get_scale_sch(fft_t* p_fft_inst);

//
// fft - Fast Fourier Transform
//
int fft(fft_t* p_fft_inst, cplx_data_t* din, cplx_data_t* dout);

//
// fft_get_stim_buf - Expose the stimulus buffer to the user (mostly for debugging)
//
cplx_data_t* fft_get_stim_buf(fft_t* p_fft_inst);

//
// fft_get_result_buf - Expose the result buffer to the user (mostly for debugging)
//
cplx_data_t* fft_get_result_buf(fft_t* p_fft_inst);

//
// fft_print_params - Print current state of fft struct
//
void fft_print_params(fft_t* p_fft_inst);

//
// fft_print_results - Print FFT result values
//
//  Arguments
//    - p_fft_config:          Pointer to the FFT driver instance
//    - p_result_buf:          Pointer to the FFT output data
//
void fft_print_results(fft_t* p_fft_inst, cplx_data_t* p_result_buf);

#endif /* FFT_H_ */

