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

 /*! \file current_monitor_test.c
*
*   \brief Functions that configure the current regsisters and read the
*    current and power for all the four slave devices.
*
*/

#include "current_monitor_test.h"

CSL_I2cSetup     i2cSetup;
CSL_I2cConfig    i2cConfig;

/**
 *  \brief    This function is used to writting to INA219 device
 *
 *  \param    slaveAddr - Address of INA salve device
 *  		  regAddr   - Address of the INA slave register
 *  		  writeBuff - Data to be written to the INA slave register
 *
 * \return
 * \n         TEST_PASS  - Test Passed
 * \n         TEST_FAIL  - Test Failed
 */
static TEST_STATUS write_to_ina_device_reg(Uint16 slaveAddress, Uint16 regAddr, Uint16 *writeBuff)
{
	Int16   retVal;
	Uint16  startStop;

	Uint16 wrBuf[3];
	wrBuf[0] = regAddr & 0x00ff;
	wrBuf[1] = writeBuff[0] & 0xff00;
	wrBuf[1] = wrBuf[1] >> 8;
	wrBuf[2] = writeBuff[0] & 0x00ff;

	startStop  = ((CSL_I2C_START) | (CSL_I2C_STOP));

	retVal = I2C_write(wrBuf, 3, slaveAddress,
	                       TRUE, startStop, CSL_I2C_MAX_TIMEOUT);
	if(retVal != CSL_SOK)
	{
		C55x_msgWrite("I2C write failed\n\r");
		return (TEST_FAIL);
	}

	return (TEST_PASS);

}

/**
 * \brief    This function is used to configure INA219 device and read
 *  		  current and power values
 *
 * \param    slaveAddress - Address of the Slave
 * 			 regAddr      - Register Address
 *           readBuff     - Read Buffer
 *           dataLength   - Number of bytes to read
 * \return
 * \n         TEST_PASS  - Test Passed
 * \n         TEST_FAIL  - Test Failed
 */
static TEST_STATUS read_from_ina_device_reg(Uint16 slaveAddress, Uint16 regAddr,
		                                   Uint16 *readBuff, Uint16 dataLength)
{
	Int16 retVal;
	Uint16  startStop;
	Uint16  rdBuf[2];

	startStop  = ((CSL_I2C_START) | (CSL_I2C_STOP));

	retVal = I2C_read(rdBuf, dataLength, slaveAddress, &regAddr, 2, TRUE, startStop,
			          CSL_I2C_MAX_TIMEOUT, FALSE);
	if(retVal != CSL_SOK)
	{
		C55x_msgWrite("I2C read failed\n");
		return (TEST_FAIL);
	}

	readBuff[0] = ((rdBuf[0] << 8) | (0x00ff & rdBuf[1]));

	return (TEST_PASS);

}

/**
 * \brief    This function is used to configure INA219 device and read
 *  		  current and power values
 *
 * \param    testArgs   [IN]   Test arguments
 *
 * \return
 * \n         TEST_PASS  - Test Passed
 * \n         TEST_FAIL  - Test Failed
 */
