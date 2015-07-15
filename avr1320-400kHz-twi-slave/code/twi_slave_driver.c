/*! \file *********************************************************************
 *
 * \brief  XMEGA TWI slave driver source file.
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

#include "twi_slave_driver.h"
#include "clksys_driver.h"

//_____ M A C R O S ________________________________________________________

//_____ D E F I N I T I O N ________________________________________________

extern uint8_t twic_status_reg;
extern uint8_t twic_data_reg;

extern uint8_t twi_data;

/*! \brief Initalizes TWI slave driver structure.
 *
 *  Initialize the instance of the TWI Slave and set the appropriate values.
 *
 *  \param twi                  The TWI_Slave_t struct instance.
 *  \param module               Pointer to the TWI module.
 *  \param processDataFunction  Pointer to the function that handles incoming data.
 */
void TWI_SlaveInitializeDriver(TWI_Slave_t *twi,
                               TWI_t *module,
                               void (*processDataFunction) (void))
{
   twi->interface = module;
   twi->Process_Data = processDataFunction;
   twi->bytesReceived = 0;
   twi->bytesSent = 0;
   twi->status = TWIS_STATUS_READY;
   twi->result = TWIS_RESULT_UNKNOWN;
   twi->abort = false;
	  
   CLKSYS_Enable( OSC_RC32MEN_bm );
   do {} while ( CLKSYS_IsReady( OSC_RC32MRDY_bm ) == 0 );
	
   /* 32MHz Internal RC*/
   CLKSYS_Main_ClockSource_Select( CLK_SCLKSEL_RC32M_gc );
	
   CLKSYS_Disable( OSC_RC2MEN_bm | OSC_RC32KEN_bm );	
}


/*! \brief Initialize the TWI module.
 *
 *  Enables interrupts on address recognition and data available.
 *  Remember to enable interrupts globally from the main application.
 *
 *  \param twi        The TWI_Slave_t struct instance.
 *  \param address    Slave address for this module.
 *  \param intLevel   Interrupt level for the TWI slave interrupt handler.
 */
void TWI_SlaveInitializeModule(TWI_Slave_t *twi,
                               uint8_t address,
                               TWI_SLAVE_INTLVL_t intLevel)
{
   twi->interface->SLAVE.CTRLA = intLevel |
	                         TWI_SLAVE_DIEN_bm |
	                         TWI_SLAVE_APIEN_bm |
	                         TWI_SLAVE_ENABLE_bm;
   twi->interface->SLAVE.ADDR = (address<<1);
}

/*! \brief Process TWI command.
 *
 *  This function is called out of ISR to process TWI command.
 *
 *  \param twi        The TWI_Slave_t struct instance.
 */
void TWI_process(TWI_Slave_t *twi)
{
   //static uint8_t sent_NACK = 0;	//!< Holds whether NACK has been transmitted
   uint8_t currentStatus;
   currentStatus = twic_status_reg;

   twi->interface->SLAVE.CTRLB &= ~TWI_SLAVE_ACKACT_bm;	//!< Clear ACKACT

   /* If bus error. */
   if (currentStatus & TWI_SLAVE_BUSERR_bm) {
      twi->bytesReceived = 0;
      twi->bytesSent = 0;
      twi->result = TWIS_RESULT_BUS_ERROR;
      twi->status = TWIS_STATUS_READY;
   }
   else if (currentStatus & TWI_SLAVE_COLL_bm) {
      /* If transmit collision. */
      twi->bytesReceived = 0;
      twi->bytesSent = 0;
      twi->result = TWIS_RESULT_TRANSMIT_COLLISION;
      twi->status = TWIS_STATUS_READY;
   }
   else if ((currentStatus & TWI_SLAVE_APIF_bm) && (currentStatus & TWI_SLAVE_AP_bm)) {
      /* If address match. */
      TWI_SlaveAddressMatchHandler(twi);	
   }
   else if (currentStatus & TWI_SLAVE_APIF_bm) {
      /* If stop (only enabled through slave read transaction). */	  
      TWI_SlaveStopHandler(twi);
   }
   else if (currentStatus & TWI_SLAVE_DIF_bm) {
      /* If data interrupt. */     
      TWI_SlaveDataHandler(twi);
   }
   else {
      /* If unexpected state. */
      TWI_SlaveTransactionFinished(twi, TWIS_RESULT_FAIL);
   }
}

