/*
 * debug.c
 *
 *  Created on: 26. 10. 2020
 *      Author: TURCAR
 */
#include "main.h"

#if MAIN_DEBUG || MAIN_RELEASE_TRACE

/* Retargets the C library printf function to the USART. */
#include <stdio.h>

#ifdef __GNUC__
int _write(int file, char *ptr, int len)
{
  int DataIdx;
  for (DataIdx= 0; DataIdx< len; DataIdx++)
  {
	ITM_SendChar(*ptr++); // Send to SWO
  }

  return len;
}
#endif

#endif //MAIN_DEBUG


