#include "ti/driverlib/dl_gpio.h"
#include "ti/driverlib/dl_timerg.h"
#include "ti/driverlib/m0p/dl_core.h"
#include "ti_msp_dl_config.h"
#include <stdio.h>

#include "delay.h"
#include "lcd_init.h"
#include "lcd.h"
#include "measure.h"
#include "ADC.h"
#include "UI.h"
#include "adf4002.h"
#include "PWM_Generate.h"

volatile uint16_t ADC_Value[ADC_LENGTH]; //ADC转换后的数据
volatile uint16_t ADC_Value2[ADC_LENGTH]; //ADC转换后的数据

volatile uint16_t ADC_Interrupt_Flag = 0; //ADC中断次数
volatile uint16_t ADC_Flag = 0; //记录现在在处理的是ADC0还是ADC1的数据

volatile uint32_t Freq_Final = 0; //最终频率

int main(void)
{
	SYSCFG_DL_init(); //SYSCFG初始化

    DL_GPIO_setPins(LED_PORT, LED_PIN_LED_PIN); //测试用LED
    printf("By EussGaler"); //串口输出测试

    LCD_Init(); //初始化LCD
    delay_ms(200);
    ADC_Init(); //初始化ADC
    delay_ms(200);
    Key_Init(); //初始化按键
    delay_ms(200);
    InitADF4002(); // 初始化
    delay_ms(200);
    NDivideTest(50); //先设置RFIN 50分频
    delay_ms(200);
    PWM0_Init(); //PWM初始化
    delay_ms(200);
    Show_UI(); //显示菜单

    while(1)
    {
        
    }
}

//防止进入意外中断Default_Handler
void NMI_Handler(void){ __BKPT(0);} //不可屏蔽中断函数
void HardFault_Handler(void){ __BKPT(0);} //硬件故障中断函数
void SVC_Handler(void){ __BKPT(0);} //特权中断函数
void PendSV_Handler(void){ __BKPT(0);} //一种可挂起的、最低优先级的中断函数
void SysTick_Handler(void){ __BKPT(0);} //滴答定时器中断函数
void GROUP0_IRQHandler(void){ __BKPT(0);} //GROUP0的中断函数
// void GROUP1_IRQHandler(void){ __BKPT(0);} //GROUP1中断函数
void TIMG8_IRQHandler(void){ __BKPT(0);} //TIMG8的中断函数
void UART3_IRQHandler(void){ __BKPT(0);} //UART3的中断函数
// void ADC0_IRQHandler(void){ __BKPT(0);} //ADC0的中断函数
// void ADC1_IRQHandler(void){ __BKPT(0);} //ADC1的中断函数
void CANFD0_IRQHandler(void){ __BKPT(0);} //CANFD0的中断函数
void DAC0_IRQHandler(void){ __BKPT(0);} //DAC0的中断函数
void SPI0_IRQHandler(void){ __BKPT(0);} //SPI0的中断函数
void SPI1_IRQHandler(void){ __BKPT(0);} //SPI1的中断函数
void UART1_IRQHandler(void){ __BKPT(0);} //UART1的中断函数
void UART2_IRQHandler(void){ __BKPT(0);} //UART2的中断函数
void UART0_IRQHandler(void){ __BKPT(0);} //UART0的中断函数
void TIMG0_IRQHandler(void){ __BKPT(0);} //TIMG0的中断函数
void TIMG6_IRQHandler(void){ __BKPT(0);} //TIMG6的中断函数
void TIMA0_IRQHandler(void){ __BKPT(0);} //TIMA0的中断函数
void TIMA1_IRQHandler(void){ __BKPT(0);} //TIMA1的中断函数
void TIMG7_IRQHandler(void){ __BKPT(0);} //TIMG7的中断函数
void TIMG12_IRQHandler(void){ __BKPT(0);} //TIMG12的中断函数
void I2C0_IRQHandler(void){ __BKPT(0);} //I2C0的中断函数
void I2C1_IRQHandler(void){ __BKPT(0);} //I2C1的中断函数
void AES_IRQHandler(void){ __BKPT(0);} //硬件加速器的中断函数
void RTC_IRQHandler(void){ __BKPT(0);} //RTC实时时钟的中断函数
void DMA_IRQHandler(void){ __BKPT(0);} //DMA的中断函数
