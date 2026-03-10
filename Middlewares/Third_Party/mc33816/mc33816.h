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

/*
 * MC33816.h
 *
 *  MC33816 Header File
 *
 */

#ifndef MC33816_H_
#define MC33816_H_

#include <stdint.h>
#include <stdbool.h>
#include "bin/MC33816_data.h"

#define CODE_RAM1 0
#define CODE_RAM2 1
#define DATA_RAM  2

#define CH1_REG   0
#define CH2_REG   1
#define DIAG_REG  2
#define IO_REG    3
#define MAIN_REG  4

uint16_t send_single_SPI_Cmd(bool bRead, uint16_t offset, uint16_t txData);
bool send_SPI_Cmd(bool bRead, uint16_t start_addr, uint16_t length, uint16_t* pTxData, uint16_t* pRxData);


void ProgramDevice();
void download_RAM(int target);
void download_register(int r_target);

bool ID_Check ();
bool CLK_check();
bool Driver_Status_Init ();
bool DRVEN_check();
bool BIST_check(_Bool BIST_run);
bool OA_path_check(_Bool OA1_check, _Bool OA2_check);
bool Checksum_check();
uint16_t Read_VbatADC ();
unsigned long Bootstrap_check();
void Device_Lock_Unlock(unsigned char Lock_Unlock);
void Tracer(unsigned int trace_start, unsigned int trace_stop, _Bool ucore, unsigned char channel , unsigned char post_trigger_length, _Bool trace_enable);

#endif /* MC33816_H_ */
