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
                *va_arg(ap, u8*) = *buf++;
                break;

            case 'W':
                *va_arg(ap, u16*) = buf[0] + ((u16)buf[1] << 8);
                buf += 2;
                break;
            
            case 'L':
                *va_arg(ap, u32*) = buf[0] + ((u32)buf[1] << 8) +
                        ((u32)buf[2] << 16) + ((u32)buf[3] << 24);
                buf += 4;
                break;

            case 'Q':
                *va_arg(ap, u64*) = buf[0] + ((u64)buf[1] << 8) +
                        ((u64)buf[2] << 16) + ((u64)buf[3] << 24) +
                        ((u64)buf[4] << 32) + ((u64)buf[5] << 40) +
                        ((u64)buf[6] << 48) + ((u64)buf[7] << 56);
                buf += 8;
                break;

            case 'S': {
                unsigned length = 0;
                while ((ch = *fmt) >= '0' && ch <= '9') {
                    length *= 10;
                    length += ch - '0';
                    fmt++;
                }
                char *dest = va_arg(ap, char*);
                for (unsigned i = 0; i < length; i++) {
                    *dest++ = *buf++;
                }
                break;
            }
            
            default:
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
