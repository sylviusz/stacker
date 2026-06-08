#include "spi.h"

volatile uint8_t spi_tx_buf[16];
volatile uint8_t spi_tx_idx = 0;
volatile uint8_t spi_tx_len = 0;
volatile uint8_t spi_send_done = 1;

// Obsługa przerwania 
ISR(SPI_STC_vect) {
	spi_tx_idx++;
	if (spi_tx_idx < spi_tx_len) {
		SPDR = spi_tx_buf[spi_tx_idx];
	} else {
		PORTB |= (1 << CS_PIN); // Ustaw CS w stan wysoki (koniec)
		spi_send_done = 1;      // Ustaw flagę zakończenia
	}
}

// SPI blokujące jkaby bylo konieczne
void spi_transfer(uint8_t data) {
	SPDR = data;
	while (!(SPSR & (1 << SPIF)));
}

// SPI nieblokujące z użyciem przerwań 
void spi_send_nonblocking(uint8_t *data, uint8_t len) {
	while (!spi_send_done); // Oczekiwanie na zwolnienie bufora
	
	for (uint8_t i = 0; i < len; i++) {
		spi_tx_buf[i] = data[i];
	}
	spi_tx_len = len;
	spi_tx_idx = 0;
	spi_send_done = 0;
	
	PORTB &= ~(1 << CS_PIN); // Ustaw CS w stan niski 
	SPDR = spi_tx_buf[0];     // Rozpocznij wysyłanie pierwszego bajtu
}

void max_send_all(uint8_t reg, uint8_t data) {
	uint8_t buf[NUMBER_OF_SEGMENT * 2];
	for (uint8_t i = 0; i < NUMBER_OF_SEGMENT; i++) {
		buf[i * 2] = reg;
		buf[i * 2 + 1] = data;
	}
	spi_send_nonblocking(buf, NUMBER_OF_SEGMENT * 2);
}
