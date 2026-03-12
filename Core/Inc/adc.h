/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : adc.h
  * @brief          : Header for adc.c file.
  *                   This file contains the ADC management for analog outputs
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
#ifndef __ADC_H
#define __ADC_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Exported types ------------------------------------------------------------*/
typedef struct {
    uint16_t a1_raw;       // Raw ADC value for A1 (PA0/ADC1_INP16)
    uint16_t a2_raw;       // Raw ADC value for A2 (PA1/ADC1_INP17)
    float a1_voltage;      // Converted voltage for A1 (V)
    float a2_voltage;      // Converted voltage for A2 (V)
} ADC_Data_t;

/* Exported constants --------------------------------------------------------*/
#define ADC_BUFFER_SIZE         2           // Number of ADC channels
#define ADC_VREF                3.3f        // ADC reference voltage (V)
#define ADC_RESOLUTION          65536.0f    // 16-bit ADC resolution
#define ADC_VOLTAGE_DIVIDER     1.667f      // Voltage divider ratio (5V to 3V)

/* Exported macro ------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
void ADC_Init(void);
void ADC_Start(void);
void ADC_Stop(void);
void ADC_GetData(ADC_Data_t *data);
uint16_t ADC_GetA1_Raw(void);
uint16_t ADC_GetA2_Raw(void);
float ADC_GetA1_Voltage(void);
float ADC_GetA2_Voltage(void);

#ifdef __cplusplus
}
#endif

#endif /* __ADC_H */
