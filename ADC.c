//ADC相关
#include "ADC.h"

#include "delay.h"
#include "measure.h"
#include "ti/driverlib/dl_adc12.h"
#include "ti/driverlib/dl_dma.h"
#include "ti/driverlib/dl_timerg.h"
#include <sys/cdefs.h>

extern volatile uint16_t ADC_Value[ADC_LENGTH]; //ADC转换后的数据
extern volatile uint16_t ADC_Value2[ADC_LENGTH]; //ADC转换后的数据

extern volatile uint16_t ADC_Interrupt_Flag; //ADC中断次数
extern volatile uint16_t ADC_Flag; //记录现在在处理的是ADC0还是ADC1的数据

//ADC与DMA初始化
void ADC_Init(void)
{
    DL_DMA_setSrcAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t) &ADC0->ULLMEM.MEMRES[0]); //设置DMA搬运的起始地址
    DL_DMA_setDestAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t) &ADC_Value[0]); //设置DMA搬运的目的地址
    DL_DMA_setTransferSize(DMA, DMA_CH0_CHAN_ID, ADC_LENGTH); //设置DMA传输大小
    DL_DMA_enableChannel(DMA, DMA_CH0_CHAN_ID); //开启DMA
    
    DL_DMA_setSrcAddr(DMA, DMA_CH1_CHAN_ID, (uint32_t) &ADC1->ULLMEM.MEMRES[0]); //设置DMA搬运的起始地址
    DL_DMA_setDestAddr(DMA, DMA_CH1_CHAN_ID, (uint32_t) &ADC_Value2[0]); //设置DMA搬运的目的地址
    DL_DMA_setTransferSize(DMA, DMA_CH1_CHAN_ID, ADC_LENGTH); //设置DMA传输大小
    DL_DMA_enableChannel(DMA, DMA_CH1_CHAN_ID); //开启DMA

    DL_TimerG_stopCounter(TIMER_ADC_INST); //暂停计时即停止ADC转换
    NVIC_ClearPendingIRQ(TIMER_ADC_INST_INT_IRQN); //清除DMA中断标志
    NVIC_EnableIRQ(ADC_SQU_INST_INT_IRQN); //使能DMA中断
    
    DL_TimerG_stopCounter(TIMER_ADC2_INST); //暂停计时即停止ADC转换
    NVIC_ClearPendingIRQ(TIMER_ADC2_INST_INT_IRQN); //清除DMA中断标志
    NVIC_EnableIRQ(ADC_SQU2_INST_INT_IRQN); //使能DMA中断
}

//ADC的中断服务函数
void ADC_SQU_INST_IRQHandler(void)
{
    switch (DL_ADC12_getPendingInterrupt(ADC_SQU_INST)) 
    {
        case DL_ADC12_IIDX_DMA_DONE: //若为DMA搬运完成中断
            DL_TimerG_stopCounter(TIMER_ADC_INST); //暂停计时即停止ADC转换
            if (ADC_Interrupt_Flag == 0)
            {
                ADC_Interrupt_Flag = 1;
                Calculate_First(); //进行一次频率测量
            }
            else
            {
                Calculate_Second();
            }
            break;

        default:
            break;
    }
}

//ADC1的中断服务函数
void ADC_SQU2_INST_IRQHandler(void)
{
    switch (DL_ADC12_getPendingInterrupt(ADC_SQU2_INST)) 
    {
        case DL_ADC12_IIDX_DMA_DONE: //若为DMA搬运完成中断
            DL_TimerG_stopCounter(TIMER_ADC2_INST); //暂停计时即停止ADC转换
            if (ADC_Interrupt_Flag == 0)
            {
                ADC_Interrupt_Flag = 1;
                Calculate_First(); //进行一次频率测量
            }
            else
            {
                Calculate_Second();
            }
            break;

        default:
            break;
    }
}
