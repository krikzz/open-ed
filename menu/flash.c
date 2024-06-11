

#include "main.h" 

u8 flashInit_ram();
void flashWrite_ram(u16 *src, u16 *dst, u32 len);

u8 flashInit_m29();
void flashWrite_m29(u16 *src, u16 *dst, u32 len);
void flashBusyWait_m29();
void flashErase_m29(u32 addr);
void flashUnlock_m29();
void flashReset_m29();


u8 flash_type;

void flashInit() {


    if (flashInit_ram()) {
        flash_type = FLASH_TYPE_RAM;
    } else if (flashInit_m29()) {
        flash_type = FLASH_TYPE_M29;
    } else {
        flash_type = FLASH_TYPE_UNK;
    }

    flashReset_m29();

}

void flashWrite(u16 *src, u16 *dst, u32 len) {

    if (flash_type == FLASH_TYPE_RAM) {
        flashWrite_ram(src, dst, len);
        return;
    }

    if (flash_type == FLASH_TYPE_M29) {
        flashWrite_m29(src, dst, len);
        return;
    }

}

void flashErase64K(u32 addr, u8 wait_rdy) {

    if (flash_type == FLASH_TYPE_M29) {
        flashErase_m29(addr);
        if (wait_rdy) {
            flashBusyWait_m29();
        }
    }
}

u8 flashType() {
    return flash_type;
}

//****************************************************************************** RAM

u8 flashInit_ram() {


    vu16 *ptr = (vu16 *) 0x10000;
    vu16 old_val = *ptr;
    vu16 new_val;

    *ptr ^= 0xffff;
    new_val = *ptr;
    *ptr = old_val;

    if ((old_val ^ 0xffff) == new_val)return 1;

    return 0;
}

void flashWrite_ram(u16 *src, u16 *dst, u32 len) {

    len /= 2;

    while (len--) {

        *dst++ = *src++;
    }
}
//****************************************************************************** M29 series

u8 flashInit_m29() {

    
    flashReset_m29();
    return 1; //always true because any other types of flash is not implemented
}

void flashWrite_m29(u16 *src, u16 *dst, u32 len) {

    flashBusyWait_m29();

    flashUnlock_m29();
    len /= 4;

    while (len--) {

        while (*(vu16*) 0 != *(vu16*) 0); //flashBusyWait()

        //flash must support double word program instruction (CMD 555-50)
        *(vu16 *) (0x555 * 2) = 0x50;
        *dst++ = *src++;
        *dst++ = *src++;
    }

    flashBusyWait_m29();
    flashReset_m29();
}

void flashErase_m29(u32 addr) {

    flashReset_m29();

    *((vu16 *) (0x555 * 2)) = 0xaa;
    *((vu16 *) (0x2aa * 2)) = 0x55;
    *((vu16 *) (0x555 * 2)) = 0x80;
    *((vu16 *) (0x555 * 2)) = 0xaa;
    *((vu16 *) (0x2aa * 2)) = 0x55;

    for (u16 i = 0; i < 8; i++) {

        *((vu16 *) addr) = 0x30;
        addr += 8192;
    }
}

void flashBusyWait_m29() {
    while (*(vu16*) 0 != *(vu16*) 0);
}

void flashUnlock_m29() {

    *(vu16 *) (0x555 * 2) = 0xaa;
    *(vu16 *) (0x2aa * 2) = 0x55;
    *(vu16 *) (0x555 * 2) = 0x20;
}

void flashReset_m29() {

    *(vu16 *) (0) = 0xf0;
    *(vu16 *) (0) = 0x90;
    *(vu16 *) (0) = 0x00;
}


