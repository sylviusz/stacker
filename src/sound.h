#ifndef SOUND_H
#define SOUND_H

#include <avr/io.h>

// Definicja pinu buzzera (Zasada 6)
#define BUZZER_GPIO (1 << PD3)

// Dźwięki i Muzyka
#define NOTE_C4  262
#define NOTE_D4  294
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_G4  392
#define NOTE_A4  440
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_D5  587
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_G5  784
#define NOTE_A5  880
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_REST 0

// Typy stanów odtwarzacza muzyki
#define MUSIC_OFF        0
#define MUSIC_GAMEPLAY   1
#define MUSIC_WIN        2
#define MUSIC_LOSS       3
#define MUSIC_INACTIVE   4

#ifdef __cplusplus
extern "C" {
#endif

extern volatile uint8_t placementBeepTimer; // Licznik trwania wysokiego dźwięku położenia klocka

void play_tone(uint16_t freq);
void play_music_state(uint8_t newState);
void tick_music(uint8_t ms);

#ifdef __cplusplus
}
#endif

#endif // SOUND_H
