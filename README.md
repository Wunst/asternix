# The ★nix project

## Goal

To create a well documented (as to be useful for educational purposes) clone of the original UNIX (POSIX-compatible where necessary).

This means that I do not make large-scale design decisions: I follow UNIX's for the better or worse, Keeping It Simple wherever possible.

## Features

- [x] boots on x86 legacy BIOS (32-bit protected mode) using GRUB
- [x] can `printf` from the kernel
- [ ] Drivers:
  - [x] PIT
  - [x] VGA text mode
  - [ ] ~~PS/2 keyboard~~

## How to build

**If you're on Windows, you need to use either MINGW/MSYS/Cygwin or WSL2.**

Follow the [GCC Cross-Compiler](https://wiki.osdev.org/GCC_Cross-Compiler) tutorial on the OSDev wiki. You are going to need `i686-elf-gcc`, `i686-elf-as` `i686-elf-ld` and `i686-elf-ar` as well as GNU make.

Also install your distro's GRUB tools package if you want to create an ISO image, and QEMU, if you want to run it directly in an emulator.

Once you have your tools set up, run
```
make qemu
```

## Credits

Many thanks to (of course) the omniscient and omnibenevolent [OSDev wiki](https://wiki.osdev.org/) (and forum) without which we would still be living in caves.
