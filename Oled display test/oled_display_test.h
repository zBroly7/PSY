/**
 *
 * \file   oled_display_test.h
 *
 * \brief  This file contains structure, typedefs, functions and
 *         prototypes used for oled display test
 *
 *****************************************************************************/

#ifndef _OLED_TEST_H_
#define _OLED_TEST_H_

#include "platform_internals.h"
#include "platform_test.h"

#define I2C_ADDR 0x3C

typedef	Uint16 I2C_RET;

// Return values
#define	I2C_RET_OK					 (0)
#define	I2C_RET_LOST_ARB			 (1)
#define	I2C_RET_NO_ACK				 (2)
#define	I2C_RET_IDLE_TIMEOUT		 (3)
#define	I2C_RET_BAD_REQUEST			 (4)
#define	I2C_RET_CLOCK_STUCK_LOW		 (5)
#define	I2C_RET_NULL_PTR_ERROR		 (6)
#define	I2C_RET_INVALID_PARAM		 (7)
#define	I2C_RET_NO_ACK_READ			 (8)
#define	I2C_RET_GEN_ERROR			 (99)

#define CSL_I2C_OWN_ADDR         (0x2F)

#define CSL_I2C_SYS_CLK          	(100)
#define CSL_I2C_BUS_FREQ         	(10)

/** Macros for I2C slave address and instance */
#define OLED_SLAVE_ADDR					(0x3C)
#define OLED_I2C_INSTANCE				(CSL_I2C_1)

/** Macros definitons for return types */
#define OLED_ERR					(-1)
#define OLED_SUCCESS				(0)

enum {
  I2C_RELEASE_BUS,
  I2C_DO_NOT_RELEASE_BUS
};

/** \brief  This structure describes a single character's display information
*/
typedef struct
{
    /** width, in bits (or pixels), of the character */
	const Uint8 widthBits;
    /** height, in bits (or pixels), of the character */
	const Uint8 heightBits;
	/** offset of the character's bitmap, in bytes, into the the FONT_INFO's data array */
	const Uint16 offset;
} FONT_CHAR_INFO;

/** \brief  This structure describes a single font
*/
typedef struct
{
	const Uint8 			heightPages;	/**< height, in pages (8 pixels), of the font's characters */
	const Uint8 			startChar;		/**< the first character in the font (e.g. in charInfo and data) */
	const Uint8 			endChar;		/**< the last character in the font */
	//const Uint8			spacePixels;	/**< number of pixels that a space character takes up */
	const FONT_CHAR_INFO*	charInfo;		/**< pointer to array of char information */
	const Uint8*			data;			/**< pointer to generated array of character visual representation */
} FONT_INFO;

I2C_RET oledInit(void);
I2C_RET init();
void begin();
I2C_RET clear();
Int8 printchar(Uint8);
I2C_RET printstr(Int8 string[]);
void noDisplay();
void display();
I2C_RET scrollDisplayLeft();
void scrollDisplayLeftLine(Int8 line);
I2C_RET scrollDisplayRight();
void scrollDisplayRightLine(Int8 line);
void flip();
I2C_RET setOrientation(Int8 newDir);
void autoscroll();
void noAutoscroll();
I2C_RET setline(Int8 line);
void setRolling(Int8 row, Int8 status);
void resetCursor(Int8 page);

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
CSL_Status i2cProbe (Uint16 slaveAddress, Uint16 *pData, Uint16 numBytes);

/**
 * \brief This function performs oled display test
 *
 * \param    testArgs   [IN]   Test arguments
 *
 * \return
 * \n      TEST_PASS  - Test Passed
 * \n      TEST_FAIL  - Test Failed
 *
 */
TEST_STATUS oledDisplayTest(void *testArgs);
#endif // _OLED_TEST_H_

/* Nothing past this point */
