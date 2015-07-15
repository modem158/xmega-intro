/*! \file *********************************************************************
 *
 * \brief  XMEGA TWI slave example source file.
 *
 * \par Application note:
 *      AVR1320: True 400kHz operation for TWI slave
 *
 * \par Documentation
 *      For comprehensive code documentation, supported compilers, compiler
 *      settings and supported devices see readme.html
 *
 * \author
 *      Atmel Corporation: http://www.atmel.com \n
 *      Support email: avr@atmel.com
 *
 * Copyright (c) 2009 Atmel Corporation. All rights reserved.
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
 * 3. The name of Atmel may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an Atmel
 * AVR product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE EXPRESSLY AND
 * SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

//_____ I N C L U D E S ____________________________________________________

#include "avr_compiler.h"
#include "twi_slave_driver.h"

//_____ M A C R O S ________________________________________________________

//_____ D E F I N I T I O N ________________________________________________

extern void TWI_process(TWI_Slave_t* twi);

uint8_t twic_status_reg;
uint8_t twic_ctrla_reg;
uint8_t twic_ctrlb_reg;
uint8_t twic_data_reg;

/*! Defining an example slave address. */
/*! ATxmega128A1 TWI address */
#define SLAVE_ADDRESS    0x51

/*! Defining number of bytes in buffer. */
#define NUM_BYTES        8

/* Global variables */
TWI_Slave_t twiSlave;      /*!< TWI slave module. */

/*! /brief Process data from master
 *
 *  Simple function that invert the received value in the sendbuffer. This
 *  function is used in the driver and passed on as a pointer to the driver.
 */
void TWIC_SlaveProcessData(void)
{
   PORTF.OUT=twiSlave.receivedData[twiSlave.bytesReceived];
   uint8_t recvData = twiSlave.receivedData[twiSlave.bytesReceived];
   twiSlave.sendData[twiSlave.bytesReceived] = recvData;

   //init first byte to write
   twic_data_reg =  twiSlave.sendData[0];
}

/*! /brief main function
 *
 *  Example code of TWI application in slave mode with no clock stretching
 *  at 400kHz.
 *  The data received from the master are stored in receive buffer and then 
 *  copied in send buffer.
 *   
 */
int main(void)
{
   /* Initialize PORTF for debug on STK600 */
   PORTF.DIRSET = 0xFF;
	
   // Configure LEDs for STK600
   PORTCFG.MPCMASK = 0xFF; // Configure several PINxCTRL registers at the same time
   PORTF.PIN0CTRL = PORT_INVEN_bm; // Invert input to turn the leds on when port output value is 1
   PORTF.OUT = 0x00;
   
   /* Set up interrupt mask on SCL pin */
   PORTC.INT0MASK = PIN1_bm;
	
   /* Initialize TWI slave. */
   TWI_SlaveInitializeDriver(&twiSlave, &TWIC, TWIC_SlaveProcessData);
   TWI_SlaveInitializeModule(&twiSlave,
	                     SLAVE_ADDRESS,
	                     TWI_SLAVE_INTLVL_HI_gc);	
   TWIC.SLAVE.CTRLA |= TWI_SLAVE_SMEN_bm;
	
   /* interrupt level. */
   PMIC.CTRL |= PMIC_HILVLEN_bm | PMIC_MEDLVLEN_bm;
   sei();
	
   do{
      //User application	
   }while (1);  
}

/*! /brief TWI Slave Process Data Interrupt
 *
 *  ISR to process TWI
 *   
 */
ISR(PORTC_INT0_vect)
{
   PORTC.INTFLAGS |= PORT_INT0IF_bm;
   PORTC.INTCTRL = PORT_INT0LVL_OFF_gc;

   TWI_process(&twiSlave);
}
