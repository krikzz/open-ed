

#include "main.h"

#define FTYPE_FILE      1
#define FTYPE_DIR       2

typedef struct {
    u8 mame[MAX_STR_LEN + 1];
    u8 is_dir;
} RecInfo;

typedef struct {
    u16 selector;
    u16 page_size;
    u16 sel_stack_ptr;
    u16 sel_stack[MAX_SEL_STACK];
    u8 path[MAX_PATH_SIZE + 1];
    RecInfo rec[MAX_ROWS];
} FmState;

u8 fmDirOpen();
void fmDirClose();
u8 fmLoadPage();
void fmDrawPage();
u8 fmOpen();
u8 fmClose();
void fmIncSel();
void fmDecSel();
u8 fmIncPage();
u8 fmDecPage();
u8 fmFileOpen();
u16 fmJoyRead();
void fmDeviceInfo();
void fmAppendVersion(u16 date, u16 time);
void fmAppendDate(u16 date);
void fmAppendTime(u16 time);
void fmUpdate();

FmState fm;

u8 fmanager() {

    u8 resp;
    u16 joy;

    gCleanPlan();
    //fmCleanScreen();

    fm.selector = 0;
    fm.path[0] = 0;
    fm.sel_stack_ptr = 0;

    resp = fmLoadPage();
    if (resp)return resp;

    while (1) {


        fmDrawPage();

        joy = fmJoyRead();
        //joy = sysJoyWait();

        if (joy == JOY_C) {
            fmDeviceInfo();
        }

        if (joy == JOY_STA) {
            edStartROM(1);
        }

        if (joy == JOY_U) {
            fmDecSel();
        }

        if (joy == JOY_D) {
            fmIncSel();
        }

        if (joy == JOY_L) {
            fmDecPage();
        }

        if (joy == JOY_R) {
            fmIncPage();
        }

        if (joy == JOY_A) {
            resp = fmOpen();
            if (resp)return resp;
        }

        if (joy == JOY_B) {
            resp = fmClose();
            if (resp)return resp;
        }
    }

    return 0;
}

u8 fmOpen() {

    u8 resp;

    if (fm.page_size == 0)return 0;

    //gCleanPlan();


    if (fm.rec[fm.selector % MAX_ROWS].is_dir) {

        fm.sel_stack[fm.sel_stack_ptr++] = fm.selector;

        resp = fmDirOpen(fm.selector);
        if (resp)return resp;
        resp = fmLoadPage(fm.selector);
        if (resp)return resp;
        fm.selector = 0;

    } else {
        return fmFileOpen();
    }

    return 0;
}

u8 fmClose() {

    u8 resp;
    if (fm.sel_stack_ptr == 0)return 0; //root path    


    fm.selector = fm.sel_stack[--fm.sel_stack_ptr];
    fmDirClose();
    resp = fmLoadPage();
    if (resp)return resp;

    return 0;
}

u8 fmDirOpen() {

    u8 resp;
    FILINFO inf;

    resp = dirOpen(fm.path);
    if (resp)return resp;

    //skip records
    for (u16 i = 0; i < fm.selector; i++) {
        resp = dirReadNext(&inf);
        if (resp)return resp;
    }

    resp = dirReadNext(&inf);
    if (resp)return resp;

    str_append(fm.path, "/");
    str_append(fm.path, inf.fname);

    return 0;
}

void fmDirClose() {

    u16 cut_ptr = 0;

    for (u16 i = 0; i < MAX_PATH_SIZE; i++) {
        if (fm.path[i] == 0)break;
        if (fm.path[i] == '/')cut_ptr = i;
        if (fm.path[i] == '\\')cut_ptr = i;
    }

    fm.path[cut_ptr] = 0;

}

