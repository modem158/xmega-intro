//******************************************************************************
//*
//* XMEGA DES driver assembly source file optimized for speed.
//*
//* This file contains the low-level implementations for the
//* XMEGA DES driver optimized for speed. It is written for the GCC Assembler.
//*
//* Note on GCC calling convention:
//*      Scratch registers:   R0-R1, R18-R27, R30-R31
//*      Preserved registers: R2-R17, R28-R29
//*      Parameter registers: R25-R8 (2-byte alignment starting from R25)
//*      Return registers:    R25-R8 (2-byte alignment starting from R25)
//*
//* Application note:
//*      AVR1317: Using the XMEGA built in DES accelerator
//*
//* Documentation
//*      For comprehensive code documentation, supported compilers, compiler
//*      settings and supported devices see readme.html
//*
//*      Atmel Corporation: http://www.atmel.com \n
//*      Support email: avr@atmel.com
//*
//* $Revision: 1300 $
//* $Date: 2008-02-25 10:34:09 +0100 (ma, 25 feb 2008) $
//*
//* Copyright (c) 2008, Atmel Corporation All rights reserved.
//*
//* Redistribution and use in source and binary forms, with or without
//* modification, are permitted provided that the following conditions are met:
//*
//* 1. Redistributions of source code must retain the above copyright notice,
//* this list of conditions and the following disclaimer.
//*
//* 2. Redistributions in binary form must reproduce the above copyright notice,
//* this list of conditions and the following disclaimer in the documentation
//* and/or other materials provided with the distribution.
//*
//* 3. The name of ATMEL may not be used to endorse or promote products derived
//* from this software without specific prior written permission.
//*
//* THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
//* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
//* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY AND
//* SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT,
//* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
//* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
//* THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//******************************************************************************

// ----------
// This macro is called by several other routines, and contains common code
// for loading data to R7-R0.
//
// Input:
//    - R25:R24 - pointer to input buffer.
//
// Register usage during DES_INTERNAL_Load_Data:
//
// During execution:
//   - R31:R30 (Z) is used for misc memory pointing and is not preserved.
//   - R7 - R0 contains the data (plaintext or ciphertext)
// ----------
.macro DES_INTERNAL_Load_Data
	movw	r30, r24
	ld	r7,  Z+
	ld	r6,  Z+
	ld	r5,  Z+
	ld	r4,  Z+
	ld	r3,  Z+
	ld	r2,  Z+
	ld	r1,  Z+
	ld	r0,  Z+
.endm


// ----------
// This macro is called by several other routines, and contains common code
// for loading data to R7-R0.
//
// Input:
//    - R31:R30 - pointer to data buffer.
//
// Register usage during DES_INTERNAL_Load_Into_R7_R0:
//
// During execution:
//   - R31:R30 (Z) is used for misc memory pointing and is not preserved.
//   - R7 - R0 contains the data (plaintext or ciphertext)
// ----------
.macro DES_INTERNAL_Load_Into_R7_R0
	ld	r7,  Z+
	ld	r6,  Z+
	ld	r5,  Z+
	ld	r4,  Z+
	ld	r3,  Z+
	ld	r2,  Z+
	ld	r1,  Z+
	ld	r0,  Z+
.endm


// ----------
// This macro is called by several other routines, and contains common code
// for loading data to R15-R8.
//
// Input:
//    - R31:R30 - pointer to data buffer.
//
// Register usage during DES_INTERNAL_Load_Into_R15_R8:
//
// During execution:
//   - R31:R30 (Z) is used for misc memory pointing and is not preserved.
//   - R15 - R8 contains the data (plaintext or ciphertext)
// ----------
.macro DES_INTERNAL_Load_Into_R15_R8
	ld	r15, Z+
	ld	r14, Z+
	ld	r13, Z+
	ld	r12, Z+
	ld	r11, Z+
	ld	r10, Z+
	ld	r9,  Z+
	ld	r8,  Z+
.endm


