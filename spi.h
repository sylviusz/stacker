#ifndef SPI_H
#define SPI_H

#include <avr/io.h>
#include <avr/interrupt.h>

// PINY
#define CS_PIN    PB2  // D10 (CS/Load)
#define MOSI_PIN  PB3  // D11 (DIN)
#define SCK_PIN   PB5  // D13 (CLK)

// Liczba segmentow w lancuchu
#define NUMBER_OF_SEGMENT 4

#ifdef __cplusplus
extern "C" {
#endif

extern volatile uint8_t spi_send_done;

void spi_transfer(uint8_t data);
void spi_send_nonblocking(uint8_t *data, uint8_t len);
void max_send_all(uint8_t reg, uint8_t data);

#ifdef __cplusplus
}
#endif

#endif // SPI_H