static TEST_STATUS run_current_monitor_test(void *testArgs)
{
	Int16  retVal;

	float readRegData;
    float current_lsb[4] = {0.00147796630, 0.00167398071, 0.000417755126, 0.00054138183};

	Uint16 CVDD_SlaveAddr = 0x40;
	Uint16 LDOI_SlaveAddr = 0x41;
	Uint16 DSP_DVDDIO_SlaveAddr = 0x44;
	Uint16 VCC3V3_USB_SlaveAddr = 0x48;

	Uint16  writeBuff;
	Uint16  regAddr;
	Uint16  regRdBuf;
	Uint16  looper;

	/* Initialize I2C module */
	retVal = I2C_init(CSL_I2C0);
	if(retVal != CSL_SOK)
	{
		C55x_msgWrite("I2C Init Failed!!\n\r");
		return (TEST_FAIL);
	}

	/* Setup I2C module */
	i2cSetup.addrMode    = CSL_I2C_ADDR_7BIT;
	i2cSetup.bitCount    = CSL_I2C_BC_8BITS;
	i2cSetup.loopBack    = CSL_I2C_LOOPBACK_DISABLE;
	i2cSetup.freeMode    = CSL_I2C_FREEMODE_DISABLE;
	i2cSetup.repeatMode  = CSL_I2C_REPEATMODE_DISABLE;
	i2cSetup.ownAddr     = CSL_I2C_OWN_ADDR;
	i2cSetup.sysInputClk = CSL_I2C_SYS_CLK;
	i2cSetup.i2cBusFreq  = CSL_I2C_BUS_FREQ;

	retVal = I2C_setup(&i2cSetup);
	if(retVal != CSL_SOK)
	{
		C55x_msgWrite("I2C Setup Failed!!\n\r");
		return (TEST_FAIL);
	}

	regAddr   = 0x0000;/*Configuration register address*/
	writeBuff = 0x3f9f;/*Configuration register data*/
	retVal = write_to_ina_device_reg(CVDD_SlaveAddr, regAddr, &writeBuff);
	if(retVal != CSL_SOK)
	{
		C55x_msgWrite("Writing to the configuration register of the slave "
				      "address 0x%x is failed\n\r", CVDD_SlaveAddr);
		return (TEST_FAIL);
	}

	/* Give some delay */
	for(looper = 0; looper < CSL_I2C_MAX_TIMEOUT; looper++)
	{
		asm("	nop");
	}

	regAddr   = 0x0005;/*Calibration register address*/
	writeBuff = 0x6C41;/*Calibration register data*/
	retVal = write_to_ina_device_reg(CVDD_SlaveAddr, regAddr, &writeBuff);
	if(retVal != CSL_SOK)
	{
		C55x_msgWrite("Writing to the calibration register of the slave "
				      "address 0x%x is failed\n\r", CVDD_SlaveAddr);
		return (TEST_FAIL);
	}

	/* Give some delay */
	for(looper = 0; looper < CSL_I2C_MAX_TIMEOUT; looper++)
	{
		asm("	nop");
	}

	C55x_msgWrite("Reading values from CVDD port\n\r");

	regAddr = 0x0001;/*Shunt voltage reg address*/
	retVal = read_from_ina_device_reg(CVDD_SlaveAddr, regAddr, &regRdBuf, 2);

	if(retVal != 0)
	{
		C55x_msgWrite("Reading from the shunt voltage register of the slave "
				      "address - 0x%x is failed\n\r", CVDD_SlaveAddr);
		return (TEST_FAIL);
	}
	else
	{
		readRegData = regRdBuf & 0x7fff;
		readRegData = readRegData * 0.01;
		C55x_msgWrite("Shunt voltage - %fmV\n\r", readRegData);
	}

	/* Give some delay */
	for(looper = 0; looper < CSL_I2C_MAX_TIMEOUT; looper++)
	{
		asm("	nop");
	}

	regAddr = 0x0002;/*Bus voltage reg address*/
	retVal = read_from_ina_device_reg(CVDD_SlaveAddr, regAddr, &regRdBuf, 2);
	if(retVal != 0)
	{
		C55x_msgWrite("Reading from the bus voltage register of the slave"
				     " address - 0x%x is failed\n\r", CVDD_SlaveAddr);
		return (TEST_FAIL);
	}
	else
	{
		readRegData =  regRdBuf >> 3;
		readRegData = readRegData * 0.004;
		C55x_msgWrite("Bus voltage - %fV\n\r", readRegData);
	}

	/* Give some delay */
	for(looper = 0; looper < CSL_I2C_MAX_TIMEOUT; looper++)
	{
		asm("	nop");
	}

	regAddr = 0x0003;/*Power Reg addr*/
	retVal = read_from_ina_device_reg(CVDD_SlaveAddr, regAddr, &regRdBuf, 2);
	if(retVal != CSL_SOK)
	{
		C55x_msgWrite("Reading power from the slave address 0x%x is "
				      "failed\n\r", CVDD_SlaveAddr);
		return (TEST_FAIL);
	}
	else
	{
		readRegData = regRdBuf;
		readRegData = current_lsb[0] * 20 * readRegData;
		C55x_msgWrite("Power - %fmW\n\r", readRegData);
	}

	/* Give some delay */
	for(looper = 0; looper < CSL_I2C_MAX_TIMEOUT; looper++)
	{
		asm("	nop");
	}

	regAddr = 0x0004;/*Current Reg addr*/
	retVal = read_from_ina_device_reg(CVDD_SlaveAddr, regAddr, &regRdBuf, 2);;
	if(retVal != CSL_SOK)
	{
		C55x_msgWrite("Reading current from the slave address 0x%x is "
				      "failed\n\r", CVDD_SlaveAddr);
		return (TEST_FAIL);
	}
	else
	{
		regRdBuf = regRdBuf & 0x7fff;
		readRegData = regRdBuf;
		readRegData = current_lsb[0] * readRegData;
		C55x_msgWrite("Current - %fmA\n\n\r", readRegData);
	}

	/* Give some delay */
	for(looper = 0; looper < CSL_I2C_MAX_TIMEOUT; looper++)
	{
		asm("	nop");
	}

	regAddr   = 0x0000;/*Configuration register address*/
	writeBuff = 0x3F9F;/*Configuration register data*/
	retVal = write_to_ina_device_reg(LDOI_SlaveAddr, regAddr, &writeBuff);
	if(retVal != CSL_SOK)
	{
		C55x_msgWrite("Writing to the configuration register of the  slave address 0x%x is "
				      "failed\n\r", LDOI_SlaveAddr);
		return (TEST_FAIL);
	}

	/* Give some delay */
	for(looper = 0; looper < CSL_I2C_MAX_TIMEOUT; looper++)
	{
		asm("	nop");
	}

	regAddr   = 0x0005;/*Calibration register address*/
	writeBuff = 0x596C;/*Calibration register data*/

	retVal = write_to_ina_device_reg(LDOI_SlaveAddr, regAddr, &writeBuff);
	if(retVal != CSL_SOK)
	{
		C55x_msgWrite("Writing to the calibration register of the  slave address 0x%x is "
				      "failed\n\r", LDOI_SlaveAddr);
		return (TEST_FAIL);
	}

	/* Give some delay */
	for(looper = 0; looper < CSL_I2C_MAX_TIMEOUT; looper++)
	{
		asm("	nop");
	}

	C55x_msgWrite("Reading values from LDOI port\n\r");

	regAddr = 0x0001;/*Shunt voltage reg address*/
	retVal = read_from_ina_device_reg(LDOI_SlaveAddr, regAddr, &regRdBuf, 2);
	if(retVal != 0)
	{
		C55x_msgWrite("Reading from the shunt voltage register of the slave address - 0x%x is "
				      "failed\n\r", LDOI_SlaveAddr);
		return (TEST_FAIL);
	}
	else
	{
		readRegData = regRdBuf & 0x7fff;
		readRegData = readRegData * 0.01;
		C55x_msgWrite("Shunt voltage - %fmV\n\r", readRegData);
	}

	/* Give some delay */
	for(looper = 0; looper < CSL_I2C_MAX_TIMEOUT; looper++)
	{
		asm("	nop");
	}

	regAddr = 0x0002;/*Bus voltage reg address*/
	retVal = read_from_ina_device_reg(LDOI_SlaveAddr, regAddr, &regRdBuf, 2);

	if(retVal != 0)
	{
		C55x_msgWrite("Reading from the bus voltage register of the slave "
				      "address - 0x%x is failed\n\r", LDOI_SlaveAddr);
		return (TEST_FAIL);
	}
	else
	{
		readRegData =  regRdBuf >> 3;
		readRegData = readRegData * 0.004;
		C55x_msgWrite("Bus voltage - %fV\n\r", readRegData);
	}

	/* Give some delay */
	for(looper = 0; looper < CSL_I2C_MAX_TIMEOUT; looper++)
	{
		asm("	nop");
	}

	regAddr = 0x0003;/*Power Reg addr*/
	retVal = read_from_ina_device_reg(LDOI_SlaveAddr, regAddr, &regRdBuf, 2);
	if(retVal != CSL_SOK)
	{
		C55x_msgWrite("Reading power from the slave address "
				      "0x%x is failed\n\r", LDOI_SlaveAddr);
		return (TEST_FAIL);
	}
	else
	{
		readRegData = regRdBuf;
		readRegData = current_lsb[1] * 20 * readRegData;
		C55x_msgWrite("Power - %fmW\n\r", readRegData);
	}

	/* Give some delay */
	for(looper = 0; looper < CSL_I2C_MAX_TIMEOUT; looper++)
	{
		asm("	nop");
	}

	regAddr = 0x04;/*Current Reg addr*/
	retVal = read_from_ina_device_reg(LDOI_SlaveAddr, regAddr, &regRdBuf, 2);
	if(retVal != CSL_SOK)
	{
		C55x_msgWrite("Reading current from the slave address 0x%x is "
				      "failed\n\r");
		return (TEST_FAIL);
	}
	else
	{
		regRdBuf = regRdBuf & 0x7fff;
		readRegData = regRdBuf;
		readRegData = current_lsb[1] * readRegData;
		C55x_msgWrite("Current - %fmA\n\n\r", readRegData);
	}

	/* Give some delay */
	for(looper = 0; looper < CSL_I2C_MAX_TIMEOUT; looper++)
	{
		asm("	nop");
	}

	regAddr   = 0x0000;/*Configuration register address*/
	writeBuff = 0x3F9F;/*Configuration register data*/
	retVal = write_to_ina_device_reg(DSP_DVDDIO_SlaveAddr, regAddr, &writeBuff);
	if(retVal != CSL_SOK)
	{
		C55x_msgWrite("Writing to the configuration register of the slave "
				      "address 0x%x is failed\n\r", DSP_DVDDIO_SlaveAddr);
		return (TEST_FAIL);
	}

	/* Give some delay */
	for(looper = 0; looper < CSL_I2C_MAX_TIMEOUT; looper++)
	{
		asm("	nop");
	}

	regAddr    = 0x0005;/*Calibration register address*/
	writeBuff = 0xBF7F;/*Calibration register data*/
	retVal = write_to_ina_device_reg(DSP_DVDDIO_SlaveAddr, regAddr, &writeBuff);
	if(retVal != CSL_SOK)
	{
		C55x_msgWrite("Writing to the calibration register of the slave "
				      "address 0x%x is failed\n\r", DSP_DVDDIO_SlaveAddr);
		return (TEST_FAIL);
	}

	/* Give some delay */
	for(looper = 0; looper < CSL_I2C_MAX_TIMEOUT; looper++)
	{
		asm("	nop");
	}

	C55x_msgWrite("Reading values from DSP_DVDDIO port\n\r");

	regAddr = 0x0001;/*Shunt voltage reg address*/
	retVal = read_from_ina_device_reg(DSP_DVDDIO_SlaveAddr, regAddr, &regRdBuf, 2);

	if(retVal != 0)
	{
		C55x_msgWrite("Reading from the shunt voltage register of the slave "
				      "address - 0x%x is failed\n\r", DSP_DVDDIO_SlaveAddr);
		return (TEST_FAIL);
	}
	else
	{
		readRegData = regRdBuf & 0x7fff;
		readRegData = readRegData * 0.01;
		C55x_msgWrite("Shunt voltage - %fmV\n\r", readRegData);
	}

	/* Give some delay */
	for(looper = 0; looper < CSL_I2C_MAX_TIMEOUT; looper++)
	{
		asm("	nop");
	}

	regAddr = 0x0002;/*Bus voltage reg address*/
	retVal = read_from_ina_device_reg(DSP_DVDDIO_SlaveAddr, regAddr, &regRdBuf, 2);
	if(retVal != 0)
	{
		C55x_msgWrite("Reading from the bus voltage register of the slave "
				      "address - 0x%x is failed\n\r", DSP_DVDDIO_SlaveAddr);
		return (TEST_FAIL);
	}
	else
	{
		readRegData =  regRdBuf >> 3;
		readRegData = readRegData * 0.004;
		C55x_msgWrite("Bus voltage - %fV\n\r", readRegData);
	}

	/* Give some delay */
	for(looper = 0; looper < CSL_I2C_MAX_TIMEOUT; looper++)
	{
		asm("	nop");
	}

	regAddr = 0x00003;/*Power Reg addr*/
	retVal = read_from_ina_device_reg(DSP_DVDDIO_SlaveAddr, regAddr, &regRdBuf, 2);
	if(retVal != CSL_SOK)
	{
		C55x_msgWrite("Reading power from the slave address 0x%x is "
				      "failed\n\r", DSP_DVDDIO_SlaveAddr);
		return (TEST_FAIL);
	}
	else
	{
		readRegData = regRdBuf;
		readRegData = current_lsb[2] * 20 * readRegData * 2;
		C55x_msgWrite("Power - %fmW\n\r", readRegData);
	}

	/* Give some delay */
	for(looper = 0; looper < CSL_I2C_MAX_TIMEOUT; looper++)
	{
		asm("	nop");
	}

	regAddr = 0x0004;/*Current Reg addr*/
	retVal = read_from_ina_device_reg(DSP_DVDDIO_SlaveAddr, regAddr, &regRdBuf, 2);
	if(retVal != CSL_SOK)
	{
		C55x_msgWrite("Reading current from the slave address 0x%x is "
				      "failed\n\r", DSP_DVDDIO_SlaveAddr);
		return (TEST_FAIL);
	}
	else
	{
		readRegData = regRdBuf;
		readRegData = current_lsb[2] * readRegData * 2;
		C55x_msgWrite("Current - %fmA\n\n\r", readRegData);
	}

	/* Give some delay */
	for(looper = 0; looper < CSL_I2C_MAX_TIMEOUT; looper++)
	{
		asm("	nop");
	}

	regAddr = 0x0000;/*Configuration register address*/
	writeBuff = 0x3F9F;/*Configuration register data*/
	retVal = write_to_ina_device_reg(VCC3V3_USB_SlaveAddr, regAddr, &writeBuff);
	if(retVal != CSL_SOK)
	{
		C55x_msgWrite("Writing to the configuration register of the  slave address 0x%x "
				      "is failed\n\r", VCC3V3_USB_SlaveAddr);
		return (TEST_FAIL);
	}

	/* Give some delay */
	for(looper = 0; looper < CSL_I2C_MAX_TIMEOUT; looper++)
	{
		asm("	nop");
	}

	regAddr   = 0x0005;/*Calibration register address*/
	writeBuff = 0x93C5;/*Calibration register data*/
	retVal = write_to_ina_device_reg(VCC3V3_USB_SlaveAddr, regAddr, &writeBuff);
	if(retVal != CSL_SOK)
	{
		C55x_msgWrite("Writing to the calibration register of the slave "
				      "address 0x%x is failed\n\r", VCC3V3_USB_SlaveAddr);
		return (TEST_FAIL);
	}

	/* Give some delay */
	for(looper = 0; looper < CSL_I2C_MAX_TIMEOUT; looper++)
	{
		asm("	nop");
	}

	C55x_msgWrite("Reading values from VCC3V3_USB port\n\r");

	regAddr = 0x0001;/*Shunt voltage reg address*/
	retVal = read_from_ina_device_reg(VCC3V3_USB_SlaveAddr, regAddr, &regRdBuf, 2);
	if(retVal != 0)
	{
		C55x_msgWrite("Reading from the shunt voltage register of the slave "
				      "address - 0x%x is failed\n\r", VCC3V3_USB_SlaveAddr);
		return (TEST_FAIL);
	}
	else
	{
		readRegData = regRdBuf & 0x7fff;
		readRegData = readRegData * 0.01;
		C55x_msgWrite("Shunt voltage - %fmV\n\r", readRegData);
	}

	/* Give some delay */
	for(looper = 0; looper < CSL_I2C_MAX_TIMEOUT; looper++)
	{
		asm("	nop");
	}


	regAddr = 0x0002;/*Bus voltage reg address*/
	retVal = read_from_ina_device_reg(VCC3V3_USB_SlaveAddr, regAddr, &regRdBuf, 2);
	if(retVal != 0)
	{
		C55x_msgWrite("Reading from the bus voltage register of the slave "
				      "address - 0x%x is failed\n\r", CVDD_SlaveAddr);
		return (TEST_FAIL);
	}
	else
	{
		readRegData =  regRdBuf >> 3;
		readRegData = readRegData * 0.004;
		C55x_msgWrite("Bus voltage - %fV\n\r", readRegData);
	}

	/* Give some delay */
	for(looper = 0; looper < CSL_I2C_MAX_TIMEOUT; looper++)
	{
		asm("	nop");
	}

	regAddr = 0x0003;/*Power Reg addr*/
	retVal = read_from_ina_device_reg(VCC3V3_USB_SlaveAddr, regAddr, &regRdBuf, 2);
	if(retVal != CSL_SOK)
	{
		C55x_msgWrite("Reading power from the slave address "
				      "0x%x is failed\n\r", VCC3V3_USB_SlaveAddr);
		return (TEST_FAIL);
	}
	else
	{
		readRegData = regRdBuf;
		readRegData = current_lsb[3] * 20 * readRegData * 2;
		C55x_msgWrite("Power - %fmW\n\r", readRegData);
	}

	/* Give some delay */
	for(looper = 0; looper < CSL_I2C_MAX_TIMEOUT; looper++)
	{
		asm("	nop");
	}

	regAddr = 0x0004;/*Current Reg addr*/
	retVal = read_from_ina_device_reg(VCC3V3_USB_SlaveAddr, regAddr, &regRdBuf, 2);
	if(retVal != CSL_SOK)
	{
		C55x_msgWrite("Reading current from the slave address 0x%x "
				      "is failed\n\r", VCC3V3_USB_SlaveAddr);
		return (TEST_FAIL);
	}
	else
	{
		regRdBuf = regRdBuf & 0x7fff;
		readRegData = regRdBuf;
		readRegData = current_lsb[3] * readRegData * 2;
		C55x_msgWrite("Current - %fmA\n\n\r", readRegData);
	}

	return (TEST_PASS);

}

 /**
  * \brief This function performs current monitor test
  *
  * \param    testArgs   [IN]   Test arguments
  *
  * \return
  * \n      TEST_PASS  - Test Passed
  * \n      TEST_FAIL  - Test Failed
  *
  */
 TEST_STATUS currentMonitorTest(void *testArgs)
 {
     Int16 testStatus;

     C55x_msgWrite("\n************************************\n\r");
     C55x_msgWrite(  "        Current Monitor Test       \n\r");
     C55x_msgWrite(  "************************************\n\r");

     testStatus = run_current_monitor_test(testArgs);
     if(testStatus != TEST_PASS)
     {
     	C55x_msgWrite("\ncurrent Monitor Test Failed!\n\r");
     }
     else
     {
     	//C55x_msgWrite("\nCurrent Monitor Test Passed!\n\r");
     }

     C55x_msgWrite("\nCurrent Monitor Test Completed!\n\r");


     return testStatus;

 }
