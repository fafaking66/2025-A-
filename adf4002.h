#ifndef _adf4002_h_
#define _adf4002_h_

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

#define R_Address 0X000000
#define N_Address 0X000001
#define F_Address 0X000002
#define I_Address 0X000003
#define Pre_R 0X000000          //X000,0000,DAT(14),00
#define Pre_N 0X000000          //XX0,DAT(13),XXXXXX,01

//管脚声明

#define PLL_SCK_0 DL_GPIO_clearPins(GPIO_FREQUENCY_DIVIDER_PORT, GPIO_FREQUENCY_DIVIDER_SCK_DIVIDER_PIN) 
#define PLL_SCK_1 DL_GPIO_setPins(GPIO_FREQUENCY_DIVIDER_PORT, GPIO_FREQUENCY_DIVIDER_SCK_DIVIDER_PIN)   

#define PLL_SDI_0 DL_GPIO_clearPins(GPIO_FREQUENCY_DIVIDER_PORT, GPIO_FREQUENCY_DIVIDER_SDI_DIVIDER_PIN) 
#define PLL_SDI_1 DL_GPIO_setPins(GPIO_FREQUENCY_DIVIDER_PORT, GPIO_FREQUENCY_DIVIDER_SDI_DIVIDER_PIN)   

#define PLL_SEN_0 DL_GPIO_clearPins(GPIO_FREQUENCY_DIVIDER_PORT, GPIO_FREQUENCY_DIVIDER_SEN_DIVIDER_PIN) 
#define PLL_SEN_1 DL_GPIO_setPins(GPIO_FREQUENCY_DIVIDER_PORT, GPIO_FREQUENCY_DIVIDER_SEN_DIVIDER_PIN)                                                                                                                                                                                                                                                                                                                    

void InitADF4002(void);
void Delay(unsigned int z);
void DelayMs(void);
void SendDataPll(unsigned long int Data);

void RDivideTest(u16 RData);
void NDivideTest(u16 NData);


#endif
