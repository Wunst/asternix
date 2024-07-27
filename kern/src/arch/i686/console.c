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

#include <stdarg.h>

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

static void move_cursor(u8 row, u8 col)
{
    u16 pos = (u16)row * VGA_WIDTH + col;
    write_register(CURSOR_POS_LOW, (u8)pos);
    write_register(CURSOR_POS_HIGH, (u8)(pos >> 8));
}

static void _putchar(char ch)
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

// Prints an unsigned integer in a given radix.
static void _putint(unsigned n, unsigned radix, unsigned width)
{
    //assert(8 <= radix <= 16 && width <= 11 && sizeof n <= 4)
    const char digits[] = "0123456789abcdef";
    char buf[11];
    int i = 0;
    do {
        buf[i++] = digits[n % radix];
    } while ((n /= radix) || (i < width));
    do {
        _putchar(buf[--i]);
    } while (i);
}

void puts(const char *msg)
{
    printf("%s", msg);
}

void printf(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

    char ch;
    while ((ch = *fmt++)) {
        if (ch != '%') {
            _putchar(ch);
            continue;
        }

        // Handle format specifier.
        // Numeric format width. Always uses zero as filler character.
        // TODO: Filling with spaces
        unsigned nwidth = 0;
        while ((ch = *fmt++) >= '0' && ch <= '9') {
            nwidth *= 10;
            nwidth += ch - '0';
        }

        // Format specifiers.
        switch (ch) {
            case '%':
                _putchar('%');
                break;
            
            case 's': {
                // FIXME: Null-terminated strings in kernel safe?
                const char *s = va_arg(ap, const char*);
                while ((ch = *s++)) {
                    _putchar(ch);
                }
                break;
            }

            case 'd': {
                int n = va_arg(ap, int);
                if (n < 0) {
                    _putchar('-');
                    n = -n;
                }
                _putint((unsigned)n, 10, nwidth);
                break;
            }

            case 'u':
                _putint(va_arg(ap, unsigned), 10, nwidth);
                break;

            case 'x':
                _putint(va_arg(ap, unsigned), 16, nwidth);
                break;

            case 'o':
                _putint(va_arg(ap, unsigned),8, nwidth);
                break;

            default:
                // End printing on an invalid format specifier.
                goto fail;
        }
    }

fail:
    va_end(ap);

    // Only move the cursor after the entire string is written.
    move_cursor(row, col);
}
