/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * arch/i686/idt.c
 * Interrupt descriptor table
 * 
 * Copyright (C) 2024-present Ben Matthies
 * This is free software under the GNU General Public License, version 3, or,
 * at your option, any later version. See LICENSE file for details.
 */
#include "idt.h"

#include "../../kernel.h"

typedef struct {
    u16 offset_low;         // Lower half of handler address.
    u16 kernel_cs;          // Code segment for handler.
    u8 _zero;
    gate_type gt: 5;        // Gate type.
    unsigned int_dpl: 2;    // DPL required to enter via the INT instruction.
    bool present: 1;        // Present bit.
    u16 offset_high;        // Higher half of handler address.
} PACKED idt_entry;

typedef struct {
    idt_entry entry[256];
} PACKED ALIGNED(16) idt_table;

static idt_table idt;

struct {
    u16 limit;
    idt_table *ptr;
} PACKED idt_ptr = {
    .limit = sizeof idt - 1,
    .ptr = &idt,
};

void idt_init(void)
{
    // TODO: Set exception handlers.
    // TODO: Move this out with the other inline assembly.
    asm (
        "lidt %0"
        :
        : "X"(idt_ptr)
    );
}

void idt_set_gate(u8 num, gate_type gt, u8 int_dpl, interrupt_handler *func)
{
    //assert(int_dpl <= 3)
    u32 offset = (u32)func;
    idt.entry[num] = (idt_entry) {
        .present = 1,
        .offset_low = (u16)offset,
        .offset_high = (u16)(offset >> 16),
        .kernel_cs = 0x0008,
        .gt = gt,
        .int_dpl = int_dpl,
    };
}
