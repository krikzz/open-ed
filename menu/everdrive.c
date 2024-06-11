
#include "main.h"
#include "flash.h"

//control register
#define CTRL_REG        *((vu16 *) 0xA130E0)


//ctrl write
#define CTRL_SRM_ON     0x01// 4MB ROM ro 2MB ROM + 128KB SRAM
#define CTRL_RESERVED   0x02// unused
#define CTRL_ROM_BANK   0x04// ROM bank select (1=MENU or 0=GAME)
#define CTRL_LED        0x08// led controll
#define CTRL_EXP_SS     0x10// expansion SPI chip select
#define CTRL_SDC_SS     0x20// SD card chip select (SPI)
#define CTRL_SPI_CLK    0x40// SD card clock
#define CTRL_SPI_MOSI   0x80// SD card data input


//ctrl read
#define CTRL_SPI_MISO   0x01// SD card data output

//SPI_CLK can be clocked automatically at each read of CTRL_REG
//SPI_CLK should be set in 0 for autoclocking

void spi_r_512(void *dst);
void edLoadInfo();


u16 ed_cfg;
DeviceInfo dev_inf;

u8 edInit() {

    u8 resp;

    edSetBank(BANK_GAME);
    edSetLed(OFF);
    edSetSRAM(OFF);

    flashInit();

    edLoadInfo();

    resp = fsInit();
    if (resp)return resp;

    return 0;
}

void edSetBank(u16 bank) {

    switch (bank) {
        case BANK_GAME:
            ed_cfg &= ~CTRL_ROM_BANK;
            break;
        case BANK_MENU:
            ed_cfg |= CTRL_ROM_BANK;
            break;
    }

    CTRL_REG = ed_cfg;
}

void edSetLed(u8 on) {

    if (on) {
        ed_cfg |= CTRL_LED;
    } else {
        ed_cfg &= ~CTRL_LED;
    }

    CTRL_REG = ed_cfg;
}

void edSetSRAM(u8 on) {

    if (on) {
        ed_cfg |= CTRL_SRM_ON;
    } else {
        ed_cfg &= ~CTRL_SRM_ON;
    }

    CTRL_REG = ed_cfg;

}

void edLoadInfo() {

    edSetBank(BANK_MENU);
    dev_inf.cart_date = swap16(*(vu16 *) (ADDR_CART_DATE + 0));
    dev_inf.cart_time = swap16(*(vu16 *) (ADDR_CART_DATE + 2));
    dev_inf.menu_date = swap16(*(vu16 *) (ADDR_MENU_DATE + 0));
    dev_inf.menu_time = swap16(*(vu16 *) (ADDR_MENU_DATE + 2));
    edSetBank(BANK_GAME);

    dev_inf.flash_type = flashType();

}
//****************************************************************************** spi

u8 spi_rw(u8 val) {

    u8 ret = 0;

    for (u16 i = 0; i < 8; i++) {

        CTRL_REG = (val & CTRL_SPI_MOSI) | ed_cfg;
        CTRL_REG = (val & CTRL_SPI_MOSI) | ed_cfg | CTRL_SPI_CLK;
        ret <<= 1;
        ret |= CTRL_REG;
        CTRL_REG = (val & CTRL_SPI_MOSI) | ed_cfg;
        val <<= 1;
    }

    return ret;
}

u8 spi_r() {

    u8 ret = 0;

    CTRL_REG = ed_cfg | CTRL_SPI_MOSI;

    for (u16 i = 0; i < 8; i++) {

        CTRL_REG = ed_cfg | CTRL_SPI_MOSI | CTRL_SPI_CLK;
        ret <<= 1;
        ret |= CTRL_REG;
        CTRL_REG = ed_cfg | CTRL_SPI_MOSI;
    }

    return ret;
}

