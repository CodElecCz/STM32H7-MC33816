# MC33816 SPI Slave Sniffer

## Overview
This implementation provides a passive SPI sniffer that allows the STM32H7 to monitor SPI communication between a KL25Z master controller and an MC33816 slave device without interfering with the communication.

## Files
- **mc33816_sniffer.h** - Header file with sniffer function prototypes
- **mc33816_sniffer.c** - Implementation of sniffer functionality
- **SPI_SNIFFER_USAGE.md** - Complete usage documentation
- **README_SNIFFER.md** - This file

## Features
✅ Passive monitoring - does not interfere with communication  
✅ Circular buffer storage for captured data  
✅ Automatic MC33816 protocol decoding  
✅ Hex dump and formatted output  
✅ Interrupt-driven reception  
✅ Buffer overflow detection  
✅ Easy integration with existing code  
✅ Uses existing hspi1 handle (no separate SPI peripheral needed)

## Quick Start

### 1. Hardware Setup
Connect your STM32H7 SPI1 in slave mode:
- **SCK** → Monitor KL25Z SCK line
- **MISO** → Monitor MC33816 MISO line
- **MOSI** → Monitor MC33816 MOSI line
- **GND** → Common ground

⚠️ **Important:** Use high-impedance buffers to avoid affecting the bus!

### 2. CubeMX Configuration
1. Enable SPI1 in **Slave Mode** (Receive Only Slave)
2. Match CPOL/CPHA with the master SPI
3. Enable SPI1 global interrupt in NVIC
4. Generate code

### 3. Code Integration

**In your main.c:**
```c
#include "mc33816_sniffer.h"

void StartMonitoring(void)
{
    MC33816_Sniffer_Init();
    MC33816_Sniffer_Start();
}

void MainLoop(void)
{
    while(1)
    {
        MC33816_Sniffer_Process();        // Process captured data
        
        // Print every 5 seconds
        if(/* 5 seconds elapsed */)
        {
            MC33816_Sniffer_PrintBuffer();
            MC33816_Sniffer_ClearBuffer();
        }
    }
}
```

**In stm32h7xx_it.c:**
```c
#include "mc33816_sniffer.h"

void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{
    if(hspi->Instance == SPI1)
    {
        MC33816_Sniffer_RxCallback(hspi);
    }
}
```

## API Functions

| Function | Description |
|----------|-------------|
| `MC33816_Sniffer_Init()` | Initialize the sniffer (uses hspi1) |
| `MC33816_Sniffer_Start()` | Start capturing data (uses hspi1) |
| `MC33816_Sniffer_Stop()` | Stop capturing |
| `MC33816_Sniffer_Process()` | Process captured data (call periodically) |
| `MC33816_Sniffer_PrintBuffer()` | Print captured data and decode protocol |
| `MC33816_Sniffer_ClearBuffer()` | Clear buffer |
| `MC33816_Sniffer_RxCallback()` | Internal callback (call from HAL callback) |

## Output Example
```
MC33816_Sniffer: Buffer Status
  Packets captured: 5
  Buffer count: 80 bytes
  Overflow count: 0
  Is capturing: 1

Captured Data:
0000: 7F E1 00 04 AA BB 00 00 39 01 00 1F AA BB 00 00
0010: 20 E1 00 A8 AA BB 00 00 07 E1 00 01 AA BB 00 00
...

Decoded Transactions:
  WRITE Addr:0x3FF Count:1
    Data: 00 04
  WRITE Addr:0x1C8 Count:1
    Data: 00 1F
  WRITE Addr:0x107 Count:1
    Data: 00 A8
```

## Configuration
Adjust these defines in mc33816_sniffer.c:
```c
#define SNIFFER_BUFFER_SIZE    512    // Total buffer size
#define SNIFFER_PACKET_SIZE    64     // Bytes per capture
```

## Documentation
See **SPI_SNIFFER_USAGE.md** for complete documentation including:
- Detailed hardware setup
- Troubleshooting guide
- Advanced usage
- Protocol decoding details
- Complete code examples

## Notes
- The sniffer is completely passive and transparent to the master/slave communication
- Uses the existing hspi1 handle - no separate SPI peripheral configuration needed
- Ensure all SPI parameters match between master and sniffer (CPOL, CPHA, bit order)
- Buffer overflow occurs if data isn't processed fast enough - increase buffer size if needed
- Debug output uses `MAIN_DEBUG_TRACE()` macros from main.h

## License
Same as parent project

## Author
Created for STM32H7-MC33816 project
