#include "mc33816_test.h"
#include "main.h"

#include <stdio.h>
#include <string.h>

extern SPI_HandleTypeDef hspi1;

/*
	Control_word [15] r_w: 									read (1)/write (0) operations
	Control_word [14:5] offset: start address               0-1023
	Control_word [4:0] number: number of operations         0-31

	Response [0] word error
	Response [1] frame error
	Response [2] cksys missing
	Response [3:15] check byte 1010101010101
 */

/*
Selection register (0x3FF)

	Register [0] CRAM ch1 select
	Register [1] CRAM ch2 select
	Register [2] common page select
	Register [3:15] reserved

	000 - no page selected
	001 - Code RAM of channel 1, address 0x0000-0x3FE
	010 - Code RAM of channel 2, address 0x0000-0x3FE
	011 - Write operation affects both channel’s Code RAM. Read operation is not possible in this case
	100 - Common page selected, address 0x0000-0x1FF, which includes:
			Data RAM of channel 1, address 0x0000-0x3F
			Data RAM of channel 2, address 0x0040-0x7F
			Configuration Registers of Channel 1, address 0x0080-0x11F
			Configuration Registers of Channel 2, address 0x0120-0x13F
			Diagnosis Configuration Registers, address 0x0140-0x17F
			IO Configuration Registers, address 0x0180-0x1BF
			Main Configuration Registers, address 0x01C0-0x1FF

	The first step consists in selecting the communication interface as target.
	The selection is done by writing the value 0x0004 at the Selection register address (0x3FF).
	The first 16-bit words to be sent is ‘0_1111111111_00001’ (0x07FE1).

	0x7F E1 00 04 Sent      0x7FE1 >> 5 = 0x3FF Selection register address, 04 - selecting the communication interface as target

Spi_config (0x1C8)

	0x39 01 00 1F Sent      0x3901 >> 5 = 0x1C8 Spi_config register, 1F - watchdog[0:4] + irq_en[5] + protocol_mode[6] + miso_slewrate[7]

	0_0111001000_00001 		0-write operation, offset 0x1C8, number of operations 1


 */

typedef enum _EMC_RegOp
{
	EMC_RegOp_Write		=	0,
	EMC_RegOp_Read	 	=	1
} EMC_RegOp;

typedef enum _EMC_RegAdd
{
	EMC_RegAdd_Code_width_ch1	=	0x107,
	EMC_RegAdd_Code_width_ch2	=	0x127,
	EMC_RegAdd_Spi_config		=	0x1C8,
	EMC_RegAdd_Driver_status	=	0x1D2,
	EMC_RegAdd_Spi_error		=	0x1D3,
	EMC_RegAdd_Interrupt_status	=	0x1D4,
	EMC_RegAdd_Selection_reg 	=	0x3ff
} EMC_RegAdd;

#define MC_DATA_SIZE_MAX		32

static void MC33816_HexDump(const uint8_t* data, size_t size)
{
	MAIN_DEBUG_TRACE(MC33816, ("MC33816:"));
	for (size_t i = 0; i < size; i++)
	{
		MAIN_DEBUG_TRACE(MC33816, (" %02X", data[i]));
	}
	MAIN_DEBUG_TRACE(MC33816, ("\n"));
}

HAL_StatusTypeDef MC33186_Register(EMC_RegOp op, EMC_RegAdd add, const uint8_t tx_data[], uint8_t rx_data[], size_t data_size)
{
	if(data_size%2 != 0)
		return HAL_ERROR;

	if(data_size > MC_DATA_SIZE_MAX)
		return HAL_ERROR;

	HAL_StatusTypeDef ret = HAL_OK;
	uint8_t TxData[MC_DATA_SIZE_MAX + 2];
	uint8_t RxData[MC_DATA_SIZE_MAX + 2];
	uint16_t Size = 0;

	memset(RxData, 0, sizeof(RxData));

	uint16_t control16 = (op << 15) | ((add & 0x3FF) << 5) | (data_size/2 & 0x1F);
	TxData[0] = (control16 >> 8) & 0xFF;
	TxData[1] = control16 & 0xFF;

	for (size_t i = 0; i < data_size; i++)
	{
		TxData[2 + i] = tx_data[i];
	}
	Size = 2 + data_size;

	MC33816_HexDump(TxData, Size);

#if 1
	//SPI_DATASIZE_8BIT
	HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_RESET);
	ret = HAL_SPI_TransmitReceive(&hspi1, TxData, RxData, Size, 100);
	HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_SET);
#else
	//SPI_DATASIZE_16BIT
	uint16_t tx_word = 0;
	uint16_t rx_word = 0;

#define MC_SPI_DELAY 10
	//for(volatile int i=0; i<MC_SPI_DELAY; i++) __NOP(); // Short delay
	HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_RESET);
	//for(volatile int i=0; i<MC_SPI_DELAY; i++) __NOP(); // Short delay

	for(size_t i = 0; i < Size/2; i++)
	{
		//First byte is MSB
		tx_word = (TxData[2*i] << 8) | TxData[2*i + 1];
		rx_word = 0;

		ret = HAL_SPI_TransmitReceive(&hspi1, (uint8_t*)&tx_word, (uint8_t*)&rx_word, 1, 100);
		if(ret != HAL_OK)
		{
			HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_SET);
			MAIN_DEBUG_ERR(MC33816, ("MC33816: HAL_SPI_TransmitReceive() error: %d\n", ret));
			return ret;
		}

		RxData[2*i] = (rx_word >> 8) & 0xFF; // MSB
		RxData[2*i + 1] = rx_word & 0xFF;    // LSB
	}

	HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_SET);
	//for(volatile int i=0; i<MC_SPI_DELAY; i++) __NOP(); // Short delay