void spi_w(u8 val) {

    for (u16 i = 0; i < 8; i++) {

        CTRL_REG = ed_cfg | (val & CTRL_SPI_MOSI);
        CTRL_REG = ed_cfg | (val & CTRL_SPI_MOSI) | CTRL_SPI_CLK;
        CTRL_REG = ed_cfg | (val & CTRL_SPI_MOSI);
        val <<= 1;
    }
}

void spi_select(u8 target) {

    ed_cfg &= ~(CTRL_SDC_SS | CTRL_EXP_SS);

    if (target == SPI_SEL_SDC) {
        ed_cfg |= CTRL_SDC_SS;
    } else if (target == SPI_SEL_EXP) {
        ed_cfg |= CTRL_EXP_SS;
    }

    CTRL_REG = ed_cfg;
}

u8 spi_to_ram(void *dst, u8 slen) {

    u8 *dst8 = (u8 *) dst;
    u16 i;

    while (slen--) {

        spi_w(0xff);

        for (i = 1; i; i++) {
            if (spi_r() == 0xFE)break;
        }
        if (i == 0)return DISK_ERR_RD2;

        spi_r_512(dst8);
        dst8 += 512;

        spi_w(0xff);
        spi_w(0xff);
    }

    return 0;
}
//******************************************************************************

u8 edLoadROM(u8 *path) {

    u8 resp;
    u32 size;
    u8 buff[MAX_BUFF_SIZE];

    gCleanPlan();

    resp = fileOpen(path, FA_READ);
    if (resp)return resp;

    size = fileAvailable();
    if (size > MAX_ROM_SIZE) {
        size = MAX_ROM_SIZE;
    }

    gProgressBar(size, sizeof (buff), 0);

    edSetBank(BANK_GAME);

    for (u32 i = 0; i < size;) {

        u16 block = sizeof (buff);
        if (i + block > size) {
            block = size - i;
        }

        if (i % 0x10000 == 0) {
            flashErase64K(i, 0);
        }

        resp = fileRead(buff, block);
        if (resp)return resp;

        flashWrite((u16 *) buff, (u16 *) i, block);

        i += sizeof (buff);
        gProgressBar(size, block, i);

    }

    resp = fileClose();
    if (resp)return resp;

    return 0;
}

void edStartROM(u8 stop_disk) {

    if (stop_disk) {
        diskStop();
    }
    edSetBank(BANK_GAME);

    VDP_CTRL16 = 0x8006;
    VDP_CTRL16 = 0x8104;

    *(vu8 *) 0xa10009 = 0;
    *(vu8 *) 0xa1000b = 0;
    *(vu8 *) 0xa1000d = 0;

    asm("move.l 0, %sp");
    asm("move.l 4, %a0");
    asm("jmp (%a0)");
}

u8 edIsUpdateRom() {

    u8 *ref_str = "OPEN-EVERDRIVE";
    u8 *rom_str = (u8 *) 0x120;

    for (u16 i = 0; ref_str[i] != 0; i++) {
        if (ref_str[i] != rom_str[i]) {
            return 0;
        }
    }

    return 1;
}

void edUpdateMenu() {

    u8 datetime[4];
    u8 buff[MAX_BUFF_SIZE];
    u32 size = 0x10000;

    edSetBank(BANK_MENU);
    mem_copy(datetime, (void *) ADDR_CART_DATE, 4);

    gProgressBar(size, sizeof (buff), 0);
    flashErase64K(0, 1);

    for (u32 i = 0; i < size;) {

        u16 block = sizeof (buff);
        if (i + block > size) {
            block = size - i;
        }

        edSetBank(BANK_GAME);
        mem_copy(buff, (void *) i, block);
        edSetBank(BANK_MENU);

        if (i == 0) {
            //restore manufacturing date
            mem_copy(&buff[ADDR_CART_DATE], datetime, 4);
        }

        flashWrite((u16 *) buff, (u16 *) i, block);

        i += sizeof (buff);
        gProgressBar(size, block, i);
    }

    edReboot();
}

void edReboot() {

    diskStop();
    edSetBank(BANK_MENU);
    asm("move.l 4, %a0");
    asm("jmp (%a0)");
}
