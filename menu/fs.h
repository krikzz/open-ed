/* 
 * File:   fs.h
 * Author: igor
 *
 * Created on December 7, 2021, 3:40 PM
 */

#ifndef FS_H
#define	FS_H

#include "ff/ff.h"

u8 fsInit();
u8 fileOpen(u8 *path, u8 mode);
u8 fileClose();
u8 fileRead(void *dst, u32 len);
u8 fileWrite(void *src, u32 len);
u8 fileSetPtr(u32 addr);
u8 fileGetInfo(u8 *path, FILINFO *inf);
u8 fileGetSize(u8 *path, u32 *size);
u32 fileAvailable();

u8 dirOpen(u8 *path);
u8 dirReadNext(FILINFO *inf);


#endif	/* FS_H */

