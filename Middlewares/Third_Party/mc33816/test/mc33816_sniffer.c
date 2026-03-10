/**
 * @file mc33816_sniffer.c
 * @brief SPI slave sniffer implementation for MC33816 communication monitoring
 * 
 * This allows the STM32H7 to passively monitor SPI communication between
 * the KL25Z master and MC33816 slave device.
 * 
 * USAGE:
 * 1. Configure SPI1 to support SLAVE mode via CubeMX (or use dual-mode config)
 *    - Match clock settings (CPOL/CPHA) with the master SPI
 *    - Enable SPI1 global interrupt in NVIC
 * 
 * 2. Connect hardware (use buffers to avoid loading the bus):
 *    - SCK  -> KL25Z SCK line
 *    - MISO -> MC33816 MISO line (monitor data from MC33816)
 *    - MOSI -> MC33816 MOSI line (monitor data to MC33816)
 *    - GND  -> Common ground
 * 
 * 3. Initialize and start sniffer:
 *    MC33816_Sniffer_Init();
 *    MC33816_Sniffer_Start();
 * 
 * 4. Add to HAL_SPI_RxCpltCallback() in stm32h7xx_it.c:
 *    void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi) {
 *        if(hspi->Instance == SPI1) {
 *            MC33816_Sniffer_RxCallback(hspi);
 *        }
 *    }
 * 
 * 5. Process data in main loop:
 *    MC33816_Sniffer_Process();        // Process captured data
 *    MC33816_Sniffer_PrintBuffer();    // Print decoded transactions
 *    MC33816_Sniffer_ClearBuffer();    // Clear for next capture
 * 
 * See SPI_SNIFFER_USAGE.md for complete documentation.
 */

#include "mc33816_sniffer.h"
#include "main.h"
#include <string.h>

/* External SPI handle from mc33816.c */
extern SPI_HandleTypeDef hspi1;

/*******************************************************************************
 * PRIVATE DEFINITIONS
 ******************************************************************************/

#define SNIFFER_BUFFER_SIZE		512		// Number of bytes to capture
#define SNIFFER_PACKET_SIZE		64		// Max size per packet
#define SNIFFER_MAX_TRANSACTIONS	128	// Max number of transactions to track

typedef struct {
	uint8_t data[SNIFFER_BUFFER_SIZE];
	uint16_t head;
	uint16_t tail;
	uint16_t count;
	uint32_t packet_count;
	uint32_t overflow_count;
	uint8_t is_capturing;
} MC33816_SnifferBuffer_t;

typedef struct {
	uint32_t timestamp;		// HAL_GetTick() timestamp
	uint16_t ctrl_word;		// Control word (R/W, address, count)
	uint16_t data_offset;	// Offset in data buffer where payload starts
	uint8_t data_length;	// Number of data bytes
	uint8_t is_read;		// 1 = read, 0 = write
} MC33816_Transaction_t;

typedef struct {
	MC33816_Transaction_t transactions[SNIFFER_MAX_TRANSACTIONS];
	uint16_t count;
	uint16_t head;
} MC33816_TransactionLog_t;

/*******************************************************************************
 * PRIVATE VARIABLES
 ******************************************************************************/

static MC33816_SnifferBuffer_t sniffer_buffer = {0};
static MC33816_TransactionLog_t transaction_log = {0};
static uint8_t sniffer_rx_buffer[SNIFFER_PACKET_SIZE];
static volatile uint8_t sniffer_transfer_complete = 0;
static uint32_t capture_timestamp = 0;

/*******************************************************************************
 * PUBLIC FUNCTIONS
 ******************************************************************************/

/**
 * @brief Initialize SPI peripheral in slave mode for sniffing
 * @note Uses hspi1 in slave mode for monitoring
 * @retval HAL status
 */
HAL_StatusTypeDef MC33816_Sniffer_Init(void)
{
	// Clear buffer
	memset(&sniffer_buffer, 0, sizeof(sniffer_buffer));
	memset(&transaction_log, 0, sizeof(transaction_log));
	memset(sniffer_rx_buffer, 0, sizeof(sniffer_rx_buffer));
	
	MAIN_DEBUG_TRACE(MC33816, ("MC33816_Sniffer: Initialized\n"));
	
	return HAL_OK;
}

/**
 * @brief Start capturing SPI data in slave mode
 * @note Uses hspi1 for sniffing
 * @retval HAL status
 */
HAL_StatusTypeDef MC33816_Sniffer_Start(void)
{
	sniffer_buffer.is_capturing = 1;
	sniffer_transfer_complete = 0;
	
	// Start reception in interrupt mode using hspi1
	HAL_StatusTypeDef ret = HAL_SPI_Receive_IT(&hspi1, sniffer_rx_buffer, SNIFFER_PACKET_SIZE);
	
	if(ret != HAL_OK)
	{
		MAIN_DEBUG_ERR(MC33816, ("MC33816_Sniffer: Failed to start reception: %d\n", ret));
		sniffer_buffer.is_capturing = 0;
		return ret;
	}
	
	MAIN_DEBUG_TRACE(MC33816, ("MC33816_Sniffer: Started capturing on SPI1\n"));
	
	return HAL_OK;
}

