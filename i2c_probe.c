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

/*! \file i2c_probe.c
*
*   \brief Functions to probe the I2C devices on the board
*
*/

#include "platform_internals.h"
#include "cslr_i2c.h"

// Register values for specific	purpose
#define	I2C_VAL_REG_MDR_RESET		 (0x4000)
#define	I2C_VAL_REG_MDR_SLVRCV		 (0x40A0)
#define	I2C_VAL_REG_MDR_MSTRCV		 (0x64A0)
#define	I2C_VAL_REG_MDR_MSTRCVSTOP	 (0x4CA0)
#define	I2C_VAL_REG_MDR_MSTXMT		 (0x46A0)
#define	I2C_VAL_REG_MDR_MSTXMTSTRT	 (0x66A0)
#define	I2C_VAL_REG_MDR_MSTXMTSTOP	 (0x4CA0)

#define	I2C_VAL_REG_STR_RESET		 (0x0410)
#define	I2C_VAL_REG_STR_ON_FAIL		 (0x1002)  // Clear bus busy, clear nack
#define	I2C_VAL_REG_STR_CLR_BUSY	 (0x1000)  // Clear busy

//I2C_DELAY
#define DELAY_CONST 10
#define CUSTOM_DELAY 10

//Return Value
#define	I2C_MAX_MASTER_RECEIVE_TIMEOUT	 (24)
#define	I2C_RET_NO_ACK_READ			 (8)
#define	I2C_RET_IDLE_TIMEOUT		 (3)


// Bit field definitions
#define	I2C_REG_STR_FIELD_NACK       (1<<1)

CSL_Status i2cProbe (Uint16 slaveAddress,Uint16 *pData, Uint16 numBytes);

/**
 * \brief Function to probe I2C devices
 *
 * \param    slaveAddress  - i2c slave device address
 * \param	 pData        - Pointer to the buffer base address
 * \param	 numBytes      - Number of bytes in 'pData'
 *
 * \return
 * \n      TEST_PASS  - Test Passed
 * \n      TEST_FAIL  - Test Failed
 *
 */
CSL_Status i2cProbe (Uint16 slaveAddress,Uint16 *pData, Uint16 numBytes)
{
	Uint32  statusReg;
	CSL_I2cRegsOvly i2cReg = CSL_I2C_0_REGS;
	CSL_Status   returnValue;
    Uint16 startStopFlag = ((CSL_I2C_START) | (CSL_I2C_STOP));

    statusReg = i2cReg->ICSTR;

	returnValue = I2C_write(pData, numBytes, slaveAddress,
	                TRUE, startStopFlag, CSL_I2C_MAX_TIMEOUT);
	if (returnValue != CSL_SOK)
	{
		C55x_msgWrite("i2cProbe: I2C Slave Address Write Failed\n\r");
		return (returnValue);
	}

	/* On Nack return failure */
	if ((statusReg)&(I2C_REG_STR_FIELD_NACK))
	{
		/* Return to slave receiver, clear nack and bus busy */
		i2cReg->ICMDR = I2C_VAL_REG_MDR_SLVRCV;
		i2cReg->ICSTR = I2C_VAL_REG_STR_ON_FAIL;

		return (I2C_RET_NO_ACK_READ);
	}
	return (CSL_SOK);
}
