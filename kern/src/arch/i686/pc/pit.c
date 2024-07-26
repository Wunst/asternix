/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * arch/i686/pc/pit.c
 * Intel 8253/8254 Programmable Interrupt Timer
 * ISA IRQ 0
 * 
 * Copyright (C) 2024-present Ben Matthies
 * This is free software under the GNU General Public License, version 3, or,
 * at your option, any later version. See LICENSE file for details.
 */
#include "pc.h"

#include "../../../kernel.h"
#include "../asm.h"

// IO Ports
#define PIT_C0_DATA 0x40
#define PIT_CMD 0x43

// The PIT has three channels, each of which can operate in six modes. We want
// channel 0 as it's the one wired to IRQ 2, and mode 2 (rate generator).
#define C0_SEQ_M2 0x34  // Get/set low and high byte on channel 0, mode 2.

// Counts down the milliseconds we are sleeping. Needs to be volatile as it
// will be decremented by the timer interrupt while sleeping on it and must not
// be optimized out.
static volatile unsigned sleep_ms;

INTERRUPT
static void pit_fired(INTERRUPT_ARGS)
{
    // We set the timer frequency to 1000 Hz or 1 tick/ms.
    sleep_ms--;

    sendeoi(0);
}

void pit_init()
{
    puts("pit_init\n");

    // Set the frequency of the timer. The oscillator has a base frequency of
    // 1.193182 MHz. We set the reload value of the frequency divider / pulse
    // counter, i.e., the number of oscillator pulses per tick.
    u16 reload = 1193182 / 1000 /* Hz */;
    outb(PIT_CMD, C0_SEQ_M2);
    outb(PIT_C0_DATA, (u16)reload);
    outb(PIT_C0_DATA, (u16)(reload >> 8));

    setirq(0, &pit_fired);
    
    puts("pit_init done\n");
}

void msleep(unsigned millis)
{
    sleep_ms = millis;
    while (sleep_ms) {
        // An interrupt will wake us.
        hlt();
    }
}
