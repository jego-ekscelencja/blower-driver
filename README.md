# blower-driver

STM32 PWM blower controller with offset save to Flash memory.

---

## Description

This project implements a PWM blower driver using the STM32F401 microcontroller.  
It allows control of blower speed in four predefined steps (gears) with the ability  
to adjust the PWM duty cycle of each gear and store offsets in the MCU's internal Flash memory.

All control signals and selectors are **active low**.

---

## Bill of Materials (BOM)

| Component               | Specification or Part Number      |
|--------------------------|-----------------------------------|
| Microcontroller          | STM32F401CCU6                     |
| Power MOSFET             | IRLR8726 (Logic-Level N-MOSFET)   |
| Freewheeling Diode       | Schottky SR360                    |
| LEDs                     | Standard low-power LED            |
| Push Buttons             | For UP, DOWN, and SAVE            |
| Select Switches          | Mechanical switches (active low)  |
| Resistors                | Pull-up or pull-down as required  |

---

## PWM Frequency Selection

PWM frequency is selected once during power-up by reading specific GPIO pins.  
A **LOW** state on a pin activates the corresponding frequency:

| Frequency     | Pin                       |
|---------------|---------------------------|
| 500 Hz        | GPIOA Pin 7 (F_Set_1)     |
| 1 kHz         | GPIOB Pin 0 (F_Set_2)     |
| 2 kHz         | GPIOB Pin 1 (F_Set_3)     |
| Test Mode (100 Hz) | None of the above active |

In test mode, all frequencies are disabled and the PWM runs at 100 Hz.

---

## Gear Selection

Blower speed is divided into 4 gears, each selected by a dedicated GPIO input:  

| Gear   | Selector Pin       |
|--------|--------------------|
| Gear 1 | GPIOA Pin 3        |
| Gear 2 | GPIOA Pin 4        |
| Gear 3 | GPIOA Pin 5        |
| Gear 4 | GPIOA Pin 6        |

- **Active low**: A low state on a pin selects the corresponding gear.
- If no gear is active, the PWM output is turned OFF.

---

## Default Duty Cycles

Each gear has an initial default duty cycle:

- **Gear 1:** 25%
- **Gear 2:** 50%
- **Gear 3:** 75%
- **Gear 4:** 100%

---

## Offset Adjustment

Offsets allow fine-tuning of the PWM duty cycle for each gear by ±20%.  

- Increase offset with **SP_Up** button (GPIOB Pin 2, active low).
- Decrease offset with **SP_Dn** button (GPIOB Pin 10, active low).
- Each press adjusts the duty by ±1%.
- Offsets are constrained to the range **-20% to +20%**.

Example:  
If Gear 2 has a default duty of 50% and the offset is +7%, the resulting duty will be 57%.

---

## Saving Offsets

To save offsets permanently:

- Press the **SAVE** button (GPIOA Pin 2, active low).
- When any offset differs from the saved value, the **SAVE LED** (GPIOA Pin 1) lights up.
- After pressing SAVE, offsets are written into internal Flash memory at address `0x08020000`.

---

## PWM Output Control

- The PWM output is driven by TIM2 Channel 1.
- When no gear is active, PWM stops and the output pin is switched to GPIO LOW.
- When a gear is active:
  - PWM is started.
  - Duty cycle is recalculated considering the offset.

---

## LED Blinking

Status LED connected to GPIOC Pin 13 indicates the current gear:

- Gear 1 → 1 Hz (1000 ms blink interval)
- Gear 2 → ~3 Hz (333 ms blink interval)
- Gear 3 → 5 Hz (200 ms blink interval)
- Gear 4 → 10 Hz (100 ms blink interval)
- No gear → 0.5 Hz (2000 ms blink interval)

---

## Flash Memory Layout

Offsets are saved in Flash as a structure:

```c
typedef struct
{
    uint16_t signature;      // 0x55AA
    int8_t Offset_for1;
    int8_t Offset_for2;
    int8_t Offset_for3;
    int8_t Offset_for4;
    uint16_t reserved;
} OffsetsFlash_t;
