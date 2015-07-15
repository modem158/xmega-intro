/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief
 *      XMEGA IR driver source file.
 *
 *      This file contains the function implementations the XMEGA IR driver and.
 *
 *      The driver is not intended for size and/or speed critical code, since
 *      most functions are just a few lines of code, and the function call
 *      overhead would decrease code performance. The driver is intended for
 *      rapid prototyping and documentation purposes for getting started with
 *      the XMEGA IR module.
 *
 *      For size and/or speed critical code, it is recommended to copy the
 *      function contents directly into your application instead of making
 *      a function call.
 *
 *      Several functions use the following construct:
 *          "some_register = ... | (some_parameter ? SOME_BIT_bm : 0) | ..."
 *      Although the use of the ternary operator ( if ? then : else ) is discouraged,
 *      in some occasions the operator makes it possible to write pretty clean and
 *      neat code. In this driver, the construct is used to set or not set a
 *      configuration bit based on a boolean input parameter, such as
 *      the "some_parameter" in the example above.
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
 * $Revision: 619 $
 * $Date: 2007-08-31 10:49:16 +0200 (fr, 31 aug 2007) $  \n
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
#include "IR_driver.h"



/*! This function set the Transmitter Pulse Length in the IR communication module.
 *
 *  \param  transmitterPulseLength   Selects the transmitter pulse length mode.
 *
 *  transmitter pulse length modes:
 *  - 0x00          : 3/16 of baud rate (Default)
 *  - 0x01 to 0xFE  : fixed pulse length mode.
 *                    (value set the number of clock cycles the pulse last)
 *  - 0xFF          : disable transmitter pulse encoding .
 *                    (pulse length is equal to baud rate)
 */
void IRCOM_TXSetPulseLength(uint8_t transmitterPulseLength)
{
	IRCOM_TXPLCTRL = transmitterPulseLength;
}



/*! This function set the Receiver Pulse Length in the IR communication module.
 *
 *  \param  receiverPulseLength   Selects the receiver pulse length mode.
 *
 *  receiver pulse length modes:
 *  - 0x00          : 3/16 of baud rate (Default)
 *  - 0x01 to 0xFE  : fixed pulse length mode.
 *                    (value set the number of clock cycles the pulse last)
 *  - 0xFF          : disable receiver pulse encoding.
 *                    (pulse length is equal to baud rate)
 */
void IRCOM_RXSetPulseLength(uint8_t receiverPulseLength)
{
	IRCOM_RXPLCTRL = receiverPulseLength;
}



/*! This function can be used to set or disable a event channel as the
 *  receiver input.
 *
 *  \param EventChannelSelect   Selects the event channel to be used as input.
 *
*  eventChannelSelect:
 *  - 0x0          : disable event channel as input.
 *  - 0x1 to 0x7   : NOT used.
 *  - 0x8 to 0xF   : enable channel number (value -8).
 */
void IRCOM_EventChannelSelect(IRDA_EVSEL_t EventChannelSelect)
{
	IRCOM_CTRL = (IRCOM_CTRL & (~IRCOM_EVSEL_gm)) | EventChannelSelect;
}




