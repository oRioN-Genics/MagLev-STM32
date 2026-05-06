#include "spi.h"
#include "stm32f1xx.h"

void SPI1_Init(void) {
    // Enable clocks for GPIOA and SPI1
    RCC->APB2ENR |= (1 << 2);  
    RCC->APB2ENR |= (1 << 12); 

    // Configure Pins
    // Clear config for PA2, PA3, PA4, PA5, PA7
    GPIOA->CRL &= ~((0xF << 8) | (0xF << 12) | (0xF << 16) | (0xF << 20) | (0xF << 28));
    
    // PA2 (RES), PA3 (DC), PA4 (CS) -> Output Push-Pull 50MHz (0x3)
    GPIOA->CRL |= (0x3 << 8) | (0x3 << 12) | (0x3 << 16);
    // PA5 (SCK), PA7 (MOSI) -> Alternate Function Push-Pull 50MHz (0xB)
    GPIOA->CRL |= (0xB << 20) | (0xB << 28);

    // Set CS, DC, and RES High by default
    GPIOA->ODR |= (1 << 4) | (1 << 3) | (1 << 2);

    // 3. Configure SPI1 (Master, fPCLK/2, CPOL=1, CPHA=1, SSM=1, SSI=1)
    SPI1->CR1 = (1 << 2) | (1 << 1) | (1 << 0) | (1 << 9) | (1 << 8);

    // 4. Enable SPI
    SPI1->CR1 |= (1 << 6);
}

void SPI_TransmitByte(uint8_t data) {
    // Wait until Transmit Buffer is Empty (TXE)
    while (!(SPI1->SR & (1 << 1))) {}
    
    // Send data
    SPI1->DR = data;
    
    // Wait until SPI is not busy (BSY)
    while (SPI1->SR & (1 << 7)) {}
}