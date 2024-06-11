/* 
 * File:   flash.h
 * Author: igor
 *
 * Created on December 8, 2021, 3:39 PM
 */

#ifndef FLASH_H
#define	FLASH_H

#define FLASH_TYPE_UNK  0x00
#define FLASH_TYPE_RAM  0x01
#define FLASH_TYPE_M29  0x02


void flashInit();
void flashErase64K(u32 addr, u8 wait_rdy);
void flashWrite(u16 *src, u16 *dst, u32 len);
u8 flashType();

#endif	/* FLASH_H */