// ----------
// This macro is called by several other routines, and contains common code
// to XOR two 64 bits values.
//
// Input:
//     R15 - R8 - 64 bits value.
//     R7  - R0 - 64 bits value.
//
// Returns:
//     R7  - R0 - 64 bits xored value.
// ----------
.macro DES_INTERNAL_XOR_Routine
	eor	r7, r15
	eor	r6, r14
	eor	r5, r13
	eor	r4, r12
	eor	r3, r11
	eor	r2, r10
	eor	r1,  r9
	eor	r0,  r8
.endm


// ----------
// This macro is called by several other routines, and contains common code
// for loading the first key in the key buffer to register 15 to 8.
//
// Input:
//     R21:R20 - pointer to key buffer.
//
// Registers used internally:
//     R31:R30 - pointer to key buffer.
//
// Returns:
//     R15 - R8 - 64 bits key.
// ----------
.macro DES_INTERNAL_LoadKey1
	movw	R30, R20
	ldd	R15, Z+0
	ldd	R14, Z+1
	ldd	R13, Z+2
	ldd	R12, Z+3
	ldd	R11, Z+4
	ldd	R10, Z+5
	ldd	R9,  Z+6
	ldd	R8,  Z+7
.endm


// ----------
// This macro is called by several other routines, and contains common code
// for loading the second key in the key buffer to register 15 to 8.
//
// Input:
//     R21:R20 - pointer to key buffer.
//
// Registers used internally:
//     R31:R30 - pointer to key buffer.
//
// Returns:
//     R15 - R8 - 64 bits key.
// ----------
.macro DES_INTERNAL_LoadKey2
	movw	R30, R20
	ldd	R15, Z+8
	ldd	R14, Z+9
	ldd	R13, Z+10
	ldd	R12, Z+11
	ldd	R11, Z+12
	ldd	R10, Z+13
	ldd	R9,  Z+14
	ldd	R8,  Z+15
.endm


// ----------
// This macro is called by several other routines, and contains common code
// for loading the third key in the key buffer to register 15 to 8.
//
// Input:
//     R21:R20 - pointer to key buffer.
//
// Registers used internally:
//     R31:R30 - pointer to key buffer.
//
// Returns:
//     R15 - R8 - 64 bits key.
// ----------
.macro DES_INTERNAL_LoadKey3
	movw	R30, R20
	ldd	R15, Z+16
	ldd	R14, Z+17
	ldd	R13, Z+18
	ldd	R12, Z+19
	ldd	R11, Z+20
	ldd	R10, Z+21
	ldd	R9,  Z+22
	ldd	R8,  Z+23
.endm


// ----------
// This macro is called by several other routines, and contains common code
// for storing data located in register 7 to 0 to memory pointed to by Z.
//
// Input:
//     R23:R22 - pointer to ouput memory.
//
// Registers used during execution:
//     R31:R30 - Z pointer is used, and not preserved.
// ----------
.macro DES_INTERNAL_Store_Data
	movw	r30, r22
	st	Z+,  r7
	st	Z+,  r6
	st	Z+,  r5
	st	Z+,  r4
	st	Z+,  r3
	st	Z+,  r2
	st	Z+,  r1
	st	Z+,  r0
.endm


// ----------
// This macro is called by several other routines, and contains common code
// doing the DES encryption/decryption.
//
// Input:
//     R15 - R8 - 64 bits key.
//     R7  - R0 - 64 bits plaintext/ciphertext.
//
// Returns:
//     R7 -  R0 - 64 bits ciphertext/plaintext.
//     R8 - R15 - 64 bits key.
// ----------
.macro DES_INTERNAL_DES_Routine
	des	0
	des	1
	des	2
	des	3
	des	4
	des	5
	des	6
	des	7
	des	8
	des	9
	des	10
	des	11
	des	12
	des	13
	des	14
	des	15
.endm


