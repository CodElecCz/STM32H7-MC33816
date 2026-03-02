#include "rtc.h"
#include "main.h"

#include <stdio.h>
#include <sys/time.h>
#include <string.h>

extern RTC_HandleTypeDef hrtc;

//tm_mon	int	months since January	0-11
static const char* months[] =
{
	"January",
	"February",
	"March",
	"April",
	"May",
	"June",
	"July",
	"August",
	"September",
	"October",
	"November",
	"December"
};

//tm_wday	int	days since Sunday	0-6
static const char* days[] =
{
	"Sunday",
	"Monday",
	"Tuesday",
	"Wednesday",
	"Thursday",
	"Friday",
	"Saturday"
};

void RtcStatus(void)
{
	RTC_TimeTypeDef sTime;
	RTC_DateTypeDef sDate;

	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

	uint32_t msec = 1000 * (sTime.SecondFraction - sTime.SubSeconds) / (sTime.SecondFraction + 1);

	printf("RTC: %d.%d.%02d %02d:%02d:%02d.%03u\n", sDate.Date, sDate.Month, sDate.Year+2000, sTime.Hours, sTime.Minutes, sTime.Seconds, (unsigned int)msec);

	//systime.c _gettimeofday()
	struct timeval tv;
	gettimeofday(&tv, NULL);
	struct tm* t = localtime(&tv.tv_sec);

	printf("gettimeofday(): %d.%d.%02d %02d:%02d:%02d.%03u %s %s\n", t->tm_mday, t->tm_mon+1, t->tm_year+1900, t->tm_hour, t->tm_min, t->tm_sec, (unsigned int)(tv.tv_usec/1000), days[t->tm_wday], months[t->tm_mon]);
}

int set_rtc_timeval(struct timeval tv)
{
	HAL_StatusTypeDef status;

	RTC_TimeTypeDef sTime = {};
	RTC_DateTypeDef sDate = {};

	struct tm* t = localtime(&tv.tv_sec);

	sDate.Month = t->tm_mon + 1;
	sDate.Date = t->tm_mday;
	sDate.Year = t->tm_year + 1900 - 2000;
	sDate.WeekDay = t->tm_wday + 1;

	sTime.Hours = t->tm_hour;
	sTime.Minutes = t->tm_min;
	sTime.Seconds = t->tm_sec;
	sTime.SecondFraction = 0; //tv.tv_usec;

	status = HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	if(status != HAL_OK)
		return status;

	status = HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
	if(status != HAL_OK)
		return status;

	return HAL_OK;
}

struct timeval rtc_timeval()
{
	RTC_TimeTypeDef sTime = {};
	RTC_DateTypeDef sDate = {};

	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

	uint32_t msec = 1000 * (sTime.SecondFraction - sTime.SubSeconds) / (sTime.SecondFraction + 1);

	struct tm t = {};
	t.tm_mon = sDate.Month - 1;
	t.tm_mday = sDate.Date;
	t.tm_year = sDate.Year + 2000 - 1900; //The number of years since 1900
	t.tm_wday = sDate.WeekDay - 1;
	t.tm_isdst = -1;

	t.tm_hour = sTime.Hours;
	t.tm_min = sTime.Minutes;
	t.tm_sec = sTime.Seconds;

	struct timeval tv = {};
	tv.tv_sec = mktime(&t);
	tv.tv_usec = msec * 1000;

	return tv;
}

time_t rtc_time()
{
	return rtc_timeval().tv_sec;
}

/*
 * __DATE__ May 27 2021
 * __TIME__ 09:59:25
 *
 */

time_t build_time()
{
    char s_month[5];
    int month, day, year, hour, min, sec;
    struct tm t = {0};
    static const char month_names[] = "JanFebMarAprMayJunJulAugSepOctNovDec";

    sscanf(__DATE__, "%s %d %d", s_month, &day, &year);
    sscanf(__TIME__, "%02d:%02d:%02d", &hour, &min, &sec);

    month = (strstr(month_names, s_month)-month_names)/3;

    t.tm_mon = month;
    t.tm_mday = day;
    t.tm_year = year - 1900; //The number of years since 1900
    t.tm_isdst = -1;

    t.tm_hour = hour;
    t.tm_min = min;
    t.tm_sec = sec;

    return mktime(&t);
}
