/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : adc.c
  * @brief          : ADC management for MC33816 analog outputs
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

/* Includes ------------------------------------------------------------------*/
#include "adc.h"
#include <string.h>

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static ADC_HandleTypeDef *hadc_mc33816 = NULL;
static uint16_t adc_buffer[ADC_BUFFER_SIZE] = {0};
static ADC_MC33816_Data_t adc_data = {0};

/* Private function prototypes -----------------------------------------------*/
static void ADC_ConvertRawToVoltage(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Convert raw ADC values to voltages
  * @note   Takes into account voltage divider (5V to 3V)
  * @retval None
  */
static void ADC_ConvertRawToVoltage(void)
{
    // Convert raw ADC values to voltage
    // Formula: Voltage = (Raw / ADC_Resolution) * VREF * Voltage_Divider_Ratio
    adc_data.oa1_voltage = ((float)adc_data.oa1_raw / ADC_RESOLUTION) * ADC_VREF * ADC_VOLTAGE_DIVIDER;
    adc_data.oa2_voltage = ((float)adc_data.oa2_raw / ADC_RESOLUTION) * ADC_VREF * ADC_VOLTAGE_DIVIDER;
}

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Initialize ADC for MC33816 analog outputs
  * @param  hadc: pointer to ADC_HandleTypeDef structure
  * @retval None
  */
void ADC_MC33816_Init(ADC_HandleTypeDef *hadc)
{
    if (hadc == NULL)
    {
        return;
    }
    
    hadc_mc33816 = hadc;
    
    // Calibrate ADC
    if (HAL_ADCEx_Calibration_Start(hadc_mc33816, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
  * @brief  Start ADC conversions with DMA
  * @retval None
  */
void ADC_MC33816_Start(void)
{
    if (hadc_mc33816 == NULL)
    {
        return;
    }
    
    // Start ADC DMA conversion
    if (HAL_ADC_Start_DMA(hadc_mc33816, (uint32_t*)adc_buffer, ADC_BUFFER_SIZE) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
  * @brief  Stop ADC conversions
  * @retval None
  */
void ADC_MC33816_Stop(void)
{
    if (hadc_mc33816 == NULL)
    {
        return;
    }
    
    HAL_ADC_Stop_DMA(hadc_mc33816);
}

/**
  * @brief  Get all ADC data (raw and converted)
  * @param  data: pointer to ADC_MC33816_Data_t structure
  * @retval None
  */
void ADC_MC33816_GetData(ADC_MC33816_Data_t *data)
{
    if (data == NULL)
    {
        return;
    }
    
    // Update raw values from buffer
    adc_data.oa1_raw = adc_buffer[0];  // ADC_CHANNEL_10 (PA0/ADC1_INP16) - MC_OA_1
    adc_data.oa2_raw = adc_buffer[1];  // ADC_CHANNEL_15 (PA1/ADC1_INP17) - MC_OA_2
    
    // Convert to voltage
    ADC_ConvertRawToVoltage();
    
    // Copy data to output structure
    memcpy(data, &adc_data, sizeof(ADC_MC33816_Data_t));
}

/**
  * @brief  Get raw ADC value for OA_OUT1
  * @retval Raw ADC value (16-bit)
  */
uint16_t ADC_MC33816_GetOA1_Raw(void)
{
    return adc_buffer[0];
}

/**
  * @brief  Get raw ADC value for OA_OUT2
  * @retval Raw ADC value (16-bit)
  */
uint16_t ADC_MC33816_GetOA2_Raw(void)
{
    return adc_buffer[1];
}

/**
  * @brief  Get converted voltage for OA_OUT1
  * @retval Voltage in Volts
  */
float ADC_MC33816_GetOA1_Voltage(void)
{
    adc_data.oa1_raw = adc_buffer[0];
    adc_data.oa1_voltage = ((float)adc_data.oa1_raw / ADC_RESOLUTION) * ADC_VREF * ADC_VOLTAGE_DIVIDER;
    return adc_data.oa1_voltage;
}

/**
  * @brief  Get converted voltage for OA_OUT2
  * @retval Voltage in Volts
  */
float ADC_MC33816_GetOA2_Voltage(void)
{
    adc_data.oa2_raw = adc_buffer[1];
    adc_data.oa2_voltage = ((float)adc_data.oa2_raw / ADC_RESOLUTION) * ADC_VREF * ADC_VOLTAGE_DIVIDER;
    return adc_data.oa2_voltage;
}

/**
  * @brief  ADC conversion complete callback
  * @param  hadc: pointer to ADC_HandleTypeDef structure
  * @retval None
  */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    if (hadc->Instance == ADC1)
    {
        // ADC conversion complete - data is now in adc_buffer via DMA
        // No action needed here as data is automatically updated in buffer
    }
}
