# STM32H7 - MC33816

This repository contains firmware and project files for integrating the MC33816 injector driver with an STM32H7 microcontroller. This STM32H7-based board is the control board for the MC33816 development kit and communicates with the MC33816 over SPI. The MC33816 is used as a fuel injector / injector driver in automotive and embedded motor-control applications. The project includes STM32CubeMX (.ioc) configuration, STM32CubeIDE launch files, linker scripts and source folders produced by STM32CubeIDE.

Keywords: mc33816, MC33816, injector, injector driver, fuel injector, STM32H7, STM32CubeIDE, STM32CubeMX, firmware, embedded, motor driver, NXP, CAN, SPI, hardware integration, example project, control board, dev kit, ECU.

## MC33816 Development Kit

The **MC33816 Fuel Injector Driver** is a high-performance integrated circuit from NXP designed for automotive engine control applications. The MC33816 provides precise control of fuel injectors with features including SPI communication, advanced diagnostics, peak-and-hold drive capability, and integrated protection mechanisms.

The development kit combines the **NUCLEO-H753ZI** board (STM32H753ZI microcontroller) as the control platform with the **KIT33816FRDMUG** evaluation board for the MC33816 injector driver IC. This setup provides a complete reference platform for developing and testing fuel injection control algorithms.

### Documentation

- **NUCLEO-H753ZI Schematics**: [mb1364-h753zi-c01-schematic.pdf](Documents/h753zi-nucleo/mb1364-h753zi-c01-schematic.pdf)
- **NUCLEO-H753ZI User Manual**: [um2407-stm32h7-nucleo144-boards-mb1364-stmicroelectronics.pdf](Documents/h753zi-nucleo/um2407-stm32h7-nucleo144-boards-mb1364-stmicroelectronics.pdf)
- **MC33816 Development Kit User Guide**: [KIT33816FRDMUG.pdf](Documents/mc33816/KIT33816FRDMUG.pdf)
- **MC33816 Datasheet**: [MC33816.pdf](Documents/mc33816/MC33816.pdf)

### Hardware Connections

The following table shows the pin connections between the NUCLEO-H753ZI board and the MC33816 evaluation kit (KIT33816FRDMUG):

![MC33816 Connection Schematic](Documents/mc33816%20schematic.png)

#### SPI Communication Interface

| Signal      | NUCLEO-H753ZI Pin | MC33816 Kit Pin | Description                          |
|-------------|-------------------|-----------------|--------------------------------------|
| SPI_MOSI    | PA7 (SPI1_MOSI)   | MOSI            | SPI Master Out Slave In              |
| SPI_MISO    | PA6 (SPI1_MISO)   | MISO            | SPI Master In Slave Out              |
| SPI_SCK     | PA5 (SPI1_SCK)    | SCK             | SPI Clock (add 2k7 pull-down)        |
| SPI_CS      | PD14              | CS              | SPI Chip Select (active low)         |

#### Control and Status Signals

| Signal      | NUCLEO-H753ZI Pin | MC33816 Kit Pin | Description                          |
|-------------|-------------------|-----------------|--------------------------------------|
| MC_RST      | PA4               | RESET           | MC33816 Reset (active low)           |
| MC_DRV      | PB4               | DRV_EN          | Driver Enable                        |
| MC_IRQ      | PE7               | IRQ             | Interrupt Request (falling edge)     |
| MC_FLAG0    | PG14              | FLAG0           | Status Flag 0                        |
| MC_FLAG1    | PF5               | FLAG1           | Status Flag 1                        |
| MC_FLAG2    | PF4               | FLAG2           | Status Flag 2                        |
| MC_FLAG3    | PG12              | FLAG3           | Status Flag 3                        |

#### Timing and Debug Signals

| Signal      | NUCLEO-H753ZI Pin | MC33816 Kit Pin | Description                          |
|-------------|-------------------|-----------------|--------------------------------------|
| MC_CLK      | PF6 (TIM16_CH1)   | CLK             | External Clock Input                 |
| MC_DBG      | PE8               | DEBUG           | Debug Signal                         |

#### Analog Inputs

| Signal      | NUCLEO-H753ZI Pin | MC33816 Kit Pin | Description                          |
|-------------|-------------------|-----------------|--------------------------------------|
| MC_OA_1     | PA0 (ADC1_INP16)  | OA_OUT1         | Operational Amplifier Output 1       |
| MC_OA_2     | PA1 (ADC1_INP17)  | OA_OUT2         | Operational Amplifier Output 2       |

**Note**: MC_OA_1 and MC_OA_2 signals require voltage divider (5V to 3V) to protect STM32H7 ADC inputs. The MC33816 outputs operate at 5V while STM32H7 ADC inputs are 3.3V tolerant.

#### Injector Start Signals (PWM Control)

| Signal      | NUCLEO-H753ZI Pin | MC33816 Kit Pin | Description                          |
|-------------|-------------------|-----------------|--------------------------------------|
| MC_START1   | PB9               | START1          | Injector 1 Start Signal              |
| MC_START2   | PB15              | START2          | Injector 2 Start Signal              |
| MC_START3   | PC7               | START3          | Injector 3 Start Signal              |
| MC_START4   | PB8               | START4          | Injector 4 Start Signal              |
| MC_START5   | PB12              | START5          | Injector 5 Start Signal              |
| MC_START6   | PC6               | START6          | Injector 6 Start Signal              |

#### Power and Ground

- Connect **GND** pins between NUCLEO-H753ZI and MC33816 kit
- Ensure proper power supply to MC33816 kit (refer to KIT33816FRDMUG documentation)
- MC33816 kit requires external power for high-current injector drivers

**Note**: Verify all pin assignments match your hardware configuration by reviewing the .ioc file in STM32CubeMX before connecting hardware.

## Purpose

- Provide a reference and starting project for developers integrating the MC33816 injector driver with STM32H7-based systems.
- Consolidate board- and MCU-specific configuration so the project can be opened, built and flashed from STM32CubeIDE or other ARM toolchains.

## Repository contents

- Core/            - MCU application source and startup files
- Drivers/         - HAL and peripheral drivers
- Middlewares/     - Third-party middleware (LWIP, mbedTLS, etc.)
- USB_DEVICE/      - USB device stack (if used)
- web/             - Web assets (if any)
- Documents/       - Additional documentation
- STM32H7-MC33816.ioc - STM32CubeMX project file
- STM32H7-MC33816 Debug.launch / Release.launch - IDE launch configurations
- STM32H753ZITX_FLASH.ld / STM32H753ZITX_RAM.ld - Linker scripts

## Getting started

1. Clone the repository:
   git clone https://github.com/CodElecCz/STM32H7-MC33816.git

2. Open the project in STM32CubeIDE by opening STM32H7-MC33816.ioc or import the project workspace.

3. Generate code from CubeMX if needed, build the project, and flash using ST-Link or another programmer via the IDE launch configurations.

## Notes and best practices

- Review and adapt pinout and clock configuration to match your hardware. The provided .ioc file contains the project's configuration used by CubeMX.
- Verify MCU and memory settings in the linker scripts before flashing to custom hardware.
- Confirm SPI pin assignments and timing when connecting to the MC33816 injector driver.
- Keep any sensitive keys, credentials, or private data out of the repository.

## Contributing

Contributions, bug reports and improvements are welcome. Open issues or submit pull requests with a description of the change and tested platforms.

## License

Add a LICENSE file or specify your preferred license (e.g., MIT, Apache-2.0).

## Contact

For questions or support, open an issue in this repository or contact the maintainer: https://github.com/CodElecCz