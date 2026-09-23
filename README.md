# STM32 Audio-Synthesizer

Ein Projekt für das STM32 Nucleo-F303RE Board. Der Synthesizer erzeugt über Hardware-PWM auf einem Timer-Kanal (TIM2 CH1) monofone Töne mit voller 3,3V Lautstärke. Gesteuert wird das System über ein 4x4 Matrix-Tastenfeld: Die oberen drei Zeilen umfassen die 12 chromatischen Halbtöne einer Oktave, während die untere Zeile zur Oktavierung und zum Umschalten verschiedener Signalformen (Rechteck und Pulsweiten) dient.

---

## Verwendete Hardware

* **Mikrocontroller**: STM32 Nucleo-F303RE (ARM Cortex-M4)
* **Tastenfeld**: 4x4 Matrix-Keypad (16 Tasten: 1–9, 0, A–D, *, #)
* **Lautsprecher**: Passiver Kleinlautsprecher oder Piezo-Buzzer
* **Vorwiderstand**: 100 Ω Vorwiderstand für den Lautsprecher
* **Verdrahtung**: Steckbrücken (Jumper Wires)

---

## Pinbelegung / Verkabelung

### 1. Lautsprecher (an TIM2)
| Lautsprecher Pin | STM32 Pin | Beschreibung |
| :--- | :--- | :--- |
| **Signal (+)** | PA0 | PWM-Signal (TIM2 Kanal 1, Arduino A0 / Morpho CN7 Pin 28) über 100 Ω |
| **GND (-)** | GND | Masse |

### 2. 4x4 Matrix-Keypad (an GPIO)
| Keypad Leitung | STM32 Pin | Modus | Beschreibung |
| :--- | :--- | :--- | :--- |
| **Zeile 1 (R1)** | PB12 | Output Push-Pull | Morpho CN10 Pin 16 |
| **Zeile 2 (R2)** | PB13 | Output Push-Pull | Morpho CN10 Pin 18 |
| **Zeile 3 (R3)** | PB14 | Output Push-Pull | Morpho CN10 Pin 20 |
| **Zeile 4 (R4)** | PB15 | Output Push-Pull | Morpho CN10 Pin 22 |
| **Spalte 1 (C1)** | PA4 | Input (Pull-Down) | Arduino Pin A2 / Morpho CN7 Pin 32 |
| **Spalte 2 (C2)** | PA5 | Input (Pull-Down) | Arduino Pin D13 / Morpho CN10 Pin 11 (LD2) |
| **Spalte 3 (C3)** | PA6 | Input (Pull-Down) | Arduino Pin D12 / Morpho CN10 Pin 13 |
| **Spalte 4 (C4)** | PA7 | Input (Pull-Down) | Arduino Pin D11 / Morpho CN10 Pin 15 |

---

## Funktionen und Bedienung

### 1. Tastatur-Layout (4x4 Matrix)

```
+-----+-----+-----+-----+
|  1  |  2  |  3  |  A  |  <- Zeile 1: C4, C#4, D4, D#4
+-----+-----+-----+-----+
|  4  |  5  |  6  |  B  |  <- Zeile 2: E4, F4, F#4, G4
+-----+-----+-----+-----+
|  7  |  8  |  9  |  C  |  <- Zeile 3: G#4, A4, A#4, B4
+-----+-----+-----+-----+
|  *  |  0  |  #  |  D  |  <- Zeile 4: Oct-, Oct+, Wave-, Wave+
+-----+-----+-----+-----+
```

### 2. Noten & Tonleiter (12 chromatische Halbtöne)

Die oberen 3 Zeilen bilden eine vollständige chromatische Tonleiter von 12 Halbtönen:

| Zeile | Taste | Note | Basis-Frequenz (4. Oktave) |
| :---: | :---: | :--- | :---: |
| **1** | **`1`** | **C** | 262 Hz |
| **1** | **`2`** | **C# / Db** | 277 Hz |
| **1** | **`3`** | **D** | 294 Hz |
| **1** | **`A`** | **D# / Eb** | 311 Hz |
| **2** | **`4`** | **E** | 330 Hz |
| **2** | **`5`** | **F** | 349 Hz |
| **2** | **`6`** | **F# / Gb** | 370 Hz |
| **2** | **`B`** | **G** | 392 Hz |
| **3** | **`7`** | **G# / Ab** | 415 Hz |
| **3** | **`8`** | **A (Kammerton)** | 440 Hz |
| **3** | **`9`** | **A# / Bb** | 466 Hz |
| **3** | **`C`** | **B / H** | 494 Hz |

### 3. Oktavenumschaltung
* **`*` (Oktave runter)**: Senkt die Tonhöhe um jeweils eine Oktave (bis zu -2 Oktaven).
* **`0` (Oktave hoch)**: Erhöht die Tonhöhe um jeweils eine Oktave (bis zu +2 Oktaven).
* Wird während des Haltens einer Taste die Oktave gewechselt, passt sich die Tonhöhe sofort an.

### 4. Signalformen & Klangfarben
Über die Tasten **`#`** (zurück) und **`D`** (vor) wird zwischen vier verschiedenen Signalformen umgeschaltet. Das Tastverhältnis (Duty Cycle) der Hardware-PWM steuert dabei das Obertonspektrum bei gleichbleibend maximaler Lautstärke:
1. **Rechteck 50%**: Voll, warm und kräftig (klassischer 8-Bit Chiptune / Synth-Sound).
2. **Puls 25%**: Sägezahn-Charakter mit ausgeprägten Obertönen.
3. **Puls 12,5%**: Scharf, nasal und schneidend.
4. **Puls 6,25%**: Sehr hell und metallisch (Nadelimpulse).

---

## Projektstruktur

* `Core/Src/main.c`: Initialisierung der Peripherie (Clocks, GPIO, TIM2) und Hauptschleife.
* `Core/Src/synth.c` & `synth.h`: Tonerzeugung über Hardware-PWM, Frequenz- und Pulsweitenberechnung sowie Oktavenverwaltung.
* `Core/Src/keypad.c` & `keypad.h`: Nicht-blockierender 4x4 Matrix-Treiber mit Zeilenabtastung und Software-Entprellung.

---

## Kompilieren und Flashen

### Mit STM32CubeIDE
1. Das Projekt in der STM32CubeIDE über `File -> Open Projects from File System...` importieren.
2. Das Board per USB-Kabel verbinden.
3. Auf **Run** oder **Debug** klicken – die IDE kompiliert das Projekt und überträgt das Programm automatisch auf den Mikrocontroller.

---

## Besonderheiten / Hinweise

* **Volle Lautstärke**: Da die Frequenz direkt über das Periodenregister (ARR) moduliert wird, schwingt der Lautsprecher mit vollen 3,3 V Rail-to-Rail-Rechteckimpulsen direkt auf der Notenfrequenz. Dadurch wird am passiven Lautsprecher maximale Lautstärke ohne zusätzlichen Audioverstärker erreicht.
* **Pin PA5 / Onboard-LED (LD2)**: Pin PA5 teilt sich die Leitung mit der grünen Nutzer-LED auf dem Nucleo-Board. Beim Drücken einer Taste in Spalte 2 ('2', '5', '8', '0') leuchtet die LED kurz als optische Rückmeldung auf.
