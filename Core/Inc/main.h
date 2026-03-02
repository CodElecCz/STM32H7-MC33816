/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

#include "stm32h7xx_nucleo.h"
#include <stdio.h>

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN Private defines */
/*
 * DEBUG
 *
 */
#ifdef DEBUG
#define MAIN_DEBUG 			1
#define LWIP_PERF			0
#else
#define MAIN_DEBUG 			0
#define MAIN_RELEASE_TRACE	1
#define LWIP_PERF			0	/* RELEASE iperf to 1 */
#endif

#if MAIN_DEBUG || MAIN_RELEASE_TRACE

#define MAIN_DEBUG_TRACE(module, message)  \
	if(MAIN_DEBUG_##module <= MAIN_DEBUG_LEVEL_TRACE) { printf message; }

#define MAIN_DEBUG_ERR(module, message)  \
	if(MAIN_DEBUG_##module <= MAIN_DEBUG_LEVEL_ERR) { printf message; }

//redirect to:
#define MAIN_DEBUG_TO_UART6			0
#define MAIN_DEBUG_TO_USB			1
//debug level
#define MAIN_DEBUG_LEVEL_ERR		2
#define MAIN_DEBUG_LEVEL_TRACE		1


#define MAIN_DEBUG_SHELL			MAIN_DEBUG_LEVEL_ERR
#define MAIN_DEBUG_HTTPD_SSI		MAIN_DEBUG_LEVEL_ERR
#define MAIN_DEBUG_HTTPD_AUTH		MAIN_DEBUG_LEVEL_ERR
#define MAIN_DEBUG_HTTPD_POST		MAIN_DEBUG_LEVEL_ERR
#define MAIN_DEBUG_HTTPD_WS			MAIN_DEBUG_LEVEL_ERR
#define MAIN_DEBUG_REST				MAIN_DEBUG_LEVEL_TRACE
#define MAIN_DEBUG_REST_AUTH		MAIN_DEBUG_LEVEL_ERR
#define MAIN_DEBUG_REST_POST		MAIN_DEBUG_LEVEL_TRACE
#define MAIN_DEBUG_SNTP				MAIN_DEBUG_LEVEL_TRACE
#define MAIN_DEBUG_ETHIF			MAIN_DEBUG_LEVEL_ERR
#define MAIN_DEBUG_SPI				MAIN_DEBUG_LEVEL_ERR
#define MAIN_DEBUG_IAP				MAIN_DEBUG_LEVEL_ERR
#define MAIN_DEBUG_SSDP				MAIN_DEBUG_LEVEL_ERR
#define MAIN_DEBUG_SOCKET			MAIN_DEBUG_LEVEL_ERR
#define MAIN_DEBUG_MORPHO			MAIN_DEBUG_LEVEL_ERR

#else

#define MAIN_DEBUG_TRACE(module, message)
#define MAIN_DEBUG_ERR(module, message)

#endif
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
