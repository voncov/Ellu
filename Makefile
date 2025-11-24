SRC_DIR       := src
BOOT_DIR      := $(SRC_DIR)/bootldr/bootleg
DUALBOOT_DIR  := $(SRC_DIR)/dualboot
KERNEL_DIR    := $(SRC_DIR)/krnl

CACHE_DIR     := .make_cache
BOOT_CACHE    := $(CACHE_DIR)/bootldr/bootleg
DBT_CACHE     := $(CACHE_DIR)/dualboot
KRNL_CACHE    := $(CACHE_DIR)/krnl
ASM_CACHE	  := $(CACHE_DIR)

OUT_DIR       := build
IMG_FILE      := $(OUT_DIR)/Ellu.img

NASM          := nasm
CC            := i686-elf-gcc
LD            := i686-elf-ld
OBJCOPY       := i686-elf-objcopy
QEMU          := qemu-system-i386

CFLAGS        := -m32 -ffreestanding -fno-pie \
                 -I $(SRC_DIR)/includes \
                 -I $(SRC_DIR)/drivers \
                 -fno-stack-protector -O2 -Wall -Wextra -nostdinc

LDFLAGS       := -m elf_i386 -nostdlib

INC_SOURCES := $(shell find $(SRC_DIR)/includes     -type f -name "*.c")
DRV_SOURCES := $(shell find $(SRC_DIR)/drivers      -type f -name "*.c")
ASM_SOURCES := $(shell find $(SRC_DIR)/asm  -type f -name "*.asm")

DBT_SOURCES := $(wildcard $(DUALBOOT_DIR)/*.c) $(INC_SOURCES) $(DRV_SOURCES)
KRN_SOURCES := $(wildcard $(KERNEL_DIR)/*.c)   $(INC_SOURCES) $(DRV_SOURCES)

ASM_OBJECTS := $(subst $(SRC_DIR)/,$(ASM_CACHE)/,$(ASM_SOURCES:.asm=.o))
DBT_OBJECTS := $(subst $(SRC_DIR)/,$(DBT_CACHE)/,$(DBT_SOURCES:.c=.o)) $(ASM_OBJECTS)
KRN_OBJECTS := $(subst $(SRC_DIR)/,$(KRNL_CACHE)/,$(KRN_SOURCES:.c=.o)) $(ASM_OBJECTS)

BOOT_ASM      := $(BOOT_DIR)/legacy.asm
BOOT_BIN      := $(BOOT_CACHE)/legacy.bin

DBT_LD        := $(DUALBOOT_DIR)/link.ld
DBT_ELF       := $(DBT_CACHE)/dualboot.elf
DBT_BIN       := $(DBT_CACHE)/dualboot.bin

KRN_LD        := $(KERNEL_DIR)/link.ld
KRN_ELF       := $(KRNL_CACHE)/kernel.elf
KRN_BIN       := $(KRNL_CACHE)/kernel.bin

.PHONY: all clean run dirs

all: dirs $(BOOT_BIN) $(DBT_BIN) $(KRN_BIN) $(IMG_FILE)

dirs:
	mkdir -p $(OUT_DIR)
	mkdir -p $(BOOT_CACHE)
	mkdir -p $(DBT_CACHE)
	mkdir -p $(KRNL_CACHE)

$(BOOT_BIN): $(BOOT_ASM)
	$(NASM) -f bin $< -I $(BOOT_DIR) -o $@

$(ASM_CACHE)/asm/%.o: $(SRC_DIR)/asm/%.asm
	@mkdir -p $(dir $@)
	$(NASM) -f elf32 $< -I $(SRC_DIR)/asm -o $@

$(DBT_CACHE)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(KRNL_CACHE)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(DBT_ELF): $(DBT_OBJECTS)
	$(LD) -T $(DBT_LD) $(LDFLAGS) -o $@ $^

$(DBT_BIN): $(DBT_ELF)
	$(OBJCOPY) -O binary $< $@

$(KRN_ELF): $(KRN_OBJECTS)
	$(LD) -T $(KRN_LD) $(LDFLAGS) -o $@ $^

$(KRN_BIN): $(KRN_ELF)
	$(OBJCOPY) -O binary $< $@

$(IMG_FILE): $(BOOT_BIN) $(DBT_BIN)
	cat $(BOOT_BIN) $(DBT_BIN) > $@

run: all
	$(QEMU) \
		-drive file=$(IMG_FILE),format=raw,index=1 \
		-chardev stdio,id=char0,logfile=serial.log \
		-serial chardev:char0 \
		-d in_asm,int,cpu_reset -D qemu.log -m 32

clean:
	rm -rf $(CACHE_DIR) $(OUT_DIR)
