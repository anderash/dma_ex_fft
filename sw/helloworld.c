/*
 * Copyright (c) 2009-2012 Xilinx, Inc.  All rights reserved.
 *
 * Xilinx, Inc.
 * XILINX IS PROVIDING THIS DESIGN, CODE, OR INFORMATION "AS IS" AS A
 * COURTESY TO YOU.  BY PROVIDING THIS DESIGN, CODE, OR INFORMATION AS
 * ONE POSSIBLE   IMPLEMENTATION OF THIS FEATURE, APPLICATION OR
 * STANDARD, XILINX IS MAKING NO REPRESENTATION THAT THIS IMPLEMENTATION
 * IS FREE FROM ANY CLAIMS OF INFRINGEMENT, AND YOU ARE RESPONSIBLE
 * FOR OBTAINING ANY RIGHTS YOU MAY REQUIRE FOR YOUR IMPLEMENTATION.
 * XILINX EXPRESSLY DISCLAIMS ANY WARRANTY WHATSOEVER WITH RESPECT TO
 * THE ADEQUACY OF THE IMPLEMENTATION, INCLUDING BUT NOT LIMITED TO
 * ANY WARRANTIES OR REPRESENTATIONS THAT THIS IMPLEMENTATION IS FREE
 * FROM CLAIMS OF INFRINGEMENT, IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */
 
//
// Notes:
//   - Reset for counter hardware (GPIO[0]) is active low
//   - Assumes S2MM and MM2S stream data widths are the same
//

// Includes
#include <stdio.h>
#include "platform.h"
#include "xuartps_hw.h"
#include "fft.h"
#include "cplx.h"
#include "stim.h"

// Defines

// External data
extern int stim_buf[FFT_MAX_NUM_PTS]; // FFT input data

// Function prototypes
void which_fft_param(fft_t* p_fft_inst);

// Main entry point
int main()
{
	// Local variables
	int         status = 0;
	char        c;
	cplx_data_t result_buf[FFT_MAX_NUM_PTS]; // FFT output data (could use dynamic memory here...)
	fft_t*      p_fft_inst;

	// Setup UART and enable caches
    init_platform();
    xil_printf("\fHello World!\n\r");

    // Create FFT object
    p_fft_inst = fft_create
    (
    	XPAR_GPIO_0_DEVICE_ID,
    	XPAR_AXIDMA_0_DEVICE_ID,
    	XPAR_PS7_SCUGIC_0_DEVICE_ID,
    	XPAR_FABRIC_AXI_DMA_0_S2MM_INTROUT_INTR,
    	XPAR_FABRIC_AXI_DMA_0_MM2S_INTROUT_INTR
    );
    if (p_fft_inst == NULL)
    {
    	xil_printf("ERROR! Failed to create FFT instance.\n\r");
    	return XST_FAILURE;
    }

    // Main control loop
    while (1)
    {

    	// Get command
    	xil_printf("What would you like to do?\n\r");
    	xil_printf("0: Print current FFT parameters\n\r");
    	xil_printf("1: Change FFT parameters\n\r");
    	xil_printf("2: Perform FFT using current parameters\n\r");
    	xil_printf("3: Print results of previous FFT operation\n\r");
    	xil_printf("4: Quit\n\r");
    	c = XUartPs_RecvByte(XPAR_PS7_UART_1_BASEADDR);

    	if (c == '0')
    	{
    		xil_printf("---------------------------------------------\n\r");
    		fft_print_params(p_fft_inst);
    		xil_printf("---------------------------------------------\n\r");
    	}
    	else if (c == '1')
    	{
    		which_fft_param(p_fft_inst);
    	}
    	else if (c == '2') // Run FFT
		{
			// Make sure the buffer is clear before we populate it
			memset(result_buf, 0, FFT_MAX_NUM_PTS*sizeof(cplx_data_t));

			status = fft(p_fft_inst, (cplx_data_t*)stim_buf, (cplx_data_t*)result_buf);
			if (status != XST_SUCCESS)
			{
				xil_printf("ERROR! FFT failed.\n\r");
				return XST_FAILURE;
			}
			xil_printf("FFT complete!\n\r");
		}
    	else if (c == '3')
    	{
    		fft_print_results(p_fft_inst, result_buf);
    	}
    	else if (c == '4') // Quit
    	{
    		xil_printf("Okay, exiting...\n\r");
    		break;
    	}
    	else
    	{
    		xil_printf("Invalid character. Please try again.\n\r");
    	}

    }

    fft_destroy(p_fft_inst);

    return XST_SUCCESS;

}



