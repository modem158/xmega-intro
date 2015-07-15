/*   This file is prepared for Doxygen automatic documentation generation   */
/*! \file ********************************************************************
 *
 * \brief
 * 		XMEGA 32-bit Real Time Counter (RTC) example source code.
 *
 *      This file contains an example application that demonstrates the 32-bit
 *      RTC driver. It shows how to set up the 32-bit RTC together with battery
 *      backup module. It uses the compare match interrupt of 32-bit RTC module 
 *      to wake up MCU from power save mode. This example can be used to 
 *      demonstrate how low power consumption xmega can be.
 *
 * \par Application note:
 *      AVR1321: Using the XMEGA 32-bit RTC
 *
 * \par Documentation
 *      For comprehensive code documentation, supported compilers, compiler
 *      settings and supported devices see readme.html
 *
 * \author
 *      Atmel Corporation: http://www.atmel.com \n
 *      Support email: avr@atmel.com
 *
 * $Revision: 112 $
 * $Date: 2009-11-18 14:31:39 +0800 (WED, 18 Nov 2009) $  \n
 ****************************************************************************/

/*! \page License
Copyright (c) 2009 Atmel Corporation. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

3. The name of Atmel may not be used to endorse or promote products derived
from this software without specific prior written permission.

4. This software may only be redistributed and used in connection with an Atmel
AVR product.

THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE
 */

/*============================ INCLUDES ======================================*/
#include "avr_compiler.h"
#include "rtc32_driver.h"
#include "vbat.h"

/*============================ IMPLEMENTATION ================================*/
void chip_init(void)
{
	/* 
	* Disable all modules except the 32-bit RTC to minimise power
	* consumption 
	*/
	PR.PRGEN = PR_AES_bm | PR_EBI_bm | PR_EVSYS_bm | PR_DMA_bm;
	PR.PRPA = PR_ADC_bm | PR_AC_bm;
	PR.PRPB = PR_DAC_bm | PR_ADC_bm | PR_AC_bm;
	PR.PRPC = PR_TWI_bm | PR_USART0_bm | PR_USART1_bm | PR_SPI_bm
		| PR_HIRES_bm | PR_TC0_bm | PR_TC1_bm;
	PR.PRPD = PR_USART0_bm | PR_USART1_bm | PR_SPI_bm | PR_HIRES_bm
		| PR_TC0_bm | PR_TC1_bm;
	PR.PRPE = PR_TWI_bm | PR_USART0_bm | PR_HIRES_bm | PR_TC0_bm | PR_TC1_bm;
	PR.PRPF = PR_USART0_bm | PR_HIRES_bm | PR_TC0_bm;

	PORTA.DIR = 0x02;
	PORTA.OUTCLR = 0x02;

	/* Configure system clock to use 32 MHz internal RC oscillator */
	OSC.CTRL |= OSC_RC32MEN_bm;
	ENTER_CRITICAL_REGION( );
	CCP = 0xD8;
	CLK.PSCTRL = (uint8_t)CLK_PSADIV_1_gc | (uint8_t)CLK_PSBCDIV_1_1_gc;
	while ( (OSC.STATUS & OSC_RC32MRDY_bm) == 0 );
	CCP = 0xD8;
	CLK.CTRL = CLK_SCLKSEL_RC32M_gc;
	LEAVE_CRITICAL_REGION();
	
	/* Configure the interrupt system */
	PMIC.CTRL |= PMIC_LOLVLEN_bm;

}

void vbat_init(void)
{
	vbat_reset();
	vbat_enable_xosc(0);
	RTC32_Initialize(0xffffffff, 0, 2 );
	RTC32_SetCompareIntLevel(RTC32_COMPINTLVL_LO_gc);
}

int main(void)
{
	uint8_t vbat_status;

	chip_init();
	vbat_status = vbat_system_check(true);
	
	/* 
	* Depending on the VBAT system check appropriate actions need to 
	* be taken.
	* In this example we re-initialize the VBAT system in all
	* error cases.
	*/
	switch (vbat_status)
	{
	case VBAT_STATUS_OK:
		// Interrupts must be re-enabled
		RTC32_SetCompareIntLevel(RTC32_COMPINTLVL_LO_gc);
		break;
	case VBAT_STATUS_NO_POWER: // fall through
	case VBAT_STATUS_BBPOR: // fall through
	case VBAT_STATUS_BBBOD: // fall through
	case VBAT_STATUS_XOSCFAIL: // fall through
	default:
		vbat_init();
		break;
	}

	sei();
	
	while (true) {
        	RTC32_SetAlarm(2);
        	PORTA.OUTTGL = 0x02;
		SLEEP.CTRL = SLEEP_SMODE_PSAVE_gc | SLEEP_SEN_bm;
		cpu_sleep();
	}
}

#pragma vector= RTC32_COMP_vect
__interrupt void rtc32_comp(void)
{
}