/**
 * @brief Stop capturing SPI data
 */
void MC33816_Sniffer_Stop(void)
{
	sniffer_buffer.is_capturing = 0;
	HAL_SPI_Abort_IT(&hspi1);
	
	MAIN_DEBUG_TRACE(MC33816, ("MC33816_Sniffer: Stopped capturing\n"));
}

/**
 * @brief Process captured data - call this periodically or from SPI callback
 */
void MC33816_Sniffer_Process(void)
{
	if(!sniffer_transfer_complete)
		return;
	
	sniffer_transfer_complete = 0;
	uint16_t data_start_pos = sniffer_buffer.head;
	
	// Store received data in circular buffer
	for(uint16_t i = 0; i < SNIFFER_PACKET_SIZE; i++)
	{
		if(sniffer_buffer.count >= SNIFFER_BUFFER_SIZE)
		{
			// Buffer overflow - overwrite oldest data
			sniffer_buffer.overflow_count++;
			sniffer_buffer.tail = (sniffer_buffer.tail + 1) % SNIFFER_BUFFER_SIZE;
			sniffer_buffer.count--;
		}
		
		sniffer_buffer.data[sniffer_buffer.head] = sniffer_rx_buffer[i];
		sniffer_buffer.head = (sniffer_buffer.head + 1) % SNIFFER_BUFFER_SIZE;
		sniffer_buffer.count++;
	}
	
	sniffer_buffer.packet_count++;
	
	// Parse transactions from the captured packet and log with timestamps
	if(SNIFFER_PACKET_SIZE >= 2)
	{
		uint16_t pos = 0;
		while(pos + 1 < SNIFFER_PACKET_SIZE)
		{
			// Read control word
			uint16_t ctrl_word = (sniffer_rx_buffer[pos] << 8) | sniffer_rx_buffer[pos + 1];
			
			// Skip if it looks like padding (all zeros or all FFs)
			if(ctrl_word == 0x0000 || ctrl_word == 0xFFFF)
			{
				pos++;
				continue;
			}
			
			uint8_t rw = (ctrl_word >> 15) & 0x01;
			uint16_t offset = (ctrl_word >> 5) & 0x3FF;
			uint8_t number = ctrl_word & 0x1F;
			
			// Add transaction to log if there's space
			if(transaction_log.count < SNIFFER_MAX_TRANSACTIONS)
			{
				MC33816_Transaction_t *txn = &transaction_log.transactions[transaction_log.head];
				txn->timestamp = capture_timestamp;
				txn->ctrl_word = ctrl_word;
				txn->is_read = rw;
				txn->data_offset = (data_start_pos + pos + 2) % SNIFFER_BUFFER_SIZE;
				txn->data_length = number * 2;
				
				transaction_log.head = (transaction_log.head + 1) % SNIFFER_MAX_TRANSACTIONS;
				transaction_log.count++;
			}
			
			// Move to next transaction
			pos += 2 + (number * 2);
			
			if(number == 0)
				pos++;  // Avoid infinite loop on malformed data
		}
	}
	
	MAIN_DEBUG_TRACE(MC33816, ("MC33816_Sniffer: Packet #%lu captured (%u bytes buffered, %u transactions logged)\n", 
		sniffer_buffer.packet_count, sniffer_buffer.count, transaction_log.count));
}

/**
 * @brief Print captured buffer contents
 */
