#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "spi.h"
#include "display.h"
#include "sound.h"

// PINY
#define BUTTON    PD2  // D2  (Przycisk)

// Zmienne globalne
uint8_t tower[32];
int8_t currentRow = 0;
int8_t currentPos = 2;
uint8_t currentWidth = 3;
int8_t direction = 1;
uint8_t prevMask = 0xFF;
uint8_t gameActive = 0;

// Stan przycisku debouncer
static uint8_t debouncedButtonState = 1;
static uint8_t debounceCounter = 0;
static uint16_t lockoutTimer = 0;

// opznienie + muzyczka w tle
void delay_ms(uint16_t ms) {
	while (ms--) {
		_delay_ms(1);
		tick_music(1);
	}
}

// Czyszczenie całej wieży
static void clear_tower(void) {
	for (uint8_t i = 0; i < 32; i++) {
		tower[i] = 0;
	}
}

// Obsługa przycisku z debouncerem
uint8_t check_button_pressed(void) {
	uint8_t rawPin = (PIND & (1 << BUTTON)) ? 1 : 0;
	uint8_t buttonPressed = 0;
	
	if (debouncedButtonState == 1) {
		// oczekiwanie na klik (zbocze oapadajace)
		if (rawPin == 0 && lockoutTimer == 0) {
			buttonPressed = 1;
			lockoutTimer = 50; // czas debouncer
			debouncedButtonState = 0; // przejscie w stan wcisniety
			debounceCounter = 0;
		}
	} else {
		// stan wcisniety czeka na puszczenie
		if (rawPin == 1) {
			if (debounceCounter < 20) { // czas debouncera
						debounceCounter++;
					} else {
						debouncedButtonState = 1; //puszczony
						debounceCounter = 0;
					}
		} else {
			if (debounceCounter > 0) {
				debounceCounter--;
			}
		}
	}
	return buttonPressed;
}

// Obsługa sekwencji przegranej
void handle_game_over(void) {
	gameActive = 0; // przerwanie gry
	play_music_state(MUSIC_LOSS); // dźwięk przegranej
	
	// Błysk błędu
	max_send_all(REG_DISPLAY_TEST, 1);
	delay_ms(150);
	max_send_all(REG_DISPLAY_TEST, 0);
	refresh_display();
	
	// collaps wieży
	for (uint16_t animStep = 0; animStep < 32; animStep++) {
		for (uint8_t i = 0; i < 31; i++) {
			tower[i] = tower[i + 1];
		}
		tower[31] = 0;
		refresh_display();
		delay_ms(50);
	}
	
	// Wyczyszczenie całej wieży
	clear_tower();
	
	// Wyświetlenie smutnej minki jako przegrana
	tower[0] = 0b00111100; // Rząd 0 (Góra głowy)
	tower[1] = 0b01000010; // Rząd 1 (Czoło)
	tower[2] = 0b10100101; // Rząd 2 (Oczy)
	tower[3] = 0b10000001; // Rząd 3 (Policzki)
	tower[4] = 0b10100101; // Rząd 4 (Kąciki ust - wyżej)
	tower[5] = 0b10011001; // Rząd 5 (Środek ust - niżej, tworzy łuk w dół)
	tower[6] = 0b01000010; // Rząd 6 (Podbródek)
	tower[7] = 0b00111100; // Rząd 7 (Dół głowy)
	refresh_display();
	
	delay_ms(2000); // Wyświetlanie minki przez 2s
	
	clear_tower();
	refresh_display();
}

// Obsługa sekwencji wygranej
void handle_victory(void) {
	gameActive = 0; // Przerywamy tryb gry
	play_music_state(MUSIC_WIN);
	
	for (uint16_t animStep = 0; animStep < 64; animStep++) {
		for (int8_t j = 31; j > 0; j--) {
			tower[j] = tower[j - 1];
		}
		
		// Rysowanie chevron
		uint8_t patternIdx = animStep % 8;
		if (patternIdx == 0 || patternIdx == 1) {
			tower[0] = 0b11000011; // Skrzydła zewnętrzne
		} else if (patternIdx == 2 || patternIdx == 3) {
			tower[0] = 0b01100110; // Skrzydła środkowe
		} else if (patternIdx == 4 || patternIdx == 5) {
			tower[0] = 0b00111100; // Rdzeń
		} else {
			tower[0] = 0x00;       
		}
		
		refresh_display();
		delay_ms(35); // Czas propagacji
	}
	
	// Wyczyszczenie przed resetem
	clear_tower();
	refresh_display();
}

