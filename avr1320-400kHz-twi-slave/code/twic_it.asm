/*! \file *********************************************************************
 *
 * \brief  XMEGA ISR function for TWI.
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

#ifndef __IAR_SYSTEMS_ASM__
#error Assembler file supported only by IAR EWAVR for AVR
#endif

NAME TWIC_INT
#include "ioxm128a1.h"

//_____ M A C R O S ________________________________________________________

//_____ D E F I N I T I O N ________________________________________________
extern TWIC

COMMON INTVEC(1)
ORG TWIC_TWIS_vect
   RJMP TWIC
 
ENDMOD

NAME TWIC
#include "ioxm128a1.h"

EXTERN twic_status_reg
EXTERN twic_ctrla_reg
EXTERN twic_ctrlb_reg
EXTERN twic_data_reg

PUBLIC TWIC

RSEG CODE
waitandclear:
  ldi R16, 0x10
waitloop:
  dec R16
  breq clearackact
  rjmp waitloop
clearackact:
  lds R16, TWIC_SLAVE_CTRLA
  cbr R16, TWI_SLAVE_ACKACT_bp
  sts TWIC_SLAVE_CTRLA, R16
  rjmp twic_resume

TWIC:
  st -Y,R16
  ; Load SLAVE.STATUS into R16 and also twic_status_reg
  lds R16, TWIC_SLAVE_STATUS 
  sts twic_status_reg, R16   ;store STATUS  

  ; Check if data interrupt
  sbrs R16, TWI_SLAVE_DIF_bp
  rjmp slave_address_match  		; if not, then address or stop

  ; Check if Slave Write
  sbrs R16, TWI_SLAVE_DIR_bp
  rjmp slave_read				; if not, then slave read
  ; Write data register
  lds R16, twic_data_reg
  sts TWIC_SLAVE_DATA, R16
  rjmp twic_resume  

slave_read:
  ; Read data register
  lds R16, TWIC_SLAVE_DATA
  sts twic_data_reg, R16
  rjmp twic_resume  

slave_address_match:
  ; Load data into register ready to send, and transmit ACK
  lds R16, twic_data_reg
  sts TWIC_SLAVE_DATA, R16

  ldi R16, TWI_SLAVE_CMD_RESPONSE_gc;
  sts TWIC_SLAVE_CTRLB, R16

  ; Now the SCL should have been released minimum 0 cycles ago, maximum 3
  ; Path to release for write: 2+2+2+  1+1+      2+2      = 12 -> 372ns
  ; Path to release for read:  2+2+2+  1+1/2+    2        = 11 -> 341ns
  ; Path to release for addr:  2+2+2+  1+1/2+    2+2+2+2  = 17 -> 527ns
  ; Not including the 5+3 cycles to jump and whatever time TWI needs to trigger
  ; Add 248ns for int. response, for maximum 775ns, ~62% of clk low period.
  ; Scope time of ~1100ns matches 775ns + 8 cycles to pin change

  ;  jmp waitandclear
twic_resume:
  ; clearing ACKACT was moved to TWI_Process, as it also aborted ongoing
  ; NACK transmission, and required a busy-wait loop in the interrupt.

  ; Enable pin interrupt on port C for processing
  ldi R16, PORT_INT0LVL_MED_gc  
  sts PORTC_INTCTRL, R16
  
  ld R16, Y+
  
  reti
END