// ----------
// This macro is called by several other routines, and contains common code
// for preserving register defined in GCC.
// ----------
.macro DES_INTERNAL_Prolog
	push	r0
	push	r1
	push	r2
	push	r3
	push	r4
	push	r5
	push	r6
	push	r7
	push	r8
	push	r9
	push	r10
	push	r11
	push	r12
	push	r13
	push	r14
	push	r15
	push	r16
	push	r17
	push	r28
	push	r29
.endm


// ----------
// This macro is called by several other routines, and contains common code
// for restoring register defined in GCC.
//
// Note: This is the last macro used in the routines, and contain a ret instruction
//       to return from the routine.
// ----------
.macro DES_INTERNAL_Epilog
	pop	r29
	pop	r28
	pop	r17
	pop	r16
	pop	r15
	pop	r14
	pop	r13
	pop	r12
	pop	r11
	pop	r10
	pop	r9
	pop	r8
	pop	r7
	pop	r6
	pop	r5
	pop	r4
	pop	r3
	pop	r2
	pop	r1
	pop	r0
	ret
.endm


// ----------
// This routine does a single DES encryption.
//
// Prototype:
//    void DES_Encrypt(uint8_t * plaintext, uint8_t * ciphertext, uint8_t * key);
//
// Input:
//    - R25:R24 - pointer to plaintext buffer.
//    - R23:R22 - pointer to ciphertext buffer.
//    - R21:R20 - pointer to key buffer.
//
// Register usage during DES_Encrypt:
//
// During execution:
//   - R31:R30 (Z) is used for misc memory pointing and is not preserved.
//   - R15 - R8 contains current key being processed.
//   - R7  - R0 contains the data (plaintext or ciphertext).
// ----------
.global DES_Encrypt
DES_Encrypt:
	DES_INTERNAL_Prolog
	DES_INTERNAL_Load_Data
	DES_INTERNAL_LoadKey1
	clh
	DES_INTERNAL_DES_Routine
	DES_INTERNAL_Store_Data
	DES_INTERNAL_Epilog


// ----------
// This routine does a single DES decryption.
//
// Prototype:
//    void DES_Decrypt(uint8_t * ciphertext, uint8_t * plaintext, uint8_t * key);
//
// Input:
//    - R25:R24 - pointer to ciphertext buffer.
//    - R23:R22 - pointer to plaintext buffer.
//    - R21:R20 - pointer to key buffer.
//
// Register usage during DES_Decrypt:
//
// During execution:
//   - R31:R30 (Z) is used for misc memory pointing and is not preserved.
//   - R15 - R8 contains current key being processed.
//   - R7  - R0 contains the data (plaintext or ciphertext).
// ----------
.global DES_Decrypt
DES_Decrypt:
	DES_INTERNAL_Prolog
	DES_INTERNAL_Load_Data
	DES_INTERNAL_LoadKey1
	seh
	DES_INTERNAL_DES_Routine
	DES_INTERNAL_Store_Data
	DES_INTERNAL_Epilog


// ----------
// This routine does a triple DES encryption.
//
// Prototype:
//    void DES_3DES_Encrypt(uint8_t * plaintext, uint8_t * ciphertext, uint8_t * keys);
//
// Input:
//    - R25:R24 - pointer to plaintext buffer.
//    - R23:R22 - pointer to ciphertext buffer.
//    - R21:R20 - pointer to key buffer.
//
// Register usage during DES_3DES_Encrypt:
//
// During execution:
//   - R31:R30 (Z) is used for misc memory pointing and is not preserved.
//   - R15 - R8 contains current key being processed.
//   - R7  - R0 contains the data (plaintext or ciphertext).
// ----------
.global DES_3DES_Encrypt
DES_3DES_Encrypt:
	DES_INTERNAL_Prolog
	DES_INTERNAL_Load_Data
	DES_INTERNAL_LoadKey1
	clh
	DES_INTERNAL_DES_Routine
	DES_INTERNAL_LoadKey2
	seh
	DES_INTERNAL_DES_Routine
	DES_INTERNAL_LoadKey3
	clh
	DES_INTERNAL_DES_Routine
	DES_INTERNAL_Store_Data
	DES_INTERNAL_Epilog


