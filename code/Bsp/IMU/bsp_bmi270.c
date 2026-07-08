#include "bsp_bmi270.h"
#include "bsp_bmi270_config.h"
#include "../I2C/bsp_i2c_bus.h"

#define BMI270_REG_CHIP_ID          0x00U
#define BMI270_REG_ACC_X_LSB        0x0CU
#define BMI270_REG_GYR_X_LSB        0x12U
#define BMI270_REG_INTERNAL_STATUS  0x21U
#define BMI270_REG_ACC_CONF         0x40U
#define BMI270_REG_ACC_RANGE        0x41U
#define BMI270_REG_GYR_CONF         0x42U
#define BMI270_REG_GYR_RANGE        0x43U
#define BMI270_REG_INIT_CTRL        0x59U
#define BMI270_REG_INIT_DATA        0x5EU
#define BMI270_REG_IF_CONF          0x6BU
#define BMI270_REG_NV_CONF          0x6AU
#define BMI270_REG_PWR_CONF         0x7CU
#define BMI270_REG_PWR_CTRL         0x7DU
#define BMI270_REG_CMD              0x7EU

#define BMI270_PWR_ACC_GYR_TEMP_EN  0x0EU
#define BMI270_ACC_RANGE_16G        0x03U
#define BMI270_GYR_RANGE_2000DPS    0x00U
#define BMI270_ODR_200HZ            0x09U
#define BMI270_FILTER_PERF          0xA0U

static int16_t bmi270_make_s16(uint8_t lsb, uint8_t msb)
{
    return (int16_t)(((uint16_t)msb << 8) | lsb);
}

static uint8_t bmi270_select_addr(BSP_BMI270_Device_t *dev)
{
    uint8_t chip_id;

    dev->addr7 = BSP_BMI270_ADDR_HIGH;
    if(BSP_BMI270_ReadChipId(dev, &chip_id) == 0)
    {
        dev->chip_id = chip_id;
        return 0;
    }

    dev->addr7 = BSP_BMI270_ADDR_LOW;
    if(BSP_BMI270_ReadChipId(dev, &chip_id) == 0)
    {
        dev->chip_id = chip_id;
        return 0;
    }

    dev->addr7 = BSP_BMI270_ADDR_HIGH;
    return 1;
}

uint8_t BSP_BMI270_Init(BSP_BMI270_Device_t *dev)
{
    uint8_t status;

    if(dev == 0)
    {
        return 1;
    }

    dev->addr7 = BSP_BMI270_ADDR_HIGH;
    dev->chip_id = 0;
    dev->internal_status = 0;
    dev->inited = 0;

    BSP_I2C_Bus_Init();
    mDelaymS(10);

    if(bmi270_select_addr(dev) != 0)
    {
        return 1;
    }
    if(dev->chip_id != BSP_BMI270_CHIP_ID_VALUE)
    {
        return 1;
    }

    mDelaymS(100);
    if(BSP_I2C_WriteReg(dev->addr7, BMI270_REG_PWR_CONF, 0x00) != 0)
    {
        return 1;
    }
    mDelaymS(2);

    if(BSP_I2C_WriteReg(dev->addr7, BMI270_REG_INIT_CTRL, 0x00) != 0)
    {
        return 1;
    }
    if(BSP_I2C_WriteRegs(dev->addr7, BMI270_REG_INIT_DATA, bmi270_config_file, sizeof(bmi270_config_file)) != 0)
    {
        return 1;
    }
    if(BSP_I2C_WriteReg(dev->addr7, BMI270_REG_INIT_CTRL, 0x01) != 0)
    {
        return 1;
    }
    mDelaymS(40);

    if(BSP_BMI270_ReadInternalStatus(dev, &status) != 0)
    {
        return 1;
    }
    dev->internal_status = status;
    if(status == 0)
    {
        return 1;
    }

    if(BSP_I2C_WriteReg(dev->addr7, BMI270_REG_PWR_CTRL, BMI270_PWR_ACC_GYR_TEMP_EN) != 0)
    {
        return 1;
    }
    if(BSP_I2C_WriteReg(dev->addr7, BMI270_REG_NV_CONF, 0x00) != 0)
    {
        return 1;
    }
    if(BSP_I2C_WriteReg(dev->addr7, BMI270_REG_IF_CONF, 0x00) != 0)
    {
        return 1;
    }
    if(BSP_I2C_WriteReg(dev->addr7, BMI270_REG_GYR_RANGE, BMI270_GYR_RANGE_2000DPS) != 0)
    {
        return 1;
    }
    if(BSP_I2C_WriteReg(dev->addr7, BMI270_REG_GYR_CONF, (uint8_t)(BMI270_ODR_200HZ | BMI270_FILTER_PERF)) != 0)
    {
        return 1;
    }
    if(BSP_I2C_WriteReg(dev->addr7, BMI270_REG_ACC_RANGE, BMI270_ACC_RANGE_16G) != 0)
    {
        return 1;
    }
    if(BSP_I2C_WriteReg(dev->addr7, BMI270_REG_ACC_CONF, (uint8_t)(BMI270_ODR_200HZ | BMI270_FILTER_PERF)) != 0)
    {
        return 1;
    }

    mDelaymS(20);
    dev->inited = 1;
    return 0;
}

uint8_t BSP_BMI270_ReadChipId(BSP_BMI270_Device_t *dev, uint8_t *chip_id)
{
    if((dev == 0) || (chip_id == 0))
    {
        return 1;
    }
    return BSP_I2C_ReadReg(dev->addr7, BMI270_REG_CHIP_ID, chip_id);
}

uint8_t BSP_BMI270_ReadInternalStatus(BSP_BMI270_Device_t *dev, uint8_t *status)
{
    if((dev == 0) || (status == 0))
    {
        return 1;
    }
    return BSP_I2C_ReadReg(dev->addr7, BMI270_REG_INTERNAL_STATUS, status);
}

uint8_t BSP_BMI270_ReadRawData(BSP_BMI270_Device_t *dev, BSP_BMI270_RawData_t *raw)
{
    uint8_t acc[6];
    uint8_t gyro[6];

    if((dev == 0) || (raw == 0) || (dev->inited == 0))
    {
        return 1;
    }

    if(BSP_I2C_ReadRegs(dev->addr7, BMI270_REG_ACC_X_LSB, acc, sizeof(acc)) != 0)
    {
        return 1;
    }
    if(BSP_I2C_ReadRegs(dev->addr7, BMI270_REG_GYR_X_LSB, gyro, sizeof(gyro)) != 0)
    {
        return 1;
    }

    raw->acc_x = bmi270_make_s16(acc[0], acc[1]);
    raw->acc_y = bmi270_make_s16(acc[2], acc[3]);
    raw->acc_z = bmi270_make_s16(acc[4], acc[5]);
    raw->gyro_x = bmi270_make_s16(gyro[0], gyro[1]);
    raw->gyro_y = bmi270_make_s16(gyro[2], gyro[3]);
    raw->gyro_z = bmi270_make_s16(gyro[4], gyro[5]);

    return 0;
}