void which_fft_param(fft_t* p_fft_inst)
{
	// Local variables
	char c;

	xil_printf("Okay, which parameter would you like to change?\n\r");
	xil_printf("0: Point length\n\r");
	xil_printf("1: Direction\n\r");
	xil_printf("2: Exit\n\r");
	while (1)
	{
		c = XUartPs_RecvByte(XPAR_PS7_UART_1_BASEADDR);

		if (c == '0')
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
					fft_set_num_pts(p_fft_inst, 16);
					break;
				}
				else if (c == '1')
				{
					xil_printf("Okay, setting the core to perform a 32-point FFT.\n\r");
					fft_set_num_pts(p_fft_inst, 32);
					break;
				}
				else if (c == '2')
				{
					xil_printf("Okay, setting the core to perform a 64-point FFT.\n\r");
					fft_set_num_pts(p_fft_inst, 64);
					break;
				}
				else if (c == '3')
				{
					xil_printf("Okay, setting the core to perform a 128-point FFT.\n\r");
					fft_set_num_pts(p_fft_inst, 128);
					break;
				}
				else if (c == '4')
				{
					xil_printf("Okay, setting the core to perform a 256-point FFT.\n\r");
					fft_set_num_pts(p_fft_inst, 256);
					break;
				}
				else if (c == '5')
				{
					xil_printf("Okay, setting the core to perform a 512-point FFT.\n\r");
					fft_set_num_pts(p_fft_inst, 512);
					break;
				}
				else if (c == '6')
				{
					xil_printf("Okay, setting the core to perform a 1024-point FFT.\n\r");
					fft_set_num_pts(p_fft_inst, 1024);
					break;
				}
				else if (c == '7')
				{
					xil_printf("Okay, setting the core to perform a 2048-point FFT.\n\r");
					fft_set_num_pts(p_fft_inst, 2048);
					break;
				}
				else if (c == '8')
				{
					xil_printf("Okay, setting the core to perform a 4096-point FFT.\n\r");
					fft_set_num_pts(p_fft_inst, 4096);
					break;
				}
				else if (c == '9')
				{
					xil_printf("Okay, setting the core to perform a 8192-point FFT.\n\r");
					fft_set_num_pts(p_fft_inst, 8192);
					break;
				}
				else
		    		xil_printf("Invalid character. Please try again.\n\r");
			}
			break;
		}
		else if (c == '1')
		{
			xil_printf("What would you like to set the FFT direction to? Type '1' for forward. Type '0' for inverse.\n\r");

			c = XUartPs_RecvByte(XPAR_PS7_UART_1_BASEADDR);
			while (1)
			{
				if (c == '0')
				{
					xil_printf("Okay, setting the core to perform an inverse FFT.\n\r");
					fft_set_fwd_inv(p_fft_inst, FFT_INVERSE);
					break;
				}
				else if (c == '1')
				{
					xil_printf("Okay, setting the core to perform a forward FFT.\n\r");
					fft_set_fwd_inv(p_fft_inst, FFT_FORWARD);
					break;
				}
				else
		    		xil_printf("Invalid character. Please try again.\n\r");
			}
			break;
		}
		else if (c == '2')
		{
			return;
		}
		else
		{
			xil_printf("Invalid character. Please try again.\n\r");
		}
	}
}