// ----------
// This routine does a triple DES decryption.
//
// Prototype:
//    void DES_3DES_Decrypt(uint8_t * ciphertext, uint8_t * plaintext, uint8_t * keys);
//
// Input:
//    - R25:R24 - pointer to ciphertext buffer.
//    - R23:R22 - pointer to plaintext buffer.
//    - R21:R20 - pointer to key buffer.
//
// Register usage during DES_3DES_Decrypt:
//
// During execution:
//   - R31:R30 (Z) is used for misc memory pointing and is not preserved.
//   - R15 - R8 contains current key being processed.
//   - R7  - R0 contains the data (plaintext or ciphertext).
// ----------
.global DES_3DES_Decrypt
DES_3DES_Decrypt:
	DES_INTERNAL_Prolog
	DES_INTERNAL_Load_Data
	DES_INTERNAL_LoadKey3
	seh
	DES_INTERNAL_DES_Routine
	DES_INTERNAL_LoadKey2
	clh
	DES_INTERNAL_DES_Routine
	DES_INTERNAL_LoadKey1
	seh
	DES_INTERNAL_DES_Routine
	DES_INTERNAL_Store_Data
	DES_INTERNAL_Epilog


// ----------
// This routine does cipher block chaining encoding using DES.
// The bool triple_DES decide if single DES or triple DES is used.
// The variable block_length decide the number of blocks to be encoded.
//
// Prototype:
//    void DES_CBC_Encrypt(uint8_t * plaintext, uint8_t * ciphertext,
//                         uint8_t * keys, uint8_t * init,
//                         bool triple_DES, uint16_t block_length);
//
// Input:
//    - R25:R24 - pointer to plaintext buffer.
//    - R23:R22 - pointer to ciphertext buffer.
//    - R21:R20 - pointer to key buffer.
//    - R19:R18 - pointer to initial vector (IV).
//    - R17:R16 - variable holding triple_DES bool.
//    - R15:R14 - variable holding block_length.
//
// Register usage during DES_CBC_Encrypt:
//
// During execution:
//   - R31:R30 (Z) is used for misc memory pointing and is not preserved.
//   - R27:R26 (X) holding block_length variable (moved from R15:R14).
//   - R25:R24 points to the current position in the input buffer (plaintext)
//   - R23:R22 points to the current position in the output buffer (ciphertext)
//   - R21:R20 points to the key buffer
//   - R16 contains a variable that is non-zero for doing 3DES, zero for single DES.
//   - R15 - R8 contains current key being processed.
//   - R7  - R0 contains the data (plaintext or ciphertext).
// ----------
.global DES_CBC_Encrypt
DES_CBC_Encrypt:
	DES_INTERNAL_Prolog

	// Move R15:R14 to R27:R26 to save the block_length during DES.
	movw	r26, r14

	// Load the pointer to IV into the Z pointer, and load the IV to R15 - R8
	// to allow for XORing between the data and IV.
	movw	r30, r18
	DES_INTERNAL_Load_Into_R7_R0

DES_INTERNAL_CBC_Encrypt_Next:

	// Load the pointer to data into the Z pointer, and load the data to R15 - R8
	// to allow for XORing between the data and the last cipherblock (IV for first pass).
	movw	r30, r24
	DES_INTERNAL_Load_Into_R15_R8
	movw	r24, r30

	DES_INTERNAL_XOR_Routine

	DES_INTERNAL_LoadKey1
	clh
	DES_INTERNAL_DES_Routine

	// Test if register is zero, and if zero go to single encryption.
	tst	r16
	breq	DES_INTERNAL_CBC_Single_Encrypt

	DES_INTERNAL_LoadKey2
	seh
	DES_INTERNAL_DES_Routine

	DES_INTERNAL_LoadKey3
	clh
	DES_INTERNAL_DES_Routine

