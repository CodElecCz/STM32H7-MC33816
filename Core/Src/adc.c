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
extern TIM_HandleTypeDef htim2;  // Timer for ADC sampling

// Place ADC buffer in D3 SRAM (0x38000000) for DMA access - uses entire 64K RAM_D3
// STM32H7 DMA cannot access DTCM RAM directly
// Buffer holds 32768 uint16_t values = 16384 dual-channel samples (A2, A1, A2, A1...)
#if defined ( __ICCARM__ )
#pragma location = 0x38000000
static uint16_t adc_buffer[ADC_BUFFER_SIZE];
#elif defined ( __CC_ARM )
static uint16_t adc_buffer[ADC_BUFFER_SIZE] __attribute__((at(0x38000000)));
#elif defined ( __GNUC__ )
static uint16_t adc_buffer[ADC_BUFFER_SIZE] __attribute__((section(".dma_buffer")));
#else
static uint16_t adc_buffer[ADC_BUFFER_SIZE];
#warning "ADC buffer location not explicitly set - DMA may not work"
#endif

static ADC_Data_t adc_data = {0};

/* Private function prototypes -----------------------------------------------*/
static void ADC_ConvertRawToVoltage(void);
static uint32_t ADC_GetLastSampleIndex(uint8_t channel);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Get the index of the last complete sample in the circular buffer
  * @param  channel: 1 for A1 (odd index), 2 for A2 (even index)
  * @retval Buffer index of the last complete sample
  */
static uint32_t ADC_GetLastSampleIndex(uint8_t channel)
{
    uint32_t dma_index;
    
    // Get current DMA write position (remaining items to transfer)
    uint32_t dma_remaining = __HAL_DMA_GET_COUNTER(hadc1.DMA_Handle);
    
    // Calculate the last completed write position
    // DMA counter counts down, so actual index is (BUFFER_SIZE - remaining)
    dma_index = ADC_BUFFER_SIZE - dma_remaining;
    
    if (channel == 1)
    {
        // A1 is at odd index - move back 1 position to get last complete A1 sample
        if (dma_index >= 1)
        {
            dma_index -= 1;
        }
        else
        {
            dma_index = ADC_BUFFER_SIZE - 1;
        }
    }
    else
    {
        // A2 is at even index - move back 2 positions to get last complete A2 sample
        if (dma_index >= 2)
        {
            dma_index -= 2;
        }
        else
        {
            // Wrap around to end of circular buffer
            dma_index = ADC_BUFFER_SIZE - (2 - dma_index);
        }
    }
    
    return dma_index;
}

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
  * @param  hadc: pointer to ADC_HandleTypeDef structure
  * @retval None
  */
void ADC_Init(void)
{
    // Calibrate ADC
    if (HAL_ADCEx_Calibration_Start(&hadc1, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED) != HAL_OK)
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
    // Start ADC DMA conversion
    if (HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buffer, ADC_BUFFER_SIZE) != HAL_OK)
    {
        Error_Handler();
    }

    HAL_TIM_Base_Start(&htim2);  // Start TIM2 after ADC is ready
}

/**
  * @brief  Stop ADC conversions
  * @retval None
  */
void ADC_Stop(void)
{
	HAL_TIM_Base_Stop(&htim2);  // Start TIM2 after ADC is ready
    HAL_ADC_Stop_DMA(&hadc1);
}

/**
  * @brief  Get all ADC data (raw and converted)
  * @param  data: pointer to ADC_Data_t structure
  * @retval None
  */
void ADC_GetData(ADC_Data_t *data)
{
    uint32_t dma_index;
    
    if (data == NULL)
    {
        return;
    }
    
    // Get index of last complete A2 sample (even index)
    dma_index = ADC_GetLastSampleIndex(2);
    
    // Invalidate D-Cache to ensure fresh data from DMA
    SCB_InvalidateDCache_by_Addr((uint32_t*)&adc_buffer[dma_index], 64);
    
    // Read last complete sample from buffer
    adc_data.a2_raw = adc_buffer[dma_index];      // ADC_CHANNEL_10 (PC0/ADC1_INP10) - A2
    adc_data.a1_raw = adc_buffer[dma_index + 1];  // ADC_CHANNEL_15 (PA3/ADC1_INP15) - A1
    
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
    uint32_t dma_index = ADC_GetLastSampleIndex(1);
    
    // Invalidate D-Cache to get fresh data
    SCB_InvalidateDCache_by_Addr((uint32_t*)&adc_buffer[dma_index], 32);
    return adc_buffer[dma_index];  // ADC_CHANNEL_15 (PA3/ADC1_INP15)
}

