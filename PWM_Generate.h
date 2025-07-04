#ifndef _PWM_Generate_h_
#define _PWM_Generate_h_

#include "ti_msp_dl_config.h"

void PWM0_Init(void);
void PWM_SetFrequency(uint32_t frequency);
void PWM_SetDutyCycle(uint32_t dutyCycle);

#endif