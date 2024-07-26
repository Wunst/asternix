include config.mk

export DESTDIR := $(shell pwd)/.sysroot

ISO := asternix.iso

.PHONY: system clean qemu iso

system:
	@make -C kern all install

clean:
	-@$(RM) -r $(DESTDIR)

qemu: iso
	@$(QEMU) $(QEMUFLAGS) -cdrom $(ISO) -s

iso: system
	@grub-mkrescue $(DESTDIR) -o $(ISO)
