#include "bsp_display_touch_test.h"
#include "Disp/bsp_disp.h"
#include "TouchPad/bsp_touchpad.h"

static uint8_t g_touch_ready;
static uint16_t g_last_x = 0xFFFF;
static uint16_t g_last_y = 0xFFFF;

void BSP_DisplayTouch_TestInit(void)
{
    uint8_t chip_id = 0;

    BSP_Disp_Init();
    BSP_Disp_DrawColorBars();

    g_touch_ready = (BSP_TouchPad_Init() == 0) ? 1U : 0U;
    if(g_touch_ready && (BSP_TouchPad_ReadChipId(&chip_id) == 0))
    {
        BSP_Disp_FillRect(0, 0, BSP_DISP_WIDTH, 20, BSP_DISP_COLOR_GREEN);
    }
    else
    {
        BSP_Disp_FillRect(0, 0, BSP_DISP_WIDTH, 20, BSP_DISP_COLOR_RED);
    }
}

void BSP_DisplayTouch_TestPoll(void)
{
    BSP_TouchPad_Point_t point;

    if(!g_touch_ready)
    {
        return;
    }

    if(BSP_TouchPad_ReadPoint(&point) != 0)
    {
        return;
    }

    if(point.touched == 0)
    {
        return;
    }

    if(point.x >= BSP_DISP_WIDTH)
    {
        point.x = BSP_DISP_WIDTH - 1;
    }
    if(point.y >= BSP_DISP_HEIGHT)
    {
        point.y = BSP_DISP_HEIGHT - 1;
    }

    if((g_last_x != 0xFFFFU) && (g_last_y != 0xFFFFU))
    {
        BSP_Disp_FillRect(g_last_x, g_last_y, 7, 7, BSP_DISP_COLOR_BLACK);
    }

    BSP_Disp_FillRect(point.x, point.y, 7, 7, BSP_DISP_COLOR_WHITE);
    g_last_x = point.x;
    g_last_y = point.y;
}
