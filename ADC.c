//ADC���
#include "ADC.h"

#include "delay.h"
#include "measure.h"
#include "ti/driverlib/dl_adc12.h"
#include "ti/driverlib/dl_dma.h"
#include "ti/driverlib/dl_timerg.h"
#include <sys/cdefs.h>

extern volatile uint16_t ADC_Value[ADC_LENGTH]; //ADCת���������
extern volatile uint16_t ADC_Value2[ADC_LENGTH]; //ADCת���������

extern volatile uint16_t ADC_Interrupt_Flag; //ADC�жϴ���
extern volatile uint16_t ADC_Flag; //��¼�����ڴ������ADC0����ADC1������

//ADC��DMA��ʼ��
void ADC_Init(void)
{
    DL_DMA_setSrcAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t) &ADC0->ULLMEM.MEMRES[0]); //����DMA���˵���ʼ��ַ
    DL_DMA_setDestAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t) &ADC_Value[0]); //����DMA���˵�Ŀ�ĵ�ַ
    DL_DMA_setTransferSize(DMA, DMA_CH0_CHAN_ID, ADC_LENGTH); //����DMA�����С
    DL_DMA_enableChannel(DMA, DMA_CH0_CHAN_ID); //����DMA
    
    DL_DMA_setSrcAddr(DMA, DMA_CH1_CHAN_ID, (uint32_t) &ADC1->ULLMEM.MEMRES[0]); //����DMA���˵���ʼ��ַ
    DL_DMA_setDestAddr(DMA, DMA_CH1_CHAN_ID, (uint32_t) &ADC_Value2[0]); //����DMA���˵�Ŀ�ĵ�ַ
    DL_DMA_setTransferSize(DMA, DMA_CH1_CHAN_ID, ADC_LENGTH); //����DMA�����С
    DL_DMA_enableChannel(DMA, DMA_CH1_CHAN_ID); //����DMA

    DL_TimerG_stopCounter(TIMER_ADC_INST); //��ͣ��ʱ��ֹͣADCת��
    NVIC_ClearPendingIRQ(TIMER_ADC_INST_INT_IRQN); //���DMA�жϱ�־
    NVIC_EnableIRQ(ADC_SQU_INST_INT_IRQN); //ʹ��DMA�ж�
    
    DL_TimerG_stopCounter(TIMER_ADC2_INST); //��ͣ��ʱ��ֹͣADCת��
    NVIC_ClearPendingIRQ(TIMER_ADC2_INST_INT_IRQN); //���DMA�жϱ�־
    NVIC_EnableIRQ(ADC_SQU2_INST_INT_IRQN); //ʹ��DMA�ж�
}

//ADC���жϷ�����
void ADC_SQU_INST_IRQHandler(void)
{
    switch (DL_ADC12_getPendingInterrupt(ADC_SQU_INST)) 
    {
        case DL_ADC12_IIDX_DMA_DONE: //��ΪDMA��������ж�
            DL_TimerG_stopCounter(TIMER_ADC_INST); //��ͣ��ʱ��ֹͣADCת��
            if (ADC_Interrupt_Flag == 0)
            {
                ADC_Interrupt_Flag = 1;
                Calculate_First(); //����һ��Ƶ�ʲ���
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

//ADC1���жϷ�����
void ADC_SQU2_INST_IRQHandler(void)
{
    switch (DL_ADC12_getPendingInterrupt(ADC_SQU2_INST)) 
    {
        case DL_ADC12_IIDX_DMA_DONE: //��ΪDMA��������ж�
            DL_TimerG_stopCounter(TIMER_ADC2_INST); //��ͣ��ʱ��ֹͣADCת��
            if (ADC_Interrupt_Flag == 0)
            {
                ADC_Interrupt_Flag = 1;
                Calculate_First(); //����һ��Ƶ�ʲ���
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
