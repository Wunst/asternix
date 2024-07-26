/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * arch/i686/pc/pic.c
 * Intel 8259 PIC (Legacy PC) IRQ driver
 * 
 * Copyright (C) 2024-present Ben Matthies
 * This is free software under the GNU General Public License, version 3, or,
 * at your option, any later version. See LICENSE file for details.
 */
#include "pc.h"

#include "../../../kernel.h"
#include "../asm.h"
#include "../idt.h"

// IO Ports
// The PIC consists of two chips: the master handles IRQ 0-7 and is wired to
// the CPU, the slave IRQ 8-15 and is wired to the master on one IRQ line. We
// need to communicate with both chips to initialize them, mask and unmask IRQs
// individually.
#define PIC_MASTER_CMD 0x0020
#define PIC_MASTER_DATA 0x0021
#define PIC_SLAVE_CMD 0x00a0
#define PIC_SLAVE_DATA 0x00a1

#define EOI 0x20

#define ICW1_INIT 0x10
#define ICW1_ICW4 0x01 // Tells the PIC we will send ICW4.
#define ICW1_SNGL 0x02 // Tells the PIC there is only one (ICW3 is skipped).

#define ICW4_8086 0x01
#define ICW4_AEOI 0x02 // Enables Automatic End Of Interrupt signaling.

static u8 pic_offset;

static void pic_mask(u8 irq)
{
    //assert(irq <= 15)
    // Master handles IRQ 0-7, slave IRQ 8-15.
    u16 port = PIC_MASTER_DATA;
    if (irq >= 8) {
        port = PIC_SLAVE_DATA;
        irq -= 8;
    }

    u8 mask = inb(port);
    mask |= 1 << irq;
    outb(port, mask); 
}

static void pic_unmask(u8 irq)
{
    //assert(irq <= 15)
    u16 port = PIC_MASTER_DATA;
    if (irq >= 8) {
        port = PIC_SLAVE_DATA;
        irq -= 8;
    }

    u8 mask = inb(port);
    mask &= ~(1 << irq);
    outb(port, mask); 
}

void pic_init(u8 offset)
{
    // ICW1: Start the init sequence, in cascade mode (SNGL=0).
    outb(PIC_MASTER_CMD, ICW1_INIT | ICW1_ICW4);
    outb(PIC_SLAVE_CMD, ICW1_INIT | ICW1_ICW4);

    // ICW2: Tell the PIC their respective offsets (IRQ 0 -> CPU interrupt x).
    // CPU interrupts 0-31 are protected mode exceptions, so offset has to be
    // at least 32.
    outb(PIC_MASTER_DATA, offset);
    outb(PIC_SLAVE_DATA, offset + 8);

    // ICW3: Tell the master its cascade mask, slave its cascade IRQ. Cascade
    // is one IRQ line on the master used to receive interrupts from the slave.
    // IRQ 2 is used by convention as it's not used for anything else.
    outb(PIC_MASTER_DATA, 1 << 2);
    outb(PIC_SLAVE_DATA, 2);

    // ICW4: More flags, in our case, to signal interrupts for 8086 instead of
    // 8080.
    outb(PIC_MASTER_DATA, ICW4_8086);
    outb(PIC_SLAVE_DATA, ICW4_8086);

    // Mask all IRQ except cascade. IRQ lines are unmasked one by one by
    // drivers that handle them.
    outb(PIC_MASTER_DATA, 0b11111011);
    outb(PIC_SLAVE_DATA, 0b11111111);

    pic_offset = offset;
}

void setirq(u8 irq, interrupt_handler *func)
{
    //assert(irq <= 15)
    idt_set_gate(pic_offset + irq, gt_interrupt, 0, func);
    pic_unmask(irq);
}

void remirq(u8 irq)
{
    //assert(irq <= 15)
    pic_mask(irq);
}

// After an IRQ, an End Of Interrupt must be signaled.
void sendeoi(u8 irq)
{
    //assert(irq <= 15)
    if (irq >= 8) {
        outb(PIC_SLAVE_CMD, EOI);
    }

    // Always need to send EOI to the master since even on a slave IRQ, it's
    // always handling an IRQ 2 (Cascade).
    outb(PIC_MASTER_CMD, EOI);
}
