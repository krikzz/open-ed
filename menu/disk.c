

#include "main.h"

#define CMD0    0x40    /*software reset*/
#define CMD1    0x41    /*brings card out of idle state*/
#define CMD8    0x48    /*Reserved*/
#define CMD12   0x4C    /*stop transmission on multiple block read*/
#define CMD17   0x51    /*read single block*/
#define CMD18   0x52    /*read multiple block*/
#define CMD58   0x7A    /*reads the OCR register*/
#define CMD55   0x77
#define CMD41   0x69
#define CMD23   0x57
#define CMD24   0x58    /*writes a single block*/
#define CMD25   0x59    /*writes a single block*/
#define	ACMD23 (23 | 0x80)
#define SD_V2   2
#define SD_HC   1

#define DISK_CMD_WAIT_TIME      512
#define DISK_CMD_REP_TIME       256
#define DISK_MODE_NOP           0
#define DISK_MODE_RD            1
#define DISK_MODE_WR            2

u8 diskInitSPI();
u8 _diskRead(u32 sd_addr, void *dst, u16 slen);
u8 _diskInitSPI();
u8 diskCloseRW();


u8 disk_card_type;
u8 disk_mode;
u32 disk_cur_addr;

u8 diskCmd(u8 cmd, u32 arg) {

    u8 resp;
    u8 crc = 0x00;
    u16 wait = DISK_CMD_WAIT_TIME;

    if (cmd == CMD8)crc = 0x87;
    if (cmd == CMD0)crc = 0x95;

    spi_select(SPI_SEL_SDC);
    spi_w(0xff);
    spi_w(cmd);
    spi_w(arg >> 24);
    spi_w(arg >> 16);
    spi_w(arg >> 8);
    spi_w(arg);
    spi_w(crc);
    spi_w(0xff);
    resp = spi_r();

    while (resp == 0xff && wait--) {
        resp = spi_r();
    }

    spi_select(SPI_SEL_OFF);
    return resp;
}

u8 diskInitSPI() {

    u8 resp;

    for (u16 i = 0; i < 4; i++) {

        resp = _diskInitSPI();
        if (resp == 0)break;
    }

    return resp;
}

u8 _diskInitSPI() {

    u8 resp;
    u16 i;

    disk_card_type = 0;
    disk_mode = DISK_MODE_NOP;

    spi_select(SPI_SEL_OFF);

    //diskCmd(CMD12, 0);

    for (u16 i = 0; i < 128; i++) {
        spi_w(0xff);
    }

    resp = diskStop();
    if (resp) {
        //return DISK_ERR_INIT + 0;
        spi_w(0xff);
        spi_w(0xff);
    }

    resp = diskCmd(CMD8, 0x1aa);
    for (i = 0; i < 5; i++) {
        spi_w(0xff);
    }
    if (resp == 0xff)return DISK_ERR_INIT + 1;
    if (resp != 5)disk_card_type = SD_V2;

    if (disk_card_type == SD_V2) {

        for (i = 0; i < DISK_CMD_REP_TIME; i++) {


            resp = diskCmd(CMD55, 0xffff);
            if (resp == 0xff)return DISK_ERR_INIT + 2;
            if (resp != 1)continue;

            resp = diskCmd(CMD41, 0x40300000);
            if (resp == 0xff)return DISK_ERR_INIT + 3;
            if (resp != 0)continue;

            break;
        }
        if (i >= DISK_CMD_REP_TIME)return DISK_ERR_INIT + 4;

        resp = diskCmd(CMD58, 0);
        if (resp == 0xff)return DISK_ERR_INIT + 5;
        spi_select(SPI_SEL_SDC);
        resp = spi_r();
        for (i = 0; i < 3; i++) {
            spi_w(0xff);
        }
        if ((resp & 0x40))disk_card_type |= SD_HC;

    } else {

        i = 0;

        resp = diskCmd(CMD55, 0);
        if (resp == 0xff)return DISK_ERR_INIT + 6;
        resp = diskCmd(CMD41, 0);
        if (resp == 0xff)return DISK_ERR_INIT + 7;

        for (u16 i = 0; i < DISK_CMD_REP_TIME; i++) {
            if (resp < 1) {

                resp = diskCmd(CMD55, 0);
                if (resp == 0xff)return DISK_ERR_INIT + 8;
                if (resp != 1)continue;

                resp = diskCmd(CMD41, 0);
                if (resp == 0xff)return DISK_ERR_INIT + 9;
                if (resp != 0)continue;

            } else {

                resp = diskCmd(CMD1, 0);
                if (resp != 0)continue;
            }

            break;
        }

        if (i >= DISK_CMD_REP_TIME)return DISK_ERR_INIT + 10;
    }

    return 0;
}

u8 diskInit() {

    edSetLed(ON);
    u8 resp = diskInitSPI();
    edSetLed(OFF);
    return resp;
}

//****************************************************************************** read op

u8 diskOpenRead(u32 saddr) {

    u8 resp;
    if (disk_mode == DISK_MODE_RD && saddr == disk_cur_addr)return 0;

    diskCloseRW();
    disk_cur_addr = saddr;
    if ((disk_card_type & SD_HC) == 0)saddr *= 512;
    resp = diskCmd(CMD18, saddr);
    if (resp != 0)return DISK_ERR_RD1;
    spi_select(SPI_SEL_SDC);

    disk_mode = DISK_MODE_RD;

    return 0;
}

u8 _diskRead(u32 sd_addr, void *dst, u16 slen) {

    u8 resp = 0;

    resp = diskOpenRead(sd_addr);
    if (resp)return resp;
    disk_cur_addr += slen;

    resp = spi_to_ram(dst, slen);
    return resp;
}

u8 diskRead(u32 sd_addr, void *dst, u16 slen) {

    edSetLed(ON);
    u8 resp = _diskRead(sd_addr, dst, slen);
    edSetLed(OFF);
    return resp;

}
//****************************************************************************** var

u8 diskCloseRW() {

    u8 resp;
    if (disk_mode == DISK_MODE_NOP)return 0;


    if (disk_mode == DISK_MODE_WR) {
        resp = 0; //bi_spi_write_end();//fixme
        if (resp)return resp;
    }

    spi_select(SPI_SEL_OFF);
    diskCmd(CMD12, 0);
    disk_mode = DISK_MODE_NOP;

    return 0;
}

u8 diskStop() {

    u8 i = 0;
    u8 resp = 0;

    diskCloseRW();

    while (resp != 1 && i++ < 8) {
        resp = diskCmd(CMD0, 0);
    }

    if (resp != 1) {
        return DISK_ERR_STOP;
    }

    return 0;
}
