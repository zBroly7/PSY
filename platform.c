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
  *
  * \file   platform.c
  *
  * \brief  C5545 BoosterPack platform function implementation
  *
 *****************************************************************************/

#include "platform_internals.h"
#include <time.h>

static WRITE_info	write_type;
READ_info	read_type = PLATFORM_READ_SCANF;

#define MAX_WRITE_LEN (200)

CSL_UartObj       uartObj;
static char	write_buffer[MAX_WRITE_LEN];

/**
 * \brief    This function used to generate time delay in milliseconds
 *
 * \param    numOfmsec - number of milliseconds
 *
 * \return   void
 */
void C55x_delay_msec(int numOfmsec)
{
	volatile Uint16 delay;

	while(numOfmsec)
	{
		for(delay = 0; delay < 6250; delay++ );
		numOfmsec--;
	}
}

/**
 *
 * \brief This function used to set the uart parameters
 *
 * \param    args   [IN]   UartSetup arguments
 *
 * \return
 * \n      TEST_PASS  - Test Passed
 * \n      TEST_FAIL  - Test Failed
 *
 */
Int32 platform_uart_set_params(CSL_UartSetup *args)
{
	args->clkInput = 60000000;
	args->baud     = 115200;
	args->wordLength=CSL_UART_WORD8;
	args->stopBits=0;
	args->parity=CSL_UART_DISABLE_PARITY;
	args->fifoControl=CSL_UART_FIFO_DMA1_ENABLE_TRIG14;
	args->loopBackEnable=CSL_UART_NO_LOOPBACK;
	args->afeEnable=CSL_UART_NO_AFE;
	args->rtsEnable=CSL_UART_NO_RTS;

	return Platform_EOK;

}

/**
 *
 * \brief This function Connfigures the input source for C55x_msgRead
 *
 * \param    rdype - read from uart console or ccs console
 *
 * \return
 * \n      0 - echo to UART
 * \n      1 - printf
 */
READ_info C55x_msgReadConfigure (READ_info	rdype)
{
	READ_info original;
	original  = read_type;
	read_type = rdype;

	return original;
}

/**
 *
 * \brief This function perform's scanf or read from UART
 *
 * \param    data     - character pointer
 * \param	 length	  - charcter length
 *
 * \return
 * \n      TEST_PASS  - Test Passed
 * \n      TEST_FAIL  - Test Failed
 *
 */
Int32 C55x_msgRead(Uint8 *data, Uint32 length)
{
	Uint32 index;

	*data = '\0';

    CSL_Status retVal = 0;

	if (read_type == PLATFORM_READ_UART)
	{
		CSL_UartHandle hUart;
		hUart = (CSL_UartHandle)(&uartObj);

		for (index = 0; index < length; index++)
		{
			retVal = UART_read(hUart, (char*)data, 1, 0);
			if(retVal)
			{
				break;
			}

			if((*data == 0xD) || (*data == 0x1B))
			{
				break;
			}

			UART_fputc(hUart, *data, 0);
			data++;
		}
	}

	if (read_type == PLATFORM_READ_SCANF)
	{
		scanf("%s", data);
	}

   	return retVal;

}

/**
 *
 * \brief This function Connfigures the input source for C55x_msgWrite
 *
 * \param    write_type - writes to uart console or ccs console
 *
 * \return
 * \n      0 - echo to UART
 * \n      1 - printf
 */
WRITE_info C55x_msgWriteConfigure (WRITE_info	wtype)
{
	WRITE_info original;
	original = write_type;
	write_type = wtype;

	return original;

}

/**
 *
 * \brief This function perform's Printf or echo to UART
 *
 * \param    fmt - character pointer
 *
 * \n      TEST_PASS  - Test Passed
 * \n      TEST_FAIL  - Test Failed
 */
