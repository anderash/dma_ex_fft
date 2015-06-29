/*
 * cplx.c
 *
 *  Created on: May 11, 2015
 *      Author: bwiec
 */


#include <stdio.h>
#include "cplx.h"

void cplx_data_get_string(char* c, cplx_data_t data)
{
	sprintf(c, "%d + j*%d", data.data_re, data.data_im);
}
