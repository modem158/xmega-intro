/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief  XMEGA Analog Comparator driver example source.
 *
 *      This file contains an example application that demonstrates the Analoge
 *      comparator (AC) driver. It shows how to set up the AC using hysteresis. The example uses
 *      pin 0 (low side) and pin 1 (high side). The code waits until the
 *      AC output has toggled four times, then it disables the AC.
 *
 *      The recommended test setup for this application is to connect pin 0 to
 *      a voltage between GND and VCC and connect a 10k potentiometer
 *      between GND and AVCC and connect the wiper terminal to pin 1.
 *
 * \par Application note:
 *      AVR1302: Using the XMEGA Analog Comparator
 *
 * \par Documentation
 *      For comprehensive code documentation, supported compilers, compiler
 *      settings and supported devices see readme.html
 *
 * \author
 *      Atmel Corporation: http://www.atmel.com \n
 *      Support email: avr@atmel.com
 *
 * $Revision: 1569 $
 * $Date: 2008-04-22 13:03:43 +0200 (ti, 22 apr 2008) $  \n
 *
 * Copyright (c) 2008, Atmel Corporation All rights reserved.
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
#include "ac_driver.h"

/* The Analog comparator used in the example.*/
#define AC ACA

int main(void)
{
	/* Enable Analog comparator submodule 0. */
	AC_Enable(&AC, ANALOG_COMPARATOR0, false);

	/* By defalut the Analog comparators interupt flag triggers on changes on
	 * both edges. If another trigger setting (INTMODE) is preferd use the
	 * AC_ConfigInterrupt function to change the settings. */

	/* Set up MUXes to sense pin 0 and 1. */
	AC_ConfigMUX(&AC, ANALOG_COMPARATOR0, AC_MUXPOS_PIN0_gc, AC_MUXNEG_PIN1_gc);

	/* Set up AC with a small hysteresis. */
	AC_ConfigHysteresis(&AC, ANALOG_COMPARATOR0, AC_HYSMODE_SMALL_gc);

	/* Sense for 4 changes on comparator output. */
	for(uint8_t i=0; i < 4; i++){
		/* Wait for Comparator to change value. */
		AC_WaitForComparator_Blocking(&AC, ANALOG_COMPARATOR0);
	}

	/* Disable submodule 0. */
	AC_Disable(&ACA, ANALOG_COMPARATOR0);

	do{
		/* Stop here. */
	}while(true);
}
