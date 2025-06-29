# Blower Driver


## Project Background

This project was developed specifically for automotive applications where a blower motor is normally controlled via a multi-position resistor network (gear selector) that steps down the motor voltage to achieve different fan speeds.

In my own car, the factory blower resistor burned out. Unfortunately, replacement parts were either unavailable or disproportionately expensive. Faced with this problem right before leaving on a summer holiday trip, I built this project under time pressure to restore air conditioning operation for the journey.

Instead of the original resistor network, this driver uses PWM to regulate the blower speed. It reads the state of a selector switch (connected to input pins) and generates the appropriate PWM signal to drive a low-side logic-level N-MOSFET, which switches the blower motor.

This solution may also be useful for others facing similar issues in vehicles where:

- blower speeds are selected via hardware resistors
- replacement parts are expensive or hard to find
- PWM speed control offers more precise and reliable operation

**Important Note:**

- The circuit needs to be connected to an ignition-switched power source, so that it powers down with the car. However, this power does not need to carry the full blower motor current, since the entire high-current switching is handled by the low-side MOSFET driver.




PWM blower driver based on STM32F401CCU6 (Black Pill).  
Supports gear selection, PWM offset adjustment, and persistent settings storage in Flash memory.

---

## Hardware Overview

- **MCU:** STM32F401CCU6 (Black Pill)
- **MOSFET:** IRLB3034 N-FET 40V 195A 375W (logic-level N-MOSFET)
- **Flyback Diode:** schottky Diode MBR3045 45V/30A
- Controlled device: blower motor
- PWM frequency selectable via hardware pins

---



## Software Logic Flow

- **Startup:**
  - Initialize HAL
  - Load offsets from Flash memory
  - Read frequency selection pins to set PWM frequency

- **Main Loop:**
  - **Read Selector Switch (PA3..PA6):**
    - If any gear is active:
      - Compute duty = Base duty + Offset
      - Clamp duty to 0…100%
      - Update TIM2 PWM signal on PA0
    - Else (no gear selected → selector = 0):
      - Stop PWM
      - Configure PA0 as GPIO output LOW

  - **Read Buttons:**
    - SP_Up → increment current gear offset (max +20%)
    - SP_Dn → decrement current gear offset (min -20%)

  - **Clamp Offsets:**
    - Ensure offsets stay within ±20%

  - **Check if Offsets Changed:**
    - If changed → turn on Save LED (PA1)

  - **Check Save Button (PA2):**
    - If pressed:
      - Save offsets to Flash
      - Turn off Save LED

  - **Status LED (PC13):**
    - Blink rate depends on selected gear:
      - Gear 1 → 1 Hz
      - Gear 2 → ~3 Hz
      - Gear 3 → 5 Hz
      - Gear 4 → 10 Hz
      - No gear → 0.5 Hz

- **Loop Continuously**





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
| MOSFET              | IRLB83034                    | Logic-level N-MOSFET     |
| Flyback Diode       | MBR3045                      | Schottky, 30A / 45V      |
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

