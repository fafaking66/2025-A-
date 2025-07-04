#ifndef __DELAY_H
#define __DELAY_H

#include "ti_msp_dl_config.h"

#include "string.h"
#include "stdio.h"
#include "stdlib.h"

void delay_us(unsigned long __us); //搭配滴答定时器实现的精确us延时
void delay_ms(unsigned long ms); //搭配滴答定时器实现的精确ms延时
void delay_1us(unsigned long __us);
void delay_1ms(unsigned long ms);
int fputc(int c, FILE* stream); //发送字符
int fputs(const char* restrict s, FILE* restrict stream); //发送字符串
int puts(const char *_ptr); //发送字符串并自动换行

#endif
