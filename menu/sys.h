/* 
 * File:   sys.h
 * Author: krik
 *
 * Created on October 22, 2014, 9:02 PM
 */

//38 * 205 = 7790
//38 * 164 = 6232

#ifndef SYS_H
#define	SYS_H

#define s8      char
#define s16     short
#define s32     long

#define u8      unsigned char
#define u16     unsigned short
#define u32     unsigned long
#define u64     unsigned long long


#define vu8      volatile unsigned char
#define vu16     volatile unsigned short
#define vu32     volatile unsigned long
#define vu64     volatile unsigned long long

#define G_SCREEN_W      40
#define G_SCREEN_H      28
#define G_PLAN_W        64

#define G_MODE_320x224  0x81
#define G_MODE_256x224  0x00

typedef struct {
    u16 y;
    u8 size;
    u8 next;
    u16 tile;
    u16 x;
} Sprite;

//******************************************************************************
void gSetPal(u16 pal);
void gSetPlan(u16 plan);
void gCleanPlan();
void gAppendString(u8 *str);
void gAppendChar(u8 val);
void gConsPrint(u8 *str);
void gAppendHex4(u8 val);
void gAppendHex8(u8 val);
void gAppendHex16(u16 val);
void gAppendHex32(u32 val);
void gPrintHex(void *src, u16 len);
void gAppendNum(u32 num);
void gSetXY(u16 x, u16 y);
void gProgressBar(u32 rom_size, u16 block_size, u32 progress);
//******************************************************************************
void gVsync();
void sysInit(u8 mode, u8 plan_w, u8 plan_h);
u16 sysJoyWait();
u16 sysJoyRead();
void gSetColor(u16 color, u16 val);

void vdpVramWrite(u16 *src, u16 dst, u32 len);
void vdpCramWrite(u16 *src, u16 dst, u16 len);
void vdpVsramWrite(u16 *src, u16 dst, u16 len);
void vdpVramWriteDMA(void *src, u16 dst, u16 len);
void vdpCramWriteDMA(void *src, u16 dst, u16 len);
void vdpVsraWriteDMA(void *src, u16 dst, u16 len);



#define VDP_REG_MOD1    0x8000
#define VDP_REG_MOD2    0x8100
#define VDP_REG_PLANA   0x8200 //plan-a addr. XXX0_0000_0000_0000 
#define VDP_REG_PLANW   0x8300 //windows addr
#define VDP_REG_PLANB   0x8400 //plan-b addr. XXX0_0000_0000_0000 
#define VDP_REG_PLANS   0x8500 //sprite table addr.  H32 mod=XXXX_XXX0_0000_0000, H40 mod=XXXX_XX00_0000_0000
#define VDP_REG_BGCLR   0x8700 //BG color. 00PP_CCCC P=pal num, C=color num
#define VDP_REG_HINT    0x8A00 //set hint line number
#define VDP_REG_MOD3    0x8B00
#define VDP_REG_MOD4    0x8C00
#define VDP_REG_HSCRL   0x8D00 //hscroll table addr. XXXX_XX00_0000_0000
#define VDP_REG_AINC    0x8F00 //auto increment
#define VDP_REG_PSIZ    0x9000 //plan size
#define VDP_REG_WIN_X   0x9100 //cell pos x
#define VDP_REG_WIN_Y   0x9200 //cell pos y
#define VDP_REG_DMALL   0x9300 //dma len lo
#define VDP_REG_DMALH   0x9400 //dma len hi
#define VDP_REG_DMASL   0x9500 //dma src lo. A8-A1
#define VDP_REG_DMASM   0x9600 //dma src mid. A16-A9
#define VDP_REG_DMASH   0x9700 //dma src hi. A22-A17

#define MOD1_CTR_OFF    0x02
#define MOD1_COLOR_ON   0x04
#define MOD1_HINT_ON    0x10

#define MOD2_BIT2       0x04 //always must be set
#define MOD2_V30        0x08
#define MOD2_V28        0x00
#define MOD2_DMA_ON     0x10
#define MOD2_VINT_ON    0x20
#define MOD2_DISP_ON    0x40

#define MOD3_HSCRL_F    0x00 //hscroll mode full
#define MOD3_HSCRL_C    0x02 //hscroll mode cell
#define MOD3_HSCRL_L    0x03 //hscroll mode line
#define MOD3_VSCRL_F    0x00 //vscroll mode full
#define MOD3_VSCRL_C    0x04 //vscroll mode cell
#define MOD3_EINT_ON    0x08 //enable external interrupt

#define MOD4_H32_MOD    0x00
#define MOD4_H40_MOD    0x81
#define MOD4_INTER1X    0x02 //interlace
#define MOD4_INTER2X    0x06 //interlace double resolution


#define DMA_TO_VRAM     0x00
#define DMA_VRAM_FILL   0x80
#define DMA_VRAM_COPY   0xC0

#define STAT_PAL_MODE   0x0001
#define STAT_DMA_BUSY   0x0002
#define STAT_HBLANK     0x0004
#define STAT_VBLANK     0x0008
#define STAT_ODD_FRAME  0x0010
#define STAT_COLIS      0x0020
#define STAT_SPR_OVER   0x0040
#define STAT_VINT       0x0080
#define STAT_FIFO_FUL   0x0100
#define STAT_VINT_EMP   0x0200

#define PLANA_ADDR(addr) ((addr >> 10) & 0x38)
#define PLANW_ADDR(addr) ((addr >> 10) & 0x3C)
#define PLANB_ADDR(addr) ((addr >> 13) & 0x07)
#define PLANS_ADDR(addr) ((addr >>  9) & 0x7F)
#define HSCRL_ADDR(addr) ((addr >> 10) & 0x3F)
#define PSIZE(w, h) ((w - 1) >> 5) | ((h - 1) >> 1 & 0x30)

