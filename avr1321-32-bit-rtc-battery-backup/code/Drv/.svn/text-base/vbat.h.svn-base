/*   This file is prepared for Doxygen automatic documentation generation   */
/*! \file ********************************************************************
 *
 * \brief XMEGA VBAT system driver.
 *
 *
 * \par Application note:
 *      AVR1321: Using the XMEGA 32-bit RTC and battery backup system
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
#ifndef VBAT_H_INCLUDED
#define VBAT_H_INCLUDED

//! \name Battery backup system status macros
//@{
/**
 * \brief Everything works as expected.
 *
 * The backup system is configured and had no issues while main power was
 * lost. Because of that all data stored in the backup domain is valid.
 */
#define VBAT_STATUS_OK          0

/**
 *  \brief No power detected on VBAT.
 *
 * The backup system is not operational and all data within the backup system
 * will be lost when main power is lost.
 */
#define VBAT_STATUS_NO_POWER    1

/**
 *  \brief The backup system must be initialized.
 *
 * A POR was detected on VBAT input which indicates that a supply was connected
 * to the backup system. Since this is also the first start-up of the device we
 * then need to initialize the backup system.
 */
#define VBAT_STATUS_INIT        2

/**
 *  \brief A POR was detected on the VBAT input.
 *
 * This means the backup system is in an unkonw state.
 */
#define VBAT_STATUS_BBPOR       3

/**
 * \brief A brown-out was detected on the VBAT input.
 *
 * The backup system is in a unkown state and must be reset.
 */
#define VBAT_STATUS_BBBOD       4
/**
 * \brief A failure was detected on the oscillator.
 *
 * The oscillator stopped for at least TBD period of time and because of that
 * we can not rely on the RTC time any more.
 */
#define VBAT_STATUS_XOSCFAIL    5
//@}

/**
* \brief Check battery backup system status.
*
* \param first_time_startup When this is the first time the system starts then
* this is false otherwise true.
* \returns Backup system status.
*/
extern uint8_t vbat_system_check(bool first_time_init);

/**
 * \brief This function resets the battery backup module.
 *
 * This function will also set the access enable bit for the control register.
 */
extern void vbat_reset(void);

/**
 * \brief This function starts the crystal oscillator with 1 or 1024 Hz
 *  clock output.
 *
 *  \param use1khz Boolean for selecting 1 kHz or 1 Hz RTC clock rate.
 */
extern void vbat_enable_xosc(bool use1khz);

#endif /* VBAT_H_INCLUDED */
