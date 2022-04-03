/*
 * Copyright (c) 2016, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/**
 *  \file    system_init.c
 *
 *  \brief  This file contains the platform test initialization functions
 *
 *****************************************************************************/

#include "platform_test.h"
#include "platform_internals.h"

/**
 *  \brief    This function generates a PLL clock of 100MHZ
 *
 *  \return   void
 */
ProgramPLL_100MHz()
{
    int i;

    /*Enabling clodk*/
    CSL_CPU_REGS->ST3_55 &= ~0x4;

    //C55x_msgWrite("Configuring PLL (100 MHz).\n");
    /* Enable clocks to all peripherals */
    CSL_SYSCTRL_REGS->PCGCR1 = 0x0;
    CSL_SYSCTRL_REGS->PCGCR2 = 0x0;

    /* For 32KHz input clock */
    /* Bypass PLL */
    CSL_SYSCTRL_REGS->CCR2 = 0x0;
    /* Set CLR_CNTL = 0 */
    CSL_SYSCTRL_REGS->CGCR1 = ((CSL_SYSCTRL_REGS->CGCR1) & (0x7FFF));

    CSL_SYSCTRL_REGS->CGCR1 =  0x8BE8;
    CSL_SYSCTRL_REGS->CGCR2 =  0x8000;
    CSL_SYSCTRL_REGS->CGCR3 =  0x0806;
    CSL_SYSCTRL_REGS->CGCR4 =  0x0000;

	/* Wait for PLL lock */
    for(i=0;i<0x0fff;i++);

    /* Switch to PLL clk */
    CSL_SYSCTRL_REGS->CCR2 = 0x1;

    //C55x_msgWrite("PLL Init Done.\n");
}

/**
 *  \brief    Initializes user interface
 *
 *  User input for the diagnostic tests can be taken from
 *  serial console or CCS console. This function configures
 *  the user interface based on the selected input.
 *
 *  Below are settings needs to be used on host machine in case
 *  of serial console user interface
 *   Baud Rate    - 115200
 *   Data width   - 8 bit
 *   Parity       - None
 *   Stop Bits    - 1
 *   Flow Control - None
 *
 *  \return    - Platform_EOK on Success or error code
 */
static Platform_STATUS initUserIf(void)
{
	Platform_STATUS status = Platform_EOK;

	ProgramPLL_100MHz();

#ifdef USE_SERIAL_CONSOLE
	/* Set output console to serial port */
	C55x_msgWriteConfigure(PLATFORM_WRITE_UART);
	C55x_msgReadConfigure(PLATFORM_READ_UART);
	uart_initialisation();

#else
	/* Set output console to CCS */
	C55x_msgWriteConfigure(PLATFORM_WRITE_PRINTF);
	C55x_msgReadConfigure(PLATFORM_READ_SCANF);
#endif

	return (status);
}

/**
 *  \brief    Initializes platform test modules
 *
 *  \return   Platform_EOK on Success or error code
 */
Platform_STATUS initPlatform(void)
{
	Platform_STATUS status = 0;

	/* Initialize user interface module */
    status = initUserIf();
    if(status != Platform_EOK)
    {
    	C55x_msgWrite("User Interface Init Failed\n");
    	return (status);
    }

    return (status);
}
