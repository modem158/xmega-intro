/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief  XMEGA IR driver interrupt driven example source.
 *
 *      This file contains an example application that demonstrates the IR
 *      driver, and parts of the USART driver. It shows how to set up
 *      the IR communication module. USART C0 is used in this example.
 *      It can be tested, using a loop-back wire between I/O pins PC2 and PC3
 *
 * \par Application note:
 *      AVR1303: Use and configuration of IR communication module
 *
 * \par Documentation
 *      For comprehensive code documentation, supported compilers, compiler
 *      settings and supported devices see readme.html
 *
 * \author
 *      Atmel Corporation: http://www.atmel.com \n
 *      Support email: avr@atmel.com
 *
 * $Revision: 481 $
 * $Date: 2007-03-06 10:12:53 +0100 (ty, 06 mar 2007) $  \n
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
#include "usart_driver.h"
#include "IR_driver.h"


/*! Number of bytes to send in test example. */
#define NUM_BYTES  3
/*! Define that selects the Usart used in example. */
#define USART USARTC0


/*! USART data struct used in example. */
USART_data_t USART_data;
/*! Test data to send. */
uint8_t sendArray[NUM_BYTES] = {0x55, 0xaa, 0xf0};
/*! Array to put received data in. */
uint8_t receiveArray[NUM_BYTES];
/*! Success variable, used to test driver. */
bool success;

/*! \brief Example application.
 *
 *  Example application. This example configures USARTC0 for with the parameters:
 *      - 8 bit character size
 *      - No parity
 *      - 1 stop bit
 *      - 9600 Baud
 *
 *  This function then sends three bytes and tests if the received data is
 *  equal to the sent data. The code can be tested by connecting PC3 to PC2. If
 *  the variable 'success' is true at the end of the function, the three bytes
 *  have been successfully sent and received.
*/
void main(void)
{
	/* counter variable. */
	uint8_t i;

	/* This PORT setting is only valid to USARTC0 if other USARTs is used a
	 * different PORT and/or pins are used. */
  	/* PC3 (TXD0) as output. */
	PORTC.DIRSET   = PIN3_bm;
	/* PC2 (RXD0) as input. */
	PORTC.DIRCLR   = PIN2_bm;

	/* Use USARTC0 and initialize buffers. */
	USART_InterruptDriver_Initialize(&USART_data, &USART, USART_DREINTLVL_LO_gc);

	/* USARTC0, 8 Data bits, No Parity, 1 Stop bit. */
	USART_Format_Set(USART_data.usart, USART_CHSIZE_8BIT_gc,
                     USART_PMODE_DISABLED_gc, false);

	/* Enable RXC interrupt. */
	USART_RxdInterruptLevel_Set(USART_data.usart, USART_RXCINTLVL_LO_gc);

	/* Set Baud rate to 9600 bps:
	 * Use the default I/O clock frequency that is 2 MHz.
	 * Do not use the baud rate scale factor
	 *
	 * Baud rate select = (1/(16*(((I/O clock frequency)/Baud rate)-1)
	 *                 = 12
	 */
	USART_Baudrate_Set(&USART, 12 , 0);

	/* Set USARTC0 in IrDA mode.*/
	USART_SetMode(&USARTC0, USART_CMODE_IRDA_gc);

	/* If using another Pulse Length than the standard 3/16 use the functions
	 * shown below. */
	/*
	IRCOM_TXSetPulseLength(254);
	IRCOM_RXSetPulseLength(254);
	*/

	/* Enable both RX and TX. */
	USART_Rx_Enable(USART_data.usart);
	USART_Tx_Enable(USART_data.usart);

	/* Enable PMIC interrupt level low. */
	PMIC.CTRL |= PMIC_LOLVLEX_bm;

	/* Enable global interrupts. */
	__enable_interrupt();

	/* Send sendArray. */
	i = 0;
	while (i < NUM_BYTES) {
		bool byteToBuffer;
		byteToBuffer = USART_TXBuffer_PutByte(&USART_data, sendArray[i]);
		if(byteToBuffer){
			i++;
		}
	}

	/* Fetch received data as it is received. */
	i = 0;
	while (i < NUM_BYTES) {
		if (USART_RXBufferData_Available(&USART_data)) {
			receiveArray[i] = USART_RXBuffer_GetByte(&USART_data);
			i++;
		}
	}

	/* Test to see if sent data equals received data. */
	/* Assume success first.*/
	success = true;
	for(i = 0; i < NUM_BYTES; i++) {
		/* Check that each element is received correctly. */
		if (receiveArray[i] != sendArray[i]) {
			success = false;
		}
	}

	/* If success the program ends up inside the if statement.*/
	if(success){
		while(true);
	}else{
	  	while(true);
	}
}


/*! \brief Receive complete interrupt service routine.
 *
 *  Receive complete interrupt service routine.
 *  Calls the common receive complete handler with pointer to the correct USART
 *  as argument.
 */
#pragma vector=USARTC0_RXC_vect
__interrupt void USARTC0_RxcIsr(void)
{
	USART_RXComplete(&USART_data);
}


/*! \brief Data register empty  interrupt service routine.
 *
 *  Data register empty  interrupt service routine.
 *  Calls the common data register empty complete handler with pointer to the
 *  correct USART as argument.
 */
ISR(USARTC0_DRE_vect)
{
	USART_DataRegEmpty(&USART_data);
}

