/*
 * cplx.h
 *
 *  Created on: May 11, 2015
 *      Author: bwiec
 */

#ifndef CPLX_H_
#define CPLX_H_

// For use with our FFT application, this needs to be 16-bit samples
// packed into a 32-bit word so that the DMA can populate it automatically
//
// Should make this generic
typedef struct cplx_data
{
	short data_re;
	short data_im;
} cplx_data_t;

#endif /* CPLX_H_ */

// Public functions
void cplx_data_get_string(char* c, cplx_data_t data);
