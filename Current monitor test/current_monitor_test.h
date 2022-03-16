/**
 *
 * \file   current_monitor_test.h
 *
 * \brief  This file contains structure, typedefs, functions and
 *         prototypes used for current monitor test
 *
 *****************************************************************************/

#ifndef _CURRNT_MONITOR_TEST_H_
#define _CURRNT_MONITOR_TEST_H_

#include "platform_internals.h"
#include "platform_test.h"


#define CSL_I2C_OWN_ADDR            (0x2F)

#define CSL_I2C_SYS_CLK          	(100)
#define CSL_I2C_BUS_FREQ         	(10)



 /**
  * \brief This function performs current minitor test
  *
  * \param    testArgs   [IN]   Test arguments
  *
  * \return
  * \n      TEST_PASS  - Test Passed
  * \n      TEST_FAIL  - Test Failed
  *
  */
TEST_STATUS currentMonitorTest(void *testArgs);

#endif
