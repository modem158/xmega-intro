#include "avr_compiler.h"
#include "vbat.h"

/**
 * \brief This function resets the battery backup module.
 *
 * This function will set the access enable bit for the control register.
 */
void vbat_reset(void)
{
	// Enable R/W access to the battery backup module.
	VBAT.CTRL = VBAT_ACCEN_bm;

	// Reset the module. (Reset bit is protected by CCP.)
	ENTER_CRITICAL_REGION();
	CCP = 0xD8;
	VBAT.CTRL = VBAT_RESET_bm;
	LEAVE_CRITICAL_REGION();
}

/**
 * \brief This function starts the crystal oscillator with 1 or 1024 Hz
 *  clock output.
 *
 *  \param use1khz Boolean for selecting 1 kHz or 1 Hz RTC clock rate.
 */
void vbat_enable_xosc(bool use1khz)
{
	// Enable the failure detection.
	VBAT.CTRL |= VBAT_XOSCFDEN_bm;

	/* A delay is needed to give the voltage in the backup system some time
	 * to stabilise.
	 */
	delay_us(200);

	// Enable oscillator, with 1 kHz or 1 Hz output.
	if (use1khz)
		VBAT.CTRL |= VBAT_XOSCEN_bm | VBAT_XOSCSEL_bm;
	else
		VBAT.CTRL |= VBAT_XOSCEN_bm;
}

uint8_t vbat_system_check(bool first_time_startup)
{
	uint8_t vbat_status;
	/*
	 * Check if sufficient power was detected on the VBAT input. The brown-
	 * out detector (BBBOD) will be sampled once when the device starts up
	 * and the result is visible as the BBPWR flag.
	 */
	if (VBAT.STATUS & VBAT_BBPWR_bm)
		vbat_status = VBAT_STATUS_NO_POWER;
	else {
		/*
		 * We hav sufficient power, now we check if a power-on-reset
		 * (BBPOR) was detected on VBAT. This is visible from the BBPORF
		 * flag which is also only updated once when the device starts.
		 */
		if (VBAT.STATUS & VBAT_BBPORF_bm) {
			if (first_time_startup)
				vbat_status = VBAT_STATUS_INIT;
			else
				vbat_status = VBAT_STATUS_BBPOR;
		}
		else if (VBAT.STATUS & VBAT_BBBORF_bm)
			vbat_status = VBAT_STATUS_BBBOD;
		else {
			VBAT.CTRL = VBAT_ACCEN_bm;
			if (VBAT.STATUS & VBAT_XOSCFAIL_bm)
				vbat_status = VBAT_STATUS_XOSCFAIL;
			else
				vbat_status = VBAT_STATUS_OK;
		}
	}
	return vbat_status;
}
