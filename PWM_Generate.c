#include "PWM_Generate.h"

#define TimerClockSource 80000000  /*sysconfig中配置TIMER_PWM时钟源 = 32000000 / 4
                                    测试表明设为8MHZ时生成32khz到100khz方波的频率和占空比效果最好*/ 

void PWM0_Init(void)
{
    DL_TimerA_setCoreHaltBehavior(PWM_0_INST, DL_TIMER_CORE_HALT_IMMEDIATE);
    DL_TimerA_startCounter(PWM_0_INST);

    // PWM_SetFrequency(5000000);
}

// 动态修改PWM频率
void PWM_SetFrequency(uint32_t frequency)
{
    // 计算新的周期值
    uint32_t timerPeriod = TimerClockSource / frequency - 1;
    
    // 保持当前占空比
    uint32_t currentDutyCycle = 50;  // 默认50%
    uint32_t compareValue = (timerPeriod * currentDutyCycle) / 100; // 计算比较值

    // 更新周期和比较值
    DL_TimerA_setLoadValue(PWM_0_INST, timerPeriod);
    DL_TimerA_setCaptureCompareValue(
        PWM_0_INST, 
        compareValue, 
        DL_TIMER_CC_0_INDEX
    );
}

// 动态修改PWM占空比
void PWM_SetDutyCycle(uint32_t dutyCycle)
{
    // 获取当前周期值
    uint32_t timerPeriod = DL_TimerA_getLoadValue(PWM_0_INST);
    
    // 计算新的比较值
    uint32_t compareValue = (timerPeriod * dutyCycle) / 100;

    // 更新比较值
    DL_TimerA_setCaptureCompareValue(
        PWM_0_INST, 
        compareValue, 
        DL_TIMER_CC_0_INDEX
    );
}