/**
  * @brief  Get raw ADC value for A2
  * @retval Raw ADC value (16-bit)
  */
uint16_t ADC_GetA2_Raw(void)
{
    uint32_t dma_index = ADC_GetLastSampleIndex(2);
    
    // Invalidate D-Cache to get fresh data
    SCB_InvalidateDCache_by_Addr((uint32_t*)&adc_buffer[dma_index], 32);
    return adc_buffer[dma_index];  // ADC_CHANNEL_10 (PC0/ADC1_INP10)
}

/**
  * @brief  Get converted voltage for A1
  * @retval Voltage in Volts
  */
float ADC_GetA1_Voltage(void)
{
    uint32_t dma_index = ADC_GetLastSampleIndex(1);
    
    // Invalidate D-Cache to get fresh data
    SCB_InvalidateDCache_by_Addr((uint32_t*)&adc_buffer[dma_index], 32);
    adc_data.a1_raw = adc_buffer[dma_index];  // ADC_CHANNEL_15 (PA3/ADC1_INP15)
    adc_data.a1_voltage = ((float)adc_data.a1_raw / ADC_RESOLUTION) * ADC_VREF * ADC_VOLTAGE_DIVIDER;
    return adc_data.a1_voltage;
}

/**
  * @brief  Get converted voltage for A2
  * @retval Voltage in Volts
  */
