/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief  XMEGA RTC driver example source.
 *
 *      This file contains an example application that demonstrates the RTC
 *      driver.
 *
 * \par Application note:
 *      AVR1314: Using the XMEGA Real Time Counter
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

#include "avr_compiler.h"
#include "rtc_driver.h"


#define LED_PORT          PORTD
#define RTC_CYCLES_1S     1024


/*! \brief Struct to hold the BCD second values. */
typedef struct RTC_BCD_struct{
	uint8_t sec_ones;	/*!< Holds the BCD value of the ones. */
	uint8_t sec_tens;	/*!< Holds the BCD value of the tens. */
} RTC_BCD_t;


/*! \brief The main RTC example.
 *
 *  This function demonstrates how the RTC can be used to implement a simple
 *  second counter. The RTC is configured to produce an interrupt once every
 *  second. The RTC overflow interrupt keeps track of time by increasing a
 *  counter each time it is run. The time is shown in binary coded decimal,
 *  using 7 LEDs. Four LEDs connected to pins 0-3 shows single seconds, while
 *  three LEDs connected to pins 4-6 shows tens of seconds. The second counter
 *  wraps at 60, so it starts at 0 once every minute.
 *
 *  Hardware setup:
 *    - Connect LEDs to pins 0-6 on PORTD to watch the clock.
 */
int main(void)
{
	/* Turn on internal 32kHz. */
	OSC.CTRL |= OSC_RC32KEN_bm;

	do {
		/* Wait for the 32kHz oscillator to stabilize. */
	} while ( ( OSC.STATUS & OSC_RC32KRDY_bm ) == 0);


	/* Set internal 32kHz oscillator as clock source for RTC. */
	CLK.RTCCTRL = CLK_RTCSRC_RCOSC_gc | CLK_RTCEN_bm;

	/* Configure LED port as output. */
	LED_PORT.DIR = 0xFF;

	do {
		/* Wait until RTC is not busy. */
	} while ( RTC_Busy() );

	/* Configure RTC period to 1 second. */
	RTC_Initialize( RTC_CYCLES_1S, 0, 0, RTC_PRESCALER_DIV1_gc );

	/* Enable overflow interrupt. */
	RTC_SetIntLevels( RTC_OVFINTLVL_LO_gc, RTC_COMPINTLVL_OFF_gc );

	/* Enable interrupts. */
	PMIC.CTRL |= PMIC_LOLVLEN_bm;
	sei();

	do {
		/* Wait while the interrupt executes. */
		nop();
	} while (1);
}


/*! \brief RTC overflow interrupt service routine.
 *
 *  This ISR keeps track of the seconds and displays the current second count
 *  on LEDs connected to PORTD.
 */
ISR(RTC_OVF_vect)
{
	static RTC_BCD_t rtcTime;

	/*  Increase and check if 1's reach top value. Reset 1's and increase
	 *  10's if top is reached.
	 */
	if ( ++rtcTime.sec_ones > 9 ) {
		rtcTime.sec_ones = 0;
		rtcTime.sec_tens++;
	}

	/* Check if 10's reach top value and reset to zero if it does. */
	if ( rtcTime.sec_tens > 5 ) {
		rtcTime.sec_tens = 0;
	}

	/* Display the inverted counters on active low LEDs. */
	LED_PORT.OUT = ~( ( rtcTime.sec_tens << 4 ) | rtcTime.sec_ones );
}
