/*************************************************************************
 *
 *
 *              Task3: Runtime calibration with DFLL
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
	LEDPORT.OUT = 0xA0;  // Set initial value

	unsigned int counter = 0;

	// Enable the 32KHz internal oscillator which is used as reference for the DFLL
	CLKSYS_Enable( OSC_RC32KEN_bm );
	do {} while ( CLKSYS_IsReady( OSC_RC32KRDY_bm ) == 0 );
	
	while (1)
	{

		/*  Enable automatic calibration of the internal 2MHz oscillator.
		 *  We do not use the external crystal connected to the TOSC pins,
		 *  but the internal 32kHz oscillator as reference.
		 */
		if ( (SWITCHPORTL.IN & PIN0_bm) == 0 )
		{
			CLKSYS_AutoCalibration_Enable( OSC_RC2MCREF_bm, false );

		/*  Due to an errata on ATxmega128A1 we need to enable the 32MHz internal RC oscillator
		 *  and the Autocalibration for the 32MHz internal RC oscillator in order for the calibration
		 *  to work correctly on the 2 MHz internal RC oscillator*/

		 	CLKSYS_Enable( OSC_RC32MEN_bm );
			do {} while ( CLKSYS_IsReady( OSC_RC32MRDY_bm ) == 0 );
			CLKSYS_AutoCalibration_Enable( OSC_RC32MCREF_bm, false );

			LEDPORT.OUTCLR = PIN1_bm;
			LEDPORT.OUTSET = PIN0_bm;
		}

		/*  Disable the automatic calibration of the internal 2MHz oscillator.
		 */
		if ( (SWITCHPORTL.IN & PIN1_bm) == 0 )
		{
			CLKSYS_AutoCalibration_Disable( DFLLRC2M );

			
			//Disable the auto calibration of the 32MHz internal RC oscillator and the 32MHz internal RC oscillator
			CLKSYS_AutoCalibration_Disable( DFLLRC32M );
			CLKSYS_Disable( OSC_RC32MEN_bm );

			LEDPORT.OUTCLR = PIN0_bm;
			LEDPORT.OUTSET = PIN1_bm;
		}



		// Blink LEDS after the loop has been run several times
		// This gives us visual feedback on the MCU speed
		counter++;
		if (counter > 20000 )
		{
			LEDPORT.OUTTGL = 0xE0;
			counter=0;
		}


	}
}
