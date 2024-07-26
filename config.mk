# Change this to your own target and toolchain.
ARCH      := i686
QEMU      := qemu-system-i386
QEMUFLAGS :=
CROSS     := i686-elf-
CC        := $(CROSS)gcc
LD        := $(CROSS)ld
AS        := $(CROSS)as
AR        := $(CROSS)ar
CCFLAGS   := -std=gnu99 -g -O3 -ffreestanding
LDFLAGS   :=
