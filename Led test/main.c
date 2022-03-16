#include "led_test.h"

/**
*
* \file main.c
*
* \brief led test main file.
*
******************************************************************************/

int main(void)
{
	CSL_Status    status;
	CSL_GpioObj   testArgs;

	/* Initialize the platform */
	status = initPlatform();
	if(status != Platform_EOK)
	{
		C55x_msgWrite("Platform_init failed\n\r");
		return (-1);
	}

	status = ledTest(&testArgs);
	if(status != CSL_SOK)
	{
		return (-1);
	}

	return (0);
}