void MC33816_Sniffer_PrintBuffer(void)
{
	if(sniffer_buffer.count == 0)
	{
		//MAIN_DEBUG_TRACE(MC33816, ("  Buffer is empty\n"));
		return;
	}

	MAIN_DEBUG_TRACE(MC33816, ("MC33816_Sniffer: Buffer Status\n"));
	MAIN_DEBUG_TRACE(MC33816, ("  Packets captured: %lu\n", sniffer_buffer.packet_count));
	MAIN_DEBUG_TRACE(MC33816, ("  Buffer count: %u bytes\n", sniffer_buffer.count));
	MAIN_DEBUG_TRACE(MC33816, ("  Overflow count: %lu\n", sniffer_buffer.overflow_count));
	MAIN_DEBUG_TRACE(MC33816, ("  Is capturing: %u\n", sniffer_buffer.is_capturing));
	
	MAIN_DEBUG_TRACE(MC33816, ("\nCaptured Data:\n"));
	
	uint16_t pos = sniffer_buffer.tail;
	for(uint16_t i = 0; i < sniffer_buffer.count; i++)
	{
		if(i % 16 == 0)
		{
			if(i > 0)
				MAIN_DEBUG_TRACE(MC33816, ("\n"));
			MAIN_DEBUG_TRACE(MC33816, ("%04X: ", i));
		}
		
		MAIN_DEBUG_TRACE(MC33816, ("%02X ", sniffer_buffer.data[pos]));
		pos = (pos + 1) % SNIFFER_BUFFER_SIZE;
	}
	
	MAIN_DEBUG_TRACE(MC33816, ("\n\n"));
	
	// Try to decode MC33816 protocol
	// Print decoded transactions with timestamps
	if(transaction_log.count > 0)
	{
		MAIN_DEBUG_TRACE(MC33816, ("Decoded Transactions (with timestamps):\n"));
		MAIN_DEBUG_TRACE(MC33816, ("Total transactions: %u\n\n", transaction_log.count));
		
		uint16_t start = (transaction_log.count >= SNIFFER_MAX_TRANSACTIONS) ? 
			transaction_log.head : 0;
		uint16_t count = (transaction_log.count >= SNIFFER_MAX_TRANSACTIONS) ? 
			SNIFFER_MAX_TRANSACTIONS : transaction_log.count;
		
		for(uint16_t i = 0; i < count; i++)
		{
			uint16_t idx = (start + i) % SNIFFER_MAX_TRANSACTIONS;
			MC33816_Transaction_t *txn = &transaction_log.transactions[idx];
			
			uint16_t offset = (txn->ctrl_word >> 5) & 0x3FF;
			uint8_t number = txn->ctrl_word & 0x1F;
			
			MAIN_DEBUG_TRACE(MC33816, ("[%lu ms] %s Addr:0x%03X Count:%u", 
				txn->timestamp,
				txn->is_read ? "READ " : "WRITE", 
				offset, 
				number));
			
			// Display data bytes
			if(txn->data_length > 0 && txn->data_length <= 64)
			{
				MAIN_DEBUG_TRACE(MC33816, (" Data:"));
				uint16_t data_pos = txn->data_offset;
				for(uint8_t j = 0; j < txn->data_length; j++)
				{
					MAIN_DEBUG_TRACE(MC33816, (" %02X", sniffer_buffer.data[data_pos]));
					data_pos = (data_pos + 1) % SNIFFER_BUFFER_SIZE;
				}
			}
			MAIN_DEBUG_TRACE(MC33816, ("\n"));
		}
	}
	else
	{
		// Fallback to old decoding method if no transactions logged
		MAIN_DEBUG_TRACE(MC33816, ("Decoded Transactions:\n"));
	pos = sniffer_buffer.tail;
	uint16_t remaining = sniffer_buffer.count;
	
	while(remaining >= 2)
	{
		// Read control word
		uint16_t ctrl_word = (sniffer_buffer.data[pos] << 8);
		pos = (pos + 1) % SNIFFER_BUFFER_SIZE;
		remaining--;
		
		ctrl_word |= sniffer_buffer.data[pos];
		pos = (pos + 1) % SNIFFER_BUFFER_SIZE;
		remaining--;
		
		uint8_t rw = (ctrl_word >> 15) & 0x01;
		uint16_t offset = (ctrl_word >> 5) & 0x3FF;
		uint8_t number = ctrl_word & 0x1F;
		
		MAIN_DEBUG_TRACE(MC33816, ("  %s Addr:0x%03X Count:%u\n", 
			rw ? "READ " : "WRITE", offset, number));
		
		// Display data bytes
		uint16_t data_bytes = number * 2;
		if(data_bytes > remaining)
			data_bytes = remaining;
		
		if(data_bytes > 0)
		{
			MAIN_DEBUG_TRACE(MC33816, ("    Data:"));
			for(uint16_t i = 0; i < data_bytes; i++)
			{
				MAIN_DEBUG_TRACE(MC33816, (" %02X", sniffer_buffer.data[pos]));
				pos = (pos + 1) % SNIFFER_BUFFER_SIZE;
				remaining--;
			}
			MAIN_DEBUG_TRACE(MC33816, ("\n"));
		}
		
		// Avoid infinite loop
		if(number == 0 && remaining > 0)
		{
			pos = (pos + 1) % SNIFFER_BUFFER_SIZE;
			remaining--;
		}
	}
	}
}

/**
 * @brief Clear sniffer buffer
 */
void MC33816_Sniffer_ClearBuffer(void)
{
	sniffer_buffer.head = 0;
	sniffer_buffer.tail = 0;
	sniffer_buffer.count = 0;
	sniffer_buffer.packet_count = 0;
	sniffer_buffer.overflow_count = 0;
	
	//MAIN_DEBUG_TRACE(MC33816, ("MC33816_Sniffer: Buffer cleared\n"));
}

/**
 * @brief SPI Receive Complete Callback - call this from HAL_SPI_RxCpltCallback
 * @param hspi: Pointer to SPI handle
 */
void MC33816_Sniffer_RxCallback(SPI_HandleTypeDef *hspi)
{
	if(sniffer_buffer.is_capturing)
	{
		// Capture timestamp immediately when data arrives
		capture_timestamp = HAL_GetTick();
		sniffer_transfer_complete = 1;
		
		// Restart reception for next packet
		HAL_SPI_Receive_IT(hspi, sniffer_rx_buffer, SNIFFER_PACKET_SIZE);
	}
}

void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{
    if(hspi->Instance == SPI1)
    {
        MC33816_Sniffer_RxCallback(hspi);
    }
}