#endif
	if(ret == HAL_OK)
	{
		MC33816_HexDump(RxData, Size);
		for (size_t i = 0; i < data_size; i++)
		{
			rx_data[i] = RxData[2 + i];
		}

		//check response - first 16-bit word contains status
		uint16_t response = (RxData[0] << 8) | RxData[1];
		
		//check error bits [0:2]
		if((response & 0x0001) != 0) //bit 0: word error
		{
			MAIN_DEBUG_ERR(MC33816, ("MC33816: word error\n"));
			ret = HAL_ERROR;
		}
		if((response & 0x0002) != 0) //bit 1: frame error
		{
			MAIN_DEBUG_ERR(MC33816, ("MC33816: frame error\n"));
			ret = HAL_ERROR;
		}
		if((response & 0x0004) != 0) //bit 2: cksys missing
		{
			MAIN_DEBUG_ERR(MC33816, ("MC33816: cksys missing\n"));
			ret = HAL_ERROR;
		}
		
		//check bits [3:15] should be alternating pattern 1010101010101 = 0x1555
		uint16_t check_pattern = (response >> 3) & 0x1FFF;
		if(check_pattern != 0x1555)
		{
			MAIN_DEBUG_ERR(MC33816, ("MC33816: invalid check pattern 0x%04X (expected 0x1555)\n", check_pattern));
			ret = HAL_ERROR;
		}
	}
	else
	{
		MAIN_DEBUG_ERR(MC33816, ("MC33816: SPI communication error\n"));
	}

	return ret;
}

void MC33186_Test()
{
	uint8_t TxData[MC_DATA_SIZE_MAX];
	uint8_t RxData[MC_DATA_SIZE_MAX];

	HAL_Delay(20);
	HAL_GPIO_WritePin(MC_DRV_GPIO_Port, MC_DRV_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(MC_START1_GPIO_Port, MC_START1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(MC_RST_GPIO_Port, MC_RST_Pin, GPIO_PIN_RESET);
	HAL_Delay(20);
	HAL_GPIO_WritePin(MC_RST_GPIO_Port, MC_RST_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(MC_DRV_GPIO_Port, MC_DRV_Pin, GPIO_PIN_SET);
	HAL_Delay(100);
#if 1
	//EMC_RegAdd_Selection_reg
	TxData[0] = 0x00;
	TxData[1] = 0x04;
	MC33186_Register(EMC_RegOp_Write, EMC_RegAdd_Selection_reg, TxData, RxData, 2);

	HAL_Delay(10);

	//EMC_RegAdd_Spi_config
	TxData[0] = 0x00;
	TxData[1] = 0x1F;
	MC33186_Register(EMC_RegOp_Write, EMC_RegAdd_Spi_config, TxData, RxData, 2);

	//EMC_RegAdd_Code_width_ch1
	TxData[0] = 0x00;
	TxData[1] = 0xA8;
	MC33186_Register(EMC_RegOp_Write, EMC_RegAdd_Code_width_ch1, TxData, RxData, 2);

	//EMC_RegAdd_Selection_reg
	TxData[0] = 0x00;
	TxData[1] = 0x01;
	MC33186_Register(EMC_RegOp_Write, EMC_RegAdd_Selection_reg, TxData, RxData, 2);

	HAL_Delay(10);
	HAL_GPIO_WritePin(MC_DRV_GPIO_Port, MC_DRV_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(MC_START1_GPIO_Port, MC_START1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(MC_RST_GPIO_Port, MC_RST_Pin, GPIO_PIN_RESET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(MC_RST_GPIO_Port, MC_RST_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(MC_DRV_GPIO_Port, MC_DRV_Pin, GPIO_PIN_SET);
	HAL_Delay(4);

	//EMC_RegAdd_Driver_status
	TxData[0] = 0x00;
	TxData[1] = 0x00;
	MC33186_Register(EMC_RegOp_Read, EMC_RegAdd_Driver_status, TxData, RxData, 2);

	//EMC_RegAdd_Interrupt_status
	TxData[0] = 0x00;
	TxData[1] = 0x00;
	MC33186_Register(EMC_RegOp_Read, EMC_RegAdd_Interrupt_status, TxData, RxData, 2);
#endif
#if 0
	//EMC_RegAdd_Spi_error
	TxData[0] = 0x00;
	TxData[1] = 0x00;
	MC33186_Register(EMC_RegOp_Write, EMC_RegAdd_Spi_error, TxData, RxData, 2);

	//EMC_RegAdd_Spi_error
	TxData[0] = 0x00;
	TxData[1] = 0x00;
	MC33186_Register(EMC_RegOp_Read, EMC_RegAdd_Spi_error, TxData, RxData, 2);
#endif
}



