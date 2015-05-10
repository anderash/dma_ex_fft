/*
 * isr.c
 *
 *  Created on: Aug 2, 2013
 *      Author: bwiec
 */

// ***** Includes *****
#include "xaxidma.h"
#include "xscugic.h"

#include "isr.h"

// ***** External variables *****
extern XScuGic axi_intc;
extern XAxiDma axi_dma;
extern int dma_err;
extern int s2mm_done;
extern int mm2s_done;

void s2mm_isr(void* CallbackRef)
{
	u32 irq_status;
	int time_out;
	XAxiDma *AxiDmaInst = (XAxiDma *)CallbackRef;

	// Disable interrupts
	XAxiDma_IntrDisable(&axi_dma, XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DMA_TO_DEVICE);
	XAxiDma_IntrDisable(&axi_dma, XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DEVICE_TO_DMA);

	// Read pending interrupts
	irq_status = XAxiDma_IntrGetIrq(AxiDmaInst, XAXIDMA_DEVICE_TO_DMA);

	// Acknowledge pending interrupts
	XAxiDma_IntrAckIrq(AxiDmaInst, irq_status, XAXIDMA_DEVICE_TO_DMA);

	// If no interrupt is asserted, we do not do anything
	if (!(irq_status & XAXIDMA_IRQ_ALL_MASK))
		return;

	/*
	 * If error interrupt is asserted, raise error flag, reset the
	 * hardware to recover from the error, and return with no further
	 * processing.
	 */
	if ((irq_status & XAXIDMA_IRQ_ERROR_MASK))
	{

		dma_err = 1;

		// Reset should never fail for transmit channel
		XAxiDma_Reset(AxiDmaInst);

		time_out = RESET_TIMEOUT_COUNTER;
		while (time_out)
		{
			if (XAxiDma_ResetIsDone(AxiDmaInst))
				break;

			time_out -= 1;
		}

		return;
	}

	// Completion interrupt asserted
	if (irq_status & XAXIDMA_IRQ_IOC_MASK)
		s2mm_done = 1;

	// Re-enable interrupts
	XAxiDma_IntrEnable(&axi_dma, XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DMA_TO_DEVICE);
	XAxiDma_IntrEnable(&axi_dma, XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DEVICE_TO_DMA);

}

void mm2s_isr(void* CallbackRef)
{

	u32 irq_status;
	int time_out;
	XAxiDma *AxiDmaInst = (XAxiDma *)CallbackRef;

	// Disable interrupts
	XAxiDma_IntrDisable(&axi_dma, XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DMA_TO_DEVICE);
	XAxiDma_IntrDisable(&axi_dma, XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DEVICE_TO_DMA);

	// Read pending interrupts
	irq_status = XAxiDma_IntrGetIrq(AxiDmaInst, XAXIDMA_DMA_TO_DEVICE);

	// Acknowledge pending interrupts
	XAxiDma_IntrAckIrq(AxiDmaInst, irq_status, XAXIDMA_DMA_TO_DEVICE);

	// If no interrupt is asserted, we do not do anything
	if (!(irq_status & XAXIDMA_IRQ_ALL_MASK))
		return;

	/*
	 * If error interrupt is asserted, raise error flag, reset the
	 * hardware to recover from the error, and return with no further
	 * processing.
	 */
	if (irq_status & XAXIDMA_IRQ_ERROR_MASK)
	{

		dma_err = 1;

		// Reset could fail and hang
		XAxiDma_Reset(AxiDmaInst);

		time_out = RESET_TIMEOUT_COUNTER;

		while (time_out)
		{
			if (XAxiDma_ResetIsDone(AxiDmaInst))
				break;

			time_out -= 1;
		}

		return;
	}

	// If completion interrupt is asserted, then set RxDone flag
	if (irq_status & XAXIDMA_IRQ_IOC_MASK)
		mm2s_done = 1;

	// Re-enable interrupts
	XAxiDma_IntrEnable(&axi_dma, XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DMA_TO_DEVICE);
	XAxiDma_IntrEnable(&axi_dma, XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DEVICE_TO_DMA);

}

