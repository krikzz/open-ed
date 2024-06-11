
.equ CTRL_REG,   0xA130E0


.macro ldp  pidx, reg, ris
    move.l (4 + \pidx * 4 + \ris * 4)(%sp), \reg
.endm

.global spi_r_512
spi_r_512:
    movem.l %d1/%a0-%a1, -(%sp)
    
    ldp     0, %a1, 3
    move.w  #511, %d1
    move.l  #CTRL_REG, %a0
000:

    lsl.w   #1, %d0
    or.w    (%a0), %d0

    lsl.w   #1, %d0
    or.w    (%a0), %d0

    lsl.w   #1, %d0
    or.w    (%a0), %d0

    lsl.w   #1, %d0
    or.w    (%a0), %d0

    lsl.w   #1, %d0    
    or.w    (%a0), %d0

    lsl.w   #1, %d0
    or.w    (%a0), %d0

    lsl.w   #1, %d0
    or.w    (%a0), %d0

    lsl.w   #1, %d0
    or.w    (%a0), %d0

    move.b  %d0, (%a1)+
    dbra    %d1, 000b

    movem.l (%sp)+, %d1/%a0-%a1
    rts
