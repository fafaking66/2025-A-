#ifndef __LCD_INIT_H
#define __LCD_INIT_H

#ifndef u8
#define u8 uint8_t
#endif

#ifndef u16
#define u16 uint16_t
#endif

#ifndef u32
#define u32 uint32_t
#endif

#include "ti_msp_dl_config.h"

#define USE_HORIZONTAL 2 //设置横屏或者竖屏显示，0或1为竖屏，2或3为横屏


#if USE_HORIZONTAL == 0 || USE_HORIZONTAL == 1
#define LCD_W 240
#define LCD_H 280

#else
#define LCD_W 280
#define LCD_H 240
#endif

//-----------------LCD端口定义---------------- 

#define LCD_RES_Clr()  DL_GPIO_clearPins(LCD_PORT, LCD_RES_PIN) //RES
#define LCD_RES_Set()  DL_GPIO_setPins(LCD_PORT, LCD_RES_PIN)

#define LCD_DC_Clr()   DL_GPIO_clearPins(LCD_PORT, LCD_DC_PIN) //DC
#define LCD_DC_Set()   DL_GPIO_setPins(LCD_PORT, LCD_DC_PIN)

#define LCD_CS_Clr()   DL_GPIO_clearPins(LCD_PORT, LCD_CS_PIN) //CS
#define LCD_CS_Set()   DL_GPIO_setPins(LCD_PORT, LCD_CS_PIN)

#define LCD_BLK_Clr()  DL_GPIO_clearPins(LCD_PORT, LCD_BLK_PIN) //BLK
#define LCD_BLK_Set()  DL_GPIO_setPins(LCD_PORT, LCD_BLK_PIN)

void LCD_GPIO_Init(void); //初始化GPIO
void LCD_Writ_Bus(u8 dat); //模拟SPI时序
void LCD_WR_DATA8(u8 dat); //写入一个字节
void LCD_WR_DATA(u16 dat); //写入两个字节
void LCD_WR_REG(u8 dat); //写入一个指令
void LCD_Address_Set(u16 x1, u16 y1, u16 x2, u16 y2); //设置坐标函数
void LCD_Init(void); //LCD初始化
void delayLCD_ms(unsigned long ms);

#endif
