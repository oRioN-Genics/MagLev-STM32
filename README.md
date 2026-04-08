# Magnetic Levitation System

## 🛠️ Hardware Platform
This project is built for the standard **STM32 "Blue Pill"** development board.
* **Microcontroller:** STM32F103C8T6 (ARM Cortex-M3 core)
* **Architecture:** 32-bit
* **Max Clock Speed:** 72 MHz
* **Framework:** Embedded C (No HAL or standard peripheral libraries)

## 📚 Reference Documentation
This project relies on direct register manipulation. To understand the configurations, refer to the following official STMicroelectronics documentation:

* [RM0008 Reference Manual](https://www.st.com/resource/en/reference_manual/rm0008-stm32f101xx-stm32f102xx-stm32f103xx-stm32f105xx-and-stm32f107xx-advanced-armbased-32bit-mcus-stmicroelectronics.pdf) - Contains the memory addresses, register maps, and peripheral descriptions.
* [STM32F103xB Datasheet](https://www.st.com/resource/en/datasheet/stm32f103c8.pdf) - Contains chip-specific pinout diagrams and electrical characteristics.
* [PM0056 Programming Manual](https://www.st.com/resource/en/programming_manual/pm0056-stm32f10xxx20xxx21xxxl1xxxx-cortexm3-programming-manual-stmicroelectronics.pdf) - Details for the ARM Cortex-M3 core, SysTick, and NVIC.