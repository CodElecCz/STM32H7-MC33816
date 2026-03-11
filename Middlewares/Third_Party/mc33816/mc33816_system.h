/*******************************************************************************
* Example Code
*
* Copyright(C) 2026 NXP Semiconductors
* NXP Semiconductors Confidential and Proprietary
*
* Software that is described herein is for illustrative purposes only
* which provides customers with programming information regarding the
* NXP products.  This software is supplied "AS IS" without any warranties
* of any kind, and NXP Semiconductors and its licensor disclaim any and
* all warranties, express or implied, including all implied warranties of
* merchantability, fitness for a particular purpose and non-infringement of
* intellectual property rights.  NXP Semiconductors assumes no responsibility
* or liability for the use of the software, conveys no license or rights
* under any patent, copyright, mask work right, or any other intellectual
* property rights in or to any products. NXP Semiconductors reserves the
* right to make changes in the software without notification. NXP
* Semiconductors also makes no representation or warranty that such
* application will be suitable for the specified use without further testing
* or modification.
*
* IN NO EVENT WILL NXP SEMICONDUCTORS BE LIABLE, WHETHER IN CONTRACT, 
* TORT, OR OTHERWISE, FOR ANY INCIDENTAL, SPECIAL, INDIRECT, CONSEQUENTIAL 
* OR PUNITIVE DAMAGES, INCLUDING, BUT NOT LIMITED TO, DAMAGES FOR ANY 
* LOSS OF USE, LOSS OF TIME, INCONVENIENCE, COMMERCIAL LOSS, OR LOST 
* PROFITS, SAVINGS, OR REVENUES, TO THE FULL EXTENT SUCH MAY BE DISCLAIMED  
* BY LAW. NXP SEMICONDUCTOR’S TOTAL LIABILITY FOR ALL COSTS, DAMAGES, 
* CLAIMS, OR LOSSES WHATSOEVER ARISING OUT OF OR IN CONNECTION WITH THE 
* SOFTWARE IS LIMITED TO THE AGGREGATE AMOUNT PAID BY YOU TO NXP SEMICONDUCTORS
* IN CONNECTION WITH THE SOFTWARE TO WHICH LOSSES OR DAMAGES ARE CLAIMED.
*
* Permission to use, copy, modify, and distribute this software and its
* documentation is hereby granted, under NXP Semiconductors' and its
* licensor's relevant copyrights in the software, without fee, provided
* that it is used in conjunction with NXP Semiconductors devices.  This
* copyright, permission, and disclaimer notice must appear in all copies
* of this code.
*******************************************************************************/

#ifndef MC33816_SYSTEM_H_
#define MC33816_SYSTEM_H_

#include "main.h"

#define LOW       0
#define HIGH      1

extern void init_GPIO(void);
extern void set_reset_pin(int state);
extern void set_drven_pin(int state);

#define SET_RESETB_HIGH  HAL_GPIO_WritePin(MC_RST_GPIO_Port, MC_RST_Pin, GPIO_PIN_SET);
#define SET_RESETB_LOW   HAL_GPIO_WritePin(MC_RST_GPIO_Port, MC_RST_Pin, GPIO_PIN_RESET);
#define SET_DRVEN_HIGH   HAL_GPIO_WritePin(MC_DRV_GPIO_Port, MC_DRV_Pin, GPIO_PIN_SET);
#define SET_DRVEN_LOW    HAL_GPIO_WritePin(MC_DRV_GPIO_Port, MC_DRV_Pin, GPIO_PIN_RESET);
#define SET_START1_HIGH  HAL_GPIO_WritePin(MC_START1_GPIO_Port, MC_START1_Pin, GPIO_PIN_SET);
#define SET_START1_LOW   HAL_GPIO_WritePin(MC_START1_GPIO_Port, MC_START1_Pin, GPIO_PIN_RESET);
#define SET_START2_HIGH  HAL_GPIO_WritePin(MC_START2_GPIO_Port, MC_START2_Pin, GPIO_PIN_SET);
#define SET_START2_LOW   HAL_GPIO_WritePin(MC_START2_GPIO_Port, MC_START2_Pin, GPIO_PIN_RESET);
#define SET_START3_HIGH  HAL_GPIO_WritePin(MC_START3_GPIO_Port, MC_START3_Pin, GPIO_PIN_SET);
#define SET_START3_LOW   HAL_GPIO_WritePin(MC_START3_GPIO_Port, MC_START3_Pin, GPIO_PIN_RESET);
#define SET_START4_HIGH  HAL_GPIO_WritePin(MC_START4_GPIO_Port, MC_START4_Pin, GPIO_PIN_SET);
#define SET_START4_LOW   HAL_GPIO_WritePin(MC_START4_GPIO_Port, MC_START4_Pin, GPIO_PIN_RESET);
#define SET_START5_HIGH  HAL_GPIO_WritePin(MC_START5_GPIO_Port, MC_START5_Pin, GPIO_PIN_SET);
#define SET_START5_LOW   HAL_GPIO_WritePin(MC_START5_GPIO_Port, MC_START5_Pin, GPIO_PIN_RESET);
#define SET_START6_HIGH  HAL_GPIO_WritePin(MC_START6_GPIO_Port, MC_START6_Pin, GPIO_PIN_SET);
#define SET_START6_LOW   HAL_GPIO_WritePin(MC_START6_GPIO_Port, MC_START6_Pin, GPIO_PIN_RESET);
#define SET_START7_HIGH  HAL_GPIO_WritePin(MC_START7_GPIO_Port, MC_START7_Pin, GPIO_PIN_SET);
#define SET_START7_LOW   HAL_GPIO_WritePin(MC_START7_GPIO_Port, MC_START7_Pin, GPIO_PIN_RESET);

#define PTC12  0x1000
#define PTC13  0x2000
#define PTA3   0x8
#define Flag1  0x100

void init_ADC(void);
unsigned short read_ADC(unsigned char channel);
unsigned short read_ADC1(unsigned char channel);

void init_CLOCK(void);

void delay(unsigned long msDelay);
void delay100us(unsigned long delay);

extern void PWM_1MHz (void);
extern void InputCaptureTimer_init(void);

#endif /* SYSTEM_H_ */
