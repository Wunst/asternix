/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * bio.c
 * Binary I/O utility
 * 
 * Copyright (C) 2024-present Ben Matthies
 * This is free software under the GNU General Public License, version 3, or,
 * at your option, any later version. See LICENSE file for details.
 */
#include "kernel.h"

#include <stdarg.h>

// Formatted Read Binary, Little Endian.
// `fmt` is a template string where every character describes an argument:
//   `B`: 1 Byte, `u8`
//   `W`: 2 bytes (Word), `u16`
//   `L`: 4 bytes (Long), `u32`
//   `Q`: 8 bytes (Quadword), `u64`
//   `Sn`, where n is a number: string of length n bytes, `char*`
// Varargs are the pointers to write to.
void readble(const u8 *buf, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

    char ch;
    while ((ch = *fmt++)) {
        switch (ch) {
            case 'B':
                puts("B");
                *va_arg(ap, u8*) = *buf++;
                break;

            case 'W':
                puts("W");
                *va_arg(ap, u16*) = *buf++ + (u16)*buf++ << 8;
                break;
            
            case 'L':
                puts("L");
                *va_arg(ap, u32*) = *buf++ + (u32)*buf++ << 8 +
                        (u32)*buf++ << 16 + (u32)*buf++ << 24;
                break;

            case 'Q':
                puts("Q");
                *va_arg(ap, u64*) = *buf++ + (u64)*buf++ << 8 +
                        (u64)*buf++ << 16 + (u64)*buf++ << 24 +
                        (u64)*buf++ << 32 + (u64)*buf++ << 40 +
                        (u64)*buf++ << 48 + (u64)*buf++ << 56;
                break;

            case 'S': {
                puts("S");
                unsigned length = 0;
                while ((ch = *++fmt) >= '0' && ch <= '9') {
                    length *= 10;
                    length += ch - '0';
                }
                
                char *dest = va_arg(ap, char*);
                for (unsigned i = 0; i < length; i++) {
                    *dest++ = *buf++;
                }

                printf("%d", length);

                break;
            }
            
            default:
                puts("fail.");
                goto fail;
        }
    }

fail:
    va_end(ap);
}

// Formatted Write Binary, Little Endian.
void writeble(u8 *buf, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

    char ch;
    while ((ch = *fmt++)) {
        switch (ch) {

        }
    }
    
fail:
    va_end(ap);
}
