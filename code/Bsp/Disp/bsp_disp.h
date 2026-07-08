#ifndef __BSP_DISP_H__
#define __BSP_DISP_H__

#include "CH58x_common.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BSP_DISP_WIDTH       240
#define BSP_DISP_HEIGHT      280
#define BSP_DISP_X_SHIFT     0
#define BSP_DISP_Y_SHIFT     20

#define BSP_DISP_COLOR_BLACK   0x0000
#define BSP_DISP_COLOR_WHITE   0xFFFF
#define BSP_DISP_COLOR_RED     0xF800
#define BSP_DISP_COLOR_GREEN   0x07E0
#define BSP_DISP_COLOR_BLUE    0x001F
#define BSP_DISP_COLOR_CYAN    0x07FF
#define BSP_DISP_COLOR_YELLOW  0xFFE0
#define BSP_DISP_COLOR_MAGENTA 0xF81F
#define BSP_DISP_COLOR_GRAY    0x8410
#define BSP_DISP_COLOR_DARK    0x2104

void BSP_Disp_Init(void);
void BSP_Disp_SetBacklight(uint8_t on);
void BSP_Disp_FillColor(uint16_t color);
void BSP_Disp_FillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void BSP_Disp_DrawPixel(uint16_t x, uint16_t y, uint16_t color);
void BSP_Disp_DrawColorBars(void);
void BSP_Disp_DrawChar(uint16_t x, uint16_t y, char ch, uint16_t color, uint16_t bg, uint8_t scale);
void BSP_Disp_DrawString(uint16_t x, uint16_t y, const char *str, uint16_t color, uint16_t bg, uint8_t scale);
void BSP_Disp_DrawStatusFrame(void);
void BSP_Disp_DrawStatusLine(uint8_t row, const char *label, const char *value, uint16_t color);

#ifdef __cplusplus
}
#endif

#endif
