#include "bsp_touchpad.h"
#include "../I2C/bsp_i2c_bus.h"

#define TOUCH_INT_PIN   GPIO_Pin_0
#define TOUCH_RST_PIN   GPIO_Pin_1

#define CST816_REG_GESTURE_ID      0x01
#define CST816_REG_FINGER_NUM      0x02
#define CST816_REG_X_POSH          0x03
#define CST816_REG_CHIPID          0xA7
#define CST816_REG_MOTIONMASK      0xEC
#define CST816_REG_IRQPLUSEWIDTH   0xED
#define CST816_REG_NORSCANPER      0xEE
#define CST816_REG_IRQCTL          0xFA
#define CST816_REG_DISAUTOALEEP    0xFE

#define CST816_IRQ_EN_MOTION       0x10
#define CST816_IRQ_EN_TOUCH        0x40

static uint8_t touch_read_reg(uint8_t reg, uint8_t *data, uint16_t len)
{
    return BSP_I2C_ReadRegs(BSP_TOUCHPAD_I2C_ADDR_7BIT, reg, data, len);
}

static uint8_t touch_write_reg(uint8_t reg, uint8_t value)
{
    return BSP_I2C_WriteReg(BSP_TOUCHPAD_I2C_ADDR_7BIT, reg, value);
}

static void touch_gpio_init(void)
{
    GPIOB_ModeCfg(TOUCH_INT_PIN, GPIO_ModeIN_PU);
    GPIOB_ModeCfg(TOUCH_RST_PIN, GPIO_ModeOut_PP_5mA);

    GPIOB_ResetBits(TOUCH_RST_PIN);
    mDelaymS(10);
    GPIOB_SetBits(TOUCH_RST_PIN);
    mDelaymS(50);
}

uint8_t BSP_TouchPad_Init(void)
{
    uint8_t chip_id;

    BSP_I2C_Bus_Init();
    touch_gpio_init();

    if(BSP_TouchPad_ReadChipId(&chip_id) != 0)
    {
        return 1;
    }

    if(touch_write_reg(CST816_REG_NORSCANPER, 10) != 0)
    {
        return 1;
    }
    if(touch_write_reg(CST816_REG_MOTIONMASK, 0x07) != 0)
    {
        return 1;
    }
    if(touch_write_reg(CST816_REG_IRQPLUSEWIDTH, 10) != 0)
    {
        return 1;
    }
    if(touch_write_reg(CST816_REG_IRQCTL, CST816_IRQ_EN_TOUCH | CST816_IRQ_EN_MOTION) != 0)
    {
        return 1;
    }
    if(touch_write_reg(CST816_REG_DISAUTOALEEP, 1) != 0)
    {
        return 1;
    }

    return 0;
}

uint8_t BSP_TouchPad_ReadChipId(uint8_t *chip_id)
{
    if(chip_id == 0)
    {
        return 1;
    }
    return touch_read_reg(CST816_REG_CHIPID, chip_id, 1);
}

uint8_t BSP_TouchPad_ReadPoint(BSP_TouchPad_Point_t *point)
{
    uint8_t data[6];
    uint16_t raw_x;
    uint16_t raw_y;

    if(point == 0)
    {
        return 1;
    }

    if(touch_read_reg(CST816_REG_GESTURE_ID, data, sizeof(data)) != 0)
    {
        point->touched = 0;
        return 1;
    }

    point->gesture = (BSP_TouchPad_Gesture_t)data[0];
    point->touched = data[1] & 0x0F;

    raw_x = (uint16_t)(((uint16_t)(data[2] & 0x0F) << 8) | data[3]);
    raw_y = (uint16_t)(((uint16_t)(data[4] & 0x0F) << 8) | data[5]);

    if(raw_x >= BSP_TOUCHPAD_LCD_WIDTH)
    {
        raw_x = BSP_TOUCHPAD_LCD_WIDTH - 1U;
    }
    if(raw_y >= BSP_TOUCHPAD_LCD_HEIGHT)
    {
        raw_y = BSP_TOUCHPAD_LCD_HEIGHT - 1U;
    }

#if BSP_TOUCHPAD_MIRROR_X
    point->x = (uint16_t)(BSP_TOUCHPAD_LCD_WIDTH - 1U - raw_x);
#else
    point->x = raw_x;
#endif

#if BSP_TOUCHPAD_MIRROR_Y
    point->y = (uint16_t)(BSP_TOUCHPAD_LCD_HEIGHT - 1U - raw_y);
#else
    point->y = raw_y;
#endif
    return 0;
}

uint8_t BSP_TouchPad_IsIntActive(void)
{
    return (GPIOB_ReadPortPin(TOUCH_INT_PIN) == 0) ? 1U : 0U;
}
