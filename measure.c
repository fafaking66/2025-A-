#include "measure.h"

#include "PWM_Generate.h"
#include "ADC.h"
#include "delay.h"
#include "lcd.h"
#include "lcd_init.h"
#include "adf4002.h"
#include <stdlib.h>
#include <string.h>

extern volatile uint16_t ADC_Value[ADC_LENGTH]; //ADC转换后的数据
extern volatile uint16_t ADC_Value2[ADC_LENGTH]; //ADC转换后的数据

extern volatile uint16_t ADC_Interrupt_Flag; //ADC中断次数
extern volatile uint16_t ADC_Flag; //记录现在在处理的是ADC0还是ADC1的数据

extern volatile uint32_t Freq_Final; //最终频率

// uint16_t LoadValue[LOADVALUE_LENGTH] = {7U, 39U, 79U, 499U, 799U, 4999U, 7999U, 65519U}; //预设好的自动重装值
// uint16_t LoadValue[LOADVALUE_LENGTH] = {39U, 79U, 499U, 799U, 4999U, 7999U, 65519U}; //预设好的自动重装值
uint16_t LoadValue[LOADVALUE_LENGTH] = {39U, 79U, 499U, 799U}; //预设好的自动重装值
uint16_t LoadValue_Index = 0; //自动重装值数组索引

uint16_t TriggerIndex[TRIG_LENGTH]; //触发点索引
uint16_t IndexDiff[TRIG_LENGTH]; //触发点索引的逐差
uint16_t TriggerCount = 0; //触发次数
uint8_t TriggerFlag = 0; //是否触发
uint16_t IndexDiff_Final = 0; //索引差，用于算频率
uint16_t StartIndex = 0; //从哪里开始找触发点

uint32_t Freq = 0; //第一个ADC计算得到的频率
uint32_t Freq2 = 0; //第二个ADC计算得到的频率
volatile uint32_t DutyCycle = 10;  // 初始占空比10%

//按键初始化
void Key_Init(void)
{
    NVIC_ClearPendingIRQ(KEY_GPIOA_INT_IRQN); //清除中断标志位
    NVIC_EnableIRQ(KEY_GPIOA_INT_IRQN); //使能中断

    NVIC_ClearPendingIRQ(KEY_GPIOB_INT_IRQN); //清除中断标志位
    NVIC_EnableIRQ(KEY_GPIOB_INT_IRQN); //使能中断
}

//比较函数，用于排序
int compare(const void *a, const void *b)
{
    return (*(uint16_t*)a - *(uint16_t*)b);
}

/**
 * @brief 计算统计结果
 * @param IndexDiff 未排序的原始数组
 * @param len 数组长度
 * @return uint16_t 最终结果（众数或中位数）
 */
uint16_t Result_Calculate(uint16_t *IndexDiff, uint16_t len)
{
    if (len == 0) return 0; //空数组保护

    /*---- 数组排序 ----*/
    uint16_t *sorted = malloc(len * sizeof(uint16_t)); //分配内存
    memcpy(sorted, IndexDiff, len * sizeof(uint16_t)); //复制数组
    qsort(sorted, len, sizeof(uint16_t), compare); //快速排序

    /*---- 计算中位数 ----*/
    float median;
    if (len % 2 == 0)
    {
        median = (sorted[len / 2 - 1] + sorted[len / 2]) / 2.0f;
    }
    else
    {
        median = sorted[len / 2];
    }

    /*---- 寻找众数 ----*/
    uint16_t current_val = sorted[0]; //当前的数
    uint16_t current_cnt = 1; //当前的数出现次数
    uint16_t max_cnt = 1; //众数出现次数
    uint16_t mode = current_val; //众数

    for (uint16_t i = 1; i < len; i++)
    {
        if (sorted[i] == current_val)
        {
            current_cnt ++;
        }
        else
        {
            current_val = sorted[i];
            current_cnt = 1;
        }

        //更新众数（相同次数时取较小值）
        if ((current_cnt > max_cnt) || (current_cnt == max_cnt && current_val < mode))
        {
            max_cnt = current_cnt;
            mode = current_val;
        }
    }

    /*---- 决定最终输出 ----*/
    uint16_t final_result;
    if (max_cnt > 1) //众数出现大于固定次数
    {
        final_result = mode; //有真实众数
    }
    else
    {
        final_result = (uint16_t)(median + 0.5f); //四舍五入取中位数
    }

    free(sorted);
    return final_result;
}

//自动调整采样频率
void Adjust_LoadValue(void)
{
    if (LoadValue_Index < LOADVALUE_LENGTH)
    {
        LoadValue_Index ++; //从低到高调整自动重装值
        if (ADC_Flag == 0)
            DL_TimerG_setLoadValue(TIMER_ADC_INST, LoadValue[LoadValue_Index]);
        else if (ADC_Flag == 1)
            DL_TimerG_setLoadValue(TIMER_ADC2_INST, LoadValue[LoadValue_Index]);
    }
}

