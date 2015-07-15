/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief  XMEGA Event system example source code.
 *
 *      This file contains example code that demonstrates the event system.
 *      It shows how to set up the Event system to perform some of the examples
 *      given in the application note document AVR1001.
 *
 * \par Application note:
 *      AVR1001: Getting Started With the XMEGA Event System
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
#include "event_system_driver.h"

void Example1( void );
void Example2( void );
void Example3( void );
void Example4( void );


/*! \brief Main example function
 *
 *  This is the main function. Uncomment one of the function calls in main to
 *  test the different examples.
 */
int main( void )
{

	Example1();
	/*Example2();*/
	/*Example3();*/
	/*Example4();*/

	do {

	}while (1);
}


/*! \brief Example 1 from the application note.
 *
 *  This function implements example 1, "Input Capture" from the "Examples"
 *  section of the application note AVR1001.
 *
 *  The example shows how to set up Timer/Counter TCC0 for input capture,
 *  triggered by a change on the input of the I/O port pin PD0. The
 *  corresponding interrupt flag is set when a capture has occured and
 *  the capture time is put in the CCA register.
 */
void Example1( void )
{
	/* Configure PD0 as input, sense on both edges. */
	PORTD.PIN0CTRL |= PORT_ISC_BOTHEDGES_gc;
	PORTD.DIRCLR = 0x01;

	/* Select PD0 as event channel 0 multiplexer input. */
	EVSYS_SetEventSource( 0, EVSYS_CHMUX_PORTD_PIN0_gc );

	/*  Select event channel 0 as event source for TCC0 and input
	 *  capture as event action.
	 */
	TCC0.CTRLD = (uint8_t) TC_EVSEL_CH0_gc | TC_EVACT_CAPT_gc;

	/* Enable TCC0 "Compare or Capture" Channel A. */
	TCC0.CTRLB |= TC0_CCAEN_bm;

	/* Configure TCC0 with the desired frequency and period. */
	TCC0.PER = 0xFFFF;
	TCC0.CTRLA = TC_CLKSEL_DIV1_gc;

	while (1) {
		if ( TCC0.INTFLAGS & TC0_CCAIF_bm ) {
			/*  Clear interrupt flag when new value is captured.
			 *  The last capture value is now available
			 *  in the CCA register.
			 */
			TCC0.INTFLAGS |= TC0_CCAIF_bm;
		}
	}
}


/*! \brief Example 2 from the application note.
 *
 *  This function implements example 2, "Sweep of 4 ADC Channels on
 *  Timer/Counter Overflow" section of the application note AVR1001.
 *
 *  The example shows how to set up the sweep of 4 ADC channels on a
 *  Timer/Counter TCC0 overflow event. More information of set up
 *  of ADC can be found in the application note AVR1300.
 */
void Example2( void )
{
	/* Select TC overflow as event channel 0 multiplexer input. */
	EVSYS_SetEventSource( 0, EVSYS_CHMUX_TCC0_OVF_gc );

	/*  Configure ADC A event channel and configure which channels to sweep
	 *  and enable channel sweep.
	 */
	ADCA.EVCTRL = (uint8_t) ADC_SWEEP_0123_gc |
	              ADC_EVSEL_0123_gc |
	              ADC_EVACT_SWEEP_gc;

	/* Configure the input of the ADC cannels and single ended mode. */
	ADCA.CH0.MUXCTRL = (uint8_t) ADC_CH_MUXPOS_PIN4_gc | ADC_CH_MUXNEG_PIN0_gc;
	ADCA.CH0.CTRL = ADC_CH_INPUTMODE_SINGLEENDED_gc;
	ADCA.CH1.MUXCTRL = (uint8_t) ADC_CH_MUXPOS_PIN5_gc | ADC_CH_MUXNEG_PIN0_gc;
	ADCA.CH1.CTRL = ADC_CH_INPUTMODE_SINGLEENDED_gc;
	ADCA.CH2.MUXCTRL = (uint8_t) ADC_CH_MUXPOS_PIN6_gc | ADC_CH_MUXNEG_PIN0_gc;
	ADCA.CH2.CTRL = ADC_CH_INPUTMODE_SINGLEENDED_gc;
	ADCA.CH3.MUXCTRL = (uint8_t) ADC_CH_MUXPOS_PIN7_gc | ADC_CH_MUXNEG_PIN0_gc;
	ADCA.CH3.CTRL = ADC_CH_INPUTMODE_SINGLEENDED_gc;


	/*  Configure prescaler, resolution, singed mode and set voltage
	 *  reference to internal Vcc - 0.6. Finally enable ADC.
	 */
	ADCA.PRESCALER = ( ADCA.PRESCALER & ~ADC_PRESCALER_gm ) |
	                 ADC_PRESCALER_DIV8_gc;
	ADCA.CTRLB = ( ADCA.CTRLB & ~ADC_RESOLUTION_gm ) |
	             ADC_RESOLUTION_12BIT_gc;
	ADCA.CTRLB = ( ADCA.CTRLB & ~( ADC_CONMODE_bm | ADC_FREERUN_bm ) );
	ADCA.REFCTRL = ( ADCA.REFCTRL & ~ADC_REFSEL_gm ) |
	               ADC_REFSEL_VCC_gc;
	ADCA.CTRLA |= ADC_ENABLE_bm;

	/* Configure TCC0 with the desired frequency and period. */
	TCC0.PER = 0xFFFF;
	TCC0.CTRLA = TC_CLKSEL_DIV1_gc;

	while (1) {
		/* Wait while ADC sweeps on every TCC0 Overflow. */
	}
}


