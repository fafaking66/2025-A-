//延时函数和UART
#include "delay.h"

//搭配滴答定时器实现的精确us延时
void delay_us(unsigned long __us) 
{
    uint32_t ticks;
    uint32_t told, tnow, tcnt = 38;

    //计算需要的时钟数 = 延迟微秒数 * 每微秒的时钟数
    ticks = __us * (80000000 / 1000000);

    //获取当前的SysTick值
    told = SysTick->VAL;

    while (1)
    {
        //重复刷新获取当前的SysTick值
        tnow = SysTick->VAL;

        if (tnow != told)
        {
            if (tnow < told)
                tcnt += told - tnow;
            else
                tcnt += SysTick->LOAD - tnow + told;

            told = tnow;

            //如果达到了需要的时钟数，就退出循环
            if (tcnt >= ticks)
                break;
        }
    }
}

//搭配滴答定时器实现的精确ms延时
void delay_ms(unsigned long ms) 
{
	delay_us(ms * 1000);
}

void delay_1us(unsigned long __us)
{ 
    delay_us(__us);
}

void delay_1ms(unsigned long ms)
{
    delay_ms(ms);
}

//发送字符
int fputc(int c, FILE* stream)
{
    DL_UART_Main_transmitDataBlocking(UART_0_INST, c);
    return c;
}

//发送字符串
int fputs(const char* restrict s, FILE* restrict stream)
{
    uint16_t i, len;
    len = strlen(s);
    for(i = 0; i < len; i ++)
    {
        DL_UART_Main_transmitDataBlocking(UART_0_INST, s[i]);
    }
    return len;
}

//发送字符串并自动换行
int puts(const char *_ptr)
{
    int count = fputs(_ptr, stdout);
    count += fputs("\n", stdout);
    return count;
}
