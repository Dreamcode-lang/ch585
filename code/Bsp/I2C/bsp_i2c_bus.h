#ifndef __BSP_I2C_BUS_H__
#define __BSP_I2C_BUS_H__

#include "CH58x_common.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BSP_I2C_OK       0U
#define BSP_I2C_ERROR    1U

uint8_t BSP_I2C_Bus_Init(void);
uint8_t BSP_I2C_Bus_Recover(void);
uint8_t BSP_I2C_WriteBytes(uint8_t addr7, const uint8_t *data, uint16_t len);
uint8_t BSP_I2C_ReadBytes(uint8_t addr7, uint8_t *data, uint16_t len);
uint8_t BSP_I2C_WriteReg(uint8_t addr7, uint8_t reg, uint8_t value);
uint8_t BSP_I2C_WriteRegs(uint8_t addr7, uint8_t reg, const uint8_t *data, uint16_t len);
uint8_t BSP_I2C_ReadReg(uint8_t addr7, uint8_t reg, uint8_t *value);
uint8_t BSP_I2C_ReadRegs(uint8_t addr7, uint8_t reg, uint8_t *data, uint16_t len);
uint8_t BSP_I2C_Probe(uint8_t addr7);
uint8_t BSP_I2C_ScanFirst(uint8_t *addr7);

#ifdef __cplusplus
}
#endif

#endif