/*! \brief Example 3 from the application note.
 *
 *  This function implements example 3, "32-bit Timer/Counter with 32-bit
 *  Input Capture" from the "Examples" section of the application note AVR1001.
 *
 *  The example shows how to set up Timer/Counter TCC0 and TCC1 for 32 bit input
 *  capture, triggered by a logic change on PD0. The corresponding interrupt
 *  flag is set when a capture has occured and the capture time is put in
 *  the CCA register.
 */
void Example3( void )
{
	/* Configure PD0 as input, sense on both edges. */
	PORTD.PIN0CTRL = PORT_ISC_BOTHEDGES_gc;
	PORTD.DIRCLR = 0x01;

	/* Select TCC0 overflow as event channel 0 multiplexer input.
	 * (Overflow propagation)
	 */
	EVSYS_SetEventSource( 0, EVSYS_CHMUX_TCC0_OVF_gc );

	/* Select PD0 as event channel 1 multiplexer input. (Input capture) */
	EVSYS_SetEventSource( 1, EVSYS_CHMUX_PORTD_PIN0_gc );

	/* Select event channel 0 as clock source for TCC1. */
	TCC1.CTRLA = TC_CLKSEL_EVCH0_gc;

	/* Configure TCC0 for input capture. */
	TCC0.CTRLD = (uint8_t) TC_EVSEL_CH1_gc | TC_EVACT_CAPT_gc;

	/* Configure TCC1 for input capture with event delay. */
	TCC1.CTRLD = (uint8_t) TC_EVSEL_CH1_gc | TC0_EVDLY_bm | TC_EVACT_CAPT_gc;

	/* Enable Compare or Capture Channel A for both timers. */
	TCC0.CTRLB = TC0_CCAEN_bm;
	TCC1.CTRLB = TC1_CCAEN_bm;

	/* Select system clock as clock source for TCC0. */
	TCC0.CTRLA = TC_CLKSEL_DIV1_gc;

	while (1) {
		if ( TCC0.INTFLAGS & TC0_CCAIF_bm ) {
			/*  Clear interrupt flag when new value is captured.
			 *  The last capture value is now available
			 *  in the CCA register.
			 */
			TCC0.INTFLAGS |= TC0_CCAIF_bm;
			TCC1.INTFLAGS |= TC1_CCAIF_bm;
		}
	}
}

/*! \brief Example 4 from the application note.
 *
 *  This function implements example 4, "Event counting" from the
 *  "Examples" section of the application note AVR1001.
 *
 *  The example shows how to set up Timer/Counter TCC0 to count the number
 *  of input capture, triggered by a logic change on PD0.
 */
void Example4( void )
{
	/* Configure PD0 as input, sense on rising edge. */
	PORTD.PIN0CTRL = PORT_ISC_RISING_gc;
	PORTD.DIRCLR = 0x01;

	/* Configure port C to show result. */
	PORTC.DIRSET = 0xFF;

	/* Select PD0 as event channel 0 multiplexer input. */
	EVSYS_SetEventSource( 0, EVSYS_CHMUX_PORTD_PIN0_gc );

	/*  Set the digital filter on event channel 0 to maximum filter
	 *  samples to filter out button bounces.
	 */
	EVSYS_SetEventChannelFilter( 0, EVSYS_DIGFILT_8SAMPLES_gc );

	/* Configure TCC0 with period and event channel 0 as clock source. */
	TCC0.PER = 0xFFFF;
	TCC0.CTRLA = TC_CLKSEL_EVCH0_gc;


	while (1) {
		/* Show the inverted value on the active low LEDs. */
		PORTC.OUT = ~TCC0.CNT;
	}
}
