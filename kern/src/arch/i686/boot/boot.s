/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * arch/i686/boot/boot.s
 * Boot header and assembly
 * 
 * Copyright (C) 2024-present Ben Matthies
 * This is free software under the GNU General Public License, version 3, or,
 * at your option, any later version. See LICENSE file for details.
 */

.section .multiboot
    MAGIC       = 0xe85250d6
    ARCH        = 0x00000000
    CHECKSUM    = -(MAGIC + ARCH + HEADER_LENGTH)

    .long MAGIC
    .long ARCH
    .long HEADER_LENGTH
    .long CHECKSUM
    .word 0, 0          // End tag
    .long 8

    HEADER_LENGTH = . - .multiboot

.section .text
    .global _start

_start:
    // Disable interrupts. We don't want a hardware interrupt breaking our
    // init sequence.
    cli

    // Load the GDT and reload segment selectors.
    lgdt    gdt_ptr
    movw    $0x0010, %ax
    movw    %ax, %ds
    movw    %ax, %es
    movw    %ax, %fs
    movw    %ax, %gs
    movw    %ax, %ss
    ljmp    $0x0008, $.new_segment

.new_segment:
    // Provide a kernel stack. The stack grows downwards on x86.
    movl    $stack_top, %esp
    movl    %esp, %ebp

    // Enter high level kernel.
    call    kmain

1:  hlt
    jmp     1b

.section .bss
    .align 16
stack_bottom:
    .skip 8192
stack_top:

.section .data
    .align 16
gdt:
    BASE_FLAT   = 0x0000000000000000
    LIMIT_FLAT  = 0x000f00000000ffff

    // Segment descriptor flags.
    GRAN4K      = 1 << 55   // If 1, limit is in 4K pages instead of bytes.
    BITS32      = 1 << 54   // Marks a PM32 code or data segment.
    PRESENT     = 1 << 47   // Marks a valid segment.
    NSYSTEM     = 1 << 44   // If 1, segment is code or data, if 0, a TSS (?)
    EXEC        = 1 << 43   // If 1, can execute code from this segment.
    RW          = 1 << 41   // If 1, can write to data (read from code).

    // DPL of code from segment.
    RING0       = 0 << 45
    RING3       = 3 << 45

    // 0x0000: Null descriptor
    .quad 0

    // 0x0008: Kernel code
    .quad BASE_FLAT | LIMIT_FLAT | GRAN4K | BITS32 | PRESENT | NSYSTEM | EXEC | RW | RING0

    // 0x0010: Kernel data
    .quad BASE_FLAT | LIMIT_FLAT | GRAN4K | BITS32 | PRESENT | NSYSTEM | RW | RING0

    // 0x0018: User code
    .quad BASE_FLAT | LIMIT_FLAT | GRAN4K | BITS32 | PRESENT | NSYSTEM | EXEC | RW | RING3

    // 0x0020: User data
    .quad BASE_FLAT | LIMIT_FLAT | GRAN4K | BITS32 | PRESENT | NSYSTEM | RW | RING3

gdt_ptr:
    .word . - gdt - 1   // Limit
    .long gdt
