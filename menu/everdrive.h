/* 
 * File:   everdrive.h
 * Author: igor
 *
 * Created on December 7, 2021, 2:19 PM
 */

#ifndef SPI_H
#define	SPI_H

typedef struct {
    u16 flash_type;
    u16 cart_date;
    u16 cart_time;
    u16 menu_date;
    u16 menu_time;
} DeviceInfo;


#define BANK_MENU       1
#define BANK_GAME       0

#define ON              1
#define OFF             0

#define SPI_SEL_OFF     0
#define SPI_SEL_SDC     1
#define SPI_SEL_EXP     2

u8 edInit();
void edSetBank(u16 bank);
void edSetLed(u8 on);
void edSetSRAM(u8 on);
u8 spi_rw(u8 val);
void spi_w(u8 val);
u8 spi_r();
void spi_select(u8 target);
u8 spi_to_ram(void *dst, u8 slen);
u8 edLoadROM(u8 *path);
void edStartROM(u8 stop_disk);
u8 edIsUpdateRom();
void edUpdateMenu();
void edReboot();

extern DeviceInfo dev_inf;

#endif	/* SPI_H */

