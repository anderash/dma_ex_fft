/*
 * dma.h
 *
 *  Created on: Jul 31, 2013
 *      Author: bwiec
 */

#ifndef DMA_H_
#define DMA_H_

// Includes
#include "xaxidma.h"
#include "xscugic.h"

// Hardware-dependent parameters
#define DMA_ACCEL_IS_CACHE_COHERENT 0 // Set to 1 to avoid overhead of software cache flushes if going through the ACP

// Return types
#define DMA_ACCEL_SUCCESS          0
#define DMA_ACCEL_ALLOC_BUF_FAIL  -1
#define DMA_ACCEL_UNALLOCATED_BUF -2

// Public data structures
typedef struct dma_accel dma_accel_t;

// Public functions

//
// dma_accel_create - Create a DMA Accelerator object
//
dma_accel_t* dma_accel_create(int dma_device_id, int intc_device_id, int s2mm_intr_id,
		                      int mm2s_intr_id, int sample_size_bytes);

//
// dma_accel_destroy - Destroy DMA Accelerator object
//
void dma_accel_destroy(dma_accel_t* p_dma_accel_inst);

//
// dma_accel_get_stim_buf - Get a pointer to the stimulus buffer (not needed, I don't think)
//
void* dma_accel_get_stim_buf(dma_accel_t* p_dma_accel_inst);

//
// dma_accel_set_stim_buf - Set pointer to stimulus buffer to be used
//
void dma_accel_set_stim_buf(dma_accel_t* p_dma_accel_inst, void* p_stim_buf);

//
// dma_accel_get_result_buf - Get a pointer to the result buffer so the caller can consume the data.
//
void* dma_accel_get_result_buf(dma_accel_t* p_dma_accel_inst);

//
// dma_accel_set_result_buf - Set pointer to result buffer to be used
//
void dma_accel_set_result_buf(dma_accel_t* p_dma_accel_inst, void* p_result_buf);

//
// dma_accel_get_buf_length - Get the buffer length to be used for DMA transfers
//
int dma_accel_get_buf_length(dma_accel_t* p_dma_accel_inst);

int dma_accel_get_sample_size_bytes(dma_accel_t* p_dma_accel_inst);

void dma_accel_set_sample_size_bytes(dma_accel_t* p_dma_accel_inst, int sample_size_bytes);

//
// dma_accel_set_buf_length - Set the buffer length to use for DMA transfers
//
void dma_accel_set_buf_length(dma_accel_t* p_dma_accel_inst, int buf_length);

//
// dma_accel_xfer - Perform data transfers via DMA
//
//  Arguments
//    - p_axi_dma_inst: Pointer to AXI DMA driver instance
//
//  Return
//    - XST_SUCCESS:       No errors occurred during any DMA transfers
//    - XST_FAILURE:       Some error occurred during one of the DMA transfers
//
int dma_accel_xfer(dma_accel_t* p_dma_accel_inst);



#endif /* DMA_H_ */

