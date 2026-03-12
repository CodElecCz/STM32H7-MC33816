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
    uint16_t a1_raw;       // Raw ADC value for A1 (PA3/ADC1_INP15)
    uint16_t a2_raw;       // Raw ADC value for A2 (PC0/ADC1_INP10)
    float a1_voltage;      // Converted voltage for A1 (V)
    float a2_voltage;      // Converted voltage for A2 (V)
} ADC_Data_t;

typedef enum {
    ADC_TRIGGER_IDLE = 0,
    ADC_TRIGGER_ARMED,
    ADC_TRIGGER_TRIGGERED,
    ADC_TRIGGER_COMPLETE
} ADC_TriggerState_t;

typedef struct {
    uint8_t channel;            // Channel to monitor (1=A1, 2=A2)
    float threshold_voltage;    // Trigger threshold in volts
    uint8_t rising_edge;        // 1=rising edge, 0=falling edge
    uint32_t presample_count;   // Number of samples before trigger
    uint32_t total_samples;     // Total samples to capture
    uint16_t* buffer;           // Pointer to capture buffer
    uint32_t buffer_size;       // Size of capture buffer
    ADC_TriggerState_t state;   // Current trigger state
    uint32_t trigger_index;     // Index where trigger occurred
    uint32_t samples_captured;  // Number of samples captured
} ADC_TriggerCapture_t;

/* Exported constants --------------------------------------------------------*/
#define ADC_BUFFER_SIZE         32768       // RAM_D3 is 64K = 32768 uint16_t values (16384 dual samples)
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

// Trigger capture functions
// Uses 32KB DMA buffer - call Process() once to search historical data for trigger
void ADC_TriggerCapture_Init(ADC_TriggerCapture_t *capture, uint8_t channel, 
                              float threshold, uint8_t rising_edge,
                              uint32_t presample_count, uint32_t total_samples,
                              uint16_t* buffer, uint32_t buffer_size);
void ADC_TriggerCapture_Arm(ADC_TriggerCapture_t *capture);
void ADC_TriggerCapture_Process(ADC_TriggerCapture_t *capture);  // Searches DMA buffer
ADC_TriggerState_t ADC_TriggerCapture_GetState(ADC_TriggerCapture_t *capture);
uint32_t ADC_TriggerCapture_GetData(ADC_TriggerCapture_t *capture);

#ifdef __cplusplus
}
#endif

#endif /* __ADC_H */
