# Blower Driver

PWM blower driver based on STM32F401CCU6 (Black Pill).  
Supports gear selection, PWM offset adjustment, and persistent settings storage in Flash memory.

---

## Hardware Overview

- **MCU:** STM32F401CCU6 (Black Pill)
- **MOSFET:** IRLR8726 (logic-level N-MOSFET)
- **Flyback Diode:** STPS2L60U (Schottky, 2A / 60V)
- Controlled device: blower motor
- PWM frequency selectable via hardware pins

---

## Block Diagram

```
+---------------------+
| STM32F401CCU6       |
| (Black Pill)        |
+-----------+---------+
            |
      TIM2 CH1 (PA0)
            |
      +-----+------+
      |            |
   IRLR8726       LEDs
     Gate
```

---

## Inputs

| Signal         | Pin            | Description                          |
|----------------|----------------|--------------------------------------|
| Selector Switch| PA3, PA4, PA5, PA6 | Gear selection (1..4 or none = 0) |
| SP_Up Button   | PB2            | Increase PWM offset for active gear  |
| SP_Dn Button   | PB10           | Decrease PWM offset for active gear  |
| Save Button    | PA2            | Saves offsets to Flash memory        |
| Freq Select 1  | PA7            | LOW → 500 Hz                         |
| Freq Select 2  | PB0            | LOW → 1 kHz                          |
| Freq Select 3  | PB1            | LOW → 2 kHz                          |

**Note:** All frequency select pins HIGH → 100 Hz test mode.

---

## Outputs

| Signal          | Pin        | Description                          |
|-----------------|------------|--------------------------------------|
| PWM Output      | PA0 (TIM2_CH1) | PWM signal driving blower gate |
| Status LED      | PC13       | Blinking LED indicating active gear  |
| Save LED        | PA1        | Lit when offsets differ from saved   |

---

## Gear Selection and PWM Duty

The gear selector switch sets the active gear:

| Selector Position | Gear | Duty Cycle (before offset) |
|-------------------|------|----------------------------|
| PA3 LOW           | 1    | 25%                        |
| PA4 LOW           | 2    | 50%                        |
| PA5 LOW           | 3    | 75%                        |
| PA6 LOW           | 4    | 100%                       |
| All HIGH          | None | PWM off, blower disabled   |

- **Gear “none”** corresponds to selector position 0 (no active gear).

---

## Offset Adjustment

- **Offset range:** -20% … +20%
- Use SP_Up or SP_Dn buttons to increase or decrease offset for the currently selected gear.
- PWM duty = base duty + offset
- Duty cycle is clamped to the range 0…100%.

Example:

- Gear 2 (base 50%)
- Offset +10 → PWM duty = 60%

---

## Saving Offsets

- Press Save Button (PA2) to write all offsets to Flash.
- Saved offsets are automatically loaded on power-up.
- When current offsets differ from saved values, the Save LED (PA1) lights up.

---

## PWM Frequencies

PWM frequency is selected at startup via hardware pins:

| Pin States                    | Frequency |
|-------------------------------|-----------|
| PA7 LOW                       | 500 Hz    |
| PB0 LOW                       | 1 kHz     |
| PB1 LOW                       | 2 kHz     |
| PA7 HIGH, PB0 HIGH, PB1 HIGH  | 100 Hz    |

---

## LEDs

- **Status LED (PC13)**  
  - Blinks at different frequencies depending on active gear:
    - Gear 1 → 1 Hz
    - Gear 2 → ~3 Hz
    - Gear 3 → 5 Hz
    - Gear 4 → 10 Hz
    - No gear → 0.5 Hz

- **Save LED (PA1)**  
  - Lit when current offsets differ from saved offsets in Flash.
  - Turns off after saving.

---

## Bill of Materials (BOM)

| Part                | Type                         | Comment                  |
|---------------------|------------------------------|--------------------------|
| MCU                 | STM32F401CCU6                | Black Pill board         |
| MOSFET              | IRLR8726                     | Logic-level N-MOSFET     |
| Flyback Diode       | STPS2L60U                    | Schottky, 2A / 60V       |
| Blower motor        | —                            | Your choice              |
| LEDs                | 2 pcs                        | Status + Save indication |
| Buttons             | 3 pcs                        | SP_Up, SP_Dn, Save       |

---

## How It Works

1. On power-up, frequency selection pins determine PWM frequency.
2. Offsets are loaded from Flash if valid.
3. Selector switch chooses the active gear.
4. PWM duty is calculated as:
    ```
    PWM duty = base_duty + offset
    ```
5. Status LED blinks based on gear.
6. Save Button writes current offsets to Flash.

---

### Example Use Case

- Selector in Gear 3 → base duty 75%
- SP_Dn pressed → offset decremented from 0% → -5%
- PWM duty = 70%
- Save Button pressed → offsets saved to Flash.

---

## Repository

- Code is stored in this repository.
- Default branch: `master`.

---

## License

GPL-3.0 License

