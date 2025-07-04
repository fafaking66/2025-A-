#ifndef __MEASURE_H
#define __MEASURE_H

#include "ti_msp_dl_config.h"

#define ADC_LENGTH  2048 //ADC采样数组长度
#define TRIG_LENGTH  40 //触发点数组长度
#define LOADVALUE_LENGTH 4 //自动重装值数组长度
#define ADC_Trigger 1000U //ADC触发值

void Key_Init(void);
void Calculate_First(void);
void Calculate_Second(void);
void Calculate_and_Show(void);
void Find_TriggerPoint(uint16_t start_index);
int compare(const void *a, const void *b);
void Adjust_LoadValue(void);
uint16_t Result_Calculate(uint16_t *IndexDiff, uint16_t len);
void LCD_ShowResult(void);

#endif
