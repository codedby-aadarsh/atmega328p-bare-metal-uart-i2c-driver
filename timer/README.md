# TIMER in atmega328p 
- In this section we are going to be playing with the timer in the atmega328p , reading the datasheet , finding the details  about which regsiters we have to manipulate to do things related to the timer. 

## findings 
1. The PRTIM0 bit in Section 9.10 “Minimizing Power Consumption” on page 36 must be written to zero to enable
Timer/Counter0 module. ( this need to be done to enable the counter0 or timer0 which is of 8 bit).
2. Timer clock can also connected to the external or internal sources based on the clock select bits,when no clock source selected the timer is stopped.
3. The counting sequence is determined by the setting of the WGM01 and WGM00 bits located in the Timer/Counter control register (TCCR0A) and the WGM02 bit located in the Timer/Counter control register B (TCCR0B).
4. The Timer/Counter overflow flag (TOV0) is set according to the mode of operation selected by the WGM02:0 bits. TOV0 can be used for generating a CPU interrupt.
5. The 8 bit counter continously compares the TCNT0 counter value to the output comapare register, if comparator signals a matches then 

Good call taking the break — reading dense reference material without a clear target in mind is exactly when it stops sinking in. Here's the objective again, written so you can keep it open while reading and check "have I found what answers this yet."

## Objective

Blink an LED at a precise, fixed interval (aim for 500ms) using a hardware timer generating periodic interrupts — no `_delay_ms()` anywhere in the final version.

## The four specific questions to answer while reading, in order

**1. How does the timer count, and what happens when it reaches its limit?**
Find out: does the timer count up automatically, driven by clock pulses, all on its own once started? What happens by default when it reaches its maximum value — does it wrap around and keep going, or stop? (This default behavior is called "Normal mode" — you're looking for what happens before you configure anything special.)

**2. What is CTC mode, specifically?**
Find the mode where instead of counting all the way to the maximum, the timer resets back to zero once it hits a value *you* choose. Look for: what register do you write that chosen value into, and what register/bits do you set to actually turn CTC mode on (as opposed to Normal mode).

**3. How fast does the timer actually count — and how do you slow it down?**
The timer is driven by the system clock (16MHz), which counts way too fast for a human-visible 500ms blink. Find the concept of a **prescaler** — a way to divide the clock down before it reaches the timer, so it counts more slowly. Look for: what prescaler options exist (the available divide-by values), and which register/bits select one.

**4. Given a chosen prescaler, how do you calculate the exact compare value for a specific time interval?**
This is where the actual math lives — similar spirit to the UBRR baud rate formula you worked out for UART. You're looking for a formula relating: clock frequency, prescaler value, and the compare value, to get a specific delay in seconds/milliseconds. Try to find or derive this formula yourself from the first three answers, rather than looking for it pre-stated.

**5. How does the interrupt part connect in?**
Once CTC mode is set up and hits the compare value, what happens by default (nothing, unless you enable something) — and what do you need to enable to make it fire an actual interrupt you can catch with an ISR, same shape as your INT0 work.

Read with these five things as your checklist — you don't need to understand every paragraph of the chapter, just find the answers to these five. Take your break, come back to this list when ready.