//首次处理ADC数据
void Calculate_First(void)
{
    LoadValue_Index = 0;
    IndexDiff_Final = 0;

    StartIndex = 0;
    TriggerCount = 0;
    TriggerFlag = 0;

    if (ADC_Flag == 0)
        Freq = 0;
    else if (ADC_Flag == 1)
        Freq2 = 0;

    //找到所有的触发点
    do {
        Find_TriggerPoint(StartIndex);
        if (TriggerFlag == 1)
            StartIndex = TriggerIndex[TriggerCount - 1] + 3;
    }while (TriggerFlag == 1);

    //如果采样到的周期多于10个或者自动重装值不能再调了
    if (TriggerCount >= 10)
    {
        Calculate_and_Show(); //进行最终数据处理与显示
    }
    else if (LoadValue_Index >= (LOADVALUE_LENGTH - 1)) //如果自动重装值不能再调了
    {
        if (ADC_Flag == 0)
            Freq = 0;
        else if (ADC_Flag == 1)
            Freq2 = 0;
        
        LCD_ShowResult();
        ADC_Interrupt_Flag = 0;

        if (ADC_Flag == 0)
        {
            ADC_Flag = 1; //切换至下一个ADC
            DL_TimerG_startCounter(TIMER_ADC2_INST); //开始第二个通道的ADC转换
        }
    }
    else
    {
        Adjust_LoadValue(); //调整采样频率
        if (ADC_Flag == 0)
            DL_TimerG_startCounter(TIMER_ADC_INST); //开始下一轮的转换
        if (ADC_Flag == 1)
            DL_TimerG_startCounter(TIMER_ADC2_INST); //开始下一轮的转换
    }
}

//调整采样频率后处理ADC数据
void Calculate_Second(void)
{
    StartIndex = 0;
    TriggerCount = 0;
    TriggerFlag = 0;

    //找到所有的触发点
    do {
        Find_TriggerPoint(StartIndex);
        if (TriggerFlag == 1)
            StartIndex = TriggerIndex[TriggerCount - 1] + 3;
    }while (TriggerFlag == 1);

    if (TriggerCount >= 10) //如果采样到的周期多于10个
    {
        Calculate_and_Show(); //进行最终数据处理与显示
    }
    else if (LoadValue_Index >= (LOADVALUE_LENGTH - 1)) //如果自动重装值不能再调了
    {
        if (ADC_Flag == 0)
            Freq = 0;
        else if (ADC_Flag == 1)
            Freq2 = 0;

        LCD_ShowResult();
        ADC_Interrupt_Flag = 0;

        if (ADC_Flag == 0)
        {
            ADC_Flag = 1; //切换至下一个ADC
            DL_TimerG_startCounter(TIMER_ADC2_INST); //开始第二个通道的ADC转换
        }
    }
    else
    {
        Adjust_LoadValue(); //调整采样频率
        if (ADC_Flag == 0)
            DL_TimerG_startCounter(TIMER_ADC_INST); //开始下一轮的转换
        if (ADC_Flag == 1)
            DL_TimerG_startCounter(TIMER_ADC2_INST); //开始下一轮的转换
    }
}

//进行最终数据处理与显示
void Calculate_and_Show(void)
{
    float sample_freq = (CPUCLK_FREQ / (1.0 + LoadValue[LoadValue_Index])); //采样频率
    uint32_t Freq_Temp; //暂时储存计算得到的频率

    //计算逐差
    for (uint16_t i = 1; i < TriggerCount; i ++)
    {
        IndexDiff[i - 1] = (TriggerIndex[i] - TriggerIndex[i - 1]);
    }

    //排序并取出中位数
    uint16_t len = TriggerCount - 1;
    IndexDiff_Final = Result_Calculate(IndexDiff, len);

    //计算频率，0.5为四舍五入
    // if (LoadValue_Index == 0) //自动重装值为7，2.667或3.0为经验值（设置的采样频率与实际采样频率可能不符）
    //     Freq_Temp = (sample_freq / (3.03 * IndexDiff_Final)) + 0.5;
    // else
        Freq_Temp = (sample_freq / IndexDiff_Final) + 0.5;
    
    //计算实际频率
    if (ADC_Flag == 0) //MHz级别
    {
        Freq = Freq_Temp * 400; //总共经过了800？分频
        if (Freq > 12000000)
            Freq = 12000000;
        else if (Freq < 4000000)
            Freq = 4000000;
    }
    else if (ADC_Flag == 1) //kHz级别
    {
        Freq2 = Freq_Temp * 8; //总共经过了8分频
        if (Freq2 > 100000)
            Freq2 = 100000;
        else if (Freq2 < 32000)
            Freq2 = 32000;
    }

    LCD_ShowResult();
    ADC_Interrupt_Flag = 0;

    if (ADC_Flag == 0)
    {
        ADC_Flag = 1; //切换至下一个ADC
        DL_TimerG_startCounter(TIMER_ADC2_INST); //开始第二个通道的ADC转换
    }
}

