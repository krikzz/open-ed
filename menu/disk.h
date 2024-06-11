/* 
 * File:   disk.h
 * Author: igor
 *
 * Created on December 7, 2021, 1:10 PM
 */

#ifndef DISK_H
#define	DISK_H

#define DISK_ERR_INIT   0xC0
#define DISK_ERR_STOP   0xD0
#define DISK_ERR_RD1    0xD2
#define DISK_ERR_RD2    0xD3 

#define DISK_ERR_WR1    0xD4
#define DISK_ERR_WR2    0xD5
#define DISK_ERR_WR3    0xD6
#define DISK_ERR_WR4    0xD7
#define DISK_ERR_WR5    0xD8

u8 diskInit();
u8 diskRead(u32 sd_addr, void *dst, u16 slen);
u8 diskStop();
u8 diskCloseRW();

#endif	/* DISK_H */

