/*************************************************************************
 *
 *
 *              Task1: Dynamic clock switching
 *
 *
 *************************************************************************/

// The board.h header file defines which IO ports pheripherals like
// Switches and LEDs are connected to.
#include "../board.h"

// Include header files for GCC/IAR
#include "../avr_compiler.h"

// Include Clock system driver from application note AVR1003
#include "../clksys_driver.h"

int main(void)
{
	// Configure switches
	PORTCFG.MPCMASK = 0xFF; // Configure several PINxCTRL registers at the same time
	SWITCHPORTL.PIN0CTRL = (SWITCHPORTL.PIN0CTRL & ~PORT_OPC_gm) | PORT_OPC_PULLUP_gc; //Enable pull-up to get a defined level on the switches

	SWITCHPORTL.DIRCLR = 0xFF; // Set port as input

	// Configure LEDs
	PORTCFG.MPCMASK = 0xFF; // Configure several PINxCTRL registers at the same time
	LEDPORT.PIN0CTRL = PORT_INVEN_bm; // Invert input to turn the leds on when port output value is 1

	LEDPORT.DIRSET = 0xFF; 	// Set port as output
	LEDPORT.OUT = 0xA1;  // Set initial value

	unsigned int counter = 0;

	while (1)
	{

		/*  Enable internal 2 MHz rc oscillator and wait until it's
		 *  stable. Set the 2 MHz rc oscillator as the main clock source.
		 *  Then disable other oscillators.
		 */
		if ( (SWITCHPORTL.IN & PIN0_bm) == 0 )
		{
			CLKSYS_Enable( OSC_RC2MEN_bm );
			do {} while ( CLKSYS_IsReady( OSC_RC2MRDY_bm ) == 0 );
			CLKSYS_Main_ClockSource_Select( CLK_SCLKSEL_RC2M_gc );
			CLKSYS_Disable( OSC_RC32MEN_bm | OSC_RC32KEN_bm );

			LEDPORT.OUTCLR = PIN0_bm | PIN1_bm | PIN2_bm;
			LEDPORT.OUTSET = PIN0_bm;
		}

		/*  Enable internal 32 MHz ring oscillator and wait until it's
		 *  stable. Set the 32 MHz ring oscillator as the main clock source. 
		 *  Then disable the other oscillators.
		 */
		if ( (SWITCHPORTL.IN & PIN1_bm) == 0 )
		{
			CLKSYS_Enable( OSC_RC32MEN_bm );
			do {} while ( CLKSYS_IsReady( OSC_RC32MRDY_bm ) == 0 );
			CLKSYS_Main_ClockSource_Select( CLK_SCLKSEL_RC32M_gc );
			CLKSYS_Disable( OSC_RC2MEN_bm | OSC_RC32KEN_bm );


			LEDPORT.OUTCLR = PIN0_bm | PIN1_bm | PIN2_bm;
			LEDPORT.OUTSET = PIN1_bm;
		}

		/*  Enable internal 32 kHz calibrated oscillator and check for
		 *  it to be stable. Set the 32 kHz oscillator as the main clock source. 
		 *  Then disable the other oscillators.
		 */
		if ( (SWITCHPORTL.IN & PIN2_bm) == 0 )
		{
			CLKSYS_Enable( OSC_RC32KEN_bm );
			do {} while ( CLKSYS_IsReady( OSC_RC32KRDY_bm ) == 0 );
			CLKSYS_Main_ClockSource_Select( CLK_SCLKSEL_RC32K_gc );
			CLKSYS_Disable( OSC_RC2MEN_bm | OSC_RC32MEN_bm );

			LEDPORT.OUTCLR = PIN0_bm | PIN1_bm | PIN2_bm;
			LEDPORT.OUTSET = PIN2_bm;
		}

		// Blink LEDS after the loop has been run several times
		// This gives us visual feedback on the MCU speed
		counter++;
		if (counter > 20000 )
		{
			LEDPORT.OUTTGL = 0xF0;
			counter=0;
		}


	}
}