//logika gry
void reset_game_vars() {
	clear_tower();
	currentRow = 0;
	currentWidth = 3;
	currentPos = 2;
	prevMask = 0xFF;
	direction = 1;
}

int main(void) {
	// Inicjalizacja sprzetowa
	DDRB |= (1 << CS_PIN) | (1 << MOSI_PIN) | (1 << SCK_PIN);
	PORTB |= (1 << CS_PIN);
	
	DDRD &= ~(1 << BUTTON);
	PORTD |= (1 << BUTTON);
	
	DDRD |= BUZZER_GPIO;   // Wyjście dla buzzera 
	PORTD &= ~BUZZER_GPIO; // Domyślnie stan niski
	
	// timer dla buzzera 2
	TCCR2A = (1 << WGM21);
	TCCR2B = (1 << CS22) | (1 << CS21);
	
	// Konfiguracja SPI z przerwaniami
	SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0) | (1 << SPIE);
	
	sei(); // Włączenie globalnych przerwań
	
	play_music_state(MUSIC_INACTIVE); 
	delay_ms(100);
	max_send_all(REG_SHUTDOWN, 0x01);
	max_send_all(REG_SCAN_LIMIT, 0x07);
	max_send_all(REG_INTENSITY, 0x02); // JASNOSC MATRYCY
	max_send_all(REG_DECODE_MODE, 0x00);
	max_send_all(REG_DISPLAY_TEST, 0);

	while (1) {
		//nowa gra
		reset_game_vars();
		uint16_t moveTimer = 0;
		gameActive = 1; //aktywacja trybu gry
		play_music_state(MUSIC_GAMEPLAY); // muzyczka do gry on
		
		// Reset debouncera
		debouncedButtonState = 1;
		debounceCounter = 0;
		lockoutTimer = 0;
		
		max_send_all(REG_DISPLAY_TEST, 0);
		refresh_display();

		while (gameActive == 1) {
			delay_ms(1);
			moveTimer++;
			
			if (lockoutTimer > 0) {
				lockoutTimer--;
			}

			// obsluga przycisu debouncer
			uint8_t buttonPressed = check_button_pressed();

			if (buttonPressed) {
				uint8_t mask = 0;
				for (uint8_t i = 0; i < currentWidth; i++) mask |= (1 << (currentPos + i));
				uint8_t result = mask & prevMask;
				
				if (result == 0) {
					handle_game_over();
				} else {
					// zapisanie poziomu wiezy
					placementBeepTimer = 150; // dzwiek sukcesu polozenia klocka
					tower[currentRow] = result;
					
					uint8_t count = 0;
					for (uint8_t i = 0; i < 8; i++) if (result & (1 << i)) count++;
					
					// UTRUDNIENIE usuniecie jednego klocka
					if (currentRow >= 16 && count == 3) {
						for (uint8_t i = 0; i < 8; i++) {
							if (result & (1 << i)) {
								result &= ~(1 << i); // Usuwanie najnizszego bitu utrudnienie gry
								break;
							}
						}
						count = 2;
						tower[currentRow] = result; // Aktualizacja stanu rzędu
					}
					
					currentWidth = count;
					prevMask = result;
					currentRow++;
					
					if (currentRow >= 32) {
						handle_victory();
					} else {
						// przejdz na kolejne pietro
						for (uint8_t i = 0; i < 8; i++) {
							if (result & (1 << i)) { currentPos = i; break; }
						}
						// Wymuszenie dopasowania klocka do granic
						if (currentPos + currentWidth > 8) {
							currentPos = 8 - currentWidth;
						}
						moveTimer = 0; // RESET TIMERA RUCHU , nowy czas na poczatku gry
						refresh_display();
					}
				}
			}

			// Wyznaczanie prędkości klocka na podstawie wysokości wieży
			uint16_t speed = 180 - (currentRow * 3);
			if (speed < 60) speed = 60;

			// Automatyczny ruch klocka tylko gdy gra aktywna
			if (gameActive && moveTimer >= speed) {
				moveTimer = 0;
				currentPos += direction;
				if (currentPos <= 0) { currentPos = 0; direction = 1; }
				if (currentPos + currentWidth >= 8) { currentPos = 8 - currentWidth; direction = -1; }
				refresh_display();
			}
		}
	}
}
