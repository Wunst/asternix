/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * kernel.h
 * Common kernel library
 * 
 * Copyright (C) 2024-present Ben Matthies
 * This is free software under the GNU General Public License, version 3, or,
 * at your option, any later version. See LICENSE file for details.
 */
#ifndef KERNEL_H
#define KERNEL_H

#include <stdbool.h>
#include <stdint.h>

#if defined(__GNUC__) || defined(__clang__)
#define PACKED __attribute__((packed))
#define ALIGNED(n) __attribute__((aligned(n)))
#define INTERRUPT __attribute__((interrupt))
#ifdef __i386__
struct interrupt_frame;
#define INTERRUPT_ARGS struct interrupt_frame*
#else
#define INTERRUPT_ARGS void
#endif
#endif

typedef INTERRUPT void interrupt_handler(INTERRUPT_ARGS);

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

void readble(const u8 *buf, const char *fmt, ...);
void writeble(u8 *buf, const char *fmt, ...);

void puts(const char *msg);
void printf(const char *fmt, ...);

void setirq(u8 irq, interrupt_handler *func);
void remirq(u8 irq);
void sendeoi(u8 irq);

void msleep(unsigned millis);

#endif
