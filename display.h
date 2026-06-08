#ifndef DISPLAY_H
#define DISPLAY_H

#include <avr/io.h>

// Rejestry MAX7219
#define REG_SHUTDOWN     0x0C
#define REG_SCAN_LIMIT   0x0B
#define REG_INTENSITY    0x0A
#define REG_DECODE_MODE  0x09
#define REG_DISPLAY_TEST 0x0F

#ifdef __cplusplus
extern "C" {
#endif

// Zmienne globalne z main.c
extern uint8_t tower[32];
extern int8_t currentRow;
extern int8_t currentPos;
extern uint8_t currentWidth;
extern uint8_t gameActive;

void refresh_display(void);

#ifdef __cplusplus
}
#endif

#endif // DISPLAY_H
