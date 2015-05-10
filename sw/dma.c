/*
 * dma.c
 *
 *  Created on: Jul 31, 2013
 *      Author: bwiec
 */

// ***** Includes *****
#include "xaxidma.h"
#include "dma.h"
#include "xgpio.h"
#include "xscugic.h"
#include "xuartps_hw.h"

#include "isr.h"

// ***** External variables *****
extern XAxiDma axi_dma;
extern XGpio   axi_gpio;
extern XScuGic axi_intc;

// ***** Global variables *****
int s2mm_done; // Flags which get set by ISR
int mm2s_done; // if in interrupt mode
int dma_err;

int init_drivers()
{

	// ***** Local variables *****
	int status = 0;
	XAxiDma_Config *dma_cfg;
	XScuGic_Config *gic_cfg;

	xil_printf("Initializing drivers...\n\r");

	// ***** Initialize DMA device *****
	status = dma_init(dma_cfg);
	if (status != XST_SUCCESS)
	{
		xil_printf("ERROR! DMA initialization failed!\n\r");
		return XST_FAILURE;
	}

	// ***** Initialize Interrupts *****
	status = intr_init(gic_cfg);
	if (status != XST_SUCCESS)
	{
		xil_printf("ERROR! Interrupt controller initialization failed!\n\r");
		return XST_FAILURE;
	}

	xil_printf("Driver initialization complete!\n\r");

	return XST_SUCCESS;
}

int dma_init(XAxiDma_Config* cfg_ptr)
{
	// ***** Local variables *****
	int status = 0;

	xil_printf("Initializing DMA...\n\r");

	// Look up hardware configuration for device
	cfg_ptr = XAxiDma_LookupConfig(XPAR_AXIDMA_0_DEVICE_ID);
	if (!cfg_ptr)
	{
		xil_printf("No config found for %d\r\n", XPAR_AXIDMA_0_DEVICE_ID);
		return XST_FAILURE;
	}

	// Initialize driver
	status = XAxiDma_CfgInitialize(&axi_dma, cfg_ptr);
	if (status != XST_SUCCESS)
	{
		xil_printf("Initialization failed %d\r\n", status);
		return XST_FAILURE;
	}

	// Test for Scatter Gather
	if (XAxiDma_HasSg(&axi_dma))
	{
		xil_printf("Device configured as SG mode \r\n");
		return XST_FAILURE;
	}

	// Disable interrupts for both channels
	XAxiDma_IntrDisable(&axi_dma, XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DEVICE_TO_DMA);
	XAxiDma_IntrDisable(&axi_dma, XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DMA_TO_DEVICE);

	xil_printf("DMA initialization complete!\n\r");

	return XST_SUCCESS;
}

int intr_init(XScuGic_Config *cfg_ptr)
{

	// ***** Local variables *****
	int status = 0;

	xil_printf("Initializing interrupt controller...\n\r");

	// Look up hardware configuration for device
	cfg_ptr = XScuGic_LookupConfig(XPAR_PS7_SCUGIC_0_DEVICE_ID);
	if (!cfg_ptr)
	{
		xil_printf("No config found for %d\r\n", XPAR_PS7_SCUGIC_0_DEVICE_ID);
		return XST_FAILURE;
	}

	// Initialize driver
	status = XScuGic_CfgInitialize(&axi_intc, cfg_ptr, cfg_ptr->CpuBaseAddress);
	if (status != XST_SUCCESS)
	{
		xil_printf("Initialization failed %d\r\n", status);
		return XST_FAILURE;
	}

	// Set interrupt priorities and trigger type
	XScuGic_SetPriorityTriggerType(&axi_intc, XPAR_FABRIC_AXIDMA_0_S2MM_INTROUT_VEC_ID, 0xA0, 0x3);
	XScuGic_SetPriorityTriggerType(&axi_intc, XPAR_FABRIC_AXIDMA_0_MM2S_INTROUT_VEC_ID, 0xA8, 0x3);

	// Connect handlers
	status = XScuGic_Connect(&axi_intc, XPAR_FABRIC_AXIDMA_0_S2MM_INTROUT_VEC_ID, (Xil_InterruptHandler)s2mm_isr, &axi_dma);
	if (status != XST_SUCCESS)
	{
		xil_printf("ERROR! Failed to connect s2mm_isr to the interrupt controller \r\n", status);
		return status;
	}
	status = XScuGic_Connect(&axi_intc, XPAR_FABRIC_AXIDMA_0_MM2S_INTROUT_VEC_ID, (Xil_InterruptHandler)mm2s_isr, &axi_dma);
	if (status != XST_SUCCESS)
	{
		xil_printf("ERROR! Failed to connect mm2s_isr to the interrupt controller \r\n", status);
		return status;
	}

	// Enable interrupts
	XScuGic_Enable(&axi_intc, XPAR_FABRIC_AXIDMA_0_S2MM_INTROUT_VEC_ID);
	XScuGic_Enable(&axi_intc, XPAR_FABRIC_AXIDMA_0_MM2S_INTROUT_VEC_ID);

	// Enable DMA interrupts
	XAxiDma_IntrEnable(&axi_dma, XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DMA_TO_DEVICE);
	XAxiDma_IntrEnable(&axi_dma, XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DEVICE_TO_DMA);

	// Initialize exception table and register the interrupt controller handler with exception table
	Xil_ExceptionInit();
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT, (Xil_ExceptionHandler)XScuGic_InterruptHandler, &axi_intc);

	// Enable non-critical exceptions
	Xil_ExceptionEnable();

	xil_printf("Interrupt controller initialization complete!\n\r");

	return XST_SUCCESS;
}

