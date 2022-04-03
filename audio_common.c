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

/*! \file audio_common.c
*
*   \brief Functions for initializing i2c and i2s interfaces and read and
*          operations using these intefaces.
*
*/

#include "audio_common.h"

CSL_I2sHandle   hI2s = 0;
volatile Uint16  sw3Pressed = 0;

/**
 *  \brief  GPIO Interrupt Service Routine
 *
 *  \param testArgs   [IN]   Test arguments
 *
 *  \return none
 */
interrupt void gpioISR(void)
{
   	Int16 retVal;

	IRQ_globalDisable();
	/* Disable all the interrupts */
	IRQ_disableAll();

    /* Check for GPIO Interrupt Flag Register */
	if((1 == GPIO_statusBit(gpioHandle,CSL_GPIO_PIN13,&retVal)))
    {
        /* Clear GPIO Interrupt Flag Register */
        GPIO_clearInt(gpioHandle,CSL_GPIO_PIN13);
        sw3Pressed = 1;
    }

	IRQ_clear(GPIO_EVENT);
}

TEST_STATUS gpio_interrupt_initiliastion(void)
{

	Int16 retVal;

	CSL_GpioPinConfig    config;
	volatile Uint32 	 loop;
	sw3Pressed = 0;

	/* Set Bus for GPIOs */
	CSL_FINST(CSL_SYSCTRL_REGS->EBSR, SYS_EBSR_PPMODE, MODE1);

    /* Disable CPU interrupt */
    IRQ_globalDisable();

	/* Clear any pending interrupts */
	IRQ_clearAll();

	/* Disable all the interrupts */
	IRQ_disableAll();

    /* Initialize Interrupt Vector table */
    IRQ_setVecs((Uint32)(&VECSTART));

	/* Open GPIO module */
    gpioHandle = GPIO_open(&GpioObj,&retVal);
    if((NULL == gpioHandle) || (CSL_SOK != retVal))
    {
        C55x_msgWrite("GPIO_open failed\n");
        return (TEST_FAIL);
    }

	/* Reset the GPIO module */
    GPIO_reset(gpioHandle);

    for (loop=0; loop < 0x5F5E10; loop++){}

    C55x_msgWrite("Press SW3 on the BoosterPack for exiting from the test\n\n\r");

    /** test GPIO_config API to make PIN13 as i/p */
	config.pinNum    = CSL_GPIO_PIN13;
    config.direction = CSL_GPIO_DIR_INPUT;
    config.trigger   = CSL_GPIO_TRIG_RISING_EDGE;
    retVal = GPIO_configBit(gpioHandle,&config);
	if(CSL_SOK != retVal)
	{
		C55x_msgWrite("test failed - GPIO_configBit\n");
	     return (TEST_FAIL);
	}

	/* Enable GPIO interrupts */
    retVal = GPIO_enableInt(gpioHandle,CSL_GPIO_PIN13);
	if(CSL_SOK != retVal)
	{
		C55x_msgWrite("test failed- GPIO_enableInt\n");
		return(retVal);
	}

	/* Clear any pending Interrupt */
    IRQ_clear(GPIO_EVENT);

    IRQ_plug(GPIO_EVENT,&gpioISR);

     /* Enabling Interrupt */
    IRQ_enable(GPIO_EVENT);
    IRQ_globalEnable();

	return (TEST_PASS);

}

/**
 *
 * \brief This function used to Enable and initalize the I2S module
 *
 * \param void
 *
 * \return
 * \n      TEST_PASS  - Test Passed
 * \n      TEST_FAIL  - Test Failed
 *
 */
TEST_STATUS initialise_i2s_interface(void)
{
	I2S_Config      hwConfig;
	Int16           result = 0;

	/* Open the device with instance 0 */
	hI2s = I2S_open(I2S_INSTANCE2, I2S_POLLED, I2S_CHAN_STEREO);

	/* Set the value for the configure structure */
	hwConfig.dataType           = I2S_STEREO_ENABLE;
	hwConfig.loopBackMode       = I2S_LOOPBACK_DISABLE;
	hwConfig.fsPol              = I2S_FSPOL_LOW;
	hwConfig.clkPol             = I2S_RISING_EDGE;
	hwConfig.datadelay          = I2S_DATADELAY_ONEBIT;
	hwConfig.datapack           = I2S_DATAPACK_ENABLE;
	hwConfig.signext            = I2S_SIGNEXT_DISABLE;
	hwConfig.wordLen            = I2S_WORDLEN_32;
	hwConfig.i2sMode            = I2S_SLAVE;
	hwConfig.FError             = I2S_FSERROR_ENABLE;
	hwConfig.OuError            = I2S_OUERROR_ENABLE;

	/* Configure hardware registers */
	result += I2S_setup(hI2s, &hwConfig);
	result += I2S_transEnable(hI2s, TRUE);

	return result;

}

