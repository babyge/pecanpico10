#include "ch.h"
#include "hal.h"
#include "debug.h"
#include "portab.h"
#include "usbcfg2.h"
#include "console.h"

mutex_t debug_mtx; // Used internal to synchronize multiple chprintf in debug.h

char error_list[ERROR_LIST_SIZE][ERROR_LIST_LENGTH];
uint8_t error_counter;

/*static const SerialConfig debug_config = {
	115200,
	0,
	0,
	0
};*/

#ifdef SET_TRACE_LEVEL
uint8_t current_trace_level = SET_TRACE_LEVEL; // Set in makefile UDEFS
#else
uint8_t current_trace_level = 5; // Level: All
#endif


void pktConfigureSerialIO(void) {
	chMtxObjectInit(&debug_mtx);
    pktSerialStart();

    /* Init and start USB. */
     usbObjectInit(&USBD1);

     usbStart(&USBD1, &usbcfg);

     /* Init serial over USB. */
     sduObjectInit(&SDU1);
     sduObjectInit(&SDU2);

     /* Start serial over USB. */
     sduStart(&SDU1, &serusbcfg1);
     sduStart(&SDU2, &serusbcfg2);

     chThdSleep(TIME_MS2I(100));

     /* Signal soft disconnect to the host (DP pull-up disconnected). */
     usbDisconnectBus(&USBD1);

     chThdSleep(TIME_MS2I(1500));

     /*
      * Notify host we are here.
      * If the cable is connected the host should enumerate USB.
      */
     usbConnectBus(&USBD1);
}

void debug_print(char *type, char* filename, uint32_t line, char* format, ...)
{
	chMtxLock(&debug_mtx);

/*	uint8_t str[256];

	va_list args;
	va_start(args, format);
	chsnprintf((char*)str, sizeof(str), format, args);
	va_end(args);*/


	if(isConsoleOutputAvailable()) {
      if(TRACE_TIME) {
          chprintf((BaseSequentialStream*)&SERIAL_CONSOLE_DRIVER, "[%8d.%03d]", chVTGetSystemTime()/CH_CFG_ST_FREQUENCY, (chVTGetSystemTime()*1000/CH_CFG_ST_FREQUENCY)%1000);
      }
      chprintf((BaseSequentialStream*)&SERIAL_CONSOLE_DRIVER, "[%s]", type);
      if(TRACE_FILE) {
          chprintf((BaseSequentialStream*)&SERIAL_CONSOLE_DRIVER, "[%12s %04d]", filename, line);
      }
      if(TRACE_THREAD && TRACE_TIME && TRACE_FILE) {
          chprintf((BaseSequentialStream*)&SERIAL_CONSOLE_DRIVER, "[0x%08x]", chThdGetSelfX());
      }
      chprintf((BaseSequentialStream*)&SERIAL_CONSOLE_DRIVER, " ");
      va_list args;
      va_start(args, format);
      chvprintf((BaseSequentialStream*)&SERIAL_CONSOLE_DRIVER, format, args);
      va_end(args);
      chprintf((BaseSequentialStream*)&SERIAL_CONSOLE_DRIVER, "\r\n");
	}

#if ENABLE_SERIAL_DEBUG == TRUE
	if(TRACE_TIME) {
		chprintf((BaseSequentialStream*)&SERIAL_DEBUG_DRIVER, "[%8d.%03d]", chVTGetSystemTime()/CH_CFG_ST_FREQUENCY, (chVTGetSystemTime()*1000/CH_CFG_ST_FREQUENCY)%1000);
	}
	chprintf((BaseSequentialStream*)&SERIAL_DEBUG_DRIVER, "[%s]", type);
	if(TRACE_FILE) {
		chprintf((BaseSequentialStream*)&SERIAL_DEBUG_DRIVER, "[%12s %04d]", filename, line);
	}
    if(TRACE_THREAD && TRACE_TIME && TRACE_FILE) {
        chprintf((BaseSequentialStream*)&SERIAL_DEBUG_DRIVER, "[0x%08x]", chThdGetSelfX());
    }
    chprintf((BaseSequentialStream*)&SERIAL_DEBUG_DRIVER, " ");
    va_list args;
    va_start(args, format);
	chvprintf((BaseSequentialStream*)&SERIAL_DEBUG_DRIVER, format, args);
    va_end(args);
    chprintf((BaseSequentialStream*)&SERIAL_DEBUG_DRIVER, "\r\n");
#endif

	chMtxUnlock(&debug_mtx);
}
