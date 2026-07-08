#include "bsp_max30100.h"
#include "../I2C/bsp_i2c_bus.h"

#define MAX30100_ADDR7                 0x57U

#define MAX30100_REG_INT_STATUS        0x00U
#define MAX30100_REG_INT_ENABLE        0x01U
#define MAX30100_REG_FIFO_WR_PTR       0x02U
#define MAX30100_REG_OVF_COUNTER       0x03U
#define MAX30100_REG_FIFO_RD_PTR       0x04U
#define MAX30100_REG_FIFO_DATA         0x05U
#define MAX30100_REG_MODE_CFG          0x06U
#define MAX30100_REG_SPO2_CFG          0x07U
#define MAX30100_REG_LED_CFG           0x09U
#define MAX30100_REG_PART_ID           0xFFU

#define MAX30100_INT_A_FULL            0x80U
#define MAX30100_MODE_RESET            0x40U
#define MAX30100_MODE_HR_SPO2          0x03U
#define MAX30100_SPO2_50HZ_16BIT       0x43U
#define MAX30100_LED_CURRENT_DEFAULT   0x33U

#define MAX30100_IR_FINGER_THRESHOLD   1000U
#define MAX30100_BEAT_MIN_INTERVAL_MS  330U
#define MAX30100_BEAT_MAX_INTERVAL_MS  1600U

static int32_t s_ir_dc = 0;
static int32_t s_ir_prev_ac = 0;
static uint32_t s_last_beat_ms = 0;
static uint8_t s_hr_valid = 0;
static uint8_t s_finger_present = 0;

static uint8_t max30100_write_reg(uint8_t reg, uint8_t value)
{
    return BSP_I2C_WriteReg(MAX30100_ADDR7, reg, value);
}

static uint8_t max30100_read_reg(uint8_t reg, uint8_t *value)
{
    return BSP_I2C_ReadReg(MAX30100_ADDR7, reg, value);
}

uint8_t BSP_MAX30100_ResetFifo(void)
{
    if(max30100_write_reg(MAX30100_REG_FIFO_WR_PTR, 0x00) != BSP_I2C_OK)
    {
        return BSP_MAX30100_ERROR;
    }
    if(max30100_write_reg(MAX30100_REG_OVF_COUNTER, 0x00) != BSP_I2C_OK)
    {
        return BSP_MAX30100_ERROR;
    }
    if(max30100_write_reg(MAX30100_REG_FIFO_RD_PTR, 0x00) != BSP_I2C_OK)
    {
        return BSP_MAX30100_ERROR;
    }
    return BSP_MAX30100_OK;
}

uint8_t BSP_MAX30100_Init(BSP_MAX30100_State_t *state)
{
    uint8_t part_id = 0;

    if(state == 0)
    {
        return BSP_MAX30100_ERROR;
    }

    state->ready = 0;
    state->part_id = 0;
    state->last_ir = 0;
    state->last_red = 0;
    state->heart_rate = 0;
    state->beat_detected = 0;

    BSP_I2C_Bus_Init();
    if(max30100_read_reg(MAX30100_REG_PART_ID, &part_id) != BSP_I2C_OK)
    {
        return BSP_MAX30100_ERROR;
    }

    max30100_write_reg(MAX30100_REG_MODE_CFG, MAX30100_MODE_RESET);
    mDelaymS(10);

    if(max30100_write_reg(MAX30100_REG_MODE_CFG, MAX30100_MODE_HR_SPO2) != BSP_I2C_OK)
    {
        return BSP_MAX30100_ERROR;
    }
    if(max30100_write_reg(MAX30100_REG_INT_ENABLE, MAX30100_INT_A_FULL) != BSP_I2C_OK)
    {
        return BSP_MAX30100_ERROR;
    }
    if(max30100_write_reg(MAX30100_REG_LED_CFG, MAX30100_LED_CURRENT_DEFAULT) != BSP_I2C_OK)
    {
        return BSP_MAX30100_ERROR;
    }
    if(max30100_write_reg(MAX30100_REG_SPO2_CFG, MAX30100_SPO2_50HZ_16BIT) != BSP_I2C_OK)
    {
        return BSP_MAX30100_ERROR;
    }
    if(BSP_MAX30100_ResetFifo() != BSP_MAX30100_OK)
    {
        return BSP_MAX30100_ERROR;
    }

    s_ir_dc = 0;
    s_ir_prev_ac = 0;
    s_last_beat_ms = 0;
    s_hr_valid = 0;
    s_finger_present = 0;

    state->ready = 1;
    state->part_id = part_id;
    return BSP_MAX30100_OK;
}

uint8_t BSP_MAX30100_ReadSample(BSP_MAX30100_Sample_t *sample)
{
    uint8_t data[4];

    if(sample == 0)
    {
        return BSP_MAX30100_ERROR;
    }

    if(BSP_I2C_ReadRegs(MAX30100_ADDR7, MAX30100_REG_FIFO_DATA, data, sizeof(data)) != BSP_I2C_OK)
    {
        return BSP_MAX30100_ERROR;
    }

    sample->ir = (uint16_t)(((uint16_t)data[0] << 8) | data[1]);
    sample->red = (uint16_t)(((uint16_t)data[2] << 8) | data[3]);
    return BSP_MAX30100_OK;
}

uint8_t BSP_MAX30100_Process(BSP_MAX30100_State_t *state, uint32_t now_ms)
{
    BSP_MAX30100_Sample_t sample;
    int32_t ir_ac;

    if((state == 0) || (state->ready == 0))
    {
        return BSP_MAX30100_ERROR;
    }

    state->beat_detected = 0;
    if(BSP_MAX30100_ReadSample(&sample) != BSP_MAX30100_OK)
    {
        return BSP_MAX30100_ERROR;
    }

    state->last_ir = sample.ir;
    state->last_red = sample.red;

    if(sample.ir < MAX30100_IR_FINGER_THRESHOLD)
    {
        state->heart_rate = 0;
        s_ir_dc = sample.ir;
        s_ir_prev_ac = 0;
        s_hr_valid = 0;
        s_last_beat_ms = 0;
        s_finger_present = 0;
        return BSP_MAX30100_OK;
    }

    if((s_finger_present == 0) || (s_ir_dc == 0))
    {
        s_finger_present = 1;
        s_ir_dc = sample.ir;
        s_ir_prev_ac = 0;
        s_last_beat_ms = 0;
        s_hr_valid = 0;
        return BSP_MAX30100_OK;
    }
    s_ir_dc = ((s_ir_dc * 15) + sample.ir) / 16;
    ir_ac = (int32_t)sample.ir - s_ir_dc;

    if((s_ir_prev_ac < 0) && (ir_ac >= 0))
    {
        uint32_t interval = now_ms - s_last_beat_ms;
        if((s_last_beat_ms != 0) &&
           (interval >= MAX30100_BEAT_MIN_INTERVAL_MS) &&
           (interval <= MAX30100_BEAT_MAX_INTERVAL_MS))
        {
            uint8_t bpm = (uint8_t)(60000U / interval);
            state->heart_rate = s_hr_valid ? (uint8_t)((state->heart_rate * 3U + bpm) / 4U) : bpm;
            s_hr_valid = 1;
            state->beat_detected = 1;
        }
        s_last_beat_ms = now_ms;
    }

    s_ir_prev_ac = ir_ac;
    return BSP_MAX30100_OK;
}
