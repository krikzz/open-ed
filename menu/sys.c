
#include "main.h"


extern u16 font_base[];

u16 vdp_regs[] = {
    0x8004, 0x8104, 0x8230, 0x832c, 0x8407, 0x8554, 0x8600, 0x8700,
    0x8800, 0x8900, 0x8A00, 0x8B00, 0x8C81, 0x8D2b, 0x8E00, 0x8F01,
    0x9001, 0x9100, 0x9200, 0x93ff, 0x94ff, 0x9500, 0x9600, 0x9780
};

void sysInit(u8 mode, u8 plan_w, u8 plan_h) {

    u16 i;

    //mute psg
    for (i = 0; i < 4; i++) {
        PSG_CTRL = 0x90 | (i << 5) | 15;
        //PSG_CTRL = 0x80 | (0 << 5);
        //PSG_CTRL = 0;
    }

    for (i = 0; i < 0x13; i++) {
        VDP_CTRL16 = vdp_regs[i];
    }

    VDP_CTRL16 = VDP_REG_HSCRL | HSCRL_ADDR(HSCRL);
    VDP_CTRL16 = VDP_REG_PLANA | PLANA_ADDR(APLAN);
    VDP_CTRL16 = VDP_REG_PLANW | PLANW_ADDR(WPLAN);
    VDP_CTRL16 = VDP_REG_PLANB | PLANB_ADDR(APLAN);
    VDP_CTRL16 = VDP_REG_PLANS | PLANS_ADDR(SPLAN);
    VDP_CTRL16 = VDP_REG_MOD4 | mode;
    VDP_CTRL16 = VDP_REG_AINC | 0x02;
    VDP_CTRL16 = VDP_REG_PSIZ | PSIZE(plan_w, plan_h);

    VDP_CTRL16 = VDP_REG_MOD3 | MOD3_HSCRL_L | MOD3_VSCRL_F;


    JOY_CTRL_1 = 0x40;
    JOY_DATA_1 = 0x00;
    JOY_CTRL_2 = 0x40;
    JOY_DATA_2 = 0x00;

    VDP_CTRL32 = VDP_VSRAM_WR(0);
    VDP_DATA32 = 0; //reset scroll


    vdpVramWrite(font_base, 1024, 4096);

    gSetPlan(APLAN);

    VDP_CTRL16 = VDP_REG_MOD2 | MOD2_DISP_ON | MOD2_BIT2 | MOD2_DMA_ON;

}

u16 sysJoyRead() {

    u8 joy;

    JOY_DATA_1 = 0x40;
    asm("nop");
    asm("nop");
    asm("nop");
    joy = (JOY_DATA_1 & 0x3F);
    JOY_DATA_1 = 0x00;
    asm("nop");
    asm("nop");
    asm("nop");
    joy |= (JOY_DATA_1 & 0x30) << 2;

    joy ^= 0xff;

    return joy & 0xff;
}

u16 sysJoyWait() {

    u16 joy = 0;

    do {
        gVsync();
        joy = sysJoyRead();
    } while (joy != 0);


    do {
        gVsync();
        joy = sysJoyRead();
    } while (joy == 0);

    return joy;
}

void gVsync() {

    u16 vdp_state = VDP_STAT_VBL;

    while (vdp_state & VDP_STAT_VBL) {
        vdp_state = VDP_CTRL16;
    }

    while (!(vdp_state & VDP_STAT_VBL)) {
        vdp_state = VDP_CTRL16;
    }
}

void gSetColor(u16 color, u16 val) {

    VDP_CTRL32 = VDP_CRAM_WR(color << 1);
    VDP_DATA16 = val;
}



//****************************************************************************** internal HV funcsions

void vdpVramWrite(u16 *src, u16 dst, u32 len) {

    len >>= 1;
    VDP_CTRL32 = VDP_VRAM_WR(dst);
    while (len--)VDP_DATA16 = *src++;
}

void vdpCramWrite(u16 *src, u16 dst, u16 len) {

    len >>= 1;
    VDP_CTRL32 = VDP_CRAM_WR(dst);
    while (len--)VDP_DATA16 = *src++;
}

void vdpVsramWrite(u16 *src, u16 dst, u16 len) {

    len >>= 1;
    VDP_CTRL32 = VDP_VSRAM_WR(dst);
    while (len--)VDP_DATA16 = *src++;
}

