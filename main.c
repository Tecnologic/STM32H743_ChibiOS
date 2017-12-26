/*
    ChibiOS - Copyright (C) 2006..2016 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
 */

#include "ch.h"
#include "hal.h"
#include "chprintf.h"
#include "rt_test_root.h"
#include "oslib_test_root.h"

void callback (GPTDriver *gptp);

/*
 * GPT4 configuration. This timer is used as trigger for the ADC.
 */
static const GPTConfig gptcfg = {
		.frequency =  1000000U,
		.callback  =  callback,
		.cr2       =  0,  /* MMS = 010 = TRGO on Update Event.        */
		.dier      =  0U
};


/*
 * This is a periodic thread that does absolutely nothing except flashing
 * a LED.
 */
static THD_WORKING_AREA(waThread1, 128);
static THD_FUNCTION(Thread1, arg) {

	(void)arg;
	chRegSetThreadName("blinker");
	while (true) {
		//    palSetLine(LINE_LED2);
		//    palSetLine(LINE_LED3);
		//    chThdSleepMilliseconds(500);
		//    palClearLine(LINE_LED2);
		//    palClearLine(LINE_LED3);
		chThdSleepMilliseconds(500);
	}
}

void callback (GPTDriver *gptp)
{
	(void)gptp;
	static int32_t cnt = 0;
	static int32_t duty = 0;
	static int32_t sign = 1;
	cnt++;

	if(cnt > 100)
	{
		cnt = 0;
		duty += sign;

		if(duty > 100) sign = -1;
		if(duty < 0) sign = 1;
	}

	if(duty > cnt) palSetLine(LINE_LED1);
	else palClearLine(LINE_LED1);
}


static void pwmpcb(PWMDriver *pwmp) {

	(void)pwmp;
}

static void pwmc1cb(PWMDriver *pwmp) {

	(void)pwmp;
}

static PWMConfig pwmcfg = {
		10000,                                    /* 10kHz PWM clock frequency.   */
		10000,                                    /* Initial PWM period 1S.       */
		pwmpcb,
		{
				{PWM_OUTPUT_ACTIVE_HIGH, pwmc1cb},
				{PWM_OUTPUT_DISABLED, NULL},
				{PWM_OUTPUT_DISABLED, NULL},
				{PWM_OUTPUT_DISABLED, NULL}
		},
		0,
		0
};

/*===========================================================================*/
/* ICU driver related.                                                       */
/*===========================================================================*/

icucnt_t last_width, last_period;

static void icuwidthcb(ICUDriver *icup) {

	palSetLine(LINE_ARD_D13);
	last_width = icuGetWidthX(icup);
}

static void icuperiodcb(ICUDriver *icup) {

	palClearLine(LINE_ARD_D13);
	last_period = icuGetPeriodX(icup);
}

static void icuovfcb(ICUDriver *icup) {

	(void)icup;
}

static ICUConfig icucfg = {
		ICU_INPUT_ACTIVE_HIGH,
		10000,                                    /* 10kHz ICU clock frequency.   */
		icuwidthcb,
		icuperiodcb,
		icuovfcb,
		ICU_CHANNEL_1,
		0
};


/*
 * Application entry point.
 */
int main(void) {
	uint32_t duty = 1000;

	/*
	 * System initializations.
	 * - HAL initialization, this also initializes the configured device drivers
	 *   and performs the board-specific initializations.
	 * - Kernel initialization, the main() function becomes a thread and the
	 *   RTOS is active.
	 */
	halInit();
	chSysInit();

	/*
	 * Starting GPT1 driver, it is used for triggering the ADC.
	 */
	gptStart(&GPTD1, &gptcfg);
	gptStartContinuous(&GPTD1, 50);

	/*
	 * Activates the serial driver 1 using the driver default configuration.
	 */
	sdStart(&SD3, NULL);

	/*
	 * Starting PWM driver 3 and enabling the notifications.
	 * GPIOB0 is programmed as PWM output (channel 3 of TIM3).
	 */
	pwmStart(&PWMD3, &pwmcfg);
	pwmEnablePeriodicNotification(&PWMD3);
	palSetLineMode(LINE_TIM3_CH3, PAL_MODE_ALTERNATE(2));

	/*
	 * Starting ICU driver 2.
	 * GPIOA0 is programmed as ICU input (channel 1 of TIM2).
	 */
	icuStart(&ICUD2, &icucfg);
	palSetLineMode(LINE_TIM2_CH1, PAL_MODE_ALTERNATE(1));

	/*
	 * Starting ICU capture and enabling the notifications.
	 */
	icuStartCapture(&ICUD2);
	icuEnableNotifications(&ICUD2);

	/*
	 * Changes the PWM channel 3 to 25% duty cycle.
	 */
	pwmEnableChannel(&PWMD3, 3, PWM_PERCENTAGE_TO_WIDTH(&PWMD3, 2500));

	/*
	 * Creates the example thread.
	 */
	chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO+1, Thread1, NULL);

	/*
	 * Normal main() thread activity, in this demo it does nothing except
	 * sleeping in a loop and check the button state.
	 */
	while (1) {
		pwmEnableChannel(&PWMD3, 3, PWM_PERCENTAGE_TO_WIDTH(&PWMD3, duty));
		duty += 500;
		if(duty < 10000) duty = 500;

		if (palReadLine(LINE_BUTTON)) {
			test_execute((BaseSequentialStream *)&SD3, &rt_test_suite);
			test_execute((BaseSequentialStream *)&SD3, &oslib_test_suite);
		}
		chprintf((BaseSequentialStream *)&SD3, "ICU duty: %d, period: %d\r\n", last_width, last_period);
		chThdSleepMilliseconds(500);
	}
}
