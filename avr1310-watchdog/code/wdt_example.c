/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief  XMEGA Watchdog driver example source.
 *
 *      This file contains two example applications that demonstrate the
 *      Watchdog driver with the Watchdog being enabled by either fuses or the
 *      application. Selecting which example to run is done by
 *      commenting/uncommenting the wanted example function call in the main
 *      function. The examples illustrate configuration and operation of the
 *      Watchdog in both normal and window mode.
 *
 * \par Application note:
 *      AVR1310: Using the XMEGA Watchdog
 *
 * \par Documentation
 *      For comprehensive code documentation, supported compilers, compiler
 *      settings and supported devices see readme.html
 *
 * \author
 *      Atmel Corporation: http://www.atmel.com \n
 *      Support email: avr@atmel.com
 *
 * $Revision: 2303 $
 * $Date: 2009-04-16 14:47:58 +0200 (to, 16 apr 2009) $  \n
 *
 * Copyright (c) 2009, Atmel Corporation All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of ATMEL may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY AND
 * SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *****************************************************************************/
/*! \brief System clock used when running the code example. */
#define F_CPU (2000000UL)

#include "avr_compiler.h"
#include "wdt_driver.h"

void wdt_fuse_enable_example( void );
void wdt_sw_enable_example( void );

/*! \brief Duration of the WD timeout period (in ms). */
#define TO_WD     128

/*! \brief Duration of the WD window timeout period (in ms). */
#define TO_WDW    64

/*! \brief Delay (in ms) between each WD reset in window mode. */
#define WINDOW_MODE_DELAY   ( (TO_WDW) +  (TO_WD / 2) )

/*! \brief Delay (in ms) between each WD reset in normal mode. */
#define NORMAL_MODE_DELAY   ( TO_WD / 2 )



/*! \brief Main function to call wanted example.
 *
 *  The WD example is split into two examples, showing WD setup through fuses
 *  and software. Correct WDR execution is also shown for window and normal mode.
 */
int main( void )
{
	wdt_sw_enable_example();
	/*wdt_fuse_enable_example();*/

}



/*! \brief Example showing WD windowed operation when WD is enabled by the
 *         application.
 *
 *  \note  This example requires the WDLOCK fuse to NOT be set.
 */
void wdt_sw_enable_example( void )
{
	/* To run the WD in normal mode, set the timer period and enable WD. */
	WDT_EnableAndSetTimeout( WDT_PER_32CLK_gc );

	/* The WD is now configured and enabled. */


	/* If it is required to configure or reconfigure the WD, it is recommended
	 * to either reset or disable the WDT first. */
	WDT_Disable();

	/* When reconfiguring the WD to window mode, start with enabling normal
	 * mode with the new settings, before configuring window mode. The reason
	 * for this is that the window mode control register can only be written
	 * when normal mode is enabled. */
	WDT_EnableAndSetTimeout( WDT_PER_128CLK_gc );

	/* Configure and enable window mode. */
	WDT_EnableWindowModeAndSetTimeout( WDT_WPER_64CLK_gc );

	/* Enter a timed loop to show timely execution of WD reset. */
	while(true)
	{
		uint16_t repeatCounter;
		/* Make sure that the WDT is not reset too early - insert delay */
		for (repeatCounter = WINDOW_MODE_DELAY; repeatCounter > 0; --repeatCounter ) {
			delay_us( 1000 ); /* 1ms delay @ 2MHz */
		}
		WDT_Reset();
	}
}



/*! \brief Example showing WD normal operation when WD is enabled through fuses.
 *
 *  \note  This example requires the WDLOCK fuse to be set. Also, the WD timeout
 *         period should be set by fuse to 128 cycles and the WD Window period
 *         to 64 cycles.
 */
void wdt_fuse_enable_example( void )
{
	/* When the WD is enabled through fuses, always start by resetting the WDT. */
	WDT_Reset();

	if (true == WDT_IsWindowModeEnabled()){
		while(1){
			/* If window mode is already enabled, an error or a
			 * software reset has occured. It is therfore recommended
			 * to wait until the watchdog resets the entire system.
			 */
		}
	}

	/* Since window mode cannot be enabled through fuses, this must be done by
	 * the application. The window mode period is defined by the fuses and
	 * cannot be changed since WDLOCK is set. */
	WDT_EnableWindowMode();


	/* WD is now configured and enabled. */

	/* If it is required to reconfigure the WD, it is recommended
	 * to reset the WDT first, to avoid timer overflow during synchronization.
	 * Since window mode is enabled, it is nescessary to wait until the timer is
	 * in the open window before issuing the WDR. Be sure to wait for at least
	 * (TO_WDW * 1.3) and no more than ((TO_WDW + TO_WD)*0.7) between resets to
	 * take clock variation into account. In this example, 90 ms is chosen,
	 * which satisfies this constraint.*/

	delay_us( 90000 );
	WDT_Reset();

	WDT_DisableWindowMode();

	/* Enter a timed loop to show timely execution of WD reset. */
	while(true)
	{
		uint16_t repeatCounter;
		/* Make sure that the WDT is not reset too early - insert delay */
		for (repeatCounter = NORMAL_MODE_DELAY; repeatCounter > 0; --repeatCounter ) {
			delay_us( 1000 ); /* 1ms delay @ 2MHz */
		}
		WDT_Reset();
	}
}
