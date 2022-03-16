/**
 *
 * \file   led_test.h
 *
 * \brief  This file contains structure, typedefs, functions and
 *         prototypes used for led test
 *
 *****************************************************************************/

#ifndef _LED_TEST_H_
#define _LED_TEST_H_

#include "platform_internals.h"
#include "platform_test.h"

#define LED_BLINK_DELAY      (250)
#define LED_BLINK_ITERATIONS (10)

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
TEST_STATUS ledTest(void *testArgs);

#endif
