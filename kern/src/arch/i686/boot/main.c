/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * arch/i686/boot/main.c
 * Kernel init entry point
 * 
 * Copyright (C) 2024-present Ben Matthies
 * This is free software under the GNU General Public License, version 3, or,
 * at your option, any later version. See LICENSE file for details.
 */
#include "../../../kernel.h"
#include "../asm.h"
#include "../idt.h"
#include "../pc/pc.h"

// Called from _start.
void kmain()
{
    puts("Hello, world!\n");

    // printf tests
    printf("%%8d: %8d\n", 0xffffffff);
    printf("%%8u: %8u\n", 0xffffffff);
    printf("%%8x: %8x\n", 0xffffffff);
    printf("A %s line with %d format specifiers (%x in hex)\n", "long", 3, 3);

    idt_init();
    pic_init(32);
    pit_init();
    sti();

    char a[] = "0";
    while (1) {
        puts(a);
        msleep(1000);
        a[0]++;
    }
}
