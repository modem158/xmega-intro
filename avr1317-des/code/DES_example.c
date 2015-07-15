/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief  XMEGA DES driver example source.
 *
 *      This file contains an example application that demonstrates the DES
 *      driver. It shows how to do a DES encryption and decryption, a 3DES
 *      encryption and decryption and a DES cipher block chaining encryption and
 *      decryption. It can be used with both the driver optimized for speed and
 *      the driver optimized for size.
 *
 * \par Application note:
 *      AVR1317: Using the XMEGA built in DES accelerator
 *
 * \par Documentation
 *      For comprehensive code documentation, supported compilers, compiler
 *      settings and supported devices see readme.html
 *
 * \author
 *      Atmel Corporation: http://www.atmel.com \n
 *      Support email: avr@atmel.com
 *
 * $Revision: 1544 $
 * $Date: 2008-04-21 08:36:12 +0200 (ma, 21 apr 2008) $  \n
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
#include "DES_driver.h"

#define DES_BLOCK_LENGTH  8
#define DES_BLOCK_COUNT   3

/*! \brief Plaintext block used by DES and 3DES.
 *
 *  \note  The MSB in the block is byte 0, and LSB is byte 7.
 */
uint8_t data[DES_BLOCK_LENGTH] = {0xAB, 0xBA, 0x00, 0xBE, 0xEF, 0x00, 0xDE, 0xAD};

/*! \brief Variable used to store DES and tripleDES results. */
uint8_t single_ans[DES_BLOCK_LENGTH];

/*! \brief Keys used by all DES operations. (single DES only uses the first 64-bit key).
 *
 *  \note  The MSB of the 3 keys is byte 0,8 and 16. The LSB of each key is byte 7, 15 and 23.
 */
uint8_t keys[DES_BLOCK_LENGTH * DES_BLOCK_COUNT] =
                    {0x94, 0x74, 0xB8, 0xE8, 0xC7, 0x3B, 0xCA, 0x7D,
                     0x28, 0x34, 0x76, 0xAB, 0x38, 0xCF, 0x37, 0xC2,
                     0xFE, 0x98, 0x6C, 0x38, 0x23, 0xFC, 0x2D, 0x23};

/*! \brief Initial vector used during DES Cipher Block Chaining. */
uint8_t init[DES_BLOCK_LENGTH] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};

/*! \brief Plain text used during DES Cipher Block Chaining. */
uint8_t data_block[DES_BLOCK_LENGTH * DES_BLOCK_COUNT]=
                    {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                     0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x00,
                     0xAB, 0xBA, 0x00, 0x00, 0xDE, 0xAD, 0x00, 0x00};

/*! \brief Variable used to store ciphertext from DES Cipher Block Chaining. */
uint8_t cipher_block_ans[DES_BLOCK_LENGTH * DES_BLOCK_COUNT];

/*! \brief Variable used to store decrypted plaintext from DES Cipher Block Chaining.
 *
 *  \note  It is not possible to use the same variable to store the plaintext after a
 *         Cipher Block Chaining operation, as the previous cipher block is used to
 *         decode the current data block.
 *
 */
uint8_t block_ans[DES_BLOCK_LENGTH * DES_BLOCK_COUNT];



/*! \brief Main example doing DES encryption/decryption.
 */
int main( void )
{
	bool success = true;

	/* Example of how to use Single DES encryption and decryption functions. */
	DES_Encrypt(data, single_ans, keys);
	DES_Decrypt(single_ans, single_ans, keys);

	/* Check if decrypted answer is equal to plaintext. */
	for (uint8_t i = 0; i < DES_BLOCK_LENGTH ; i++ ){
		if (data[i] != single_ans[i]){
			success = false;
			break;
		}
	}
	
	if (success){
	
		/* Example of how to use 3DES encryption and decryption functions. */
		DES_3DES_Encrypt(data, single_ans, keys);
		DES_3DES_Decrypt(single_ans, single_ans, keys);

		/* Check if decrypted answer is equal to plaintext. */
		for (uint8_t i = 0; i < DES_BLOCK_LENGTH ; i++ ){
			if (data[i] != single_ans[i]){
				success = false;
				break;
		 	}
		}
	}

	if (success){
		/* Example of how to use DES Cipher Block Chaining encryption and
		 * decryption functions.
		 */
		DES_CBC_Encrypt(data_block, cipher_block_ans, keys, init, true, DES_BLOCK_COUNT);
		DES_CBC_Decrypt(cipher_block_ans, block_ans, keys, init, true, DES_BLOCK_COUNT);

		/* Check if decrypted answer is equal to plaintext. */
		for (uint8_t i = 1; i < (DES_BLOCK_LENGTH * DES_BLOCK_COUNT); i++ ){
			if (data_block[i] != block_ans[i]){
				success = false;
				break;
			}
		}
	}

	if (success){
	    	while (true){
			/* If the example ends up here everything is ok. */
                        nop();
		}
	}else{
		while (true){
			/* If the example ends up here something is wrong. */
                        nop();
		}
	}
}
