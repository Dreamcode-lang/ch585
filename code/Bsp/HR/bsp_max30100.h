#ifndef __BSP_MAX30100_H__
#define __BSP_MAX30100_H__

#include "CH58x_common.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BSP_MAX30100_OK       0U
#define BSP_MAX30100_ERROR    1U

typedef struct
{
    uint16_t ir;
    uint16_t red;
} BSP_MAX30100_Sample_t;

typedef struct
{
    uint8_t ready;
    uint8_t part_id;
    uint16_t last_ir;
    uint16_t last_red;
    uint8_t heart_rate;
    uint8_t beat_detected;
} BSP_MAX30100_State_t;

uint8_t BSP_MAX30100_Init(BSP_MAX30100_State_t *state);
uint8_t BSP_MAX30100_ReadSample(BSP_MAX30100_Sample_t *sample);
uint8_t BSP_MAX30100_Process(BSP_MAX30100_State_t *state, uint32_t now_ms);
uint8_t BSP_MAX30100_ResetFifo(void);

#ifdef __cplusplus
}
#endif

#endif
