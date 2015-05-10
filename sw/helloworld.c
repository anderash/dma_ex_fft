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

#include <stdio.h>

#include "platform.h"
#include "xaxidma.h"
#include "xgpio.h"
#include "xscugic.h"
#include "xuartps_hw.h"

#include "dma.h"
#include "stim.h"

XAxiDma axi_dma;
XGpio   axi_gpio;
XScuGic axi_intc;

extern int stim_buf[MAX_FFT_LENGTH]; // FFT input data

int main()
{
	// ***** Local variables *****
	int status = 0;
	fft_config_t fft_params;
	int fft_buf[MAX_FFT_LENGTH]; // FFT output data
	char c;

	// Setup UART and enable caching
    init_platform();
	
    xil_printf("\fHello World!\n\r");

    // ***** Initialize drivers *****
	status = init_drivers();
	if (status != XST_SUCCESS)
	{
		xil_printf("Driver initialization failed!\n\r");
		return XST_FAILURE;
	}

	// ***** Initialize default FFT parameters *****
	fft_params.fft_len   = 1024;
	fft_params.fwd_inv   = FFT_FWD;
	fft_params.scale_sch = 0x2AB;

    while (1)
    {

    	// Get commnad
    	xil_printf("Please type a command. 'c' = configure the FFT core's parameters. 'r' = run the FFT. 'q' = quit. 'p' = print results.\n\r");
    	c = XUartPs_RecvByte(XPAR_PS7_UART_1_BASEADDR);

    	if (c == 'c') // Configure FFT parameters
    	{
    		status = which_fft_param(&fft_params);
    		if (status != XST_SUCCESS)
    		{
    			xil_printf("ERROR! Failed to connect mm2s_isr to the interrupt controller \r\n", status);
    			return status;
    		}
    		xil_printf("Done configuring the FFT core! Please use the 'r' command to run the FFT under the new configuration.\n\r");
    	}
    	else if (c == 'r') // Run FFT
    	{
			status = run_fft(&fft_params, stim_buf, fft_buf);
			if (status != XST_SUCCESS)
			{
				xil_printf("ERROR! FFT failed.\n\r");
				return XST_FAILURE;
			}
    	}
    	else if (c == 'q') // Quit
    	{
    		xil_printf("Okay, exiting...\n\r");
    		break;
    	}
    	else if (c == 'p')
    	{
			status = print_results(&fft_params, fft_buf);
			if (status != XST_SUCCESS)
			{
				xil_printf("ERROR! Failed to print previous results\n\r");
				return XST_FAILURE;
			}
    	}
    	else
    		xil_printf("Invalid character. Please try again.\n\r");

    }

    return 0;
}