DES_INTERNAL_CBC_Single_Encrypt:
	// Store ciphertext and save the pointer for next ciphertext store.
	DES_INTERNAL_Store_Data
	movw	r22, r30
	
	// Subtract one block from the counter for each pass and go to end if zero.
	sbiw	r26, 1
	breq	DES_INTERNAL_CBC_Encrypt_End
	
	// Branching can't be done, because the code is too far away so we need
	// to do a jump to the encryption of the next block.
	rjmp	DES_INTERNAL_CBC_Encrypt_Next

DES_INTERNAL_CBC_Encrypt_End:
	DES_INTERNAL_Epilog


// ----------
// This routine does cipher block chaining decoding using DES.
// The bool triple_DES decide if single DES or triple DES is used.
// The variable block_length decide the number of blocks encoded.
//
// Prototype:
//    void DES_CBC_Decrypt(uint8_t * ciphertext, uint8_t * plaintext,
//                         uint8_t * keys, uint8_t * init,
//                         bool triple_DES, uint16_t block_length);
//
// Input:
//    - R25:R24 - pointer to ciphertext buffer.
//    - R23:R22 - pointer to plaintext buffer.
//    - R21:R20 - pointer to key buffer.
//    - R19:R18 - pointer to initial vector (IV).
//    - R17:R16 - variable holding triple_DES bool.
//    - R15:R14 - variable holding block_length.
//
// Register usage during DES_CBC_Decrypt:
//
// During execution:
//   - R31:R30 (Z) is used for misc memory pointing and is not preserved.
//   - R27:R26 (X) holding block_length variable (moved from R15:R14).
//   - R25:R24 points to the current position in the input buffer (ciphertext)
//   - R23:R22 points to the current position in the output buffer (plaintext)
//   - R21:R20 points to the key buffer
//   - R16 contains a variable that is non-zero for doing 3DES, zero for single DES.
//   - R15 - R8 contains current key being processed.
//   - R7  - R0 contains the data (plaintext or ciphertext).
// ----------
.global DES_CBC_Decrypt
DES_CBC_Decrypt:
	DES_INTERNAL_Prolog

	// Move R15:R14 to R27:26 to save the block_count during DES.
	movw	r26, r14

DES_INTERNAL_CBC_Decrypt_Next:

	// Load data and save the pointer for next data load.
	DES_INTERNAL_Load_Data
	movw	r24, r30

	// Test if register is zero, and if zero go to single decryption.
	tst	r16
	breq	DES_INTERNAL_CBC_Single_Decrypt

	DES_INTERNAL_LoadKey3
	seh
	DES_INTERNAL_DES_Routine

	DES_INTERNAL_LoadKey2
	clh
	DES_INTERNAL_DES_Routine

DES_INTERNAL_CBC_Single_Decrypt:
	DES_INTERNAL_LoadKey1
	seh
	DES_INTERNAL_DES_Routine

	// Load the initial vector in to Z pointer and load to R15 - R8.
	// The IV is loaded on the first pass only. For the next pass, this will be
	// the pointer to the previous cipher block.
	movw	r30, r18
	DES_INTERNAL_Load_Into_R15_R8

	// Save the pointer to the cipher block to the IV pointer and subtract 8 to
	// get the previous cipher block to be used in XOR operation. The cipher block
	// pointer is updated when loading in next cipher.
	movw	r18, r24
	subi	r18, 8
	sbci	r19, 0

	DES_INTERNAL_XOR_Routine

	// Store plaintext and save the pointer for next plaintext store.
	DES_INTERNAL_Store_Data
	movw	r22, r30
		
	// Subtract one block from the counter for each pass and go to end if zero.
	sbiw	r26, 1
	breq	DES_INTERNAL_CBC_Decrypt_End

	// Branching can't be done, because the code is too far away so we need
	// to do a jump to the decryption of the next block.
	rjmp	DES_INTERNAL_CBC_Decrypt_Next

DES_INTERNAL_CBC_Decrypt_End:
	DES_INTERNAL_Epilog