/*! \brief TWI address match interrupt handler.
 *
 *  Prepares TWI module for transaction when an address match occures.
 *
 *  \param twi The TWI_Slave_t struct instance.
 */
static void TWI_SlaveAddressMatchHandler(TWI_Slave_t *twi)
{
   twi->status = TWIS_STATUS_BUSY;
   twi->result = TWIS_RESULT_UNKNOWN;

   /* Disable stop interrupt. */
   uint8_t currentCtrlA = twi->interface->SLAVE.CTRLA;
   twi->interface->SLAVE.CTRLA = currentCtrlA & ~TWI_SLAVE_PIEN_bm;

   twi->bytesReceived = 0;
   twi->bytesSent = 0;

   //init second byte to write
   twic_data_reg = twi->sendData[0];	
}

/*! \brief TWI stop condition interrupt handler.
 *
 *  \param twi The TWI_Slave_t struct instance.
 */
static void TWI_SlaveStopHandler(TWI_Slave_t *twi)
{
   /* Disable stop interrupt. */
   uint8_t currentCtrlA = twi->interface->SLAVE.CTRLA;
   twi->interface->SLAVE.CTRLA = currentCtrlA & ~TWI_SLAVE_PIEN_bm;
   uint8_t currentStatus = twi->interface->SLAVE.STATUS;
   twi->interface->SLAVE.STATUS = currentStatus | TWI_SLAVE_APIF_bm;

   TWI_SlaveTransactionFinished(twi, TWIS_RESULT_OK);
}

/*! \brief TWI data interrupt handler.
 *
 *  Calls the appropriate slave read or write handler.
 *
 *  \param twi The TWI_Slave_t struct instance.
 */
static void TWI_SlaveDataHandler(TWI_Slave_t *twi)
{	
   if (twic_status_reg & TWI_SLAVE_DIR_bm) {
      TWI_SlaveWriteHandler(twi);		
   } else {
      TWI_SlaveReadHandler(twi);	  
   }
}

/*! \brief TWI slave read interrupt handler.
 *
 *  Handles TWI slave read transactions and responses.
 *
 *  \param twi The TWI_Slave_t struct instance.
 */
static void TWI_SlaveReadHandler(TWI_Slave_t *twi)
{
   /* Enable stop interrupt. */
   uint8_t currentCtrlA = twi->interface->SLAVE.CTRLA;
   twi->interface->SLAVE.CTRLA = currentCtrlA | TWI_SLAVE_PIEN_bm;

   /* If free space in buffer.*/
   if (twi->bytesReceived < TWIS_RECEIVE_BUFFER_SIZE) {
      /* Fetch data */
      uint8_t data = twic_data_reg;
      twi->receivedData[twi->bytesReceived] = data;
      /* Process data. */
      twi->Process_Data();
      twi->bytesReceived++;
   } else {
      // We will ignore the overshooting packets.
   }
	
   /* buffer overflow next time*/
   if  (!(twi->bytesReceived < TWIS_RECEIVE_BUFFER_SIZE)){
      twi->interface->SLAVE.CTRLB |= TWI_SLAVE_ACKACT_bm;
  }
}


/*! \brief TWI slave write interrupt handler.
 *
 *  Handles TWI slave write transactions and responses.
 *
 *  \param twi The TWI_Slave_t struct instance.
 */
static void TWI_SlaveWriteHandler(TWI_Slave_t *twi)
{
   /* If NACK, slave write transaction finished. */
   if ((twi->bytesSent > 0) && (twic_status_reg & TWI_SLAVE_RXACK_bm)) {
      //init first byte to write
      twic_data_reg = twi->sendData[0];
      TWI_SlaveTransactionFinished(twi, TWIS_RESULT_OK);
   }
   /* If ACK, master expects more data. */
   else {
      if (twi->bytesSent < TWIS_SEND_BUFFER_SIZE) {
         twi->bytesSent++;
         uint8_t data = twi->sendData[twi->bytesSent];
         twic_data_reg = data;			
      }
      else {
         /* If buffer overflow. */
         TWI_SlaveTransactionFinished(twi, TWIS_RESULT_BUFFER_OVERFLOW);
      }
   }
} 
 
/*! \brief TWI transaction finished function.
 *
 *  Prepares module for new transaction.
 *
 *  \param twi    The TWI_Slave_t struct instance.
 *  \param result The result of the transaction.
 */
static void TWI_SlaveTransactionFinished(TWI_Slave_t *twi, uint8_t result)
{
   twi->result = result;
   twi->status = TWIS_STATUS_READY;
}