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

/*
 * Application entry point.
 */
int main(void) {

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
   * Creates the example thread.
   */
  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO+1, Thread1, NULL);

  /*
   * Normal main() thread activity, in this demo it does nothing except
   * sleeping in a loop and check the button state.
   */
  while (1) {
    if (palReadLine(LINE_BUTTON)) {
      test_execute((BaseSequentialStream *)&SD3, &rt_test_suite);
      test_execute((BaseSequentialStream *)&SD3, &oslib_test_suite);
    }
    chThdSleepMilliseconds(500);
  }
}
