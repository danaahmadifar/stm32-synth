/*
 * keypad.c - Erfassung und Entprellung der 4x4 Matrix-Tastatur
 */

#include "keypad.h"

// Tastenmatrix (4 Zeilen x 4 Spalten)
static const char tasten_matrix[4][4] = {
    { '1', '2', '3', 'A' },
    { '4', '5', '6', 'B' },
    { '7', '8', '9', 'C' },
    { '*', '0', '#', 'D' }
};

// Port- und Pinzuordnungen
static GPIO_TypeDef* const zeilen_ports[4] = {
    KEYPAD_R1_PORT, KEYPAD_R2_PORT, KEYPAD_R3_PORT, KEYPAD_R4_PORT
};

static const uint16_t zeilen_pins[4] = {
    KEYPAD_R1_PIN, KEYPAD_R2_PIN, KEYPAD_R3_PIN, KEYPAD_R4_PIN
};

static GPIO_TypeDef* const spalten_ports[4] = {
    KEYPAD_C1_PORT, KEYPAD_C2_PORT, KEYPAD_C3_PORT, KEYPAD_C4_PORT
};

static const uint16_t spalten_pins[4] = {
    KEYPAD_C1_PIN, KEYPAD_C2_PIN, KEYPAD_C3_PIN, KEYPAD_C4_PIN
};

// Zeitstempel und Entprell-Zustaende
static uint32_t letzter_scan = 0;
static char rohwert_kandidat = KEYPAD_KEINE_TASTE;
static uint8_t kandidat_zaehler = 0;
static char stabile_taste = KEYPAD_KEINE_TASTE;
static char vorherige_taste = KEYPAD_KEINE_TASTE;

// Flankenerkennung
static volatile bool taste_neu_gedrueckt = false;
static char neu_gedrueckte_taste = KEYPAD_KEINE_TASTE;
static volatile bool taste_losgelassen = false;
static char losgelassene_taste = KEYPAD_KEINE_TASTE;

// Liest die Matrix unentprellt ein
static char Matrix_Scan(void)
{
    char gefunden = KEYPAD_KEINE_TASTE;

    for (uint8_t z = 0; z < 4; z++)
    {
        HAL_GPIO_WritePin(zeilen_ports[z], zeilen_pins[z], GPIO_PIN_SET);

        for (volatile uint32_t w = 0; w < 40; w++) {
            __NOP();
        }

        for (uint8_t s = 0; s < 4; s++) {
            if (HAL_GPIO_ReadPin(spalten_ports[s], spalten_pins[s]) == GPIO_PIN_SET) {
                gefunden = tasten_matrix[z][s];
                break;
            }
        }

        HAL_GPIO_WritePin(zeilen_ports[z], zeilen_pins[z], GPIO_PIN_RESET);

        if (gefunden != KEYPAD_KEINE_TASTE) break;
    }

    return gefunden;
}

// Initialisiert alle Zeilen auf LOW
void Keypad_Init(void)
{
    for (uint8_t z = 0; z < 4; z++) {
        HAL_GPIO_WritePin(zeilen_ports[z], zeilen_pins[z], GPIO_PIN_RESET);
    }

    letzter_scan = 0;
    rohwert_kandidat = KEYPAD_KEINE_TASTE;
    kandidat_zaehler = 0;
    stabile_taste = KEYPAD_KEINE_TASTE;
    vorherige_taste = KEYPAD_KEINE_TASTE;
    taste_neu_gedrueckt = false;
    taste_losgelassen = false;
}

// Periodische Abtastung und Entprellung
void Keypad_Update(uint32_t jetzt)
{
    if ((jetzt - letzter_scan) < KEYPAD_SCAN_INTERVALL_MS) return;
    letzter_scan = jetzt;

    char roh = Matrix_Scan();

    if (roh == rohwert_kandidat) {
        if (kandidat_zaehler < KEYPAD_ENTPRELL_ZYKLEN) {
            kandidat_zaehler++;
            if (kandidat_zaehler >= KEYPAD_ENTPRELL_ZYKLEN) {
                stabile_taste = roh;

                if (stabile_taste != vorherige_taste) {
                    if (stabile_taste != KEYPAD_KEINE_TASTE) {
                        taste_neu_gedrueckt = true;
                        neu_gedrueckte_taste = stabile_taste;
                    } else {
                        taste_losgelassen = true;
                        losgelassene_taste = vorherige_taste;
                    }
                    vorherige_taste = stabile_taste;
                }
            }
        }
    } else {
        rohwert_kandidat = roh;
        kandidat_zaehler = 1;
    }
}

char Keypad_GetActiveKey(void)
{
    return stabile_taste;
}

bool Keypad_WasKeyPressed(char *taste)
{
    if (taste_neu_gedrueckt) {
        if (taste != NULL) *taste = neu_gedrueckte_taste;
        taste_neu_gedrueckt = false;
        return true;
    }
    return false;
}

bool Keypad_WasKeyReleased(char *taste)
{
    if (taste_losgelassen) {
        if (taste != NULL) *taste = losgelassene_taste;
        taste_losgelassen = false;
        return true;
    }
    return false;
}