void vdpVramWriteDMA(void *src, u16 dst, u16 len) {

    VDP_CTRL16 = VDP_REG_DMALL | ((len >> 1) & 0xff);
    VDP_CTRL16 = VDP_REG_DMALH | ((len >> 9) & 0xff);

    VDP_CTRL16 = VDP_REG_DMASL | (((u32) src >> 1) & 0xff);
    VDP_CTRL16 = VDP_REG_DMASM | (((u32) src >> 9) & 0xff);
    VDP_CTRL16 = VDP_REG_DMASH | (((u32) src >> 17) & 0x7f);

    VDP_CTRL32 = VDP_VRAM_DMA_RUN(dst);
}

void vdpCramWriteDMA(void *src, u16 dst, u16 len) {


    VDP_CTRL16 = VDP_REG_DMALL | ((len >> 1) & 0xff);
    VDP_CTRL16 = VDP_REG_DMALH | ((len >> 9) & 0xff);

    VDP_CTRL16 = VDP_REG_DMASL | (((u32) src >> 1) & 0xff);
    VDP_CTRL16 = VDP_REG_DMASM | (((u32) src >> 9) & 0xff);
    VDP_CTRL16 = VDP_REG_DMASH | (((u32) src >> 17) & 0x7f);

    VDP_CTRL32 = VDP_CRAM_DMA_RUN(dst);
}
//****************************************************************************** gfx base
u16 g_plan;
u16 g_pal;
u16 g_addr;

void gSetPal(u16 pal) {
    g_pal = pal;
}

void gSetPlan(u16 plan) {
    g_plan = plan;
    VDP_CTRL32 = VDP_VRAM_WR(g_plan + g_addr);
}

void gCleanPlan() {

    u16 len = (G_SCREEN_W * G_SCREEN_H);
    g_addr = 0;

    VDP_CTRL32 = VDP_VRAM_WR(g_plan);
    while (len--) {
        VDP_DATA32 = 0;
    }
    VDP_CTRL32 = VDP_VRAM_WR(g_plan);
}

void gAppendString(u8 *str) {

    while (*str != 0) {
        VDP_DATA16 = *str++ | g_pal;
    }
}

void gAppendChar(u8 val) {
    VDP_DATA16 = val | g_pal;
}

void gConsPrint(u8 *str) {
    g_addr += G_PLAN_W * 2;
    VDP_CTRL32 = VDP_VRAM_WR(g_plan + g_addr);
    gAppendString(str);
}

void gAppendHex4(u8 val) {

    val += (val < 10 ? '0' : '7');
    VDP_DATA16 = val | g_pal;
}

void gAppendHex8(u8 val) {

    gAppendHex4(val >> 4);
    gAppendHex4(val & 15);
}

void gAppendHex16(u16 val) {

    gAppendHex8(val >> 8);
    gAppendHex8(val);
}

void gAppendHex32(u32 val) {

    gAppendHex16(val >> 16);
    gAppendHex16(val);

}

void gPrintHex(void *src, u16 len) {

    u8 *ptr8 = src;
    u16 i;

    for (i = 0; i < len; i++) {
        if (i % 16 == 0)gConsPrint("");
        gAppendHex8(*ptr8++);

    }
}

void gAppendNum(u32 num) {

    u16 i;
    u8 buff[11];
    u8 *str = (u8 *) & buff[10];


    *str = 0;
    if (num == 0)*--str = '0';
    for (i = 0; num != 0; i++) {

        *--str = num % 10 + '0';
        num /= 10;
    }

    gAppendString(str);

}

void gSetXY(u16 x, u16 y) {

    g_addr = (x + y * G_PLAN_W) * 2;
    VDP_CTRL32 = VDP_VRAM_WR(g_plan + g_addr);
}

void gProgressBar(u32 rom_size, u16 block_size, u32 progress) {

    if (progress == 0) {

        gSetPal(PAL_B2);
        gSetXY(G_SCREEN_W / 2 - 5, G_SCREEN_H / 2 - 4);
        gConsPrint("Loading...");

        gSetPal(PAL_B1);
        gSetXY(4, G_SCREEN_H / 2 - 1);
        for (u32 i = 0; i < rom_size; i += block_size) {
            if ((i / block_size) % 32 == 0) {
                gConsPrint("");
            }
            gAppendString(".");
        }
        gSetXY(4, G_SCREEN_H / 2);

    } else {

        gSetPal(PAL_B4);
        gAppendString("*");

        if ((progress / block_size) % 32 == 0) {
            gConsPrint("");
        }
    }
}
