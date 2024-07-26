/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * arch/i686/idt.h
 * Interrupt descriptor table
 * 
 * Copyright (C) 2024-present Ben Matthies
 * This is free software under the GNU General Public License, version 3, or,
 * at your option, any later version. See LICENSE file for details.
 */
#ifndef IDT_H
#define IDT_H

#include "../../kernel.h"

typedef enum {
    gt_interrupt = 0x0e,
    gt_trap = 0x0f,
} gate_type;

void idt_init(void);

void idt_set_gate(u8 num, gate_type gt, u8 int_dpl, interrupt_handler *func);

#endif