int gpio_init()
{
	// ***** Local variables *****
	int status = 0;

	xil_printf("Initializing GPIO...\n\r");

	status = XGpio_Initialize(&axi_gpio, XPAR_AXI_GPIO_0_DEVICE_ID);
	if (status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}

	xil_printf("GPIO initialization complete!\n\r");

	return XST_SUCCESS;
}

int which_fft_param(fft_config_t* fft_params)
{
	// ***** Local variables *****
	int c;

	xil_printf("Okay, which parameter would you like to update? Type 'l' for FFT point length. Type 'd' for FFT direction. Type 'c' to cancel.\n\r");
	while (1)
	{
		c = XUartPs_RecvByte(XPAR_PS7_UART_1_BASEADDR);

		if (c == 'l')
		{
			xil_printf("What would you like to set the FFT point length to? Type:\n\r");
			xil_printf("0: FFT point length = 16\n\r");
			xil_printf("1: FFT point length = 32\n\r");
			xil_printf("2: FFT point length = 64\n\r");
			xil_printf("3: FFT point length = 128\n\r");
			xil_printf("4: FFT point length = 256\n\r");
			xil_printf("5: FFT point length = 512\n\r");
			xil_printf("6: FFT point length = 1024\n\r");
			xil_printf("7: FFT point length = 2048\n\r");
			xil_printf("8: FFT point length = 4096\n\r");
			xil_printf("9: FFT point length = 8192\n\r");

			c = XUartPs_RecvByte(XPAR_PS7_UART_1_BASEADDR);
			while (1)
			{
				if (c == '0')
				{
					xil_printf("Okay, setting the core to perform a 16-point FFT.\n\r");
					fft_params->fft_len = 16;
					break;
				}
				else if (c == '1')
				{
					xil_printf("Okay, setting the core to perform a 32-point FFT.\n\r");
					fft_params->fft_len = 32;
					break;
				}
				else if (c == '2')
				{
					xil_printf("Okay, setting the core to perform a 64-point FFT.\n\r");
					fft_params->fft_len = 64;
					break;
				}
				else if (c == '3')
				{
					xil_printf("Okay, setting the core to perform a 128-point FFT.\n\r");
					fft_params->fft_len = 128;
					break;
				}
				else if (c == '4')
				{
					xil_printf("Okay, setting the core to perform a 256-point FFT.\n\r");
					fft_params->fft_len = 256;
					break;
				}
				else if (c == '5')
				{
					xil_printf("Okay, setting the core to perform a 512-point FFT.\n\r");
					fft_params->fft_len = 512;
					break;
				}
				else if (c == '6')
				{
					xil_printf("Okay, setting the core to perform a 1024-point FFT.\n\r");
					fft_params->fft_len = 1024;
					break;
				}
				else if (c == '7')
				{
					xil_printf("Okay, setting the core to perform a 2048-point FFT.\n\r");
					fft_params->fft_len = 2048;
					break;
				}
				else if (c == '8')
				{
					xil_printf("Okay, setting the core to perform a 4096-point FFT.\n\r");
					fft_params->fft_len = 4096;
					break;
				}
				else if (c == '9')
				{
					xil_printf("Okay, setting the core to perform a 8192-point FFT.\n\r");
					fft_params->fft_len = 8192;
					break;
				}
				else
		    		xil_printf("Invalid character. Please try again.\n\r");
			}
			break;
		}
		else if (c == 'd')
		{
			xil_printf("What would you like to set the FFT direction to? Type '1' for forward. Type '0' for inverse.\n\r");

			c = XUartPs_RecvByte(XPAR_PS7_UART_1_BASEADDR);
			while (1)
			{
				if (c == '0')
				{
					fft_params->fwd_inv = FFT_INV;
					xil_printf("Okay, setting the core to perform an inverse FFT.\n\r");
					break;
				}
				else if (c == '1')
				{
					fft_params->fwd_inv = FFT_FWD;
					xil_printf("Okay, setting the core to perform a forward FFT.\n\r");
					break;
				}
				else
		    		xil_printf("Invalid character. Please try again.\n\r");
			}
			break;
		}
		else if (c == 'c')
			break;
		else
			xil_printf("Invalid character. Please try again.\n\r");
	}

	return XST_SUCCESS;
}

