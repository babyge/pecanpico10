#include "ch.h"
#include "hal.h"
#include "pktconf.h"
//#include "console.h"
#include "debug.h"
#include "threads.h"

/**
  * Main routine is starting up system, runs the software watchdog (module monitoring), controls LEDs
  */
int main(void) {
	halInit();					// Startup HAL
	chSysInit();				// Startup RTOS
    /*
     * Setup packet system...
     * Create memory (CCM based heap).
     * - Most DSP related data is held in CCM.
     * - There is no DMA involved in DSP filtering/decoding.
     * Configure core IO/bus devices
     * Setup the debug output (UART, USB)
     * Configure radios.
     * Start packet event monitoring.
     * Start applications.
     */
    pktSystemInit();

	TRACE_INFO("MAIN > Loop active");
#if DISABLE_HW_WATCHDOG == TRUE
	TRACE_WARN("MAIN > *** Hardware watchdog is disabled ***");
#endif
	while(true) {
	  /* Trace events from packet decoder system. */
      pktTraceEvents();
      chThdSleep(TIME_MS2I(200));
	}
}