Int32 C55x_msgWrite(const char *fmt, ...)
{
	va_list 	    arg_ptr;
	Uint32	        length;
    CSL_Status      retVal;
	CSL_UartHandle  hUart;

    /* Initial printf to temporary buffer.. at least try some sort of sanity check so we don't write all over
	 * memory if the print is too large.
	 */
	if (strlen(fmt) > MAX_WRITE_LEN)
	{
		printf("Exceed's the max write length\n");
		return (-1);
	}

	va_start( arg_ptr, fmt );
	length = vsprintf( (char *)write_buffer, fmt, arg_ptr );
	va_end( arg_ptr );
	length = length;

	if ((write_type == PLATFORM_WRITE_PRINTF) || (write_type == PLATFORM_WRITE_ALL))
	{
		/*Used to print on the CCS console*/
		printf("%s", write_buffer);
		fflush(stdout);
	}

	/* Handle created */
	hUart = (CSL_UartHandle)(&uartObj);

	if ((write_type == PLATFORM_WRITE_UART) || (write_type == PLATFORM_WRITE_ALL))
	{
		/*Used to print on the serial console*/
		retVal = UART_fputs(hUart,write_buffer,0);
		if(CSL_SOK != retVal)
		{
			printf("UART_fputs failed error code %d\n",retVal);
			return retVal;
		}
	}

	return retVal;

}

/**
 *  \brief  Function to calculate the clock at which system is running
 *
 *  \param    void
 *
 *  \return   System clock value in Hz
 */
#if (defined(CHIP_C5505_C5515) || defined(CHIP_C5504_C5514) || defined(CHIP_C5535) || defined(CHIP_C5545))

Uint32 C55x_getSysClk(void)
{
	Bool      pllRDBypass;
	Bool      pllOutDiv;
	Uint32    sysClk;
	Uint16    pllM;
	Uint16    pllRD;
	Uint16    pllOD;

	pllM = CSL_FEXT(CSL_SYSCTRL_REGS->CGCR1, SYS_CGCR1_M);

	pllRD = CSL_FEXT(CSL_SYSCTRL_REGS->CGCR2, SYS_CGCR2_RDRATIO);
	pllOD = CSL_FEXT(CSL_SYSCTRL_REGS->CGCR4, SYS_CGCR4_ODRATIO);

	pllRDBypass = CSL_FEXT(CSL_SYSCTRL_REGS->CGCR2, SYS_CGCR2_RDBYPASS);
	pllOutDiv   = CSL_FEXT(CSL_SYSCTRL_REGS->CGCR4, SYS_CGCR4_OUTDIVEN);

	sysClk = CSL_PLL_CLOCKIN;

	if (0 == pllRDBypass)
	{
		sysClk = sysClk/(pllRD + 4);
	}

	sysClk = (sysClk * (pllM + 4));

	if (1 == pllOutDiv)
	{
		sysClk = sysClk/(pllOD + 1);
	}

	/* Return the value of system clock in KHz */
	return(sysClk/1000);
}

#elif (defined(CHIP_C5517))

Uint32 C55x_getSysClk(void)
{
	Uint32    sysClk;
	float    Multiplier;
	Uint16    OD;
	Uint16    OD2;
	Uint16    RD, RefClk;
	Uint32	  temp1, temp2, temp3, vco;

	temp2 =  PLL_CNTL2;
	temp3 =  (temp2 & 0x8000) <<1 ;
	temp1 = temp3 + PLL_CNTL1;
	Multiplier = temp1/256.0 +1;
	RD = (PLL_CNTL2 & 0x003F) ;

	RefClk = 12000/(RD+1);

	vco = Multiplier * (Uint32)RefClk;

	OD = (PLL_CNTL4 & 0x7);

	sysClk = vco/(OD+1);

	OD2 = ((PLL_CNTL4 >> 10) & 0x1F) ;

	if (PLL_CNTL3 & 0x8000)	// PLL Bypass
		sysClk = RefClk;
	else
		sysClk = vco/(OD+1);

	if ((PLL_CNTL4 & 0x0020) == 0)	/* OutDiv2 */
		sysClk = sysClk / ( 2*(OD2+1));

	/* Return the value of system clock in Hz */
	return(sysClk);
}

#else

