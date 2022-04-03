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

/*! \file audio_playback_test.c
*
*   \brief Functions that configure the audio codec regsiters for the audio
*          playback loopback operation.
*
*/

#include "audio_playback_test.h"
#include "audio_common.h"

extern TEST_STATUS audio_playback_test(void *testArgs);

/**
 *
 * \brief This function configures all audio codec registers for
 *        playback test
 *
 * \param    testArgs   [IN]   Test arguments
 *
 * \return
 * \n      TEST_PASS  - Test Passed
 * \n      TEST_FAIL  - Test Failed
 *
 */
static TEST_STATUS AIC3206_playback_config(void *testArgs)
{
    /* Pre-generated sine wave data, 16-bit signed samples */
    Int16 sinetable[48] = {
        0x0000, 0x10b4, 0x2120, 0x30fb, 0x3fff, 0x4dea, 0x5a81, 0x658b,
        0x6ed8, 0x763f, 0x7ba1, 0x7ee5, 0x7ffd, 0x7ee5, 0x7ba1, 0x76ef,
        0x6ed8, 0x658b, 0x5a81, 0x4dea, 0x3fff, 0x30fb, 0x2120, 0x10b4,
        0x0000, 0xef4c, 0xdee0, 0xcf06, 0xc002, 0xb216, 0xa57f, 0x9a75,
        0x9128, 0x89c1, 0x845f, 0x811b, 0x8002, 0x811b, 0x845f, 0x89c1,
        0x9128, 0x9a76, 0xa57f, 0xb216, 0xc002, 0xcf06, 0xdee0, 0xef4c
    };
    Int16 sec, msec;
    Int16 sample;

    /* Configure AIC3206 */
    AIC3206_write( 0,  0x00 );  // Select page 0
    AIC3206_write( 1,  0x01 );  // Reset codec
    C55x_delay_msec(1);  			// Wait 1ms after reset
    AIC3206_write( 0,  0x01 );  // Select page 1
    AIC3206_write( 1,  0x08 );  // Disable crude AVDD generation from DVDD
    AIC3206_write( 2,  0x01 );  // Enable Analog Blocks, use LDO power
    AIC3206_write( 123,0x05 );  // Force reference to power up in 40ms
    C55x_delay_msec(40); 			// Wait at least 40ms
    AIC3206_write( 0,  0x00 );  // Select page 0

    /* PLL and Clocks config and Power Up  */
    AIC3206_write( 27, 0x0d );  // BCLK and WCLK are set as o/p; AIC3206(Master)
    AIC3206_write( 28, 0x00 );  // Data ofset = 0
    AIC3206_write( 4,  0x03 );  // PLL setting: PLLCLK <- MCLK, CODEC_CLKIN <-PLL CLK
    AIC3206_write( 6,  0x07 );  // PLL setting: J=7
    AIC3206_write( 7,  0x06 );  // PLL setting: HI_BYTE(D=1680)
    AIC3206_write( 8,  0x90 );  // PLL setting: LO_BYTE(D=1680)
    AIC3206_write( 30, 0x88 );  // For 32 bit clocks per frame in Master mode ONLY
    							// BCLK=DAC_CLK/N =(12288000/8) = 1.536MHz = 32*fs
    AIC3206_write( 5,  0x91 );  // PLL setting: Power up PLL, P=1 and R=1
    C55x_delay_msec(10); 		// Wait for PLL to come up
    AIC3206_write( 13, 0x00 );  // Hi_Byte(DOSR) for DOSR = 128 decimal or 0x0080 DAC oversamppling
    AIC3206_write( 14, 0x80 );  // Lo_Byte(DOSR) for DOSR = 128 decimal or 0x0080
    AIC3206_write( 20, 0x80 );  // AOSR for AOSR = 128 decimal or 0x0080 for decimation filters 1 to 6
    AIC3206_write( 11, 0x82 );  // Power up NDAC and set NDAC value to 2
    AIC3206_write( 12, 0x87 );  // Power up MDAC and set MDAC value to 7
    AIC3206_write( 18, 0x87 );  // Power up NADC and set NADC value to 7
    AIC3206_write( 19, 0x82 );  // Power up MADC and set MADC value to 2

    /* DAC ROUTING and Power Up */
    AIC3206_write( 0,  0x01 );  // Select page 1
    AIC3206_write( 12, 0x08 );  // LDAC AFIR routed to HPL
    AIC3206_write( 13, 0x08 );  // RDAC AFIR routed to HPR
    AIC3206_write( 0,  0x00 );  // Select page 0
    AIC3206_write( 64, 0x02 );  // Left vol=right vol
    AIC3206_write( 65, 0x00 );  // Left DAC gain to 0dB VOL; Right tracks Left
    AIC3206_write( 63, 0xd4 );  // Power up left,right data paths and set channel
    AIC3206_write( 0,  0x01 );  // Select page 1
    AIC3206_write( 16, 0x00 );  // Unmute HPL , 0dB gain
    AIC3206_write( 17, 0x00 );  // Unmute HPR , 0dB gain
    AIC3206_write( 9 , 0x30 );  // Power up HPL,HPR
    C55x_delay_msec(1);         // Wait 1 msec

    /* ADC ROUTING and Power Up */
    AIC3206_write( 0,  0x01 );  // Select page 1
    AIC3206_write( 52, 0x30 );  // STEREO 1 Jack
    							// IN2_L to LADC_P through 40 kohm
    AIC3206_write( 55, 0x30 );  // IN2_R to RADC_P through 40 kohmm
    AIC3206_write( 54, 0x03 );  // CM_1 (common mode) to LADC_M through 40 kohm
    AIC3206_write( 57, 0xc0 );  // CM_1 (common mode) to RADC_M through 40 kohm
    AIC3206_write( 59, 0x00 );  // MIC_PGA_L unmute
    AIC3206_write( 60, 0x00 );  // MIC_PGA_R unmute
    AIC3206_write( 0,  0x00 );  // Select page 0
    AIC3206_write( 81, 0xc0 );  // Powerup Left and Right ADC
    AIC3206_write( 82, 0x00 );  // Unmute Left and Right ADC
    AIC3206_write( 0,  0x00 );  // Select page 0
    C55x_delay_msec(1);         // Wait 1 msec

    /* Initialize I2S */
    initialise_i2s_interface();

    gpio_interrupt_initiliastion();
    /* Play Tone for 5 seconds*/
    for ( sec = 0 ; sec < 5 ; sec++ )
    {
        for ( msec = 0 ; msec < 5000 ; msec++ )
        {
            for ( sample = 0 ; sample < 48 ; sample++ )
            {
                /* Write 16-bit left channel Data */
                I2S_writeLeft( sinetable[sample]);

                /* Write 16-bit right channel Data */
                I2S_writeRight(sinetable[sample]);
            }

            if(sw3Pressed == TRUE)
            {
            	break;
            }
        }
        if(sw3Pressed == TRUE)
        {
        	break;
        }

    }
    I2S_close(hI2s);    // Disble I2S
    AIC3206_write( 0,  0x00 );  // Select page 0
    AIC3206_write( 1,  0x01 );  // Reset codec

	return (TEST_PASS);

}

