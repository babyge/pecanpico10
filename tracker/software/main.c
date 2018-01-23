#include "ch.h"
#include "hal.h"

#include "debug.h"
#include "threads.h"
#include "padc.h"
#include "usbcfg.h"
#include "shell.h"

static const ShellCommand commands[] = {
	{"debug", debugOnUSB},
	{"picture", printPicture},
	{"log", readLog},
	{"config", printConfig},
	{"command", command2Camera},
	{"aprs_message", send_aprs_message},
	{"test_rx", test_rx},
	{NULL, NULL}
};

static const ShellConfig shell_cfg = {
	(BaseSequentialStream*)&SDU1,
	commands
};

/**
  * Main routine is starting up system, runs the software watchdog (module monitoring), controls LEDs
  */
int main(void) {
	halInit();					// Startup HAL
	chSysInit();				// Startup RTOS

	// Init debugging (Serial debug port, LEDs)
	DEBUG_INIT();
	TRACE_INFO("MAIN > Startup");

	// Start USB
	#if ACTIVATE_USB
	sduObjectInit(&SDU1);
	sduStart(&SDU1, &serusbcfg);

	usbDisconnectBus(serusbcfg.usbp);
	chThdSleep(TIME_MS2I(100));
	usbStart(serusbcfg.usbp, &usbcfg);
	usbConnectBus(serusbcfg.usbp);
	usb_initialized = true;
	#endif

	// Startup threads
	start_essential_threads();	// Startup required modules (tracking managemer, watchdog)
	start_user_modules();		// Startup optional modules (eg. POSITION, LOG, ...)

	while(true) {
		#if ACTIVATE_USB
		if(SDU1.config->usbp->state == USB_ACTIVE) {
			thread_t *shelltp = chThdCreateFromHeap(NULL, THD_WORKING_AREA_SIZE(512), "shell", NORMALPRIO+1, shellThread, (void*)&shell_cfg);
			chThdWait(shelltp);
		}
		#endif
		chThdSleep(TIME_S2I(1));
	}
}

