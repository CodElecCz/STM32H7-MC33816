/**
 * @file mc33816_sniffer.h
 * @brief SPI slave sniffer for MC33816 communication monitoring
 * 
 * This module provides functions to passively monitor SPI communication
 * between a KL25Z master and MC33816 slave device.
 */

#ifndef _MC33816_SNIFFER_H
#define _MC33816_SNIFFER_H

#include "stm32h7xx_hal.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize SPI sniffer
 * @note Uses hspi1 configured in slave mode
 * @retval HAL_OK on success, HAL_ERROR on failure
 */
HAL_StatusTypeDef MC33816_Sniffer_Init(void);

/**
 * @brief Start capturing SPI data
 * @retval HAL_OK on success, HAL_ERROR on failure
 */
HAL_StatusTypeDef MC33816_Sniffer_Start(void);

/**
 * @brief Stop capturing SPI data
 */
void MC33816_Sniffer_Stop(void);

/**
 * @brief Process captured data
 * @note Call this periodically in main loop or task
 */
void MC33816_Sniffer_Process(void);

/**
 * @brief Print captured buffer contents with protocol decoding
 */
void MC33816_Sniffer_PrintBuffer(void);

/**
 * @brief Clear the capture buffer
 */
void MC33816_Sniffer_ClearBuffer(void);

/**
 * @brief SPI receive complete callback
 * @param hspi Pointer to SPI handle
 * @note Call this from HAL_SPI_RxCpltCallback()
 */
void MC33816_Sniffer_RxCallback(SPI_HandleTypeDef *hspi);

#ifdef __cplusplus
}
#endif

#endif /* _MC33816_SNIFFER_H */