Uint32 C55x_getSysClk(void)
{
	Bool      pllRDBypass;
	Bool      pllOutDiv;
	Bool      pllOutDiv2;
	Uint32    sysClk;
	Uint16    pllVP;
	Uint16    pllVS;
	Uint16    pllRD;
	Uint16    pllVO;
	Uint16    pllDivider;
	Uint32    pllMultiplier;

	pllVP = CSL_FEXT(CSL_SYSCTRL_REGS->CGCR1, SYS_CGCR1_MH);
	pllVS = CSL_FEXT(CSL_SYSCTRL_REGS->CGCR2, SYS_CGCR2_ML);

	pllRD = CSL_FEXT(CSL_SYSCTRL_REGS->CGCR2, SYS_CGCR2_RDRATIO);
	pllVO = CSL_FEXT(CSL_SYSCTRL_REGS->CGCR4, SYS_CGCR4_ODRATIO);

	pllRDBypass = CSL_FEXT(CSL_SYSCTRL_REGS->CGCR2, SYS_CGCR2_RDBYPASS);
	pllOutDiv   = CSL_FEXT(CSL_SYSCTRL_REGS->CGCR4, SYS_CGCR4_OUTDIVEN);
	pllOutDiv2  = CSL_FEXT(CSL_SYSCTRL_REGS->CGCR4, SYS_CGCR4_OUTDIV2BYPASS);

	pllDivider = ((pllOutDiv2) | (pllOutDiv << 1) | (pllRDBypass << 2));

	pllMultiplier = ((Uint32)CSL_PLL_CLOCKIN * ((pllVP << 2) + pllVS + 4));

	switch(pllDivider)
	{
		case CSL_PLL_DIV_000:
		case CSL_PLL_DIV_001:
			sysClk = pllMultiplier / (pllRD + 4);
		break;

		case CSL_PLL_DIV_002:
			sysClk = pllMultiplier / ((pllRD + 4) * (pllVO + 4) * 2);
		break;

		case CSL_PLL_DIV_003:
			sysClk = pllMultiplier / ((pllRD + 4) * 2);
		break;

		case CSL_PLL_DIV_004:
		case CSL_PLL_DIV_005:
			sysClk = pllMultiplier;
		break;

		case CSL_PLL_DIV_006:
			sysClk = pllMultiplier / ((pllVO + 4) * 2);
		break;

		case CSL_PLL_DIV_007:
			sysClk = pllMultiplier / 2;
		break;
	}

	/* Return the value of system clock in KHz */
	return(sysClk/1000);
}
#endif

/**
 *
 * \brief This function initialises the UART to transfer data to serial console
 *
 * \param    void
 *
 * \return
 * \n      TEST_PASS  - Test Passed
 * \n      TEST_FAIL  - Test Failed
 *
 */
Platform_STATUS uart_initialisation(void)
{
	Platform_STATUS   retVal;
	Uint32            sysClk;
	CSL_UartHandle    hUart;

	CSL_UartSetup uartSetup;

	platform_uart_set_params(&uartSetup);

	sysClk = C55x_getSysClk();
	uartSetup.clkInput = sysClk * 1000;

	retVal = SYS_setEBSR(CSL_EBSR_FIELD_PPMODE,
			CSL_EBSR_PPMODE_1);
	if(CSL_SOK != retVal)
	{
		printf("SYS_setEBSR failed\n");
		return(retVal);
	}

	/* Handle created */
	hUart = (CSL_UartHandle)(&uartObj);

	/* Loop counter and error flag */
	retVal = UART_init(&uartObj,CSL_UART_INST_0,UART_POLLED);
	if(CSL_SOK != retVal)
	{
		printf("UART_init failed error code %d\n",retVal);
		return(retVal);
	}
	else
	{
		printf("UART_init Successful\n");
	}

	retVal = SYS_setEBSR(CSL_EBSR_FIELD_PPMODE,
			CSL_EBSR_PPMODE_1);
	if(CSL_SOK != retVal)
	{
		printf("SYS_setEBSR failed\n");
		return (retVal);
	}

	/* Handle created */
	hUart = (CSL_UartHandle)(&uartObj);

	/* Configure UART registers using setup structure */
	retVal = UART_setup(hUart,&uartSetup);
	if(CSL_SOK != retVal)
	{
		printf("UART_setup failed error code %d\n",retVal);
		return(retVal);
	}
	else
	{
		printf("UART_setup Successful\n");
	}

	return(retVal);

}

