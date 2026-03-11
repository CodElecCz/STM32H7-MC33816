# STM32H7 - MC33816

This repository contains firmware and project files for integrating the MC33816 injector driver with an STM32H7 microcontroller. This STM32H7-based board is the control board for the MC33816 development kit and communicates with the MC33816 over SPI. The MC33816 is used as a fuel injector / injector driver in automotive and embedded motor-control applications. The project includes STM32CubeMX (.ioc) configuration, STM32CubeIDE launch files, linker scripts and source folders produced by STM32CubeIDE.

Keywords: mc33816, MC33816, injector, injector driver, fuel injector, STM32H7, STM32CubeIDE, STM32CubeMX, firmware, embedded, motor driver, NXP, CAN, SPI, hardware integration, example project, control board, dev kit, ECU.

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