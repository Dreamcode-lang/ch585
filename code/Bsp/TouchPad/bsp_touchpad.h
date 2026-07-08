#ifndef __BSP_TOUCHPAD_H__
#define __BSP_TOUCHPAD_H__

#include "CH58x_common.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BSP_TOUCHPAD_I2C_ADDR_7BIT  0x15
#define BSP_TOUCHPAD_LCD_WIDTH      240
#define BSP_TOUCHPAD_LCD_HEIGHT     280
#define BSP_TOUCHPAD_MIRROR_X       1
#define BSP_TOUCHPAD_MIRROR_Y       1

typedef enum
{
    BSP_TOUCHPAD_GESTURE_NONE        = 0x00,
    BSP_TOUCHPAD_GESTURE_UP          = 0x01,
    BSP_TOUCHPAD_GESTURE_DOWN        = 0x02,
    BSP_TOUCHPAD_GESTURE_RIGHT       = 0x03,
    BSP_TOUCHPAD_GESTURE_LEFT        = 0x04,
    BSP_TOUCHPAD_GESTURE_CLICK       = 0x05,
    BSP_TOUCHPAD_GESTURE_DOUBLECLICK = 0x0B,
    BSP_TOUCHPAD_GESTURE_LONGPRESS   = 0x0C
} BSP_TouchPad_Gesture_t;

typedef struct
{
    uint16_t x;
    uint16_t y;
    uint8_t touched;
    BSP_TouchPad_Gesture_t gesture;
} BSP_TouchPad_Point_t;

uint8_t BSP_TouchPad_Init(void);
uint8_t BSP_TouchPad_ReadChipId(uint8_t *chip_id);
uint8_t BSP_TouchPad_ReadPoint(BSP_TouchPad_Point_t *point);
uint8_t BSP_TouchPad_IsIntActive(void);

#ifdef __cplusplus
}
#endif

#endif
