/*
 * time.c
 *
 *  Created on: Jan 22, 2021
 *      Author: TURCAR
 */

#include "main.h"
#include "rtc.h"

#include <time.h>

int _gettimeofday(struct timeval *tv, void *tzvp)
{
#if RTC_DISABLED
	uint32_t tick_ms = HAL_GetTick();

	tv->tv_sec = tick_ms / 1000;
	tv->tv_usec = (tv->tv_sec - tick_ms / 1000) / 1000;
#else
	if(tv) *tv = rtc_timeval();
#endif

	return 0;
}
