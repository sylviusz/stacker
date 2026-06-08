#include "sound.h"

// Melodie w pamięci RAM
uint16_t gameplayMelody[16] = {
	NOTE_C4, NOTE_E4, NOTE_G4, NOTE_C5,
	NOTE_D4, NOTE_F4, NOTE_A4, NOTE_D5,
	NOTE_E4, NOTE_G4, NOTE_B4, NOTE_E5,
	NOTE_F4, NOTE_A4, NOTE_C5, NOTE_F5
};
#define GAMEPLAY_LEN 16
#define GAMEPLAY_NOTE_DUR 150

uint16_t winMelody[7] = {
	NOTE_C5, NOTE_E5, NOTE_G5, NOTE_C6, NOTE_REST, NOTE_G5, NOTE_C6
};
uint8_t winDurations[7] = {
	100, 100, 100, 200, 50, 100, 300
};
#define WIN_LEN 7

uint16_t lossMelody[4] = {
	NOTE_G4, NOTE_E4, NOTE_C4, NOTE_REST
};
uint8_t lossDurations[4] = {
	200, 200, 400, 100
};
#define LOSS_LEN 4

uint16_t inactiveMelody[8] = {
	NOTE_C4, NOTE_G4, NOTE_A4, NOTE_F4, NOTE_E4, NOTE_D4, NOTE_C4, NOTE_REST
};
#define INACTIVE_LEN 8
#define INACTIVE_NOTE_DUR 300

volatile uint8_t currentMusicState = MUSIC_OFF;
volatile uint16_t musicNoteTimer = 0;
volatile uint8_t musicNoteIdx = 0;
volatile uint8_t gameplayNoteIdx = 0;   // Zapisuje bieżący indeks melodii
volatile uint8_t placementBeepTimer = 0; // Licznik trwania wysokiego dźwięku położenia klocka

void play_tone(uint16_t freq) {
	if (freq == NOTE_REST) {
		TCCR2A &= ~(1 << COM2B0); // Wyłącz 
	} else {
		uint32_t ocr = 16000000UL / (2 * 256UL * freq) - 1;
		if (ocr > 255) ocr = 255;
		OCR2A = (uint8_t)ocr;
		OCR2B = (uint8_t)(ocr / 2); // Wypełnienie 50%
		TCCR2A |= (1 << COM2B0); // Włącz 
	}
}

void play_music_state(uint8_t newState) {
	currentMusicState = newState;
	musicNoteIdx = 0;
	musicNoteTimer = 0;
	if (newState == MUSIC_GAMEPLAY) {
		gameplayNoteIdx = 0;
	}
}

void tick_music(uint8_t ms) {
	musicNoteTimer += ms;
	
	// Aktualizacja licznika beepa
	if (placementBeepTimer > 0) {
		if (placementBeepTimer >= ms) {
			placementBeepTimer -= ms;
		} else {
			placementBeepTimer = 0;
		}
	}
	
	uint16_t note = NOTE_REST;
	uint16_t duration = 0;
	
	switch (currentMusicState) {
		case MUSIC_GAMEPLAY:
			duration = GAMEPLAY_NOTE_DUR;
			if (musicNoteTimer >= duration) {
				musicNoteTimer = 0;
				musicNoteIdx++;
				if (musicNoteIdx >= GAMEPLAY_LEN) {
					musicNoteIdx = 0;
				}
				gameplayNoteIdx = musicNoteIdx;
			}
			note = gameplayMelody[musicNoteIdx];
			
			if (placementBeepTimer > 0) {
				note = 1500; // Pisk 1500 Hz na polozeniu klocka
			}
			break;
			
		case MUSIC_INACTIVE:
			duration = INACTIVE_NOTE_DUR;
			if (musicNoteTimer >= duration) {
				musicNoteTimer = 0;
				musicNoteIdx++;
				if (musicNoteIdx >= INACTIVE_LEN) {
					musicNoteIdx = 0;
				}
			}
			note = inactiveMelody[musicNoteIdx];
			break;
			
		case MUSIC_WIN:
			if (musicNoteIdx >= WIN_LEN) {
				note = NOTE_REST; // Cisza po wygranej
				break;
			}
			duration = winDurations[musicNoteIdx];
			if (musicNoteTimer >= duration) {
				musicNoteTimer = 0;
				musicNoteIdx++;
				if (musicNoteIdx >= WIN_LEN) {
					note = NOTE_REST;
					break;
				}
			}
			note = winMelody[musicNoteIdx];
			break;
			
		case MUSIC_LOSS:
			if (musicNoteIdx >= LOSS_LEN) {
				note = NOTE_REST; // Cisza po muzyczce przgranej
				break;
			}
			duration = lossDurations[musicNoteIdx];
			if (musicNoteTimer >= duration) {
				musicNoteTimer = 0;
				musicNoteIdx++;
				if (musicNoteIdx >= LOSS_LEN) {
					note = NOTE_REST;
					break;
				}
			}
			note = lossMelody[musicNoteIdx];
			break;
			
		default:
			note = NOTE_REST;
			break;
	}
	
	play_tone(note);
}
