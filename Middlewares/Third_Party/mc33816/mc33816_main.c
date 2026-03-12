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
 * main implementation: use this 'C' sample to create your own application
 *
 */

#include "mc33816_main.h"
#include "mc33816_irq.h"
#include "mc33816_spi_map.h"

//========================================================================================
// Global Variables
//========================================================================================
const int sw_retry = 5;
const int auto_retry = 2;
int result = 0;
int iprof_counter = 0;
int count = 0;
int drv_int_counter = 0;
volatile char states = 0;
volatile int flag_IRQB = 0;
volatile int seq_counter = 0;
unsigned short rxData = 0;
unsigned short cksys_missing_error = 0;
unsigned short cksys_missing = 0;
unsigned short drv_irq = 0;
unsigned short checksum_irq =0;
unsigned short drv_status = 0;
unsigned short spi_irq = 0;
unsigned short auto_irq = 0;
unsigned short sw_irq = 0;
unsigned short interrupt_reg = 0;
unsigned short inj1_error = 0;
unsigned short inj2_error = 0;
unsigned short inj3_error = 0;
unsigned short inj4_error = 0;
unsigned short inj5_error = 0;
unsigned short inj6_error = 0;
unsigned short fp_error = 0;
unsigned short dcdc_error = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function name   : main
// Description     : Program entry point. Programs the MC33816 and runs an injection
//                   sequence.
// Return type     : int
// Argument        : void
///////////////////////////////////////////////////////////////////////////////////////////////////////////
int MC33816_init(void)
{
    // Initialize the hardware
    init_CLOCK();
    init_GPIO();
    init_SPI();
    init_ADC();
    PWM_1MHz();

    // Reset the MC33816 device
    SET_RESETB_LOW;
    delay(1);
    SET_RESETB_HIGH;
    delay100us(1);                                 // Wait for the PLL time to lock

    if (ID_Check () == 0) 		// Check if SPI works and that device is the good one
    {
    	MAIN_DEBUG_ERR(MC33816, ("MC33816 ID_Check() failed\n"));
    	return -1;
    };
    if (CLK_check() == 0) 		// Check if 1MHz CLK is properly connected
    {
    	MAIN_DEBUG_ERR(MC33816, ("MC33816 CLK_check() failed\n"));
    	return -1;
    };
    if (DRVEN_check() == 0) 	// Check if DRVEN monitoring works using SPI register 1B2h
    {
    	MAIN_DEBUG_ERR(MC33816, ("MC33816 DRVEN_check() failed\n"));
    	return -1;
    };
    if (BIST_check(1) == 0) 	// Run BIST
    {
    	MAIN_DEBUG_ERR(MC33816, ("MC33816 BIST_check() failed\n"));
    	return -1;
    };
    if (OA_path_check(1,1) == 0) 	// Check OA pin connections to MCU ADC
    {
    	MAIN_DEBUG_ERR(MC33816, ("MC33816 OA_path_check() failed\n"));
    	return -1;
    };

    // Program the device
    ProgramDevice();

    if (Checksum_check() == 0) 	// Checksum pass or fail
    {
    	MAIN_DEBUG_ERR(MC33816, ("MC33816 Checksum_check() failed\n"));
    	return -1;
    };
    if (Driver_Status_Init() == 0) // Check if all regulators are at ON
    {
    	MAIN_DEBUG_ERR(MC33816, ("MC33816 Driver_Status_Init() failed\n"));
    	return -1;
    };
    if (Bootstrap_check() == 0) // Check if bootstrap caps are charged (should be done after 35ms)
    {
    	MAIN_DEBUG_ERR(MC33816, ("MC33816 Bootstrap_check() failed\n"));
    	return -1;
    };

    // Add code to enable interrupts
    flag_IRQB = 0;
    uint8_t pin_IRQ = GET_IRQ_STATE();
    if(pin_IRQ == 0)
	{
		MAIN_DEBUG_ERR(MC33816, ("MC33816 IRQ state 0\n"));
		return -1;
	};
    uint8_t pin_FLAG0 = GET_FLAG0_STATE();
    if(pin_FLAG0 == 0)
	{
    	//green led on devkit
    	MAIN_DEBUG_ERR(MC33816, ("MC33816 FLAG0 state 0\n"));
		return -1;
	};
    uint8_t pin_FLAG1 = GET_FLAG1_STATE();
    if(pin_FLAG1 == 0)
	{
    	//green led on devkit
    	MAIN_DEBUG_ERR(MC33816, ("MC33816 FLAG1 state 0\n"));
		return -1;
	};
    uint8_t pin_FLAG2 = GET_FLAG2_STATE();
    if(pin_FLAG2 == 0)
	{
    	//green led on devkit
    	MAIN_DEBUG_ERR(MC33816, ("MC33816 FLAG2 state 0\n"));
		return -1;
	};

    SET_DRVEN_HIGH;
    delay(100);         // Wait before sending start pulse to let DCDC reach 65V

    return 0;
}

int MC33816_process(void)
{
	//========================================================================================
	// START OF IRQB CHECK
	//========================================================================================
	if (flag_IRQB == 1)
	{
		flag_IRQB = 0;

		// Read interrupt register
		// This latches only the first interrupt and only gets cleared when all interrupt are cleared
		interrupt_reg = send_single_SPI_Cmd(READ, main_Interrupt_register, 0x00);

		if (interrupt_reg != 0)
		{
			// Driver disable IRQ
			drv_irq = interrupt_reg & 0x1F00;
			if (drv_irq != 0)  // One of the halt bits is at 1
			{
				ProcessDriverInterrupts();
			}

			// Automatic IRQ (halt bits)
			auto_irq = interrupt_reg & 0x000F;
			if (auto_irq != 0)  // One of the halt bits is at 1
			{
				ProcessAutomaticInterrupts();
				ProcessSoftwareInterrupts();  // Needed in case a SW interrupt also happened
			}

			// Software IRQ (irq bits)
			sw_irq = interrupt_reg & 0x00F0;
			if (sw_irq != 0)  // One of the irq bits is at 1
			{
				ProcessSoftwareInterrupts();
				ProcessAutomaticInterrupts();
			}
		} // End of Interrupt auto SW interrupt
	}

	return 0;
} // End of for loop (main)

