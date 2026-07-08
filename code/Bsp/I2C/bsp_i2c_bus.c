#include "bsp_i2c_bus.h"

#define BSP_I2C_SDA_PIN        GPIO_Pin_20
#define BSP_I2C_SCL_PIN        GPIO_Pin_21
#define BSP_I2C_TIMEOUT_LOOP   12000U

static uint8_t s_i2c_inited = 0;

static uint8_t i2c_wait_event(uint32_t event)
{
    uint32_t timeout = BSP_I2C_TIMEOUT_LOOP;

    while(I2C_CheckEvent(event) == RESET)
    {
        if(--timeout == 0)
        {
            I2C_GenerateSTOP(ENABLE);
            I2C_AcknowledgeConfig(ENABLE);
            I2C_ClearFlag(I2C_FLAG_AF);
            return BSP_I2C_ERROR;
        }
    }
    return BSP_I2C_OK;
}

uint8_t BSP_I2C_Bus_Init(void)
{
    GPIOPinRemap(ENABLE, RB_PIN_I2C);
    GPIOB_ModeCfg(BSP_I2C_SDA_PIN | BSP_I2C_SCL_PIN, GPIO_ModeIN_PU);

    I2C_Cmd(DISABLE);
    I2C_Init(I2C_Mode_I2C, 100000, I2C_DutyCycle_2, I2C_Ack_Enable, I2C_AckAddr_7bit, 0x00);
    I2C_Cmd(ENABLE);

    s_i2c_inited = 1;
    return BSP_I2C_OK;
}

uint8_t BSP_I2C_Bus_Recover(void)
{
    uint8_t i;

    I2C_GenerateSTOP(ENABLE);
    I2C_AcknowledgeConfig(ENABLE);
    I2C_ClearFlag(I2C_FLAG_AF);
    I2C_Cmd(DISABLE);
    s_i2c_inited = 0;

    GPIOB_ModeCfg(BSP_I2C_SDA_PIN, GPIO_ModeIN_PU);
    GPIOB_ModeCfg(BSP_I2C_SCL_PIN, GPIO_ModeOut_PP_5mA);
    GPIOB_SetBits(BSP_I2C_SCL_PIN);
    mDelayuS(5);

    for(i = 0; i < 9U; i++)
    {
        GPIOB_ResetBits(BSP_I2C_SCL_PIN);
        mDelayuS(5);
        GPIOB_SetBits(BSP_I2C_SCL_PIN);
        mDelayuS(5);
    }

    GPIOB_ModeCfg(BSP_I2C_SDA_PIN, GPIO_ModeOut_PP_5mA);
    GPIOB_ResetBits(BSP_I2C_SDA_PIN);
    mDelayuS(5);
    GPIOB_SetBits(BSP_I2C_SCL_PIN);
    mDelayuS(5);
    GPIOB_SetBits(BSP_I2C_SDA_PIN);
    mDelayuS(5);

    GPIOB_ModeCfg(BSP_I2C_SDA_PIN | BSP_I2C_SCL_PIN, GPIO_ModeIN_PU);
    mDelaymS(2);
    return BSP_I2C_Bus_Init();
}

uint8_t BSP_I2C_WriteBytes(uint8_t addr7, const uint8_t *data, uint16_t len)
{
    uint16_t i;

    if((data == 0) || (len == 0))
    {
        return BSP_I2C_ERROR;
    }
    if(s_i2c_inited == 0)
    {
        BSP_I2C_Bus_Init();
    }

    I2C_AcknowledgeConfig(ENABLE);
    I2C_GenerateSTART(ENABLE);
    if(i2c_wait_event(I2C_EVENT_MASTER_MODE_SELECT) != BSP_I2C_OK)
    {
        return BSP_I2C_ERROR;
    }

    I2C_Send7bitAddress((uint8_t)(addr7 << 1), I2C_Direction_Transmitter);
    if(i2c_wait_event(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) != BSP_I2C_OK)
    {
        return BSP_I2C_ERROR;
    }

    for(i = 0; i < len; i++)
    {
        I2C_SendData(data[i]);
        if(i2c_wait_event(I2C_EVENT_MASTER_BYTE_TRANSMITTED) != BSP_I2C_OK)
        {
            return BSP_I2C_ERROR;
        }
    }

    I2C_GenerateSTOP(ENABLE);
    return BSP_I2C_OK;
}