u8 fmLoadPage() {

    u16 page;
    u8 resp;
    FILINFO inf;

    gCleanPlan();

    page = fm.selector / MAX_ROWS * MAX_ROWS;
    mem_set(fm.rec, 0, sizeof (fm.rec));

    resp = dirOpen(fm.path);
    if (resp)return resp;

    for (u16 i = 0; i < page; i++) {
        resp = dirReadNext(&inf);
        if (resp)return resp;
        if (inf.fname[0] == 0) {
            fm.page_size = i;
            return 0;
        }
    }

    for (u16 i = 0; i < MAX_ROWS; i++) {

        resp = dirReadNext(&inf);
        if (resp)return resp;
        if (inf.fname[0] == 0) {
            fm.page_size = i;
            return 0;
        }

        inf.fname[MAX_STR_LEN] = 0; //cut name len
        str_copy(inf.fname, fm.rec[i].mame);
        fm.rec[i].is_dir = inf.fattrib & AM_DIR;
    }

    fm.page_size = MAX_ROWS;

    return 0;
}

void fmDrawPage() {


    if (fm.page_size == 0) {

        gSetPal(PAL_B2);
        gSetXY(G_SCREEN_W / 2 - 9, G_SCREEN_H / 2 - 1);
        gAppendString("There are no files");
        return;
    }

    gSetXY(BORDER_X, BORDER_Y);

    for (u16 i = 0; i < MAX_ROWS; i++) {
        if (fm.rec[i].mame[0] == 0)break; //end of list

        if (fm.selector % MAX_ROWS == i) {
            gSetPal(PAL_B2);
        } else if (fm.rec[i].is_dir) {
            gSetPal(PAL_B3);
        } else {
            gSetPal(PAL_B1);
        }
        gAppendString(fm.rec[i].mame);
        gConsPrint("");
    }
}

void fmIncSel() {

    u16 page = fm.selector / MAX_ROWS * MAX_ROWS;
    u16 inpage = fm.selector - page;

    inpage++;
    if (inpage >= fm.page_size) {
        inpage = 0;
    }

    fm.selector = page + inpage;
}

void fmDecSel() {

    u16 page = fm.selector / MAX_ROWS * MAX_ROWS;
    u16 inpage = fm.selector - page;

    if (inpage != 0) {
        inpage--;
    } else if (fm.page_size != 0) {
        inpage = fm.page_size - 1;
    }

    fm.selector = page + inpage;
}

u8 fmIncPage() {

    u8 resp;

    if (fm.page_size == 0)return 0;

    if (fm.page_size < MAX_ROWS) {
        u16 page = fm.selector / MAX_ROWS * MAX_ROWS;
        fm.selector = page + fm.page_size - 1;
        return 0;
    }

    //gCleanPlan();

    fm.selector += MAX_ROWS;
    resp = fmLoadPage();
    if (resp)return resp;

    if (fm.page_size == 0) {
        fm.selector -= MAX_ROWS;
        return fmLoadPage();
    }

    u16 page = fm.selector / MAX_ROWS * MAX_ROWS;
    u16 inpage = fm.selector - page;
    if (inpage > fm.page_size) {
        fm.selector = page + fm.page_size - 1;
    }

    return 0;
}

u8 fmDecPage() {

    if (fm.page_size == 0)return 0;

    u16 page = fm.selector / MAX_ROWS * MAX_ROWS;
    u16 inpage = fm.selector - page;

    if (page == 0) {
        fm.selector = 0;
        return 0;
    }

    //gCleanPlan();

    page -= MAX_ROWS;
    fm.selector = page + inpage;

    return fmLoadPage();

}

u8 fmFileOpen() {

    u8 resp;

    gCleanPlan();

    resp = fmDirOpen();
    if (resp)return resp;

    resp = edLoadROM(fm.path);
    if (resp)return resp;

    if (edIsUpdateRom()) {
        fmUpdate();
        return 0;
    }

    edStartROM(1);

    return 0;
}

