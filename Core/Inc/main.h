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
#define MC_FLAG2_Pin GPIO_PIN_4
#define MC_FLAG2_GPIO_Port GPIOF
#define MC_FLAG1_Pin GPIO_PIN_5
#define MC_FLAG1_GPIO_Port GPIOF
#define MC_CLK_Pin GPIO_PIN_6
#define MC_CLK_GPIO_Port GPIOF
#define MC_OA2_Pin GPIO_PIN_0
#define MC_OA2_GPIO_Port GPIOC
#define MC_OA1_Pin GPIO_PIN_3
#define MC_OA1_GPIO_Port GPIOA
#define MC_RST_Pin GPIO_PIN_4
#define MC_RST_GPIO_Port GPIOA
#define MC_IRQ_Pin GPIO_PIN_7
#define MC_IRQ_GPIO_Port GPIOE
#define MC_IRQ_EXTI_IRQn EXTI9_5_IRQn
#define MC_DBG_Pin GPIO_PIN_8
#define MC_DBG_GPIO_Port GPIOE
#define MC_START5_Pin GPIO_PIN_12
#define MC_START5_GPIO_Port GPIOB
#define MC_START2_Pin GPIO_PIN_15
#define MC_START2_GPIO_Port GPIOB
#define SPI_CS_Pin GPIO_PIN_14
#define SPI_CS_GPIO_Port GPIOD
#define MC_START6_Pin GPIO_PIN_6
#define MC_START6_GPIO_Port GPIOC
#define MC_START3_Pin GPIO_PIN_7
#define MC_START3_GPIO_Port GPIOC
#define MC_FLAG3_Pin GPIO_PIN_12
#define MC_FLAG3_GPIO_Port GPIOG
#define MC_FLAG0_Pin GPIO_PIN_14
#define MC_FLAG0_GPIO_Port GPIOG
#define MC_DRV_Pin GPIO_PIN_4
#define MC_DRV_GPIO_Port GPIOB
#define MC_START4_Pin GPIO_PIN_8
#define MC_START4_GPIO_Port GPIOB
#define MC_START1_Pin GPIO_PIN_9
#define MC_START1_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
/*
 * DEBUG
 *
 */
#ifdef DEBUG
#define MAIN_DEBUG 			1
#define LWIP_PERF			1
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

//debug level
#define MAIN_DEBUG_LEVEL_ERR		2
#define MAIN_DEBUG_LEVEL_TRACE		1

#define MAIN_DEBUG_SHELL			MAIN_DEBUG_LEVEL_ERR
#define MAIN_DEBUG_SNTP				MAIN_DEBUG_LEVEL_ERR
#define MAIN_DEBUG_DNS				MAIN_DEBUG_LEVEL_ERR
#define MAIN_DEBUG_MC33816			MAIN_DEBUG_LEVEL_TRACE

#else

#define MAIN_DEBUG_TRACE(module, message)
#define MAIN_DEBUG_ERR(module, message)

#endif
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