#define VDP_DATA        0xC00000
#define VDP_CTRL        0xC00004
#define VDP_DATA16      *((vu16 *)VDP_DATA)
#define VDP_DATA32      *((vu32 *)VDP_DATA)
#define VDP_CTRL16      *((vu16 *)VDP_CTRL)
#define VDP_CTRL32      *((vu32 *)VDP_CTRL)
#define VDP_HV_CTR      *((vu16 *)0xC00008)
#define SYS_VERSION     *(volatile u16*) 0xa10000

#define JOY_DATA_1      *((volatile u8 *) 0xa10003)
#define JOY_CTRL_1      *((volatile u8 *) 0xa10009)

#define JOY_DATA_2      *((volatile u8 *) 0xa10005)
#define JOY_CTRL_2      *((volatile u8 *) 0xa1000B)

#define PSG_CTRL        *((volatile u8 *) 0xC00011)

#define VDP_VRAM_WR(adr)        ((0x4000 + ((adr) & 0x3FFF)) << 16) + (((adr) >> 14) | 0x00)
#define VDP_VRAM_RD(adr)        ((0x0000 + ((adr) & 0x3FFF)) << 16) + (((adr) >> 14) | 0x00)
#define VDP_CRAM_WR(adr)        ((0xC000 + ((adr) & 0x3FFF)) << 16) + (((adr) >> 14) | 0x00)
#define VDP_VSRAM_WR(adr)       ((0x4000 + ((adr) & 0x3FFF)) << 16) + (((adr) >> 14) | 0x10)


#define VDP_CPY_DMA_RUN(adr)    ((0x4000 + ((adr) & 0x3FFF)) << 16) + (((adr) >> 14) | 0xC0)
#define VDP_VRAM_DMA_RUN(adr)   ((0x4000 + ((adr) & 0x3FFF)) << 16) + (((adr) >> 14) | 0x80)
#define VDP_CRAM_DMA_RUN(adr)   ((0xC000 + ((adr) & 0x3FFF)) << 16) + (((adr) >> 14) | 0x80)
#define VDP_VSRAM_DMA_RUN(adr)  ((0x4000 + ((adr) & 0x3FFF)) << 16) + (((adr) >> 14) | 0x90)


#define VDP_DMA_VRAM(src, dst, len)       \
VDP_CTRL32 = ((0x8F02) << 16) + ((0x9300 + (((len) >> 1) & 0xff)) << 0);       \
VDP_CTRL32 = ((0x9400 + (((len) >> 9) & 0xff)) << 16) + ((0x9500 + (((src) >> 1) & 0xff)) << 0);        \
VDP_CTRL32 = ((0x9600 + (((src) >> 9) & 0xff)) << 16) + ((0x9700 + (((src) >> 17) & 0x7f)) << 0);       \
VDP_CTRL32 = VDP_VRAM_DMA_RUN(dst)

#define VDP_DMA_CRAM(src, dst, len)       \
VDP_CTRL32 = ((0x8F02) << 16) + ((0x9300 + (((len) >> 1) & 0xff)) << 0);       \
VDP_CTRL32 = ((0x9400 + (((len) >> 9) & 0xff)) << 16) + ((0x9500 + (((src) >> 1) & 0xff)) << 0);        \
VDP_CTRL32 = ((0x9600 + (((src) >> 9) & 0xff)) << 16) + ((0x9700 + (((src) >> 17) & 0x7f)) << 0);       \
VDP_CTRL32 = VDP_CRAM_DMA_RUN(dst)

#define SPR_ARG_PR      0x8000
#define SPR_ARG_VF      0x1000
#define SPR_ARG_HF      0x0800
#define SPR_ARG_PL(pal) ((pal & 3) << 13)

#define SPR_SIZE(w, h)   ((((w) - 1) << 2) | ((h) - 1))
#define SPR_ATTR(pal, pri, flipV, flipH)   (((flipH) << 11) + ((flipV) << 12) + ((pal) << 13) + ((pri) << 15))

#define FONT_ADDR       1024
#define SPLAN           0xB800 //1K
#define HSCRL           0xBC00 //1K
#define APLAN           0xC000
#define BPLAN           0xE000

#define WPLAN           0

#define VDP_STAT_VBL    (1 << 3)

#define JOY_A   0x0040
#define JOY_B   0x0010
#define JOY_C   0x0020
#define JOY_STA 0x0080
#define JOY_U   0x0001
#define JOY_D   0x0002
#define JOY_L   0x0004
#define JOY_R   0x0008


#define REGION_J        0x00
#define REGION_U        0x80
#define REGION_E        0xC0
#define REGION_J_PAL    0x40
#define REGIO_MASK      0xC0



#define Z80_HALT_PORT       0xA11100
#define Z80_RESET_PORT      0xA11200
#define Z80_RAM             0xA00000
#define Z80_YM2612          0xA04000
#define Z80_BANK_REGISTER   0xA06000
#define Z80_PSG             0xA07F11

#define Z80_BUSREQ_ON   *((vu16 *) Z80_HALT_PORT) = 0x0100
#define Z80_BUSREQ_OFF  *((vu16 *) Z80_HALT_PORT) = 0x0000
#define Z80_BUS_REQADY  (*((vu16 *) Z80_HALT_PORT) & 0x0100)

#define Z80_RESET_ON    *((vu16 *) Z80_RESET_PORT) = 0
#define Z80_RESET_OFF   *((vu16 *) Z80_RESET_PORT) = 0x0100

#endif	/* SYS_H */

