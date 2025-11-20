# ==== ПУТИ ====
SRC_DIR       := src
BOOT_DIR      := $(SRC_DIR)/bootldr/bootleg
DUALBOOT_DIR  := $(SRC_DIR)/dbt
KERNEL_DIR    := $(SRC_DIR)/krnl

CACHE_DIR     := .make_cache
BOOT_CACHE    := $(CACHE_DIR)/bootldr/bootleg
DBT_CACHE     := $(CACHE_DIR)/dbt
KRNL_CACHE    := $(CACHE_DIR)/krnl

OUT_DIR       := build
IMG_FILE      := $(OUT_DIR)/Ellu.img

# ==== ИНСТРУМЕНТЫ ====
NASM          := nasm
CC            := i686-elf-gcc
LD            := i686-elf-ld
OBJCOPY       := i686-elf-objcopy
QEMU          := qemu-system-i386

# ==== ФЛАГИ ====
CFLAGS        := -m32 -ffreestanding -fno-pie \
                 -I $(SRC_DIR)/includes \
                 -I $(SRC_DIR)/drivers \
                 -fno-stack-protector -O2 -Wall -Wextra -nostdinc

LDFLAGS       := -m elf_i386 -nostdlib

# ==== ИСХОДНИКИ ====
INC_SOURCES := $(shell find $(SRC_DIR)/includes -type f -name "*.c")
DRV_SOURCES := $(shell find $(SRC_DIR)/drivers  -type f -name "*.c")

DBT_SOURCES := $(wildcard $(DUALBOOT_DIR)/*.c) $(INC_SOURCES) $(DRV_SOURCES)
KRN_SOURCES := $(wildcard $(KERNEL_DIR)/*.c)   $(INC_SOURCES) $(DRV_SOURCES)

# Преобразование путей src/... → .cache/...
DBT_OBJECTS := $(subst $(SRC_DIR)/,$(DBT_CACHE)/,$(DBT_SOURCES:.c=.o))
KRN_OBJECTS := $(subst $(SRC_DIR)/,$(KRNL_CACHE)/,$(KRN_SOURCES:.c=.o))

BOOT_ASM      := $(BOOT_DIR)/legacy.asm
BOOT_BIN      := $(BOOT_CACHE)/legacy.bin

DBT_LD        := $(DUALBOOT_DIR)/link.ld
DBT_ELF       := $(DBT_CACHE)/dualboot.elf
DBT_BIN       := $(DBT_CACHE)/dualboot.bin

KRN_LD        := $(KERNEL_DIR)/link.ld
KRN_ELF       := $(KRNL_CACHE)/kernel.elf
KRN_BIN       := $(KRNL_CACHE)/kernel.bin

# ==== PHONY ====
.PHONY: all clean run dirs

# ==== ГЛАВНАЯ ЦЕЛЬ ====
all: dirs $(BOOT_BIN) $(DBT_BIN) $(KRN_BIN) $(IMG_FILE)

# ==== ДИРЕКТОРИИ ====
dirs:
	mkdir -p $(OUT_DIR)
	mkdir -p $(BOOT_CACHE)
	mkdir -p $(DBT_CACHE)
	mkdir -p $(KRNL_CACHE)

# ==== BOOTLOADER ====
$(BOOT_BIN): $(BOOT_ASM)
	$(NASM) -f bin $< -I $(BOOT_DIR) -o $@

# ==== КОМПИЛЯЦИЯ C → O (универсальная, с поддиректориями) ====
$(DBT_CACHE)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(KRNL_CACHE)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# ==== DUALBOOT ====
$(DBT_ELF): $(DBT_OBJECTS)
	$(LD) -T $(DBT_LD) $(LDFLAGS) -o $@ $^

$(DBT_BIN): $(DBT_ELF)
	$(OBJCOPY) -O binary $< $@

# ==== KERNEL ====
$(KRN_ELF): $(KRN_OBJECTS)
	$(LD) -T $(KRN_LD) $(LDFLAGS) -o $@ $^

$(KRN_BIN): $(KRN_ELF)
	$(OBJCOPY) -O binary $< $@

# ==== IMG ====
$(IMG_FILE): $(BOOT_BIN) $(DBT_BIN)
	cat $(BOOT_BIN) $(DBT_BIN) > $@

# ==== RUN ====
run: all
	$(QEMU) \
		-drive file=$(IMG_FILE),format=raw,index=1 \
		-d in_asm,int,cpu_reset -D qemu.log -m 2

# ==== CLEAN ====
clean:
	rm -rf $(CACHE_DIR) $(OUT_DIR)
