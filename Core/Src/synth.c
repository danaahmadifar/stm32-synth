/*
 * synth.c - Tonerzeugung ueber Hardware-PWM
 */

#include "synth.h"
#include <stddef.h>

// Timer-Konfiguration
static TIM_HandleTypeDef *synth_timer = NULL;
static uint32_t synth_kanal = 0;
static uint32_t timer_takt = 1000000;   // 1 MHz Zaehltakt (1 µs)

// Ton-Zustand
static bool ton_laeuft = false;
static int8_t oktaven_versatz = 0;      // -2 bis +2
static char aktive_note_taste = '\0';

// Signalformen (Tastverhaeltnis in Promille)
static Signalform_t aktuelle_signalform = SIGNAL_RECHTECK_50;
static const uint16_t tastverhaeltnisse[SIGNAL_ANZAHL] = { 500, 250, 125, 62 };

static const char * const signalform_namen[SIGNAL_ANZAHL] = {
    "Rechteck 50%",
    "Puls 25%",
    "Puls 12.5%",
    "Puls 6.25%"
};

// Grundfrequenzen der 12 Tasten
static uint16_t Frequenz_Fuer_Taste(char taste)
{
    switch (taste)
    {
        case '1': return NOTE_C4;
        case '2': return NOTE_CS4;
        case '3': return NOTE_D4;
        case 'A': return NOTE_DS4;

        case '4': return NOTE_E4;
        case '5': return NOTE_F4;
        case '6': return NOTE_FS4;
        case 'B': return NOTE_G4;

        case '7': return NOTE_GS4;
        case '8': return NOTE_A4;
        case '9': return NOTE_AS4;
        case 'C': return NOTE_B4;

        default:  return 0;
    }
}

// Berechnet Frequenz inklusive Oktave
static uint16_t Aktive_Frequenz_Berechnen(void)
{
    uint16_t basis = Frequenz_Fuer_Taste(aktive_note_taste);
    if (basis == 0) return 0;

    if (oktaven_versatz > 0) return basis << oktaven_versatz;
    if (oktaven_versatz < 0) return basis >> (-oktaven_versatz);
    return basis;
}

// Initialisiert Timer und PWM
void Synth_Init(TIM_HandleTypeDef *htim, uint32_t kanal, uint32_t timer_takt_hz)
{
    synth_timer = htim;
    synth_kanal = kanal;
    timer_takt = (timer_takt_hz > 0) ? timer_takt_hz : 1000000;

    oktaven_versatz = 0;
    aktuelle_signalform = SIGNAL_RECHTECK_50;
    aktive_note_taste = '\0';
    ton_laeuft = false;

    if (synth_timer != NULL) {
        // Prescaler auf 1 MHz setzen (PSC = 71 bei 72 MHz)
        __HAL_TIM_SET_PRESCALER(synth_timer, 71);
        synth_timer->Instance->EGR = TIM_EGR_UG;
        __HAL_TIM_CLEAR_FLAG(synth_timer, TIM_FLAG_UPDATE);

        HAL_TIM_PWM_Stop(synth_timer, synth_kanal);
    }
}

void Synth_Update(uint32_t jetzt)
{
    (void)jetzt;
}

// Spielt Note mit gewaehlter Frequenz
void Synth_PlayNote(uint16_t frequenz_hz)
{
    if (synth_timer == NULL || frequenz_hz == 0) {
        Synth_Stop();
        return;
    }

    // Periode (ARR) und Pulsweite (CCR) setzen
    uint32_t arr = (timer_takt / frequenz_hz) - 1;
    if (arr > 0xFFFF) arr = 0xFFFF;

    uint32_t ccr = ((arr + 1) * tastverhaeltnisse[aktuelle_signalform]) / 1000;
    if (ccr == 0) ccr = 1;

    __HAL_TIM_SET_AUTORELOAD(synth_timer, arr);
    __HAL_TIM_SET_COMPARE(synth_timer, synth_kanal, ccr);

    if (!ton_laeuft) {
        __HAL_TIM_SET_COUNTER(synth_timer, 0);
        HAL_TIM_PWM_Start(synth_timer, synth_kanal);
        ton_laeuft = true;
    }
}

// Stoppt Tonausgabe
void Synth_Stop(void)
{
    if (synth_timer != NULL && ton_laeuft) {
        HAL_TIM_PWM_Stop(synth_timer, synth_kanal);
        ton_laeuft = false;
    }
}

// Tastendruck verarbeiten
void Synth_HandleKeyPress(char taste)
{
    switch (taste)
    {
        case '*': Synth_OctaveDown(); break;
        case '0': Synth_OctaveUp();   break;
        case '#': Synth_PrevWave();   break;
        case 'D': Synth_NextWave();   break;

        default:
        {
            uint16_t basis = Frequenz_Fuer_Taste(taste);
            if (basis > 0) {
                aktive_note_taste = taste;
                Synth_PlayNote(Aktive_Frequenz_Berechnen());
            }
            break;
        }
    }
}

// Taste losgelassen
void Synth_HandleKeyRelease(char taste)
{
    if (taste == aktive_note_taste) {
        aktive_note_taste = '\0';
        Synth_Stop();
    }
}

void Synth_OctaveUp(void)
{
    if (oktaven_versatz < 2) {
        oktaven_versatz++;
        if (aktive_note_taste != '\0') {
            Synth_PlayNote(Aktive_Frequenz_Berechnen());
        }
    }
}

void Synth_OctaveDown(void)
{
    if (oktaven_versatz > -2) {
        oktaven_versatz--;
        if (aktive_note_taste != '\0') {
            Synth_PlayNote(Aktive_Frequenz_Berechnen());
        }
    }
}

void Synth_NextWave(void)
{
    aktuelle_signalform = (Signalform_t)((aktuelle_signalform + 1) % SIGNAL_ANZAHL);
    if (aktive_note_taste != '\0') {
        Synth_PlayNote(Aktive_Frequenz_Berechnen());
    }
}

void Synth_PrevWave(void)
{
    if (aktuelle_signalform == 0) {
        aktuelle_signalform = (Signalform_t)(SIGNAL_ANZAHL - 1);
    } else {
        aktuelle_signalform = (Signalform_t)(aktuelle_signalform - 1);
    }

    if (aktive_note_taste != '\0') {
        Synth_PlayNote(Aktive_Frequenz_Berechnen());
    }
}

const char* Synth_GetWaveName(void)
{
    return signalform_namen[aktuelle_signalform];
}
