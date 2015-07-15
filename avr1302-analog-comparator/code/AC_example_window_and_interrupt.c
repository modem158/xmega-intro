/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief  XMEGA Analog Comparator driver polled example source.
 *
 *      This file contains an example application that demonstrates the Analog comparator (AC)
 *      driver. It shows how to set up the AC to use interrupt and window mode.
 *      The example compares pin 0 to the range indicated by pin 1 (low side)
 *      and pin 2 (high side). First, the code waits for the window midpoint to
 *      be above the window range, then to get below the range again,
 *      then repeating.
 *
 *      The recommended test setup for this application is to connect three
 *      10k resistors in series between GND and AVCC and connect the low-side
 *      resistor junction to analog input ACA1 and high-side resistor junction
 *      to analog input ACA2. In addition, connect a 10k potentiometer between GND
 *      and AVCC and connect the wiper terminal to analog input ACA0.
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
 * $Revision: 1126 $
 * $Date: 2007-12-14 15:45:54 +0100 (fr, 14 des 2007) $  \n
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
#include "avr_compiler.h"
#include "ac_driver.h"

/* The Analog comparator used in the example.*/
#define AC ACA

/* Global bool that indicates if event is above window is used or not. */
volatile bool above;


int main(void)
{
	/* Enable window mode with event on "midpoint above window". */
	AC_EnableWindowMode(&AC, AC_WINTMODE_ABOVE_gc, AC_WINTLVL_LO_gc);
	above = true;

	/* Set up MUXes to sense pin 1 and 2, with pin 0 as midpoint. */
	AC_ConfigMUX(&AC, ANALOG_COMPARATOR0, AC_MUXPOS_PIN0_gc, AC_MUXNEG_PIN1_gc);
	AC_ConfigMUX(&AC, ANALOG_COMPARATOR1, AC_MUXPOS_PIN0_gc, AC_MUXNEG_PIN3_gc);


	/* Enable both comparators in low power mode. */
	AC_Enable(&AC, ANALOG_COMPARATOR0, false);
	AC_Enable(&AC, ANALOG_COMPARATOR1, false);


	/* Enable output on PORTD. */
	PORTD.DIRSET = 0xFF;
	PORTD.OUT = 0xFF;

	/* Enable PMIC interrupt level low. */
	PMIC.CTRL |= PMIC_LOLVLEX_bm;

	/* Enable global interrupts. */
	sei();

	do {
		/* Wait for interrupts. To check the if interrupts occur, put a breakpoint in
		 * the interrupt handler.*/
		if( AC_GetWindowState(&AC) == AC_WSTATE_ABOVE_gc){
			PORTD.OUT = 0xFB;
		}else if( AC_GetWindowState(&AC) == AC_WSTATE_INSIDE_gc){
			PORTD.OUT = 0xFD;
		}else if( AC_GetWindowState(&AC) == AC_WSTATE_BELOW_gc){
			PORTD.OUT = 0xFE;
		}

	} while(true);
}


/*!  Interrupt handler that alternate the window mode settings between
 *   "midpoint below window" and "midpoint above window".
 */
ISR(ACA_ACW_vect)
{
	if(above){
		/* Enable window mode with event on "midpoint below window". */
		AC_EnableWindowMode(&AC, AC_WINTMODE_BELOW_gc, AC_WINTLVL_LO_gc);
		above = false;
	}else{
		/* Enable window mode with event on "midpoint above window". */
		AC_EnableWindowMode(&AC, AC_WINTMODE_ABOVE_gc, AC_WINTLVL_LO_gc);
		above = true;
  }
}