int run_fft(fft_config_t* fft_params, int* stim_buf, int* result_buf)
{
	// ***** Local variables *****
	int status = 0;

	xil_printf("Performing FFT...\n\r");

	// Write config channel of FFT
	status = fft_config(fft_params); // Due to hardware design, this will only actually update FFT core when a different value is written
	if (status != XST_SUCCESS)
	{
		xil_printf("ERROR! Failed to configure the FFT core.\r\n", status);
		return status;
	}

	// Kick off DMA transfers
	status = dma_xfer(fft_params, result_buf, stim_buf);
	if (status != XST_SUCCESS)
	{
		xil_printf("ERROR! Failed to kick off dma transfer.\r\n", status);
		return status;
	}

	xil_printf("FFT completed successfully!\n\r");

	return XST_SUCCESS;
}

int fft_config(fft_config_t* fft_params)
{
	// ***** Local variables *****
	int reg;
	int log2_N;
	int tmp;

	xil_printf("Configuring the FFT core...\n\r");

	// Calculate log2 of the FFT length for use in the FFT core config channel
	tmp = fft_params->fft_len-1;
	for (log2_N = 0;  tmp > 0; log2_N++)
		tmp >>= 1;

	// Set up register bits
	reg  = (fft_params->scale_sch << 9) & FFT_SCALE_SCH_MASK;
	reg |= (fft_params->fwd_inv   << 8) & FFT_FWD_INV_MASK;
	reg |= (log2_N    << 0)             & FFT_LENGTH_MASK;

	// Write config data to GPIO (which drives the FFT config interface)
	XGpio_WriteReg(XPAR_AXI_GPIO_0_BASEADDR, XGPIO_DATA_OFFSET, reg);

	xil_printf("FFT core configuration complete!\n\r");

	return XST_SUCCESS;
}

int dma_xfer(fft_config_t* fft_params, int* s2mm_buf, int* mm2s_buf)
{
	// ***** Local variables *****
	int status = 0;

	xil_printf("Transferring data...\n\r");

	// Flush cache
	Xil_DCacheFlushRange((u32)s2mm_buf, fft_params->fft_len*4);
	Xil_DCacheFlushRange((u32)mm2s_buf, fft_params->fft_len*4);

	s2mm_done = 0; // Initialize flags which get set by ISR
	mm2s_done = 0;
	dma_err   = 0;

	// Kick off S2MM transfer
	status = XAxiDma_SimpleTransfer(&axi_dma, (u32)s2mm_buf, fft_params->fft_len*4, XAXIDMA_DEVICE_TO_DMA);
	if (status != XST_SUCCESS)
	{
		xil_printf("ERROR! Failed to kick off S2MM transfer!\n\r");
		return XST_FAILURE;
	}

	// Kick off MM2S transfer
	status = XAxiDma_SimpleTransfer(&axi_dma, (u32)mm2s_buf, fft_params->fft_len*4, XAXIDMA_DMA_TO_DEVICE);
	if (status != XST_SUCCESS)
	{
		xil_printf("ERROR! Failed to kick off MM2S transfer!\n\r");
		return XST_FAILURE;
	}

	// Wait for transfers to complete
	while (!(s2mm_done && mm2s_done) && !dma_err)
	{
			// Do nothing
	}

	// Check if DMA controller threw an error
	if (dma_err)
	{
		xil_printf("ERROR! DMA device threw an error!");
		return XST_FAILURE;
	}

	xil_printf("DMA transfer complete!\n\r");

	return XST_SUCCESS;

}

int print_results(fft_config_t* fft_params, int* result_buf)
{
	// ***** Local variables *****
	int ii;
	signed int xk_re, xk_im;

	xil_printf("Printing results...\n\r");
	for (ii = 0; ii < fft_params->fft_len; ii++)
	{
		// Grab output values
		xk_re = result_buf[ii] & XK_RE_MASK;         // Lower bits
		xk_im = (result_buf[ii] & XK_IM_MASK) >> 16; // Upper bits

		// Sign extend output values
		if (xk_re & SIGN_TEST_MASK)
			xk_re += SIGN_EXTEND_MASK;
		if (xk_im & SIGN_TEST_MASK)
			xk_im += SIGN_EXTEND_MASK;

		xil_printf("Xk(%d) = %d + j*%d\n\r", ii, xk_re, xk_im);
	}

	xil_printf("Done printing results!\n\r");

	return XST_SUCCESS;
}


