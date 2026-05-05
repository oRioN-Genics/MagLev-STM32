#ifndef SPI_H
#define SPI_H

#include <stdint.h>

void SPI1_Init(void);
void SPI_TransmitByte(uint8_t data);

#endif