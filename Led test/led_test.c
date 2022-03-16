/*! \file led_test.c
*
*   \brief Functions that setup and controls gpio pins of the led's for
*          toggling them.
*
*/

#include "led_test.h"

CSL_GpioObj    *gpioObject;

/**
 *  \brief    This function is used to toggle the 3 LED's for 20 times
 *
 *  \param    testArgs   [IN]   Test arguments
 *
 * \return
 * \n         TEST_PASS  - Test Passed
 * \n         TEST_FAIL  - Test Failed
 */
static run_led_test(void *testArgs)
{
	CSL_Status           status;
	CSL_GpioPinConfig    config;
	Uint16               times;
	Uint8                c = 0;

	CSL_GpioObj *gpioObj = (CSL_GpioObj *)testArgs;

	times = LED_BLINK_ITERATIONS;

	/* Enable clocks to all peripherals */
	CSL_SYSCTRL_REGS->PCGCR1 = 0x0000;
	CSL_SYSCTRL_REGS->PCGCR2 = 0x0000;

	C55x_msgWrite("\n\rCheck if all three LED's on the BoosterPack are toggling\n\r");

	CSL_FINST(CSL_SYSCTRL_REGS->EBSR, SYS_EBSR_PPMODE, MODE6);

	/* Open GPIO module */
	gpioObject = GPIO_open(gpioObj, &status);
	if((NULL == gpioObject) || (CSL_SOK != status))
	{
		C55x_msgWrite("GPIO_open failed\n\r");
	    return (TEST_FAIL);
	}

	/* Reset all the pins */
	GPIO_reset(gpioObject);

	/* Configure GPIO pin 16 as output pin */
	config.pinNum    = CSL_GPIO_PIN16;
	config.direction = CSL_GPIO_DIR_OUTPUT;
	config.trigger   = CSL_GPIO_TRIG_CLEAR_EDGE;

	status = GPIO_configBit(gpioObject, &config);
	if(CSL_SOK != status)
	{
		C55x_msgWrite("GPIO_configBit failed\n\r");
	    return (TEST_FAIL);
	}

	/* Configure GPIO pin 17 as output pin */
	config.pinNum    = CSL_GPIO_PIN17;
	config.direction = CSL_GPIO_DIR_OUTPUT;
	config.trigger   = CSL_GPIO_TRIG_CLEAR_EDGE;

	status = GPIO_configBit(gpioObject, &config);
	if(CSL_SOK != status)
	{
		C55x_msgWrite("GPIO_configBit failed\n\r");
	    return (TEST_FAIL);
	}

	/* Configure GPIO pin 28 as output pin */
	config.pinNum    = CSL_GPIO_PIN28;
	config.direction = CSL_GPIO_DIR_OUTPUT;
	config.trigger   = CSL_GPIO_TRIG_CLEAR_EDGE;

	status = GPIO_configBit(gpioObject, &config);
	if(CSL_SOK != status)
	{
		C55x_msgWrite("GPIO_configBit failed\n\r");
	    return (TEST_FAIL);
	}

	while(times)
	{

		/* Write 1 to output pin */
		status = GPIO_write(gpioObject, CSL_GPIO_PIN16, 1);
		if(CSL_SOK != status)
		{
			C55x_msgWrite("GPIO_write Failed\n\r");
		    return (TEST_FAIL);
		}

		C55x_delay_msec(LED_BLINK_DELAY);

		/* Write 1 to output pin */
		status = GPIO_write(gpioObject, CSL_GPIO_PIN16, 0);
		if(CSL_SOK != status)
		{
			C55x_msgWrite("GPIO_write Failed\n\r");
		    return (TEST_FAIL);
		}

		/* Write 1 to output pin */
		status = GPIO_write(gpioObject, CSL_GPIO_PIN17, 1);
		if(CSL_SOK != status)
		{
			C55x_msgWrite("GPIO_write Failed\n\r");
		    return (TEST_FAIL);
		}

		C55x_delay_msec(LED_BLINK_DELAY);

		/* Write 1 to output pin */
		status = GPIO_write(gpioObject, CSL_GPIO_PIN17, 0);
		if(CSL_SOK != status)
		{
			C55x_msgWrite("GPIO_write Failed\n\r");
		    return (TEST_FAIL);
		}

		/* Set Bus for GPIOs */
		CSL_FINST(CSL_SYSCTRL_REGS->EBSR, SYS_EBSR_PPMODE, MODE2);

		/* Write 1 to output pin */
		status = GPIO_write(gpioObject, CSL_GPIO_PIN28, 1);
		if(CSL_SOK != status)
		{
			C55x_msgWrite("GPIO_write Failed\n\r");
		    return (TEST_FAIL);
		}

		C55x_delay_msec(LED_BLINK_DELAY);

		/* Write 1 to output pin */
		status = GPIO_write(gpioObject, CSL_GPIO_PIN28, 0);
		if(CSL_SOK != status)
		{
			C55x_msgWrite("GPIO_write Failed\n\r");
		    return (TEST_FAIL);
		}

		CSL_FINST(CSL_SYSCTRL_REGS->EBSR, SYS_EBSR_PPMODE, MODE6);

		times--;

	}

	CSL_FINST(CSL_SYSCTRL_REGS->EBSR, SYS_EBSR_PPMODE, MODE1);

	/* Close the GPIO module */
	status = GPIO_close(gpioObject);
	if(CSL_SOK != status)
	{
		printf("GPIO_close failed\n\r");
	    return (TEST_FAIL);
	}

#ifdef USE_USER_INPUT

	C55x_msgWrite("\n\rPress Y/y if three LED's are blinking properly, Any other key for failure:\n\r");

	C55x_msgRead(&c, 1);
	if((c != 'y') && (c != 'Y'))
	{
		C55x_msgWrite("LED's are not blinking properly\n\r");
	    return (TEST_FAIL);
	}
#endif

    return (TEST_PASS);

}

/**
 * \brief This function performs led test
 *
 * \param testArgs  - Test arguments
 *
 * \return
 * \n      TEST_PASS  - Test Passed
 * \n      TEST_FAIL  - Test Failed
 *
 */
TEST_STATUS ledTest(void *testArgs)
{
    TEST_STATUS testStatus;

    C55x_msgWrite("\n**********************\n\r");
    C55x_msgWrite(  "       LED Test       \n\r");
    C55x_msgWrite(  "**********************\n\r");

    if(testArgs == NULL)
    {
    	C55x_msgWrite("Invalid Test Arguments!\n\r");
    	C55x_msgWrite("Aborting the Test!!\n\r");
        return (TEST_FAIL);
    }

    testStatus = run_led_test(testArgs);
    if(testStatus != TEST_PASS)
    {
    	C55x_msgWrite("\n\rLED Test Failed!\n\r");
    }
    else
    {
    	C55x_msgWrite("\n\n\rLED Test Passed!\n\r");
    }

    C55x_msgWrite("\n\n\rLED Test Completed!!\n\r");

    return testStatus;
}