uint8_t BSP_I2C_ReadBytes(uint8_t addr7, uint8_t *data, uint16_t len)
{
    uint16_t i;

    if((data == 0) || (len == 0))
    {
        return BSP_I2C_ERROR;
    }
    if(s_i2c_inited == 0)
    {
        BSP_I2C_Bus_Init();
    }

    I2C_GenerateSTART(ENABLE);
    if(i2c_wait_event(I2C_EVENT_MASTER_MODE_SELECT) != BSP_I2C_OK)
    {
        return BSP_I2C_ERROR;
    }

    if(len == 1)
    {
        I2C_AcknowledgeConfig(DISABLE);
    }
    else
    {
        I2C_AcknowledgeConfig(ENABLE);
    }

    I2C_Send7bitAddress((uint8_t)(addr7 << 1), I2C_Direction_Receiver);
    if(i2c_wait_event(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) != BSP_I2C_OK)
    {
        return BSP_I2C_ERROR;
    }

    for(i = 0; i < len; i++)
    {
        if(i == (len - 1U))
        {
            I2C_AcknowledgeConfig(DISABLE);
            I2C_GenerateSTOP(ENABLE);
        }
        if(i2c_wait_event(I2C_EVENT_MASTER_BYTE_RECEIVED) != BSP_I2C_OK)
        {
            I2C_AcknowledgeConfig(ENABLE);
            return BSP_I2C_ERROR;
        }
        data[i] = I2C_ReceiveData();
    }

    I2C_AcknowledgeConfig(ENABLE);
    return BSP_I2C_OK;
}

uint8_t BSP_I2C_WriteReg(uint8_t addr7, uint8_t reg, uint8_t value)
{
    uint8_t data[2];

    data[0] = reg;
    data[1] = value;
    return BSP_I2C_WriteBytes(addr7, data, sizeof(data));
}

uint8_t BSP_I2C_WriteRegs(uint8_t addr7, uint8_t reg, const uint8_t *data, uint16_t len)
{
    uint16_t i;

    if((data == 0) || (len == 0))
    {
        return BSP_I2C_ERROR;
    }
    if(s_i2c_inited == 0)
    {
        BSP_I2C_Bus_Init();
    }

    I2C_AcknowledgeConfig(ENABLE);
    I2C_GenerateSTART(ENABLE);
    if(i2c_wait_event(I2C_EVENT_MASTER_MODE_SELECT) != BSP_I2C_OK)
    {
        return BSP_I2C_ERROR;
    }

    I2C_Send7bitAddress((uint8_t)(addr7 << 1), I2C_Direction_Transmitter);
    if(i2c_wait_event(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) != BSP_I2C_OK)
    {
        return BSP_I2C_ERROR;
    }

    I2C_SendData(reg);
    if(i2c_wait_event(I2C_EVENT_MASTER_BYTE_TRANSMITTED) != BSP_I2C_OK)
    {
        return BSP_I2C_ERROR;
    }

    for(i = 0; i < len; i++)
    {
        I2C_SendData(data[i]);
        if(i2c_wait_event(I2C_EVENT_MASTER_BYTE_TRANSMITTED) != BSP_I2C_OK)
        {
            return BSP_I2C_ERROR;
        }
    }

    I2C_GenerateSTOP(ENABLE);
    return BSP_I2C_OK;
}

uint8_t BSP_I2C_ReadReg(uint8_t addr7, uint8_t reg, uint8_t *value)
{
    if(value == 0)
    {
        return BSP_I2C_ERROR;
    }
    return BSP_I2C_ReadRegs(addr7, reg, value, 1);
}

uint8_t BSP_I2C_ReadRegs(uint8_t addr7, uint8_t reg, uint8_t *data, uint16_t len)
{
    if(BSP_I2C_WriteBytes(addr7, &reg, 1) != BSP_I2C_OK)
    {
        return BSP_I2C_ERROR;
    }
    return BSP_I2C_ReadBytes(addr7, data, len);
}

uint8_t BSP_I2C_Probe(uint8_t addr7)
{
    if(s_i2c_inited == 0)
    {
        BSP_I2C_Bus_Init();
    }

    I2C_AcknowledgeConfig(ENABLE);
    I2C_GenerateSTART(ENABLE);
    if(i2c_wait_event(I2C_EVENT_MASTER_MODE_SELECT) != BSP_I2C_OK)
    {
        return BSP_I2C_ERROR;
    }

    I2C_Send7bitAddress((uint8_t)(addr7 << 1), I2C_Direction_Transmitter);
    if(i2c_wait_event(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) != BSP_I2C_OK)
    {
        return BSP_I2C_ERROR;
    }

    I2C_GenerateSTOP(ENABLE);
    return BSP_I2C_OK;
}

uint8_t BSP_I2C_ScanFirst(uint8_t *addr7)
{
    uint8_t addr;

    if(addr7 == 0)
    {
        return BSP_I2C_ERROR;
    }

    for(addr = 0x08; addr < 0x78; addr++)
    {
        if(BSP_I2C_Probe(addr) == BSP_I2C_OK)
        {
            *addr7 = addr;
            return BSP_I2C_OK;
        }
    }

    return BSP_I2C_ERROR;
}
