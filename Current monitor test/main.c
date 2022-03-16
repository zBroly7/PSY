#include "current_monitor_test.h"

/**
*
* \file main.c
*
* \brief current monitor test main file.
*
******************************************************************************/

int main(void)
{
	Int16  retVal;
	//void *testArgs 	= NULL;
    CSL_GpioObj   testArgs;

	/* Initialize the platform */
	retVal = initPlatform();
	if(retVal != Platform_EOK)
	{
		C55x_msgWrite("Systen_init Failed\n\r");
		return (-1);
	}

	retVal = currentMonitorTest(&testArgs);
	if(retVal != 0)
	{
		return (-1);
	}

	return (0);

}
