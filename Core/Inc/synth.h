/*
 * synth.h - Schnittstelle fuer den Hardware-PWM Synthesizer
 */

#ifndef __SYNTH_H
#define __SYNTH_H

#include "main.h"
#include <stdbool.h>
#include <stdint.h>

// 12 Halbtoene (Basis 4. Oktave, A4 = 440 Hz)
#define NOTE_C4     262  // 1: C4
#define NOTE_CS4    277  // 2: C#4
#define NOTE_D4     294  // 3: D4
#define NOTE_DS4    311  // A: D#4

#define NOTE_E4     330  // 4: E4
#define NOTE_F4     349  // 5: F4
#define NOTE_FS4    370  // 6: F#4
#define NOTE_G4     392  // B: G4

#define NOTE_GS4    415  // 7: G#4
#define NOTE_A4     440  // 8: A4
#define NOTE_AS4    466  // 9: A#4
#define NOTE_B4     494  // C: B4

// Signalformen (Pulsweiten)
typedef enum {
    SIGNAL_RECHTECK_50 = 0,  // 50% Rechteck
    SIGNAL_PULS_25,          // 25% Puls
    SIGNAL_PULS_12,          // 12.5% Puls
    SIGNAL_PULS_6,           // 6.25% Puls
    SIGNAL_ANZAHL
} Signalform_t;

// Funktionsprototypen
void Synth_Init(TIM_HandleTypeDef *htim, uint32_t kanal, uint32_t timer_takt_hz);
void Synth_Update(uint32_t jetzt);

void Synth_HandleKeyPress(char taste);
void Synth_HandleKeyRelease(char taste);

void Synth_PlayNote(uint16_t frequenz_hz);
void Synth_Stop(void);

void Synth_OctaveUp(void);
void Synth_OctaveDown(void);
void Synth_NextWave(void);
void Synth_PrevWave(void);
const char* Synth_GetWaveName(void);

#endif /* __SYNTH_H */
