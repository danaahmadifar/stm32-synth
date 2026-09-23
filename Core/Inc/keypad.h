/*
 * keypad.h - Treiber fuer 4x4 Matrix-Tastatur
 */

#ifndef __KEYPAD_H
#define __KEYPAD_H

#include "main.h"
#include <stdbool.h>
#include <stdint.h>

// 4 Zeilen (Outputs, Push-Pull, initial LOW)
#define KEYPAD_R1_PORT       GPIOB
#define KEYPAD_R1_PIN        GPIO_PIN_12

#define KEYPAD_R2_PORT       GPIOB
#define KEYPAD_R2_PIN        GPIO_PIN_13

#define KEYPAD_R3_PORT       GPIOB
#define KEYPAD_R3_PIN        GPIO_PIN_14

#define KEYPAD_R4_PORT       GPIOB
#define KEYPAD_R4_PIN        GPIO_PIN_15

// 4 Spalten (Inputs mit internem Pull-Down)
#define KEYPAD_C1_PORT       GPIOA
#define KEYPAD_C1_PIN        GPIO_PIN_4   // Arduino Pin A2

#define KEYPAD_C2_PORT       GPIOA
#define KEYPAD_C2_PIN        GPIO_PIN_5   // Arduino Pin D13 (LD2)

#define KEYPAD_C3_PORT       GPIOA
#define KEYPAD_C3_PIN        GPIO_PIN_6   // Arduino Pin D12

#define KEYPAD_C4_PORT       GPIOA
#define KEYPAD_C4_PIN        GPIO_PIN_7   // Arduino Pin D11

// Zeitkonstanten
#define KEYPAD_SCAN_INTERVALL_MS   10   // 10 ms Abtastintervall
#define KEYPAD_ENTPRELL_ZYKLEN      2   // 20 ms Entprellzeit

#define KEYPAD_KEINE_TASTE        '\0'

// Funktionsprototypen
void Keypad_Init(void);
void Keypad_Update(uint32_t jetzt);
char Keypad_GetActiveKey(void);
bool Keypad_WasKeyPressed(char *taste);
bool Keypad_WasKeyReleased(char *taste);

#endif /* __KEYPAD_H */
