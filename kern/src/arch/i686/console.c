/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * arch/i686/console.c
 * VGA text mode console driver
 * 
 * Copyright (C) 2024-present Ben Matthies
 * This is free software under the GNU General Public License, version 3, or,
 * at your option, any later version. See LICENSE file for details.
 */
#include "../../kernel.h"
#include "asm.h"

// IO Ports
#define VGA_INDEX 0x03d4
#define VGA_DATA 0x03d5

// VGA indexed registers addressed via the VGA_INDEX IO port.
#define CURSOR_POS_HIGH 0x0e
#define CURSOR_POS_LOW 0x0f

// VGA framebuffer (character buffer in our case).
#define VGA_BUFFER 0xb8000
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

typedef struct {
    u8 character;   // Character code point.
    u8 attribute;   // Color and blink attribute.
} vga_entry;

static volatile vga_entry *buffer = (vga_entry*)VGA_BUFFER;

static u8 row, col;

// Writes to a VGA indexed register.
static void write_register(u8 index, u8 data)
{
    outb(VGA_INDEX, index);
    outb(VGA_DATA, data);
}

static void putchar(char ch)
{
    switch (ch) {
        case '\n':
            row++;
            col = 0;
        break;

        default:
            buffer[row * VGA_WIDTH + col].character = ch;
            col++;
    }

    if (col >= VGA_WIDTH) {
        row++;
        col = 0;
    }

    if (row >= VGA_HEIGHT) {
        // TODO: scrolling.
    }
}

static void move_cursor(u8 row, u8 col)
{
    u16 pos = (u16)row * VGA_WIDTH + col;
    write_register(CURSOR_POS_LOW, (u8)pos);
    write_register(CURSOR_POS_HIGH, (u8)(pos >> 8));
}

void puts(const char *msg)
{
    char ch;
    while ((ch = *msg++)) {
        putchar(ch);
    }

    // Only move the cursor after the entire string is written.
    move_cursor(row, col);
}
