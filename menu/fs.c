
#include "main.h"


FATFS fs;
FIL file_io;
DIR dir;

u8 fsInit() {

    u8 resp;
   
    mem_set(&fs, 0, sizeof (FATFS));
    mem_set(&file_io, 0, sizeof (FIL));
    mem_set(&dir, 0, sizeof (DIR));

    resp = f_mount(&fs, "", 1);
    return resp;
}
//****************************************************************************** files

u8 fileOpen(u8 *path, u8 mode) {
    return f_open(&file_io, (TCHAR *) path, mode);
}

u8 fileClose() {
    return f_close(&file_io);
}

u8 fileRead(void *dst, u32 len) {

    UINT readed;
    u8 resp;

    resp = f_read(&file_io, dst, len, &readed);
    if (resp)return resp;

    return 0;
}

/*
u8 fileWrite(void *src, u32 len) {

    u8 resp;
    UINT written;

    resp = f_write(&file_io, src, len, &written);

    return resp;
}
 */

u8 fileSetPtr(u32 addr) {

    return f_lseek(&file_io, addr);
}

u8 fileGetInfo(u8 *path, FILINFO *inf) {

    u8 resp;

    resp = f_stat(path, inf);
    if (resp)return resp;

    return 0;
}

u8 fileGetSize(u8 *path, u32 *size) {

    u8 resp;
    FILINFO inf;

    resp = fileGetInfo(path, &inf);
    if (resp)return resp;

    *size = inf.fsize;

    return 0;
}

u32 fileAvailable() {
    return file_io.obj.objsize - file_io.fptr;
}

//****************************************************************************** dir

u8 dirOpen(u8 *path) {

    return f_opendir(&dir, (TCHAR *) path);
}

u8 dirReadNext(FILINFO *inf) {

    u8 resp;

    do {
        inf->fname[0] = 0;
        inf->altname[0] = 0;
        resp = f_readdir(&dir, inf);
        if (resp)return resp;
        if (inf->fname[0] == 0)return 0;

    } while (inf->fname[0] == '.' || (inf->fattrib & AM_HID));

    return 0;
}

