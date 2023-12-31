/* SPDX-License-Identifier: GPL-3.0-or-later */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <stdio.h>
#include <string.h>

#include <vendor/grub/multiboot2.h>

#include <drivers/major.h>
#include <drivers/tty.h>
#include <drivers/block/ramdisk.h>
#include <x86/interrupts.h>
#include <x86/mem.h>

#include "fs.h"
#include "panic.h"

struct multiboot_info {
    uint32_t total_size;
    uint32_t _resv04;
    struct multiboot_tag tags[0];
};

/* defined in boot0.s */
extern void halt_loop(void);

extern struct fs_driver tmpfs_driver;

typedef void (*initcall_f)(void);

extern initcall_f __initcall_start, __initcall_end;

void hlinit(struct multiboot_info *mbi_phys)
{
    mem_init();
    tty_init();

    bool got_rd = false;
    bool got_meminfo = false;
    bool got_mmap = false;

    uint32_t rd_start = 0;
    uint32_t rd_end = 0;

    /*
     * Parse multiboot information structure.
     *
     * For now, only the memory map/info is useful for us.
     * TODO: Parse ACPI tables, reclaim ACPI memory, get framebuffer info
     */
    struct multiboot_tag *tag;

    for (tag = mbi_phys->tags;
        tag->type != MULTIBOOT_TAG_TYPE_END;
        tag = (void *)tag + ((tag->size + 7) & ~7)) {

        switch (tag->type) {
        case MULTIBOOT_TAG_TYPE_BASIC_MEMINFO:
            mem_init_regions(
                ((struct multiboot_tag_basic_meminfo *)tag)->mem_lower * 1024,
                ((struct multiboot_tag_basic_meminfo *)tag)->mem_upper * 1024);
            got_meminfo = true;
            break;
        
        case MULTIBOOT_TAG_TYPE_MMAP:
            for (struct multiboot_mmap_entry *mmap =
                    ((struct multiboot_tag_mmap *)tag)->entries;
                
                (void *)mmap < (void *)tag + tag->size;

                mmap = (void *)mmap +
                    ((struct multiboot_tag_mmap *)tag)->entry_size) {
                
                if (mmap->type != MULTIBOOT_MEMORY_AVAILABLE)
                    mem_set_used(mmap->addr, mmap->len);
            }
            got_mmap = true;
            break;
        
        case MULTIBOOT_TAG_TYPE_MODULE:
            /* Initial RAM disk is passed as a multiboot module. Map it
             * immediately after the kernel binary. */
            rd_start = ((struct multiboot_tag_module *)tag)->mod_start;
            rd_end = ((struct multiboot_tag_module *)tag)->mod_end;

            /* Defer mapping. We should set up the memory maps first. */
            got_rd = true;

            break;
        }   
    }

    if (!got_meminfo || !got_mmap)
        panic(
            "Did not get required memory maps from GRUB.\n"
            "\tgot meminfo? %s\n"
            "\tgot mmap? %s\n"
            "Are you using legacy (BIOS) boot mode? Legacy BIOS is still"
            "required to boot *nix at this point. Sorry for the"
            "inconvenience! :(\n",
            got_meminfo ? "yes" : "no",
            got_mmap ? "yes" : "no");

    if (got_rd) {
        void *ramdisk = mem_map_range(
            K_MEM_START, rd_start, rd_end, DEFAULT_PAGE_FLAGS);
        
        dev_t rd = add_ramdisk(PAGE_SIZE, ramdisk, rd_end - rd_start);
        printf("info: added RAM disk as dev %d:%d\n", MAJOR(rd), MINOR(rd));
    }

    setup_interrupts();

    for (initcall_f *pp = &__initcall_start; pp < &__initcall_end; pp++)
        (**pp)();

    printf("Hello, world!\n");

    struct fs_instance *fs = tmpfs_driver.mount(NULL, 0, NULL);
    
    fs->driver->create(fs->root, "tty1", IT_CHR);
    
    struct dentry *de = fs->root->fs_on->driver->lookup(fs->root, "tty1");
    de->ino->dev_type = DEV(2, 1);
    de->ino->fs_on->driver->write(de->ino, 0, "\e[91;47mHello COM1", 19);

    fs->driver->create(fs->root, "console", IT_CHR);
    
    de = fs->root->fs_on->driver->lookup(fs->root, "console");
    de->ino->dev_type = DEV(2, 0);
    de->ino->fs_on->driver->write(de->ino, 0, "Hello from character device", 27);

    fs->driver->create(fs->root, "ram0", IT_BLK);

    char buf[4097];
    struct dentry *ram0 = fs->root->fs_on->driver->lookup(fs->root, "ram0");
    ram0->ino->dev_type = DEV(1, 0);
    ram0->ino->fs_on->driver->read(ram0->ino, 0, buf, 4096);
    buf[4096] = 0;
    printf("%s\n", buf);

    while (1) {
        char buf[10];
        int n;
        n = de->ino->fs_on->driver->read(de->ino, 0, buf, 10);
        de->ino->fs_on->driver->write(de->ino, 0, buf, n);
    }

    halt_loop();
}
