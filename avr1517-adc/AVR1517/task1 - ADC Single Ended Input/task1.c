/*
 *                     Task 1
 *
 * In this task we will use the Virtual Channel 0 (CH0),
 * (do not worry about Virtual Channel's yet as they
 * will be covered in task3.) To the Virtual Channel 0,
 * we will attach and convert a Light sensor from the 
 * Xmega-A1-Xplained board.
 *
 */

// The board.h header file defines which IO ports pheripherals like
// Switches and LEDs are connected to.
#include "../board.h"

// Setup for different compilers
#include "../avr_compiler.h"

// ADC Drivers
#include "../adc_driver.h"

// Defines what you want to connect to the ADC.
// Light sensor (XMEGA-A1-Xplained)
#define ADC_INPUT ADC_CH_MUXPOS_PIN1_gc

//NTC 
//#define ADC_INPUT ADC_CH_MUXPOS_PIN0_gc

volatile uint16_t ADC_result;

int main( void )
{
	// Variable for use when we read the result from an ADC channel
	int8_t offset;

	LEDPORT.DIR = 0xFF; //Set as ouput, the 8 MSB of the result is output here
	LEDPORT.OUT = 0xFF; //Default off for LED
	
	ntc_enable(); //Pulls a pin low to enable the NTC.

	/* Move stored calibration values to ADC B */
	ADC_CalibrationValues_Load(&ADCB);

	/* Set up ADC B to have signed conversion mode and 12 bit resolution. */
	ADC_ConvMode_and_Resolution_Config(&ADCB, ADC_ConvMode_Signed, ADC_RESOLUTION_12BIT_gc);

	// The ADC has different voltage reference options, controlled by the REFSEL bits in the
	// REFCTRL register. Here the internal reference is selected
	ADC_Reference_Config(&ADCB, ADC_REFSEL_VCC_gc);

	// The clock into the ADC decide the maximum sample rate and the conversion time, and
	// this is controlled by the PRESCALER bits in the PRESCALER register. Here, the
	// Peripheral Clock is divided by 8 ( gives 250 KSPS with 2Mhz clock )
	ADC_Prescaler_Config(&ADCB, ADC_PRESCALER_DIV8_gc);

	// The used Virtual Channel (CH0) must be set in the correct mode
	// In this task we will use single ended input, so this mode is selected 

	/* Setup channel 0 to have single ended input. */
	ADC_Ch_InputMode_and_Gain_Config(&ADCB.CH0,
	                                 ADC_CH_INPUTMODE_SINGLEENDED_gc,
	                                 ADC_CH_GAIN_1X_gc);


	// Setting up the which pins to convert.
	// Note that the negative pin is internally connected to ground
	ADC_Ch_InputMux_Config(&ADCB.CH0, ADC_INPUT, ADC_CH_MUXNEG_PIN0_gc);

	// Before the ADC can be used it must be enabled
	ADC_Enable(&ADCB);

	// Wait until the ADC is ready
	ADC_Wait_8MHz(&ADCB);

	// In the while(1) loop, a conversion is started on CH0 and the 8 MSB of the result is
	// ouput on the LEDPORT when the conversion is done

	/* Get offset value for ADC B.  */
	offset = ADC_Offset_Get_Signed(&ADCB, &(ADCB.CH0), true);
	
	//Read the ADC forever...
	while (1) {
		// Start a single conversion
		ADC_Ch_Conversion_Start(&ADCB.CH0);

		// Wait for the conversion to complete
		while(!ADC_Ch_Conversion_Complete(&ADCB.CH0));
		
		// Get the result
		ADC_result =   ADC_ResultCh_GetWord_Signed(&ADCB.CH0, offset);     
		
		// The result is a 12 bit value, 4 rightshifts maps MSB to LED7
		// invert as the LED is on when pin is low
		LEDPORT.OUT = ~(ADC_result >> 4);
	}
}

