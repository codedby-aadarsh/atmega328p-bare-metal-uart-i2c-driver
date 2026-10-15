# ATmega328p — External Interrupts + Debounce

Register-level external interrupt handling on the ATmega328p, triggered by a push button, with a real hardware debounce problem hit and solved. No Arduino library — direct EICRA/EIMSK/ISR register work.

## What it does

- Push button on D2 (INT0), configured with the ATmega's internal pull-up (no external pull-up resistor needed)
- Falling-edge triggered interrupt fires when the button is pressed
- ISR sets a flag; the main loop checks the flag and toggles an LED (D10) in response
- Debounce implemented via a short delay after each accepted press, so mechanical contact bounce doesn't register as multiple presses

## Things I learned

1. **Interrupt vector priority** — lower vector table addresses have higher priority; this is fixed by hardware, not configurable per-source.
2. **Global vs local interrupt enable** — `EIMSK` arms a specific interrupt source (INT0 here), but nothing fires unless the global `I` bit in `SREG` is also set, via `sei()`. Both gates have to be open.
3. **`EICRA` edge selection** — `ISC01`/`ISC00` bits select which edge (or level) triggers INT0. Falling edge = `ISC01 = 1, ISC00 = 0`.
4. **ISR should stay minimal** — the ISR just sets a flag; the actual response (toggling the LED) happens in the main loop, not inside the ISR itself. Heavier work belongs outside the interrupt context.
5. **`DDRx` vs `PORTx`, again** — same category of bug as an earlier TWCR/TWSR mixup: accidentally toggled `DDRx` (direction) instead of `PORTx` (output level) when trying to toggle the LED. Direction and output level are separate registers, easy to confuse when moving fast.
6. **Push button wiring — diagonal, not adjacent** — a 4-leg tactile button is internally two permanently-connected leg pairs; the pairs only connect to each other when pressed. Wiring GND and the signal pin to legs on the *same* pair shorts them permanently regardless of button state. Wiring to *diagonal* legs (one from each pair) is what makes the connection depend on an actual press. Found this by debugging a "button does nothing" symptom on real hardware.
7. **Contact bounce is real, not a theoretical concern.** A mechanical button doesn't transition cleanly from open to closed — it bounces for a few milliseconds, and an edge-triggered interrupt sees every bounce as a separate edge. Without debounce, a single physical press produced multiple ISR firings and erratic LED behavior.
8. **Why blocking inside an ISR is costly, specifically on a single-core MCU.** There's no OS, no scheduler, no second thread or core to pick up slack while the CPU is blocked in a delay — a blocking ISR freezes the *entire system* for its duration, not just the interrupt handling. This is architecturally different from blocking on a multi-core/multi-threaded Linux system, where the kernel can run something else in the meantime.

## Current implementation & known limitation

Debounce is currently implemented as a **blocking delay inside the ISR** (`_delay_ms(200)` after setting the flag). This works correctly for this isolated exercise, but blocks the entire system for 200ms per press — not appropriate once other peripherals (timers, UART reporting, I2C) need to run concurrently.

**Planned improvement:** replace with a **non-blocking, timestamp-based debounce** once Timers are implemented — the ISR will record a timestamp on each edge, and the main loop will compare the current timer count against the last *accepted* press's timestamp, only acting if enough time has passed. This keeps the ISR minimal and doesn't block the rest of the system. Tracked as the natural next step, tied to the upcoming Timers topic.

## Hardware

- ATmega328p (Arduino Uno, bare register access — no Arduino core)
- Push button, wired diagonally across D2 and GND, internal pull-up enabled in software
- LED + 1kΩ resistor on D10

## Debugging notes

Initial button wiring connected GND and D2 to adjacent (same-pair) legs of the push button, which permanently shorted the pin regardless of button state. Isolated by first confirming the LED/GPIO path worked independently (simple blink test on D10), then confirming the ISR fired correctly when triggered directly (bypassing the flag/main-loop path), before finally tracing the remaining issue back to button wiring — same divide-and-conquer debugging approach as the earlier I2C bring-up issue.