/****************************************************************************
 *           
 *                           -= XMEGA USART Task 1 =-
 *
 *        Task 1: USART in polling mode
 *
 *        The application sends and receives TEST_CHARS bytes, in a loop-
 *        back connection. Latch the TX(PD3) to RX(PD2) and start having fun.
 *
 *                                 -= XMEGA =-
 *
 *****************************************************************************/

#include <avr/io.h>
#include <stdbool.h>

// The board.h header file defines which IO ports pheripherals like
// Switches and LEDs are connected to.
#include "../board.h"

#define BSCALE_VALUE  0
#define BSEL_VALUE   12

#define TEST_CHARS  100


/*  Example USART application.
 *
 *  This example configures USARTx0 with the parameters:
 *      - 8 bit character size
 *      - No parity
 *      - 1 stop bit
 *      - 9600 Baud
 *
 *  This function will send the values 0-100 and test if the received data is
 *  equal to the sent data. The code can be tested by connecting PF3 to PF2 which
 *  is RXD and TXD on the header J1 on the XMEGA-A1 Xplained board. If
 *  the variable 'TransferError' is false and LEDs are lit up at the end of
 *  the function, the bytes have been successfully sent and received by USARTx0.
 */

int main(void)
{
  
	LEDPORT.DIR = 0xFF;
	LEDPORT.OUT = 0xFF;

	uint8_t i;

	char Tx_Buf[TEST_CHARS];
	char Rx_Buf[TEST_CHARS];

	bool TransferError = false;

	USART_PORT.DIRSET   = PIN3_bm;   // Pin 3 (TX) as output.
	USART_PORT.DIRCLR   = PIN2_bm;   // Pin 2 (RX) as input.

	// USARTx0, 8 Data bits, No Parity, 1 Stop bit
	USART.CTRLC = (uint8_t) USART_CHSIZE_8BIT_gc | USART_PMODE_DISABLED_gc | false;

   /* Target: Internal RC 2MHz (default) 
    * Example (9600bps)  :   - I/O clock = 2MHz
 	*                        - 9600bps can be acheived by 9600bps / 2^0
 	*                        - UBRR = 2MHz/(16*9600)-1 = 12.02
 	*                        - ScaleFactor = 0
	*/
	
	USART.BAUDCTRLA = BSEL_VALUE;

	// Enable both RX and TX
	USART.CTRLB |= USART_RXEN_bm;
	USART.CTRLB |= USART_TXEN_bm;

	i = 0;

    while(i != TEST_CHARS)
    {
        Tx_Buf[i] = i;
        // Transmit a character, first wait for the previous to be sent
        while( (USART.STATUS & USART_DREIF_bm) == 0 ) {}
       	  // Now, transmit the character
        USART.DATA = Tx_Buf[i];      
	
        // Wait until the data is received
        while( (USART.STATUS & USART_RXCIF_bm) == 0 ) {}
         
        // Read out the received data
        Rx_Buf[i] = USART.DATA;

        if(Rx_Buf[i] != Tx_Buf[i]) 
        {
            TransferError = true;
        }
	  
        i++;
    }

    while(1)
    {
  	    if(TransferError)
	    {
           // No light
            LEDPORT.OUT = 0xFF;
	    }
	    else
	    {  
            // Light          
            LEDPORT.OUT = 0x00;
	    }
    }
}