float ADC_GetA2_Voltage(void)
{
    uint32_t dma_index = ADC_GetLastSampleIndex(2);
    
    // Invalidate D-Cache to get fresh data
    SCB_InvalidateDCache_by_Addr((uint32_t*)&adc_buffer[dma_index], 32);
    adc_data.a2_raw = adc_buffer[dma_index];  // ADC_CHANNEL_10 (PC0/ADC1_INP10)
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
    
    // Store current DMA position - we'll only search data acquired after this point
    capture->arm_dma_index = ADC_BUFFER_SIZE - __HAL_DMA_GET_COUNTER(hadc1.DMA_Handle);
    
    capture->state = ADC_TRIGGER_ARMED;
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
    uint32_t dma_current_index;
    uint32_t channel_offset;
    uint32_t step;
    static uint32_t trigger_dma_index = 0;  // Store trigger position in DMA buffer
    
    if (capture == NULL || capture->state == ADC_TRIGGER_IDLE || capture->state == ADC_TRIGGER_COMPLETE)
    {
        return;
    }
    
    // Determine channel offset and step for interleaved dual-channel buffer
    // Buffer format: [A2, A1, A2, A1, A2, A1, ...]
    if (capture->channel == 1)
    {
        channel_offset = 1;  // A1 at odd indices
    }
    else
    {
        channel_offset = 0;  // A2 at even indices
    }
    step = 2;  // Step by 2 to get only the selected channel
    
    // Invalidate DMA buffer cache to ensure fresh data
    SCB_InvalidateDCache_by_Addr((uint32_t*)adc_buffer, ADC_BUFFER_SIZE * sizeof(uint16_t));
    
    // Get current DMA write position
    dma_current_index = ADC_BUFFER_SIZE - __HAL_DMA_GET_COUNTER(hadc1.DMA_Handle);
    
    if (capture->state == ADC_TRIGGER_ARMED)
    {
        // PHASE 1: Search for trigger
        uint32_t search_start_index;
        uint32_t samples_to_search;
        uint32_t i;
        uint16_t threshold_raw;
        uint16_t current_value, previous_value;
        uint8_t trigger_found = 0;
        
        // Convert threshold voltage to raw ADC value
        threshold_raw = (uint16_t)((capture->threshold_voltage / (ADC_VREF * ADC_VOLTAGE_DIVIDER)) * ADC_RESOLUTION);
        
        // Calculate how many samples to search from arm point to current position
        int32_t dma_distance = (int32_t)dma_current_index - (int32_t)capture->arm_dma_index;
        if (dma_distance < 0)
        {
            dma_distance += ADC_BUFFER_SIZE;  // Handle circular buffer wrap
        }
        samples_to_search = dma_distance / step;  // Convert to channel samples
        
        // Limit search range
        if (samples_to_search > capture->buffer_size + 1000)
        {
            samples_to_search = capture->buffer_size + 1000;
        }
        
        if (samples_to_search < 2)
        {
            return;  // Not enough new data yet
        }
        
        // Start searching backwards from current DMA position to arm position
        search_start_index = dma_current_index;
        if (search_start_index % 2 != channel_offset)
        {
            search_start_index = (search_start_index + 1) % ADC_BUFFER_SIZE;
        }
        
        // Search backwards through the buffer for trigger condition
        // Only search data acquired AFTER arming
        for (i = 0; i < samples_to_search - 1; i++)
        {
            uint32_t current_idx = (search_start_index - (i * step) + ADC_BUFFER_SIZE) % ADC_BUFFER_SIZE;
            uint32_t previous_idx = (current_idx - step + ADC_BUFFER_SIZE) % ADC_BUFFER_SIZE;
            
            // Stop searching if we've reached the arm position (don't search older data)
            int32_t distance_to_arm = (int32_t)current_idx - (int32_t)capture->arm_dma_index;
            if (distance_to_arm < 0)
            {
                distance_to_arm += ADC_BUFFER_SIZE;
            }
            if (distance_to_arm < step)
            {
                break;  // Reached arm position, stop
            }
            
            current_value = adc_buffer[current_idx];
            previous_value = adc_buffer[previous_idx];
            
            // Check for trigger condition
            if (capture->rising_edge)
            {
                if (previous_value < threshold_raw && current_value >= threshold_raw)
                {
                    trigger_found = 1;
                    trigger_dma_index = current_idx;
                    break;
                }
            }
            else
            {
                if (previous_value >= threshold_raw && current_value < threshold_raw)
                {
                    trigger_found = 1;
                    trigger_dma_index = current_idx;
                    break;
                }
            }
        }
        
        if (!trigger_found)
        {
            return;  // No trigger found yet, keep waiting
        }
        
        // Trigger found! Capture pre-samples
        uint32_t actual_presamples = capture->presample_count;
        
        for (i = 0; i < actual_presamples && i < capture->buffer_size; i++)
        {
            int32_t offset_from_trigger = (int32_t)i - (int32_t)actual_presamples;
            uint32_t src_idx = (trigger_dma_index + (offset_from_trigger * step) + ADC_BUFFER_SIZE) % ADC_BUFFER_SIZE;
            capture->buffer[i] = adc_buffer[src_idx];
        }
        
        capture->samples_captured = actual_presamples;
        capture->trigger_index = actual_presamples;
        capture->state = ADC_TRIGGER_TRIGGERED;
    }
    else if (capture->state == ADC_TRIGGER_TRIGGERED)
    {
        // PHASE 2: Continue capturing post-trigger samples
        uint32_t i;
        
        // Calculate how many more samples we need
        uint32_t remaining = capture->buffer_size - capture->samples_captured;
        
        // Calculate how many post-samples have been acquired in DMA buffer since trigger
        // Need to check if enough new samples are available
        int32_t samples_since_trigger;
        
        // Calculate distance from trigger to current DMA position (in channel samples)
        int32_t dma_distance = (int32_t)dma_current_index - (int32_t)trigger_dma_index;
        if (dma_distance < 0)
        {
            dma_distance += ADC_BUFFER_SIZE;  // Wrap around circular buffer
        }
        samples_since_trigger = dma_distance / step;  // Convert to channel samples
        
        // Check if we have enough post-samples available
        uint32_t post_samples_needed = remaining;
        uint32_t post_samples_available = (samples_since_trigger > 0) ? samples_since_trigger : 0;
        
        if (post_samples_available < post_samples_needed)
        {
            // Not enough data yet - wait for more samples
            // Only copy what's available so far
            uint32_t samples_to_copy = post_samples_available;
            
            for (i = 0; i < samples_to_copy; i++)
            {
                uint32_t buffer_idx = capture->samples_captured + i;
                
                // Calculate position in DMA buffer relative to trigger
                int32_t offset_from_trigger = (int32_t)buffer_idx - (int32_t)capture->trigger_index;
                uint32_t src_idx = (trigger_dma_index + (offset_from_trigger * step) + ADC_BUFFER_SIZE) % ADC_BUFFER_SIZE;
                
                capture->buffer[buffer_idx] = adc_buffer[src_idx];
            }
            
            capture->samples_captured += samples_to_copy;
            // Stay in TRIGGERED state - not complete yet
            return;
        }
        
        // Enough data available - copy all remaining samples
        for (i = 0; i < remaining; i++)
        {
            uint32_t buffer_idx = capture->samples_captured + i;
            if (buffer_idx >= capture->buffer_size)
            {
                break;  // Buffer full
            }
            
            // Calculate position in DMA buffer relative to trigger
            int32_t offset_from_trigger = (int32_t)buffer_idx - (int32_t)capture->trigger_index;
            uint32_t src_idx = (trigger_dma_index + (offset_from_trigger * step) + ADC_BUFFER_SIZE) % ADC_BUFFER_SIZE;
            
            capture->buffer[buffer_idx] = adc_buffer[src_idx];
        }
        
        capture->samples_captured = capture->buffer_size;
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
