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
#include "../../../fs/ext2/ext2.h"
#include "../asm.h"
#include "../idt.h"
#include "../pc/pc.h"
#include "grub/multiboot2.h"

typedef struct {
    u32 total_size;
    u32 _zero;
    struct multiboot_tag tags[0];
} multiboot_info;

static void *find_info(multiboot_info *info, u32 type)
{
    void *tag;
    for (tag = info->tags; tag < (void*)info + info->total_size;) {
        if (((struct multiboot_tag*)tag)->type == type) {
            return tag;
        }
            tag += ((struct multiboot_tag*)tag)->size;

        if ((u32)tag % 8 != 0) {
            tag += 8 - (u32)tag % 8;
        }
    }
    return 0;
}

// Called from _start.
void kmain(multiboot_info *info)
{
    puts("Hello, world!\n");

    // Find the RAM disk.
    struct multiboot_tag_module *moduleinfo = find_info(info,
            MULTIBOOT_TAG_TYPE_MODULE);
    printf("Module start: %x\n", moduleinfo->mod_start);

    ext2fs fs;
    ext2_inode ino;
    bool success = ext2_fsopen(&fs, (char*)moduleinfo->mod_start);
    success = ext2_readinode(&fs, &ino, 2);

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
