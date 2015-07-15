/*************************************************************************
 *
 *
 *              Task4: Real Time Counter
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

#define RTC_HOURS_PER_DAY 24
#define RTC_MINUTES_PER_HOUR 60
#define RTC_SECONDS_PER_MINUTE 60
#define RTC_TICKS_PER_SECOND 10  // Change this if RTC prescaling changes.

volatile uint8_t	RTC_days;     // Holds day of week.
volatile uint8_t    RTC_hours;    // Holds hour of day.
volatile uint8_t    RTC_minutes;  // Holds minute of hour.
volatile uint8_t    RTC_seconds;  // Holds second of minute.
volatile uint8_t 	RTC_ticks;    // Holds tick count. Tick granularity depends on prescaler setting.


ISR( RTC_OVF_vect )
{
	// Count ticks or wrap back to zero.
	if (RTC_ticks < (RTC_TICKS_PER_SECOND - 1)) 
	{
		RTC_ticks++;
	} else 
	{
		RTC_ticks = 0;
		
		// Count seconds or wrap back to zero.
		if (RTC_seconds < (RTC_SECONDS_PER_MINUTE - 1)) 
		{
			RTC_seconds++;
		} else 
		{
			RTC_seconds = 0;
			
			// Count minutes or wrap back to zero.
			if (RTC_minutes < (RTC_MINUTES_PER_HOUR - 1)) 
			{
				RTC_minutes++;
			} else 
			{
				RTC_minutes = 0;
				
				// Count hours or wrap back to zero.
				if (RTC_hours < (RTC_HOURS_PER_DAY - 1)) 
				{
					RTC_hours++;
				} else 
				{
					RTC_hours = 0;
	
					// Count days
					RTC_days++;

					// Date processing could be added here
				}
			}
		}
	}
}

int main(void)
{
	// Configure switches: Enable pull-up to get a defined level on the switches
	// and invert the input to make the switches go high when pressed
	PORTCFG.MPCMASK = 0xFF; // Configure several PINxCTRL registers at the same time
	SWITCHPORTL.PIN0CTRL = (SWITCHPORTL.PIN0CTRL & ~PORT_OPC_gm) | PORT_OPC_PULLUP_gc | PORT_INVEN_bm; 

	SWITCHPORTL.DIRCLR = 0xFF; // Set port as input

	// Configure LEDs
	PORTCFG.MPCMASK = 0xFF; // Configure several PINxCTRL registers at the same time
	LEDPORT.PIN0CTRL = PORT_INVEN_bm; // Invert input to turn the leds on when port output value is 1

	LEDPORT.DIRSET = 0xFF; 	// Set port as output
	LEDPORT.OUT = 0xA0;  // Set initial value

	// Configure RTC
	// Set Internal 32kHz oscillator prescaled to 1kHz as source for RTC
	CLKSYS_RTC_ClockSource_Enable( CLK_RTCSRC_RCOSC_gc );
	// Enable Low level interrupts for the RTC Overflow
	RTC.INTCTRL = ( RTC.INTCTRL & ~RTC_OVFINTLVL_gm) | RTC_OVFINTLVL_LO_gc;

	// Enable low level interrupts in the programmable interrupt controller
	PMIC.CTRL |= PMIC_LOLVLEN_bm;

	// Set initial period for RTC and start the RTC with no prescaler
	RTC.PER = 100;
	RTC.CTRL |= RTC_PRESCALER_DIV1_gc;

	// Enable interrupts.
	sei();

	while (1)
	{
		switch ( SWITCHPORTL.IN ) 
		{
			case PIN0_bm:
				// Show 1/10th seconds
				LEDPORT.OUT = RTC_ticks;
				break;
			case PIN1_bm:
			default:
				// Show seconds
				LEDPORT.OUT = RTC_seconds;
				break;
			case PIN2_bm:
				// Show minutes
				LEDPORT.OUT = RTC_minutes;
				break;
			case PIN3_bm:
				// Show hours
				LEDPORT.OUT = RTC_hours;
				break;

		}
	}
}
