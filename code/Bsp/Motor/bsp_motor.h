#ifndef __BSP_MOTOR_H__
#define __BSP_MOTOR_H__

#include "CH58x_common.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void BSP_Motor_Init(void);
void BSP_Motor_Set(uint8_t enable);
void BSP_Motor_On(void);
void BSP_Motor_Off(void);

#ifdef __cplusplus
}
#endif

#endif
