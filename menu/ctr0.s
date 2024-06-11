.text
    .org	0x0000

    .long   0, 0x200, BER, AER, IER, INT, INT, INT
    .long   INT, INT, INT, INT, INT, INT, INT, INT
    .long   INT, INT, INT, INT, INT, INT, INT, INT
    .long   INT, INT, INT, INT, HBL, INT, VBL, INT
    .long   INT, INT, INT, INT, INT, INT, INT, INT
    .long   INT, INT, INT, INT, INT, INT, INT, INT
    .long   INT, INT, INT, INT, INT, INT, INT, INT
    .long   INT, INT, INT, INT, INT, INT, INT, INT

    .ascii  "SEGA MEGA DRIVE "      | SEGA must be the first four chars for TMSS
    .ascii  "KRIKzz 2021.DEC "
    .ascii  "OPEN-EVERDRIVE  "      | export name
    .ascii  "                "
    .ascii  "www.krikzz.com  "
    .ascii  "OPEN-EVERDRIVE  "      | domestic (Japanese) name
    .ascii  "Designed by     "
    .ascii  "Igor Golubovskiy"
    .ascii  "GM MK-0000 -01"
    .word   0x0000                  | checksum - not needed
    .ascii  "J6              "
    .long   0x00ff0000, 0x00ff7fff  | ROM start, end
    .long   0x00ff8000, 0x00ffffff  | RAM start, end
    .ascii  "RA"                    | SRAM
    .word   0xF820                  | SRAM type
    .long   0x00200001, 0x0023ffff  | SRAM start, end
    .ascii  "    "                  | modem support
    .ascii  "        "              | 
    .long   0x5B07F60A              | cart date-time. DOS format, intel endian
    .long   0x00000000              | menu date-time. DOS format, intel endian
    .ascii  "                "      | 
    .ascii  "                "      |
    .ascii  "F               "      | enable any hardware configuration

RST:
    move.w  #0x2700, %sr            | disable interrupts
    tst.l   0xa10008                | check CTRL1 and CTRL2 setup
    bne.b   1f
    tst.w   0xa1000c                | check CTRL3 setup
1:
    bne.b   skip_tmss               | if any controller control port is setup, skip TMSS handling
    move.b  0xa10001, %d0
    andi.b  #0x0f, %d0              | check hardware version
    beq.b   skip_tmss               | 0 = original hardware, TMSS not present
    move.l  #0x53454741, 0xa14000   | Store Sega Security Code "SEGA" to TMSS
skip_tmss:
    lea     0, %sp

|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    lea     0xc00008, %a4 | the default global VDP hvcounter port
    lea     0xc00004, %a5 | the default global VDP ctrl port
    lea     0xc00000, %a6 | the default global VDP data port

    move.w  #0x8104, (%a5)  | disable display to speed up vdp transfers
    move.w  #0x8f02, (%a5)

| clear cram
    move.l  #0xc0000000, (%a5)
    move.w  #0x40-1, %d0
    moveq   #0, %d1
1:
    move.w  %d1, (%a6)
    dbra    %d0, 1b

| clear vram
    move.l  #0x40000000, (%a5)
    move.w  #0x2000-1, %d0
    moveq   #0, %d1
1:
    move.w  %d1, (%a6)
    move.w  %d1, (%a6)
    move.w  %d1, (%a6)
    move.w  %d1, (%a6)
    dbra    %d0, 1b

| clear vsram
    move.l  #0x40000010, (%a5)
    move.w  #0x50-1, %d0
    moveq   #0, %d1
1:
    move.w  %d1, (%a6)
    dbra    %d0, 1b

| default vdp reg values
    move.w  #0x8004, (%a5)

    move.w  #0x8154, (%a5) | display enabled | dma disabled | vblank disabled | ntsc
    move.w  #0x8200, (%a5)
    move.w  #0x8300, (%a5)
    move.w  #0x8400, (%a5)
    move.w  #0x855e, (%a5)
    move.w  #0x8600, (%a5)
    move.w  #0x8700, (%a5)
    move.w  #0x8800, (%a5)
    move.w  #0x8900, (%a5)
    move.w  #0x8a00, (%a5)
    move.w  #0x8b00, (%a5)
    move.w  #0x8c81, (%a5)
    move.w  #0x8d00, (%a5)
    move.w  #0x8e00, (%a5)
    move.w  #0x8f02, (%a5)
    move.w  #0x9011, (%a5)
    move.w  #0x9100, (%a5)
    move.w  #0x9200, (%a5)

| copy code to ram

    move.w  #16383, %d0
    move.l  #0x000000, %a0
    move.l  #0xff0000, %a1
1:
    move.l  (%a0)+, (%a1)+
    dbra    %d0, 1b
    jmp     main

VBL:
HBL:
INT:
    rte

BER:
    move.l  #0xc0000000, 0xc00004
    move.w  #0x00e, 0xc00000
    rte

AER:
    move.l  #0xc0000000, 0xc00004
    move.w  #0x0ee, 0xc00000
    rte

IER:
    move.l  #0xc0000000, 0xc00004
    move.w  #0x0e0, 0xc00000
    rte


.global font_base
font_base:
.incbin "font.bin"