u16 fmJoyRead() {

    u16 joy;
    static u16 joy_ctr;

    do {
        gVsync();
        joy = sysJoyRead();

        if (joy == JOY_U || joy == JOY_D) {
            joy_ctr++;
        }
        if (joy_ctr >= 20) {
            joy_ctr -= 4;
            break;
        }
    } while (joy != 0);

    while (joy == 0) {
        gVsync();
        joy = sysJoyRead();
        joy_ctr = 0;
    }

    return joy;
}

void fmAppendVersion(u16 date, u16 time) {

    u8 buff[64];
    buff[0] = 0;
    str_append_version(buff, date, time);
    gAppendString(buff);
}

void fmAppendDate(u16 date) {

    u8 buff[64];
    buff[0] = 0;
    str_append_date(buff, date);
    gAppendString(buff);
}

void fmAppendTime(u16 time) {

    u8 buff[64];
    buff[0] = 0;
    str_append_time(buff, time);
    gAppendString(buff);
}

void fmDeviceInfo() {

    gCleanPlan();
    gSetXY(0, 1);
    gSetPal(PAL_B1);
    gConsPrint("-----------Device Information-----------");
    gSetXY(6, 6);
    gSetPal(PAL_B4);


    gConsPrint("device name : Open-ED");
    gConsPrint("");
    gConsPrint("menu version: ");
    fmAppendVersion(dev_inf.menu_date, dev_inf.menu_time);
    gConsPrint("");

    gConsPrint("flash type  : ");
    switch (flashType()) {
        case FLASH_TYPE_M29:
            gAppendString("M29");
            break;
        case FLASH_TYPE_RAM:
            gAppendString("RAM");
            break;
        case FLASH_TYPE_UNK:
            gAppendString("unknown");
            break;
        default:
            gAppendString("unnamed ");
            gAppendHex8(flashType());
            break;
    }
    gConsPrint("");


    gConsPrint("cart date   : ");
    fmAppendDate(dev_inf.cart_date);
    gConsPrint("");

    gConsPrint("cart time   : ");
    fmAppendTime(dev_inf.cart_time);
    gConsPrint("");

    gConsPrint("menu date   : ");
    fmAppendDate(dev_inf.menu_date);
    gConsPrint("");

    gConsPrint("menu time   : ");
    fmAppendTime(dev_inf.menu_time);
    gConsPrint("");

    gConsPrint("homepage    : www.krikzz.com");
    gConsPrint("");

    gSetPal(PAL_B1);
    gSetXY(0, G_SCREEN_H - 3);
    gConsPrint("----------------------------------------");


    sysJoyWait();
    gCleanPlan();

}

void fmUpdate() {

    u16 buff[2];

    gCleanPlan();
    gSetXY(0, 1);
    gSetPal(PAL_B1);
    gConsPrint("---------------Menu Update--------------");

    gSetXY(6, 6);
    gSetPal(PAL_B4);


    edSetBank(BANK_MENU);
    mem_copy(buff, (void *) ADDR_MENU_DATE, 4);
    gConsPrint("current version: ");
    fmAppendVersion(swap16(buff[0]), swap16(buff[1]));
    gConsPrint("");

    edSetBank(BANK_GAME);
    mem_copy(buff, (void *) ADDR_MENU_DATE, 4);
    gConsPrint("new version    : ");
    fmAppendVersion(swap16(buff[0]), swap16(buff[1]));


    gSetXY(0, 13);
    gSetPal(PAL_B1);
    gConsPrint("  do you want to install menu update?   ");

    gSetXY(9, 15);
    gSetPal(PAL_B2);
    gConsPrint("(A)INSTALL    (B)CANCEL");

    gSetPal(PAL_B1);
    gSetXY(0, G_SCREEN_H - 3);
    gConsPrint("----------------------------------------");

    while (1) {

        u16 joy = sysJoyWait();

        if ((joy & JOY_B)) {
            edReboot();
        }

        if ((joy & JOY_A)) {
            gCleanPlan();
            edUpdateMenu();
            return;
        }
    }

}