/**
 *
 * \brief This function Reads incoming I2S left channel word and writes it
 *      to the location of "data".
 *
 * \param   *data - Pointer to location if I2S data destination
 *
 * \return void
 *
 */
void I2S_readLeft(Int16* data)
{
    ioport  CSL_I2sRegs   *regs;

    regs = hI2s->hwRegs;
    while((0x08 & regs->I2SINTFL) == 0);  // Wait for receive interrupt to be pending
    *data = regs->I2SRXLT1 ;              // 16 bit left channel receive audio data
}

/**
 *
 * \brief This function used to Writes I2S left channel word
 *
 * \param  data -I2S left data
 *
 * \return void
 *
 */
void I2S_writeLeft(Int16 data)
{
    ioport  CSL_I2sRegs   *regs;

    regs = hI2s->hwRegs;
    while((CSL_I2S_I2SINTFL_XMITSTFL_MASK & regs->I2SINTFL) == 0);  // Wait for transmit interrupt to be pending
    regs->I2STXLT1 = (data) ;            // 16 bit left channel transmit audio data
}

/**
 *
 * \brief This function Reads incoming I2S right channel word and writes it
 *      to the location of "data".
 *
 * \param   *data - Pointer to location if I2S data destination
 *
 * \return void
 *
 */
void I2S_readRight(Int16* data)
{
    ioport  CSL_I2sRegs   *regs;

    regs = hI2s->hwRegs;
//  while((0x08 & regs->I2SINTFL) == 0);  // Wait for receive interrupt to be pending
    *data = regs->I2SRXRT1 ;              // 16 bit left channel receive audio data
}

/**
 *
 * \brief This function used to Writes I2S right channel word
 *
 * \param  data -I2S right data
 *
 * \return void
 *
 */
void I2S_writeRight(Int16 data)
{
    ioport  CSL_I2sRegs   *regs;

    regs = hI2s->hwRegs;
//  while((CSL_I2S_I2SINTFL_XMITSTFL_MASK & regs->I2SINTFL) == 0);  // Wait for transmit interrupt to be pending
    regs->I2STXRT1 = (data) ;              // 16 bit left channel transmit audio data
}

/**
 *
 * \brief This function used to Enable and initalize the I2C module
 *		   The I2C clk is set to run at 100 KHz
 *
 * \param    testArgs   [IN]   Test arguments
 *
 * \return
 * \n      TEST_PASS  - Test Passed
 * \n      TEST_FAIL  - Test Failed
 *
 */
TEST_STATUS initialise_i2c_interface(void *testArgs)
{
	CSL_Status         status;
	CSL_I2cConfig         i2cConfig;

	status = I2C_init(CSL_I2C0);

	/* Configure I2C module for write */
	i2cConfig.icoar  = CSL_I2C_ICOAR_DEFVAL;
	i2cConfig.icimr  = CSL_I2C_ICIMR_DEFVAL;
	i2cConfig.icclkl = 20;
	i2cConfig.icclkh = 20;
	i2cConfig.icsar  = CSL_I2C_ICSAR_DEFVAL;
	i2cConfig.icmdr  = CSL_I2C_ICMDR_WRITE_DEFVAL;
	i2cConfig.icemdr = CSL_I2C_ICEMDR_DEFVAL;
	i2cConfig.icpsc  = 20;

	status |= I2C_config(&i2cConfig);

	return 0;

}

/**
 *
 * \brief This function used to write into the audio codec registers
 *
 * \param testArgs  regnum - register number
 *                  regVal - register data
 *
 * \return
 * \n      TEST_PASS  - Test Passed
 * \n      TEST_FAIL  - Test Failed
 *
 */
TEST_STATUS AIC3206_write(Uint16 regnum, Uint16 regval)
{
	Int16 retVal;
	Uint16 startStop            = ((CSL_I2C_START) | (CSL_I2C_STOP));
	Uint16 cmd[2];
    cmd[0] = regnum & 0x007F;       // 7-bit Device Register
    cmd[1] = regval;                // 8-bit Register Data

    C55x_delay_msec(3);

    /* I2C Write */
    retVal = I2C_write(cmd, 2, AIC3206_I2C_ADDR,
    		 TRUE, startStop, CSL_I2C_MAX_TIMEOUT);
     if(retVal != 0)
     {
    	C55x_msgWrite("I2C Write failed\n\r");
 		//return -1;
     }

     return (0);
}
