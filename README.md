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

## 🖨️ Hardware Wiring Schematic

```text
       STM32F103C8T6 "Blue Pill"
             +--------------+
        VBAT | 1         40 | 5V
        PC13 | 2         39 | GND
        PC14 | 3         38 | 3.3V
        PC15 | 4         37 | RESET
 [LED] <-PA0 | 5         36 | PB12
[SENS] ->PA1 | 6         35 | PB13
         PA2 | 7         34 | PB14
         PA3 | 8         33 | PB15
         PA4 | 9         32 | PA8
         PA5 | 10        31 | PA9 
         PA6 | 11        30 | PA10
         PA7 | 12        29 | PA11
         PB0 | 13        28 | PA12
         PB1 | 14        27 | PA15
        PB10 | 15        26 | PB3
        PB11 | 16        25 | PB4
       RESET | 17        24 | PB5
 [VCC] <-3.3V| 18        23 | PB6
 [GND] <-GND | 19        22 | PB7
         GND | 20        21 | PB8
             +--------------+
                [ST-LINK]

---------------------------------------------------
WIRING PATHS:
PA0 (Pin 5)  -> 220Ω Resistor -> LED (Long Leg)
PA1 (Pin 6)  <- Hacked Analog Out on TCRT5000
3.3V(Pin 18) -> VCC on TCRT5000 Module
GND (Pin 19) -> GND on TCRT5000 AND LED (Short Leg)
---------------------------------------------------