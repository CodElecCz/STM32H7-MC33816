#ifndef _RTC_H
#define _RTC_H

#ifdef __cplusplus
 extern "C" {
#endif

#include <time.h>

//RTC
time_t rtc_time();
struct timeval rtc_timeval();
int set_rtc_timeval(struct timeval tv);
void RtcStatus();

//time functions
time_t build_time();

#ifdef __cplusplus
}
#endif

#endif

