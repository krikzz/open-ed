
#include "main.h"


void mainInit();
void printError(u8 error);

int main() {

    u8 resp;

    mainInit();


    while (1) {

        resp = edInit();

        if (resp) {
            printError(resp);
            continue;
        }

        resp = fmanager();
        printError(resp);
    }


    return 0;
}

void mainInit() {

    static u16 pal_std[] = {
        0x000, 0x444, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0xAAA,
        0x000, 0x444, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0xFFF,
        0x000, 0x444, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x2cc,
        0x000, 0x444, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x0F0
    };

    sysInit(G_MODE_320x224, 64, 32);
    for (u16 i = 0; i < 16 * 4; i++) {
        gSetColor(i, pal_std[i]);
    }
    gCleanPlan();
}

void printError(u8 error) {

    extern u8 dresp;

    gCleanPlan();
    gSetPal(0);
    gSetXY(G_SCREEN_W / 2 - 6, G_SCREEN_H / 2 - 1);

    gConsPrint("ERROR: ");
    gAppendHex8(error);
    if (error < 19) {//disk error details
        gAppendString(":");
        gAppendHex8(dresp);
    }

    sysJoyWait();
}
