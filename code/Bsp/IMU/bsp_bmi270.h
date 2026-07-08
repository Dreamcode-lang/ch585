#ifndef __BSP_BMI270_H__
#define __BSP_BMI270_H__

#include "CH58x_common.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BSP_BMI270_ADDR_HIGH      0x69U
#define BSP_BMI270_ADDR_LOW       0x68U
#define BSP_BMI270_CHIP_ID_VALUE  0x24U

typedef struct
{
    int16_t acc_x;
    int16_t acc_y;
    int16_t acc_z;
    int16_t gyro_x;
    int16_t gyro_y;
    int16_t gyro_z;
} BSP_BMI270_RawData_t;

typedef struct
{
    uint8_t addr7;
    uint8_t chip_id;
    uint8_t internal_status;
    uint8_t inited;
} BSP_BMI270_Device_t;

uint8_t BSP_BMI270_Init(BSP_BMI270_Device_t *dev);
uint8_t BSP_BMI270_ReadChipId(BSP_BMI270_Device_t *dev, uint8_t *chip_id);
uint8_t BSP_BMI270_ReadInternalStatus(BSP_BMI270_Device_t *dev, uint8_t *status);
uint8_t BSP_BMI270_ReadRawData(BSP_BMI270_Device_t *dev, BSP_BMI270_RawData_t *raw);

#ifdef __cplusplus
}
#endif

#endif
