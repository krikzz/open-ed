/* 
 * File:   cfg.h
 * Author: igor
 *
 * Created on December 7, 2021, 4:32 PM
 */

#ifndef CFG_H
#define	CFG_H



#define BORDER_X        1
#define BORDER_Y        2

#define MAX_PATH_SIZE   256
#define MAX_ROWS        (G_SCREEN_H - BORDER_Y*2)
#define MAX_STR_LEN     (G_SCREEN_W - BORDER_X*2)
#define MAX_SEL_STACK   32
#define MAX_ROM_SIZE    0x400000
#define MAX_BUFF_SIZE   16384

#define PAL_B1          0x0000   //black bgr, gray txt
#define PAL_B2          0x2000   //black bgr, white txt
#define PAL_B3          0x4000   //black bgr, yellow txt
#define PAL_B4          0x6000   //black bgr, green txt

#define ADDR_CART_DATE  0x1c8
#define ADDR_MENU_DATE  0x1cc
#endif	/* CFG_H */

