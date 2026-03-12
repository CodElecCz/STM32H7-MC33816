/*
 * mc33816_dram.c
 *
 *  DRAM Source File
 *
 */

#include <stddef.h>

#include "mc33816_dram.h"
#include "mc33816.h"
#include "mc33816_spi_map.h"
#include "mc33816_spi.h"

void DRAM_Parameter_Read(uint8_t param_addr, uint16_t* value)
{
	uint16_t rxData = 0;

	// Check if value pointer is valid
	if (value == NULL)
	{
		return;
	}

	// First, switch to DRAM by selecting COMMON_PAGE memory area
	// This is required before accessing DRAM parameters
	rxData = send_single_SPI_Cmd(WRITE, selection_register, COMMON_PAGE);

	// Read the parameter value from the specified DRAM address
	// The first parameter (READ) indicates this is a READ operation
	// param_addr is the DRAM address (0x00-0x08 for DRAM1, 0x40-0x48 for DRAM2)
	// 0x0000 is the dummy TX data for read operations
	*value = send_single_SPI_Cmd(READ, param_addr, 0x0000);
}

void DRAM_Parameter_Write(uint8_t param_addr, uint16_t value)
{
	uint16_t rxData = 0;

	// First, switch to DRAM by selecting COMMON_PAGE memory area
	// This is required before accessing DRAM parameters
	rxData = send_single_SPI_Cmd(WRITE, selection_register, COMMON_PAGE);

	// Write the parameter value to the specified DRAM address
	// The first parameter (WRITE) indicates this is a WRITE operation
	// param_addr is the DRAM address (0x00-0x08 for DRAM1, 0x40-0x48 for DRAM2)
	// value is the 16-bit parameter value to write
	rxData = send_single_SPI_Cmd(WRITE, param_addr, value);
}
