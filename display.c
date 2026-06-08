#include "display.h"
#include "spi.h"

// Pomocnicza funkcja pobierająca dane dla pojedynczego segmentu 
static uint8_t get_segment_data(int8_t m, uint8_t col) {
	uint8_t outputByte = 0;
	for (uint8_t bit = 0; bit < 8; bit++) {
		uint8_t levelIdx = (m * 8) + bit;
		uint8_t levelData = 0;
		
		// ruszajacy sie klocek w czasiee gry
		if (gameActive && levelIdx == currentRow) {
			for (uint8_t i = 0; i < currentWidth; i++) {
				levelData |= (1 << (currentPos + i));
			}
		} else {
			//czysty bufor przgrana
			levelData = tower[levelIdx];
		}
		
		if (levelData & (1 << (col - 1))) {
			outputByte |= (1 << bit);
		}
	}
	return outputByte;
}

// odsiwezanie matrycy
void refresh_display() {
	for (uint8_t col = 1; col <= 8; col++) {
		uint8_t buf[NUMBER_OF_SEGMENT * 2];
		uint8_t idx = 0;
		for (int8_t m = 3; m >= 0; m--) {
			uint8_t outputByte = get_segment_data(m, col);
			buf[idx++] = col;
			buf[idx++] = outputByte;
		}
		spi_send_nonblocking(buf, NUMBER_OF_SEGMENT * 2);
	}
}
