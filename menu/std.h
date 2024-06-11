/* 
 * File:   std.h
 * Author: igor
 *
 * Created on December 7, 2021, 4:42 PM
 */

#ifndef STR_H
#define	STR_H

void mem_set(void *dst, u8 val, u32 size);
void mem_copy(void *dst, void *src, u32 size);
u8 *str_append(u8 *dst, u8 *src);
void str_copy(u8 *src, u8 *dst);
u8* str_append_date(u8 *dst, u16 date);
u8* str_append_time(u8 *dst, u16 time);
u8* str_append_version(u8 *dst, u16 date, u16 time);
u8 *str_append_hex8(u8 *dst, u8 num);
u8 *str_append_num(u8 *dst, u32 num);
u8 decToBcd(u8 val);
u16 swap16(u16 val);

#endif	/* STR_H */

