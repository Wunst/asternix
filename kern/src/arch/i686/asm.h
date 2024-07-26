/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * arch/i686/asm.h
 * Inline assembly bits
 * 
 * Copyright (C) 2024-present Ben Matthies
 * This is free software under the GNU General Public License, version 3, or,
 * at your option, any later version. See LICENSE file for details.
 */
#ifndef ASM_H
#define ASM_H

#include "../../kernel.h"

static inline void outb(u16 port, u8 data)
{
    asm (
        "outb %0, %1"
        :
        : "a"(data), "Nd"(port)
        : "memory"
    );
}

static inline void outw(u16 port, u16 data)
{
    asm (
        "outw %0, %1"
        :
        : "a"(data), "Nd"(port)
        : "memory"
    );
}

static inline void outl(u16 port, u32 data)
{
    asm (
        "outl %0, %1"
        :
        : "a"(data), "Nd"(port)
        : "memory"
    );
}

static inline u8 inb(u16 port)
{
    u8 ret;
    asm (
        "inb %1, %0"
        : "=a"(ret)
        : "Nd"(port)
        : "memory"
    );
    return ret;
}

static inline u16 inw(u16 port)
{
    u16 ret;
    asm (
        "inw %1, %0"
        : "=a"(ret)
        : "Nd"(port)
        : "memory"
    );
    return ret;
}

static inline u32 inl(u16 port)
{
    u32 ret;
    asm (
        "inl %1, %0"
        : "=a"(ret)
        : "Nd"(port)
        : "memory"
    );
    return ret;
}

static inline void hlt()
{
    asm ("hlt");
}

static inline void sti()
{
    asm ("sti");
}

#endif
