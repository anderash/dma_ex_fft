/*
 * isr.h
 *
 *  Created on: Aug 2, 2013
 *      Author: bwiec
 */

#ifndef ISR_H_
#define ISR_H_

// ***** Defines *****
#define RESET_TIMEOUT_COUNTER	10000

// ***** Includes *****
void s2mm_isr(void* CallbackRef);
void mm2s_isr(void* CallbackRef);

#endif /* ISR_H_ */