/**
 *
 * \brief This function used to to initialise i2c interface and run
 *        audio playback test
 *
 * \param    testArgs   [IN]   Test arguments
 *
 * \return
 * \n      TEST_PASS  - Test Passed
 * \n      TEST_FAIL  - Test Failed
 *
 */
static TEST_STATUS run_audio_playback_test(void *testArgs)
{
	TEST_STATUS retVal;
	Uint8  c = 0;

    /* Enable clocks to all peripherals */
    CSL_SYSCTRL_REGS->PCGCR1 = 0x0000;
    CSL_SYSCTRL_REGS->PCGCR2 = 0x0000;

    retVal =  SYS_setEBSR(CSL_EBSR_FIELD_PPMODE,
                          CSL_EBSR_PPMODE_1);    // Configure Parallel Port for I2S2
    retVal |= SYS_setEBSR(CSL_EBSR_FIELD_SP1MODE,
                          CSL_EBSR_SP1MODE_1);  // Serial Port mode 1 (I2S1 and GP[11:10]).

    retVal = initialise_i2c_interface(testArgs);
    if(retVal != 0)
    {
    	C55x_msgWrite("I2C initialisation failed\n\r");
    	return (TEST_FAIL);
    }

    C55x_msgWrite("Test outputs a sine tone on HEADPHONE port of BoosterPack\n\n\r");

    C55x_msgWrite("Connect the headset to the HEADPHONE port of the BoosterPack\n\r");
    C55x_msgWrite("Observe the Audio Tone at HEADPHONE port\n\n\r");

    /*Sending the Pre-generated sine wave through the STREO OUT to headset*/
    C55x_msgWrite("Starting the Audio Tone Play...\n\n\r" );
    retVal = AIC3206_playback_config(testArgs);
    if(retVal != 0)
    {
    	return (TEST_FAIL);
    }

#ifdef USE_USER_INPUT

	C55x_msgWrite("Press Y/y if Audio output from the HEADPHONE port is proper, \n\r"
			      "any other key for failure:\n\r");

	C55x_msgRead(&c, 1);
	if((c != 'y') && (c != 'Y'))
	{
		C55x_msgWrite("No Audio stream is observed from the headset\n\r"
				      "connected at the HEADPHONE port\n\r");
	    return (TEST_FAIL);
	}
#endif

    return (TEST_PASS);

}

/**
 * \brief This function performs audio playback test
 *
 * \param    testArgs   [IN]   Test arguments
 *
 * \return
 * \n      TEST_PASS  - Test Passed
 * \n      TEST_FAIL  - Test Failed
 *
 */
 TEST_STATUS audioPlaybackTest(void *testArgs)
 {
     TEST_STATUS testretVal;

     C55x_msgWrite("\n***************************************\n\r");
     C55x_msgWrite(  "          AUDIO PLAYBACK TEST       \n\r");
     C55x_msgWrite(  "***************************************\n\r");

     testretVal = run_audio_playback_test(testArgs);
     if(testretVal != TEST_PASS)
     {
     	C55x_msgWrite("\n\nAudio Playback Test Failed!\n\r");
     	return (TEST_FAIL);
     }
     else
     {
     	C55x_msgWrite("\n\nAudio Playback Test Passed!\n\r");
     }

     C55x_msgWrite("\nAudio Playback Test Completed!\n\r");

     return testretVal;

 }

