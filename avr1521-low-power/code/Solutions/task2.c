/*************************************************************************
 *
 *
 *              Task2: Entering and exiting Power-save mode
 *
 *
 *************************************************************************/

// Include header files for GCC/IAR
#include "../avr_compiler.h"

// The board.h header file defines which IO ports peripherals like
// Switches and LEDs are connected to. The header file is configured
// for use with XMEGA-A1 Xplained.
#include "../board.h"

#define SW_TOSC PIN0_bm
#define SW_RCOSC PIN1_bm
#define SW_ULP PIN2_bm


// The SLEEP-instruction
#define sleep() __asm__ __volatile__ ("sleep")


void facilitatePowersaving(void);

int main(void)
{
	facilitatePowersaving();

	// Configure switches
	PORTCFG.MPCMASK = 0xff; // Configure several PINxCTRL registers at the same time
	SWITCHPORTL.PIN0CTRL = (SWITCHPORTL.PIN0CTRL & ~PORT_OPC_gm) | PORT_OPC_PULLUP_gc; //Enable pull-up to get a defined level on the switches
	SWITCHPORTL.DIRCLR = 0xff; // Set port as input

	// Set up interrupt on buttons
	SWITCHPORTL.INTCTRL = (SWITCHPORTL.INTCTRL & ~PORT_INT0LVL_gm) | PORT_INT0LVL_LO_gc;
	SWITCHPORTL.INT0MASK = 0x0F;

	// Configure LEDs
	PORTCFG.MPCMASK = 0xff; // Configure several PINxCTRL registers at the same time
	LEDPORT.PIN0CTRL = PORT_INVEN_bm; // Invert input to turn the leds on when port output value is 1

	LEDPORT.DIRSET = 0xff; 	// Set port as output
	LEDPORT.OUT = 0x01;  // Set initial value

	// Set up RTC timer and overflow interrupt
	CLK.RTCCTRL = (CLK.RTCCTRL & ~CLK_RTCSRC_gm) | CLK_RTCSRC_TOSC_gc; // Choose 1kHz input from external 32kHz oscillator as RTC source as this is most power-efficient.
	CLK.RTCCTRL	|= CLK_RTCEN_bm;	// Enable RTC
	
	// Now we have 1024 ticks per second.	
	// We set the period to 2048, resulting in 1Hz toggling of the LED.
	RTC.PER = 2048;
	RTC.CTRL = (RTC.CTRL & ~RTC_PRESCALER_gm) | RTC_PRESCALER_DIV1_gc; // Set RTC prescaler 1
	RTC.INTCTRL = (RTC.INTCTRL & ~RTC_COMPINTLVL_gm) | RTC_COMPINTLVL_LO_gc; // Enable LO interrupt on compare match
	
	// Set up sleep registers
	SLEEP.CTRL = (SLEEP.CTRL & ~SLEEP_SMODE_gm) | SLEEP_SMODE_PSAVE_gc;
	SLEEP.CTRL |= SLEEP_SEN_bm; // Enable sleep, else SLEEP-instruction is ineffective.


	// Enable low interrupt level in PMIC and enable global interrupts.
	PMIC.CTRL |= PMIC_LOLVLEN_bm;
	sei();

	// Main loop.
	char pressed = 0;
	while (1) {

// == Button 0 pressed
		if (((SWITCHPORTL.IN & SW_TOSC) | pressed) == 0x00) {		
			_delay_ms(5); // Debounce switch
			pressed = 1; // Used for toggling
			LEDPORT.OUT = SW_TOSC; // Toggle led
			
			CLK.RTCCTRL = (CLK.RTCCTRL & ~CLK_RTCSRC_gm) | CLK_RTCSRC_TOSC_gc;

// == Button 1 pressed
		} else if (((SWITCHPORTL.IN & SW_RCOSC) | pressed) == 0x00) {	
			_delay_ms(5); // Debounce switch
			pressed = 1; // Used for toggling
			LEDPORT.OUT = SW_RCOSC; // Toggle led
			
			CLK.RTCCTRL = (CLK.RTCCTRL & ~CLK_RTCSRC_gm) | CLK_RTCSRC_RCOSC_gc;

 // == Button 2 pressed
		} else if (((SWITCHPORTL.IN & SW_ULP) | pressed) == 0x00) {
			_delay_ms(5); // Debounce switch
			pressed = 1; // Used for toggling.
			LEDPORT.OUT = SW_ULP; // Toggle led		

			CLK.RTCCTRL = (CLK.RTCCTRL & ~CLK_RTCSRC_gm) | CLK_RTCSRC_ULP_gc;

// == Buttons released
		} else if ( (SWITCHPORTL.IN & (SW_TOSC | SW_RCOSC | SW_ULP )) == 0x07) {
			pressed = 0;
			LEDPORT.OUTCLR = 0x0F;
			sleep();
		}


	}

}

ISR(RTC_COMP_vect) {
	LEDPORT.OUTTGL = 0xF0;
}

ISR(SWITCHPORT_INT0_vect) {
	// Empty
	asm("nop");
}

void facilitatePowersaving(void) {
	// Pull-up on all ports, to ensure a defined state.
	PORTCFG.MPCMASK = 0xFF;
	PORTA.PIN0CTRL = PORT_OPC_PULLUP_gc;
	PORTCFG.MPCMASK = 0xFF;
	PORTB.PIN0CTRL = PORT_OPC_PULLUP_gc;
	PORTCFG.MPCMASK = 0xFF;
	PORTC.PIN0CTRL = PORT_OPC_PULLUP_gc;
	PORTCFG.MPCMASK = 0xFF;
	PORTD.PIN0CTRL = PORT_OPC_PULLUP_gc;
	PORTCFG.MPCMASK = 0xFF;
	PORTE.PIN0CTRL = PORT_OPC_PULLUP_gc;
	PORTCFG.MPCMASK = 0xFF;
	PORTF.PIN0CTRL = PORT_OPC_PULLUP_gc;
	PORTCFG.MPCMASK = 0xFF;
	PORTH.PIN0CTRL = PORT_OPC_PULLUP_gc;
	PORTCFG.MPCMASK = 0xFF;
	PORTJ.PIN0CTRL = PORT_OPC_PULLUP_gc;
	PORTCFG.MPCMASK = 0xFF;
	PORTK.PIN0CTRL = PORT_OPC_PULLUP_gc;
	PORTCFG.MPCMASK = 0xFF;
	PORTQ.PIN0CTRL = PORT_OPC_PULLUP_gc;
	PORTCFG.MPCMASK = 0xFF;
	PORTR.PIN0CTRL = PORT_OPC_PULLUP_gc;

	// Because of the audio amplifier, we have to define pull-down on PQ3
	// NTC is already disabled via pull-up
	PORTQ.PIN3CTRL = PORT_OPC_PULLDOWN_gc;

	// The potentiometer has an analog voltage, thus both pull-up and pull-down in tristate will source current. ~35µA.
	PORTB.DIRSET = PIN1_bm;

}
