#include "oled_display_test.h"

/**
*
* \file main.c
*
* \brief oled display test main file.
*
******************************************************************************/

int main(void)
{
	Int16 status;
	void *testArgs = NULL;

	/* Initialize Plarform */
	status = initPlatform();
	if(status != Platform_EOK)
	{
		C55x_msgWrite("Platform Init Failed\n");
		return (-1);
	}

	/* Invoke oled display test */
	status = oledDisplayTest(testArgs);
	if(status != 0)
	{
		return (-1);
	}

	return (0);
}
