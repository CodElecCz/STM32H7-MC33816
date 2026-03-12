/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : adc.c
  * @brief          : ADC management for analog outputs
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
extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;
extern TIM_HandleTypeDef htim2;  // Timer for ADC sampling

// ADC buffers in D3 SRAM for DMA access (STM32H7 DMA cannot access DTCM)
// Each buffer: 32768 samples = 64KB
// ADC1: Channel 15 (PA3) - A1
// ADC2: Channel 10 (PC0) - A2
static uint16_t adc1_buffer[ADC_BUFFER_SIZE] __attribute__((section(".dma_buffer"), aligned(32)));
static uint16_t adc2_buffer[ADC_BUFFER_SIZE] __attribute__((section(".dma_buffer"), aligned(32)));

static ADC_Data_t adc_data = {0};

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
    adc_data.a1_voltage = ((float)adc_data.a1_raw / ADC_RESOLUTION) * ADC_VREF * ADC_VOLTAGE_DIVIDER;
    adc_data.a2_voltage = ((float)adc_data.a2_raw / ADC_RESOLUTION) * ADC_VREF * ADC_VOLTAGE_DIVIDER;
}

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Initialize ADC for analog outputs
  * @retval None
  */
void ADC_Init(void)
{
    // Calibrate ADC1
    if (HAL_ADCEx_Calibration_Start(&hadc1, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED) != HAL_OK)
    {
        Error_Handler();
    }
    
    // Calibrate ADC2
    if (HAL_ADCEx_Calibration_Start(&hadc2, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
  * @brief  Start ADC conversions with DMA
  * @retval None
  */
void ADC_Start(void)
{
    // Start ADC1 DMA conversion (A1)
    if (HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc1_buffer, ADC_BUFFER_SIZE) != HAL_OK)
    {
        Error_Handler();
    }
    
    // Start ADC2 DMA conversion (A2)
    if (HAL_ADC_Start_DMA(&hadc2, (uint32_t*)adc2_buffer, ADC_BUFFER_SIZE) != HAL_OK)
    {
        Error_Handler();
    }

    HAL_TIM_Base_Start(&htim2);  // Start TIM2 to trigger both ADCs
}

/**
  * @brief  Stop ADC conversions
  * @retval None
  */
void ADC_Stop(void)
{
    HAL_TIM_Base_Stop(&htim2);
    HAL_ADC_Stop_DMA(&hadc1);
    HAL_ADC_Stop_DMA(&hadc2);
}

/**
  * @brief  Get all ADC data (raw and converted)
  * @param  data: pointer to ADC_Data_t structure
  * @retval None
  */
void ADC_GetData(ADC_Data_t *data)
{
    uint32_t adc1_index, adc2_index;
    
    if (data == NULL)
    {
        return;
    }
    
    // Get current DMA positions
    adc1_index = ADC_BUFFER_SIZE - __HAL_DMA_GET_COUNTER(hadc1.DMA_Handle);
    adc2_index = ADC_BUFFER_SIZE - __HAL_DMA_GET_COUNTER(hadc2.DMA_Handle);
    
    // Get last complete sample (one position back from DMA pointer)
    if (adc1_index > 0) adc1_index--; else adc1_index = ADC_BUFFER_SIZE - 1;
    if (adc2_index > 0) adc2_index--; else adc2_index = ADC_BUFFER_SIZE - 1;
    
    // Invalidate D-Cache to ensure fresh data from DMA
    SCB_InvalidateDCache_by_Addr((uint32_t*)&adc1_buffer[adc1_index], 32);
    SCB_InvalidateDCache_by_Addr((uint32_t*)&adc2_buffer[adc2_index], 32);
    
    // Read samples
    adc_data.a1_raw = adc1_buffer[adc1_index];
    adc_data.a2_raw = adc2_buffer[adc2_index];
    
    // Convert to voltage
    ADC_ConvertRawToVoltage();
    
    // Copy data to output structure
    memcpy(data, &adc_data, sizeof(ADC_Data_t));
}

/**
  * @brief  Get raw ADC value for A1
  * @retval Raw ADC value (16-bit)
  */
uint16_t ADC_GetA1_Raw(void)
{
    uint32_t dma_index = ADC_BUFFER_SIZE - __HAL_DMA_GET_COUNTER(hadc1.DMA_Handle);
    if (dma_index > 0) dma_index--; else dma_index = ADC_BUFFER_SIZE - 1;
    
    // Invalidate D-Cache to get fresh data
    SCB_InvalidateDCache_by_Addr((uint32_t*)&adc1_buffer[dma_index], 32);
    return adc1_buffer[dma_index];
}

/**
  * @brief  Get raw ADC value for A2
  * @retval Raw ADC value (16-bit)
  */
uint16_t ADC_GetA2_Raw(void)
{
    uint32_t dma_index = ADC_BUFFER_SIZE - __HAL_DMA_GET_COUNTER(hadc2.DMA_Handle);
    if (dma_index > 0) dma_index--; else dma_index = ADC_BUFFER_SIZE - 1;
    
    // Invalidate D-Cache to get fresh data
    SCB_InvalidateDCache_by_Addr((uint32_t*)&adc2_buffer[dma_index], 32);
    return adc2_buffer[dma_index];
}

/**
  * @brief  Get converted voltage for A1
  * @retval Voltage in Volts
  */
float ADC_GetA1_Voltage(void)
{
    uint32_t dma_index = ADC_BUFFER_SIZE - __HAL_DMA_GET_COUNTER(hadc1.DMA_Handle);
    if (dma_index > 0) dma_index--; else dma_index = ADC_BUFFER_SIZE - 1;
    
    // Invalidate D-Cache to get fresh data
    SCB_InvalidateDCache_by_Addr((uint32_t*)&adc1_buffer[dma_index], 32);
    adc_data.a1_raw = adc1_buffer[dma_index];
    adc_data.a1_voltage = ((float)adc_data.a1_raw / ADC_RESOLUTION) * ADC_VREF * ADC_VOLTAGE_DIVIDER;
    return adc_data.a1_voltage;
}

/**
  * @brief  Get converted voltage for A2
  * @retval Voltage in Volts
  */
float ADC_GetA2_Voltage(void)
{
    uint32_t dma_index = ADC_BUFFER_SIZE - __HAL_DMA_GET_COUNTER(hadc2.DMA_Handle);
    if (dma_index > 0) dma_index--; else dma_index = ADC_BUFFER_SIZE - 1;
    
    // Invalidate D-Cache to get fresh data
    SCB_InvalidateDCache_by_Addr((uint32_t*)&adc2_buffer[dma_index], 32);
    adc_data.a2_raw = adc2_buffer[dma_index];
    adc_data.a2_voltage = ((float)adc_data.a2_raw / ADC_RESOLUTION) * ADC_VREF * ADC_VOLTAGE_DIVIDER;
    return adc_data.a2_voltage;
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

void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc)
{
    uint32_t error_code = HAL_ADC_GetError(hadc);

    if (error_code & HAL_ADC_ERROR_OVR)
    {
        // Handle Overrun: Clear flag and restart
        __HAL_ADC_CLEAR_FLAG(hadc, ADC_FLAG_OVR);
    }
}

/* Called if the DMA fails to move data to RAM */
void HAL_ADC_DMAErrorCallback(ADC_HandleTypeDef *hadc)
{
    // This usually means your buffer is in the wrong RAM (DTCM)
    Error_Handler();
}

/* Trigger capture functions -------------------------------------------------*/

/**
  * @brief  Initialize trigger capture configuration
  * @param  capture: pointer to ADC_TriggerCapture_t structure
  * @param  channel: channel to monitor (1=A1, 2=A2)
  * @param  threshold: trigger threshold voltage
  * @param  rising_edge: 1=rising edge trigger, 0=falling edge trigger
  * @param  presample_count: number of samples to capture before trigger
  * @param  buffer: pointer to user-provided buffer for capture
  * @param  buffer_size: size of the capture buffer (total samples to capture)
  * @retval None
  */
void ADC_TriggerCapture_Init(ADC_TriggerCapture_t *capture, uint8_t channel, 
                              float threshold, uint8_t rising_edge,
                              uint32_t presample_count,
                              uint16_t* buffer, uint32_t buffer_size)
{
    if (capture == NULL || buffer == NULL)
    {
        return;
    }
    
    capture->channel = channel;
    capture->threshold_voltage = threshold;
    capture->rising_edge = rising_edge;
    capture->presample_count = presample_count;
    capture->buffer = buffer;
    capture->buffer_size = buffer_size;
    capture->state = ADC_TRIGGER_IDLE;
    capture->trigger_index = 0;
    capture->samples_captured = 0;
    
    // Validate parameters
    if (presample_count >= buffer_size)
    {
        capture->presample_count = buffer_size / 2;
    }
}

/**
  * @brief  Arm the trigger capture
  * @param  capture: pointer to ADC_TriggerCapture_t structure
  * @retval None
  * @note   Stores current DMA position - search will only look at data acquired after this point
  */
void ADC_TriggerCapture_Arm(ADC_TriggerCapture_t *capture)
{
    if (capture == NULL)
    {
        return;
    }
    
    // Store current DMA position based on selected channel
    if (capture->channel == 1)
    {
        capture->arm_dma_index = ADC_BUFFER_SIZE - __HAL_DMA_GET_COUNTER(hadc1.DMA_Handle);
    }
    else
    {
        capture->arm_dma_index = ADC_BUFFER_SIZE - __HAL_DMA_GET_COUNTER(hadc2.DMA_Handle);
    }
    
    capture->state = ADC_TRIGGER_ARMED;
    capture->samples_captured = 0;
    capture->trigger_index = 0;
    capture->trigger_dma_index = 0;
    capture->last_read_dma_index = 0;
}

void ADC_TriggerCapture_Idle(ADC_TriggerCapture_t *capture)
{
    if (capture == NULL)
    {
        return;
    }

    capture->state = ADC_TRIGGER_IDLE;
    capture->samples_captured = 0;
    capture->trigger_index = 0;
}

/**
  * @brief  Process ADC trigger capture by searching through DMA buffer
  * @param  capture: pointer to ADC_TriggerCapture_t structure
  * @retval None
  * @note   Call this function repeatedly:
  *         - First call: Searches for trigger and captures pre-samples
  *         - Subsequent calls: Continues capturing post-samples until buffer full
  *         - State changes: ARMED -> TRIGGERED -> COMPLETE
  */
void ADC_TriggerCapture_Process(ADC_TriggerCapture_t *capture)
{
    uint32_t i;
    uint32_t dma_current_index;
    uint16_t *adc_buffer_ptr;
    ADC_HandleTypeDef *hadc_ptr;

    if (capture == NULL || capture->state == ADC_TRIGGER_IDLE || capture->state == ADC_TRIGGER_COMPLETE)
    {
        return;
    }

    // Select correct buffer and ADC handle based on channel
    if (capture->channel == 1)
    {
        adc_buffer_ptr = adc1_buffer;
        hadc_ptr = &hadc1;
    }
    else
    {
        adc_buffer_ptr = adc2_buffer;
        hadc_ptr = &hadc2;
    }

    // Invalidate DMA buffer cache to ensure fresh data
    SCB_InvalidateDCache_by_Addr((uint32_t*)adc_buffer_ptr, ADC_BUFFER_SIZE * sizeof(uint16_t));

    // Get current DMA write position
    dma_current_index = ADC_BUFFER_SIZE - __HAL_DMA_GET_COUNTER(hadc_ptr->DMA_Handle);

    if (capture->state == ADC_TRIGGER_ARMED)
    {
        // PHASE 1: Search for trigger in data acquired after arm position
        uint16_t threshold_raw;
        int32_t dma_distance;
        uint32_t samples_to_search;
        uint8_t trigger_found;

        threshold_raw = (uint16_t)((capture->threshold_voltage / (ADC_VREF * ADC_VOLTAGE_DIVIDER)) * ADC_RESOLUTION);

        // How many samples available since arm?
        dma_distance = (int32_t)dma_current_index - (int32_t)capture->arm_dma_index;
        if (dma_distance < 0) dma_distance += ADC_BUFFER_SIZE;

        samples_to_search = (uint32_t)dma_distance;
        if (samples_to_search > capture->buffer_size + 1000)
            samples_to_search = capture->buffer_size + 1000;

        if (samples_to_search < 2)
            return;  // Not enough data yet

        // Search FORWARDS from arm position to current DMA position
        // This finds the OLDEST (first) trigger, maximising post-sample data available
        trigger_found = 0;
        for (i = 1; i < samples_to_search; i++)
        {
            uint32_t prev_idx = (capture->arm_dma_index + i - 1) % ADC_BUFFER_SIZE;
            uint32_t curr_idx = (capture->arm_dma_index + i    ) % ADC_BUFFER_SIZE;

            uint16_t prev_val = adc_buffer_ptr[prev_idx];
            uint16_t curr_val = adc_buffer_ptr[curr_idx];

            if (capture->rising_edge)
            {
                if (prev_val < threshold_raw && curr_val >= threshold_raw)
                {
                    trigger_found = 1;
                    capture->trigger_dma_index = curr_idx;
                    break;
                }
            }
            else
            {
                if (prev_val >= threshold_raw && curr_val < threshold_raw)
                {
                    trigger_found = 1;
                    capture->trigger_dma_index = curr_idx;
                    break;
                }
            }
        }

        if (!trigger_found)
            return;

        // Copy pre-samples (go backwards from trigger into history)
        for (i = 0; i < capture->presample_count; i++)
        {
            // buffer[0] = oldest pre-sample, buffer[presample_count-1] = newest pre-sample
            int32_t offset = (int32_t)i - (int32_t)capture->presample_count;
            int32_t src = (int32_t)capture->trigger_dma_index + offset;
            if (src < 0) src += ADC_BUFFER_SIZE;
            capture->buffer[i] = adc_buffer_ptr[(uint32_t)src % ADC_BUFFER_SIZE];
        }

        // Copy trigger sample itself
        capture->buffer[capture->presample_count] = adc_buffer_ptr[capture->trigger_dma_index];
        capture->trigger_index = capture->presample_count;
        capture->samples_captured = capture->presample_count + 1;

        // Post-samples exist in DMA buffer right after trigger_dma_index.
        // last_read_dma_index = trigger sample position, TRIGGERED phase reads from +1 onwards.
        capture->last_read_dma_index = capture->trigger_dma_index;
        capture->state = ADC_TRIGGER_TRIGGERED;
    }
    else if (capture->state == ADC_TRIGGER_TRIGGERED)
    {
        // PHASE 2: Copy post-samples sequentially, never ahead of DMA write position.
        uint32_t remaining;
        uint32_t new_data;
        uint32_t to_copy;

        remaining = capture->buffer_size - capture->samples_captured;
        new_data  = (dma_current_index - capture->last_read_dma_index + ADC_BUFFER_SIZE) % ADC_BUFFER_SIZE;

        if (new_data == 0)
            return;

        to_copy = (new_data < remaining) ? new_data : remaining;

        for (i = 0; i < to_copy; i++)
        {
            uint32_t src_idx = (capture->last_read_dma_index + 1 + i) % ADC_BUFFER_SIZE;
            capture->buffer[capture->samples_captured + i] = adc_buffer_ptr[src_idx];
        }

        capture->samples_captured   += to_copy;
        capture->last_read_dma_index = (capture->last_read_dma_index + to_copy) % ADC_BUFFER_SIZE;

        if (capture->samples_captured >= capture->buffer_size)
            capture->state = ADC_TRIGGER_COMPLETE;
    }
}

/**
  * @brief  Get trigger capture state
  * @param  capture: pointer to ADC_TriggerCapture_t structure
  * @retval Current state
  */
ADC_TriggerState_t ADC_TriggerCapture_GetState(ADC_TriggerCapture_t *capture)
{
    if (capture == NULL)
    {
        return ADC_TRIGGER_IDLE;
    }
    return capture->state;
}

/**
  * @brief  Get captured data after trigger is complete
  * @param  capture: pointer to ADC_TriggerCapture_t structure
  * @retval Number of samples captured
  * @note   Data is already in chronological order in the buffer:
  *         [pre-samples...][trigger point][post-samples...]
  *         Trigger point is at index capture->trigger_index
  */
uint32_t ADC_TriggerCapture_GetData(ADC_TriggerCapture_t *capture)
{
    if (capture == NULL || capture->state != ADC_TRIGGER_COMPLETE)
    {
        return 0;
    }
    
    // Data is already in correct chronological order
    // No reorganization needed
    return capture->samples_captured;
}
