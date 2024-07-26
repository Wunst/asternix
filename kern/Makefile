-include ../config.mk

CCFLAGS += -mgeneral-regs-only
LDFLAGS += -nostdlib

LINKERSCRIPT := src/arch/$(ARCH)/linker.ld

SRCS := $(shell find -name \*.c -or -name \*.s)
OBJS := $(patsubst %.c,%.o,$(patsubst %.s,%.o,$(SRCS)))

.PHONY: all install clean

all: akern.bin

install:
	@mkdir -p $(DESTDIR)/boot/grub
	@cp grub.cfg $(DESTDIR)/boot/grub/grub.cfg
	@cp akern.bin $(DESTDIR)/boot/akern.bin

clean:
	-@$(RM) $(shell find -name \*.o)

akern.bin: $(LINKERSCRIPT) $(OBJS)
	@$(CC) -T $(LINKERSCRIPT) $(OBJS) -o $@ $(CCFLAGS) $(LDFLAGS)

%.o:: %.c
	@$(CC) -c $< -o $@ $(CCFLAGS)

%.o:: %.s
	@$(AS) $< -o $@
