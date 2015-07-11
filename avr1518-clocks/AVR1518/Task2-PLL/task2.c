/*************************************************************************
 *
 *
 *              Task2: Phase Locked Loop (PLL)
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
	LEDPORT.PIN0CTRL = PORT_INVEN_bm; // Invert input to turn leds on when port value is 1

	LEDPORT.DIRSET = 0xFF; 	// Set port as output
	LEDPORT.OUT = 0xA0;  // Set initial value

	unsigned int counter = 0;

	while (1)
	{

		/*  Change the clock source to the internal 2MHz. Disable PLL.
		 *  Configure PLL with the 2 MHz RC oscillator as source and
		 *  multiply by 15 to get 30 MHz PLL clock and enable it. Wait
		 *  for it to be stable and set prescaler C to divide by two
		 *  to set the CPU clock to 15 MHz
		 */
		if ( (SWITCHPORTL.IN & PIN0_bm) == 0 )
		{
			CLKSYS_Main_ClockSource_Select( CLK_SCLKSEL_RC2M_gc );

			CLKSYS_Disable( OSC_PLLEN_bm );

			CLKSYS_PLL_Config( OSC_PLLSRC_RC2M_gc, 15 );
			CLKSYS_Enable( OSC_PLLEN_bm );
			CLKSYS_Prescalers_Config( CLK_PSADIV_1_gc, CLK_PSBCDIV_1_2_gc );
			do {} while ( CLKSYS_IsReady( OSC_PLLRDY_bm ) == 0 );
			CLKSYS_Main_ClockSource_Select( CLK_SCLKSEL_PLL_gc );

			LEDPORT.OUTCLR = 0x1F;
			LEDPORT.OUTSET = PIN0_bm;
		}

		/*  Configure PLL with the 2 MHz RC oscillator as source and
		 *  multiply by 15 to get 30 MHz PLL clock and enable it. Wait
		 *  for it to be stable and set prescaler B and C to divide by two
		 *  to set the CPU clock to 7.5 MHz
		 */
		if ( (SWITCHPORTL.IN & PIN1_bm) == 0 )
		{
			CLKSYS_Main_ClockSource_Select( CLK_SCLKSEL_RC2M_gc );

			CLKSYS_Disable( OSC_PLLEN_bm );

			CLKSYS_PLL_Config( OSC_PLLSRC_RC2M_gc, 15 );
			CLKSYS_Enable( OSC_PLLEN_bm );
			CLKSYS_Prescalers_Config( CLK_PSADIV_1_gc, CLK_PSBCDIV_2_2_gc );
			do {} while ( CLKSYS_IsReady( OSC_PLLRDY_bm ) == 0 );
			CLKSYS_Main_ClockSource_Select( CLK_SCLKSEL_PLL_gc );

			LEDPORT.OUTCLR = 0x1F;
			LEDPORT.OUTSET = PIN1_bm;
		}


		/*  Configure PLL with the 2 MHz RC oscillator as source and
		 *  multiply by 6 to get 12 MHz PLL clock and enable it. Wait
		 *  for it to be stable and set prescaler B and C to divide by two
		 *  to set the CPU clock to 3 MHz
		 */
		if ( (SWITCHPORTL.IN & PIN2_bm) == 0 )
		{
			CLKSYS_Main_ClockSource_Select( CLK_SCLKSEL_RC2M_gc );

			CLKSYS_Disable( OSC_PLLEN_bm );

			CLKSYS_PLL_Config( OSC_PLLSRC_RC2M_gc, 6 );
			CLKSYS_Enable( OSC_PLLEN_bm );
			CLKSYS_Prescalers_Config( CLK_PSADIV_1_gc, CLK_PSBCDIV_2_2_gc );
			do {} while ( CLKSYS_IsReady( OSC_PLLRDY_bm ) == 0 );
			CLKSYS_Main_ClockSource_Select( CLK_SCLKSEL_PLL_gc );

			LEDPORT.OUTCLR = 0x1F;
			LEDPORT.OUTSET = PIN2_bm;
		}


		/*  Dynamically change prescaler:
		 *  Set prescaler A to divide by 2, prescaler B with no division,
		 *  and prescaler C to divide by 2. This will give the CPU a freqency
		 *  of 1/4th of the input freqency. Input freqency will depend on the
		 *  previous switch that has been pressed.
		 */
		if ( (SWITCHPORTL.IN & PIN3_bm) == 0 )
		{
			CLKSYS_Prescalers_Config( CLK_PSADIV_2_gc, CLK_PSBCDIV_1_2_gc );
			LEDPORT.OUTSET = PIN3_bm;
		}


		/*  
		 *  Add code here for generating 62 MHz to clkper4 and 15.5 MHz to the CPU
		 */
		if ( (SWITCHPORTL.IN & PIN4_bm) == 0 )
		{

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

