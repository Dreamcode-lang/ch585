#include "bsp_motor.h"

#define BSP_MOTOR_PIN           GPIO_Pin_6
#define BSP_MOTOR_ACTIVE_HIGH   1U

void BSP_Motor_Init(void)
{
    BSP_Motor_Off();
    GPIOB_ModeCfg(BSP_MOTOR_PIN, GPIO_ModeOut_PP_20mA);
}

void BSP_Motor_Set(uint8_t enable)
{
#if BSP_MOTOR_ACTIVE_HIGH
    if(enable != 0)
    {
        GPIOB_SetBits(BSP_MOTOR_PIN);
    }
    else
    {
        GPIOB_ResetBits(BSP_MOTOR_PIN);
    }
#else
    if(enable != 0)
    {
        GPIOB_ResetBits(BSP_MOTOR_PIN);
    }
    else
    {
        GPIOB_SetBits(BSP_MOTOR_PIN);
    }
#endif
}

void BSP_Motor_On(void)
{
    BSP_Motor_Set(1);
}

void BSP_Motor_Off(void)
{
    BSP_Motor_Set(0);
}
