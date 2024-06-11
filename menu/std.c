

#include "main.h"

void mem_set(void *dst, u8 val, u32 size) {

    u8 *dst8 = dst;
    while (size--)*dst8++ = val;
}

void mem_copy(void *dst, void *src, u32 size) {

    u8 *dst8 = dst;
    u8 *src8 = src;
    while (size--)*dst8++ = *src8++;
}

u8 *str_append(u8 *dst, u8 *src) {

    while (*dst != 0)dst++;
    while (*src != 0)*dst++ = *src++;
    *dst = 0;
    return dst;
}

void str_copy(u8 *src, u8 *dst) {

    while (*src != 0)*dst++ = *src++;
    *dst = 0;
}

u8* str_append_date(u8 *dst, u16 date) {

    str_append_hex8(dst, decToBcd(date & 31));
    str_append(dst, ".");
    str_append_hex8(dst, decToBcd((date >> 5) & 15));
    str_append(dst, ".");
    return str_append_num(dst, (date >> 9) + 1980);
}

u8* str_append_time(u8 *dst, u16 time) {

    str_append_hex8(dst, decToBcd(time >> 11));
    str_append(dst, ":");
    str_append_hex8(dst, decToBcd((time >> 5) & 0x3F));
    str_append(dst, ":");
    return str_append_hex8(dst, decToBcd((time & 0x1F) * 2));
}

u8* str_append_version(u8 *dst, u16 date, u16 time) {

    str_append_num(dst, ((date >> 9) + 1980) % 100);
    str_append_hex8(dst, decToBcd((date >> 5) & 15));
    str_append_hex8(dst, decToBcd(date & 31));
    str_append(dst, ".");
    str_append_hex8(dst, decToBcd(time >> 11));
    return str_append_hex8(dst, decToBcd((time >> 5) & 0x3F));
}

u8 *str_append_hex8(u8 *dst, u8 num) {

    u8 buff[3];
    buff[2] = 0;
    buff[0] = (num >> 4) + '0';
    buff[1] = (num & 15) + '0';

    if (buff[0] > '9')buff[0] += 7;
    if (buff[1] > '9')buff[1] += 7;

    return str_append(dst, buff);
}

u8 *str_append_num(u8 *dst, u32 num) {

    u16 i;
    u8 buff[11];
    u8 *str = (u8 *) & buff[10];

    *str = 0;
    if (num == 0)*--str = '0';
    for (i = 0; num != 0; i++) {
        *--str = num % 10 + '0';
        num /= 10;
    }

    return str_append(dst, str);
}

u8 decToBcd(u8 val) {

    if (val > 99)val = 99;
    return (val / 10 << 4) | val % 10;
}

u16 swap16(u16 val) {
    return (val << 8) | (val >> 8);
}