/**
 * @brief 查找触发点
 * @param start_index 从哪个索引开始
 */
void Find_TriggerPoint(uint16_t start_index)
{
    if (TriggerCount >= TRIG_LENGTH) //超出数组范围直接返回
    {
        TriggerFlag = 0;
        return;
    }

    if (ADC_Flag == 0)
    {
        for (uint16_t i = start_index; i < (ADC_LENGTH - 3); i ++) //找触发点
        {
            //当前点 <= 触发电平 且 后两点平均值 > 触发电平
            if ((ADC_Value[i] <= ADC_Trigger) && (ADC_Value[i+1] > ADC_Trigger))
            {
                TriggerIndex[TriggerCount] = i; //记录触发位置
                TriggerCount ++;
                TriggerFlag = 1;
                return;
            }
        }
    }
    else if (ADC_Flag == 1)
    {
        for (uint16_t i = start_index; i < (ADC_LENGTH - 3); i ++) //找触发点
        {
            //当前点 <= 触发电平 且 后两点平均值 > 触发电平
            if ((ADC_Value2[i] <= ADC_Trigger) && (ADC_Value2[i+1] > ADC_Trigger))
            {
                TriggerIndex[TriggerCount] = i; //记录触发位置
                TriggerCount ++;
                TriggerFlag = 1;
                return;
            }
        }
    }

    TriggerFlag = 0;
}

//显示最终结果
void LCD_ShowResult(void)
{
    //计算最终频率
    if (Freq > 3500000 && Freq < 12500000)
        Freq_Final = Freq;
    else if (Freq2 > 31000 && Freq2 < 110000)
    {
        Freq_Final = Freq2;
    }
    else
    {
        Freq_Final = 0;
    }
    
    if (Freq2 > 31000 && Freq2 < 110000)
        PWM_SetFrequency(Freq2); //修改频率

    LCD_Fill(110, 70, 170, 87, WHITE);
    LCD_ShowInt32Num(110, 70, Freq, 9, BLACK, WHITE, 16);
    // LCD_ShowIntNum(110, 90, IndexDiff_Final, 5, BLACK, WHITE, 16);
    // LCD_ShowIntNum(110, 110, LoadValue[LoadValue_Index], 5, BLACK, WHITE, 16);
    // LCD_ShowIntNum(110, 130, TriggerCount, 5, BLACK, WHITE, 16);
    LCD_ShowInt32Num(110, 90, Freq2, 9, BLACK, WHITE, 16);
    LCD_ShowInt32Num(110, 110, Freq_Final, 9, BLACK, WHITE, 16);

    if (ADC_Flag == 1)
    {
        LCD_Fill(0, 190, LCD_W, LCD_H, WHITE);
        LCD_ShowString(30, 190, (u8 *)"Measure done!", GREEN, WHITE, 32, 1);
    }
}

//GPIO的中断服务函数
void GROUP1_IRQHandler(void)
{
    uint32_t GPIO_IIDX = DL_Interrupt_getPendingGroup(DL_INTERRUPT_GROUP_1); //获取中断标志位
    switch (GPIO_IIDX)
    {
        case KEY_GPIOA_INT_IIDX: //若为GPIOA
            if (DL_GPIO_readPins(KEY_PIN_KEY1_PORT, KEY_PIN_KEY1_PIN) > 0) //PA18
            {
                delay_ms(20);

                LCD_Fill(0, 190, LCD_W, LCD_H, WHITE);
                LCD_ShowString(30, 190, (u8 *)"Measure start!", GREEN, WHITE, 32, 1);
                
                ADC_Flag = 0;
                DL_TimerG_setLoadValue(TIMER_ADC_INST, LoadValue[0]); //先使用最小的自动重装值
                DL_TimerG_setLoadValue(TIMER_ADC2_INST, LoadValue[0]); //先使用最小的自动重装值
                DL_TimerG_startCounter(TIMER_ADC_INST); //开始计时

                while (DL_GPIO_readPins(KEY_PIN_KEY1_PORT, KEY_PIN_KEY1_PIN)); //等待变为低电平
                delay_ms(20);
            }
            break;

        case KEY_GPIOB_INT_IIDX: //若为GPIOB
            if(DL_GPIO_readPins(KEY_PIN_KEY2_PORT, KEY_PIN_KEY2_PIN) > 0)
            {
                delay_ms(10);
                // // LED翻转表示按键按下
                // DL_GPIO_togglePins(LED_PORT, LED_PIN_14_PIN);
                
                // 增加占空比，步进10%
                DutyCycle += 10;
                
                // 如果超过90%，重新从10%开始
                if(DutyCycle > 90)
                {
                    DutyCycle = 10;
                }
                PWM_SetDutyCycle(DutyCycle);
                while (DL_GPIO_readPins(KEY_PIN_KEY2_PORT, KEY_PIN_KEY2_PIN));// 防抖
                delay_ms(10);
            }

        default:
            break;
    }
}
