/*
 * dma.h
 *
 *  Created on: Jul 31, 2013
 *      Author: bwiec
 */

#ifndef DMA_H_
#define DMA_H_

// ***** Includes *****
#include "xaxidma.h"
#include "xscugic.h"

// ***** Defines *****
#define MAX_FFT_LENGTH     8192
#define FFT_LENGTH_MASK    0x0000001F // Bits [4:0]
#define FFT_FWD            0x00000001
#define FFT_INV            0x00000000
#define FFT_FWD_INV_MASK   0x00000100 // Bit 8
#define FFT_SCALE_SCH_MASK 0x007FFE00 // Bits [22:9]
#define XK_RE_MASK         0x0000FFFF
#define XK_IM_MASK         0xFFFF0000
#define SIGN_TEST_MASK     0x00008000 // Testing for signedness. Used for sign extension
#define SIGN_EXTEND_MASK   0xFFFF0000

// ***** Data structures *****
typedef struct fft_config
{
	int fft_len;  // Must be a power of 2
	int fwd_inv;  // 1 = forward, 0 = inverse
	int scale_sch;
} fft_config_t;

// ***** Prototypes *****
int init_drivers();
int dma_init(XAxiDma_Config* cfg_ptr);
int intr_init(XScuGic_Config *gic_cfg);
int gpio_init();
int which_fft_param(fft_config_t* fft_params);
int run_fft(fft_config_t* fft_params, int* stim_buf, int* result_buf);
int fft_config(fft_config_t* fft_params);
int dma_xfer(fft_config_t* fft_params, int* s2mm_buf, int* mm2s_buf);
int print_results(fft_config_t* fft_params, int* result_buf);

#endif /* DMA_H_ */
