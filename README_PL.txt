======================================================
STACKER - autorzy: Sylwiusz Starczewski, Przemysław Jasiński
======================================================

Prosta, dynamiczna gra zręcznościowa napisana w języku C na mikrokontroler
ATmega328p (klon Arduino Nano). Głównym celem jest zbudowanie wieży 
o wysokości 32 bloków.

Im wyżej się znajdziesz, tym gra staje się szybsza, co sprawia, że dotarcie
na sam szczyt to prawdziwe wyzwanie.

======================================================
FUNKCJE
======================================================
* 32 poziomy o rosnącej prędkości i trudności
* Mechanika wygranej i przegranej z efektami audiowizualnymi
* Kod napisany w czystym C, bez użycia frameworka Arduino

======================================================
SPRZĘT
======================================================
Komponenty kupione prosto od zaufanych przyjaciół z Chin :)
* Mikrokontroler: Klon Arduino Nano
* Wyświetlacz: Matryca LED MAX7219 8x32
* Sterowanie: 1x prosty przycisk
* Dźwięk: 1x buzzer do efektów dźwiękowych

======================================================
PLIKI PROJEKTU
======================================================
* stacker.c
* display.h
* display.c
* sound.h
* sound.c
* spi.h
* spi.c
* stacker.ino (pisanie w dedykowanym IDE było łatwiejsze niż w Microchip Studio)
* gameplay.mp4