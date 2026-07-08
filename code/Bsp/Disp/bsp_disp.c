#include "bsp_disp.h"

#define LCD_SCK_PIN     GPIO_Pin_0
#define LCD_MOSI_PIN    GPIO_Pin_1
#define LCD_DC_PIN      GPIO_Pin_7
#define LCD_RST_PIN     GPIO_Pin_8
#define LCD_CS_PIN      GPIO_Pin_12
#define LCD_BLK_PIN     GPIO_Pin_4

#define ST7789_SWRESET  0x01
#define ST7789_SLPOUT   0x11
#define ST7789_NORON    0x13
#define ST7789_INVON    0x21
#define ST7789_DISPON   0x29
#define ST7789_CASET    0x2A
#define ST7789_RASET    0x2B
#define ST7789_RAMWR    0x2C
#define ST7789_MADCTL   0x36
#define ST7789_COLMOD   0x3A

#define ST7789_MADCTL_MY   0x80
#define ST7789_MADCTL_MX   0x40
#define ST7789_MADCTL_RGB  0x00
#define ST7789_COLOR_MODE_16BIT 0x55

static void lcd_cs_low(void)  { GPIOA_ResetBits(LCD_CS_PIN); }
static void lcd_cs_high(void) { GPIOA_SetBits(LCD_CS_PIN); }
static void lcd_dc_low(void)  { GPIOA_ResetBits(LCD_DC_PIN); }
static void lcd_dc_high(void) { GPIOA_SetBits(LCD_DC_PIN); }
static void lcd_rst_low(void) { GPIOA_ResetBits(LCD_RST_PIN); }
static void lcd_rst_high(void){ GPIOA_SetBits(LCD_RST_PIN); }

static void lcd_write_bytes(const uint8_t *data, uint32_t len)
{
    while(len != 0)
    {
        uint16_t chunk = (len > 4095U) ? 4095U : (uint16_t)len;
        SPI1_MasterTrans((uint8_t *)data, chunk);
        data += chunk;
        len -= chunk;
    }
}

static void lcd_write_cmd(uint8_t cmd)
{
    lcd_cs_low();
    lcd_dc_low();
    lcd_write_bytes(&cmd, 1);
    lcd_cs_high();
}

static void lcd_write_data(const uint8_t *data, uint32_t len)
{
    lcd_cs_low();
    lcd_dc_high();
    lcd_write_bytes(data, len);
    lcd_cs_high();
}

static void lcd_write_u8(uint8_t data)
{
    lcd_write_data(&data, 1);
}

static void lcd_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    uint16_t xs = x0 + BSP_DISP_X_SHIFT;
    uint16_t xe = x1 + BSP_DISP_X_SHIFT;
    uint16_t ys = y0 + BSP_DISP_Y_SHIFT;
    uint16_t ye = y1 + BSP_DISP_Y_SHIFT;
    uint8_t data[4];

    lcd_write_cmd(ST7789_CASET);
    data[0] = (uint8_t)(xs >> 8);
    data[1] = (uint8_t)xs;
    data[2] = (uint8_t)(xe >> 8);
    data[3] = (uint8_t)xe;
    lcd_write_data(data, sizeof(data));

    lcd_write_cmd(ST7789_RASET);
    data[0] = (uint8_t)(ys >> 8);
    data[1] = (uint8_t)ys;
    data[2] = (uint8_t)(ye >> 8);
    data[3] = (uint8_t)ye;
    lcd_write_data(data, sizeof(data));

    lcd_write_cmd(ST7789_RAMWR);
}

static void lcd_gpio_init(void)
{
    GPIOA_ModeCfg(LCD_SCK_PIN | LCD_MOSI_PIN, GPIO_ModeOut_PP_20mA);
    GPIOA_ModeCfg(LCD_CS_PIN | LCD_DC_PIN | LCD_RST_PIN, GPIO_ModeOut_PP_20mA);
    GPIOB_ModeCfg(LCD_BLK_PIN, GPIO_ModeOut_PP_20mA);

    lcd_cs_high();
    lcd_dc_high();
    lcd_rst_high();
    BSP_Disp_SetBacklight(0);
}

static void lcd_spi_init(void)
{
    SPI1_MasterDefInit();
    SPI1_DataMode(Mode0_HighBitINFront);
    SPI1_CLKCfg(4);
}

void BSP_Disp_SetBacklight(uint8_t on)
{
    if(on)
    {
        GPIOB_SetBits(LCD_BLK_PIN);
    }
    else
    {
        GPIOB_ResetBits(LCD_BLK_PIN);
    }
}

void BSP_Disp_Init(void)
{
    lcd_gpio_init();
    lcd_spi_init();

    mDelaymS(10);
    lcd_rst_low();
    mDelaymS(10);
    lcd_rst_high();
    mDelaymS(120);

    lcd_write_cmd(ST7789_COLMOD);
    lcd_write_u8(ST7789_COLOR_MODE_16BIT);

    lcd_write_cmd(0xB2);
    {
        const uint8_t data[] = {0x0C, 0x0C, 0x00, 0x33, 0x33};
        lcd_write_data(data, sizeof(data));
    }

    lcd_write_cmd(ST7789_MADCTL);
    lcd_write_u8(ST7789_MADCTL_MX | ST7789_MADCTL_MY | ST7789_MADCTL_RGB);

    lcd_write_cmd(0xB7);
    lcd_write_u8(0x35);
    lcd_write_cmd(0xBB);
    lcd_write_u8(0x19);
    lcd_write_cmd(0xC0);
    lcd_write_u8(0x2C);
    lcd_write_cmd(0xC2);
    lcd_write_u8(0x01);
    lcd_write_cmd(0xC3);
    lcd_write_u8(0x12);
    lcd_write_cmd(0xC4);
    lcd_write_u8(0x20);
    lcd_write_cmd(0xC6);
    lcd_write_u8(0x0F);
    lcd_write_cmd(0xD0);
    lcd_write_u8(0xA4);
    lcd_write_u8(0xA1);

    lcd_write_cmd(0xE0);
    {
        const uint8_t data[] = {0xD0, 0x04, 0x0D, 0x11, 0x13, 0x2B, 0x3F, 0x54, 0x4C, 0x18, 0x0D, 0x0B, 0x1F, 0x23};
        lcd_write_data(data, sizeof(data));
    }
    lcd_write_cmd(0xE1);
    {
        const uint8_t data[] = {0xD0, 0x04, 0x0C, 0x11, 0x13, 0x2C, 0x3F, 0x44, 0x51, 0x2F, 0x1F, 0x1F, 0x20, 0x23};
        lcd_write_data(data, sizeof(data));
    }

    lcd_write_cmd(ST7789_INVON);
    lcd_write_cmd(ST7789_SLPOUT);
    mDelaymS(120);
    lcd_write_cmd(ST7789_NORON);
    lcd_write_cmd(ST7789_DISPON);
    mDelaymS(50);

    BSP_Disp_SetBacklight(1);
    BSP_Disp_FillColor(BSP_DISP_COLOR_BLACK);
}

void BSP_Disp_FillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    uint32_t pixels;
    uint8_t line[BSP_DISP_WIDTH * 2];
    uint16_t line_pixels;
    uint16_t i;

    if((x >= BSP_DISP_WIDTH) || (y >= BSP_DISP_HEIGHT) || (w == 0) || (h == 0))
    {
        return;
    }
    if((x + w) > BSP_DISP_WIDTH)
    {
        w = BSP_DISP_WIDTH - x;
    }
    if((y + h) > BSP_DISP_HEIGHT)
    {
        h = BSP_DISP_HEIGHT - y;
    }

    line_pixels = w;
    for(i = 0; i < line_pixels; i++)
    {
        line[i * 2] = (uint8_t)(color >> 8);
        line[i * 2 + 1] = (uint8_t)color;
    }

    lcd_set_window(x, y, x + w - 1, y + h - 1);
    lcd_cs_low();
    lcd_dc_high();
    pixels = h;
    while(pixels-- != 0)
    {
        lcd_write_bytes(line, (uint32_t)line_pixels * 2U);
    }
    lcd_cs_high();
}

void BSP_Disp_FillColor(uint16_t color)
{
    BSP_Disp_FillRect(0, 0, BSP_DISP_WIDTH, BSP_DISP_HEIGHT, color);
}

void BSP_Disp_DrawPixel(uint16_t x, uint16_t y, uint16_t color)
{
    uint8_t data[2];

    if((x >= BSP_DISP_WIDTH) || (y >= BSP_DISP_HEIGHT))
    {
        return;
    }

    data[0] = (uint8_t)(color >> 8);
    data[1] = (uint8_t)color;
    lcd_set_window(x, y, x, y);
    lcd_write_data(data, sizeof(data));
}

void BSP_Disp_DrawColorBars(void)
{
    const uint16_t colors[] = {
        BSP_DISP_COLOR_RED,
        BSP_DISP_COLOR_GREEN,
        BSP_DISP_COLOR_BLUE,
        BSP_DISP_COLOR_CYAN,
        BSP_DISP_COLOR_YELLOW,
        BSP_DISP_COLOR_MAGENTA,
        BSP_DISP_COLOR_WHITE
    };
    uint16_t bar_h = BSP_DISP_HEIGHT / (uint16_t)(sizeof(colors) / sizeof(colors[0]));
    uint16_t y = 0;
    uint8_t i;

    for(i = 0; i < (uint8_t)(sizeof(colors) / sizeof(colors[0])); i++)
    {
        uint16_t h = (i == ((sizeof(colors) / sizeof(colors[0])) - 1U)) ? (BSP_DISP_HEIGHT - y) : bar_h;
        BSP_Disp_FillRect(0, y, BSP_DISP_WIDTH, h, colors[i]);
        y += h;
    }
}

static const uint8_t *disp_font5x7(char ch)
{
    static const uint8_t blank[5] = {0x00, 0x00, 0x00, 0x00, 0x00};
    static const uint8_t colon[5] = {0x00, 0x36, 0x36, 0x00, 0x00};
    static const uint8_t dash[5] = {0x08, 0x08, 0x08, 0x08, 0x08};
    static const uint8_t zero[5] = {0x3E, 0x51, 0x49, 0x45, 0x3E};
    static const uint8_t one[5] = {0x00, 0x42, 0x7F, 0x40, 0x00};
    static const uint8_t two[5] = {0x42, 0x61, 0x51, 0x49, 0x46};
    static const uint8_t three[5] = {0x21, 0x41, 0x45, 0x4B, 0x31};
    static const uint8_t four[5] = {0x18, 0x14, 0x12, 0x7F, 0x10};
    static const uint8_t five[5] = {0x27, 0x45, 0x45, 0x45, 0x39};
    static const uint8_t six[5] = {0x3C, 0x4A, 0x49, 0x49, 0x30};
    static const uint8_t seven[5] = {0x01, 0x71, 0x09, 0x05, 0x03};
    static const uint8_t eight[5] = {0x36, 0x49, 0x49, 0x49, 0x36};
    static const uint8_t nine[5] = {0x06, 0x49, 0x49, 0x29, 0x1E};
    static const uint8_t a[5] = {0x7E, 0x11, 0x11, 0x11, 0x7E};
    static const uint8_t b[5] = {0x7F, 0x49, 0x49, 0x49, 0x36};
    static const uint8_t c[5] = {0x3E, 0x41, 0x41, 0x41, 0x22};
    static const uint8_t d[5] = {0x7F, 0x41, 0x41, 0x22, 0x1C};
    static const uint8_t e[5] = {0x7F, 0x49, 0x49, 0x49, 0x41};
    static const uint8_t f[5] = {0x7F, 0x09, 0x09, 0x09, 0x01};
    static const uint8_t g[5] = {0x3E, 0x41, 0x49, 0x49, 0x7A};
    static const uint8_t h[5] = {0x7F, 0x08, 0x08, 0x08, 0x7F};
    static const uint8_t i[5] = {0x00, 0x41, 0x7F, 0x41, 0x00};
    static const uint8_t k[5] = {0x7F, 0x08, 0x14, 0x22, 0x41};
    static const uint8_t l[5] = {0x7F, 0x40, 0x40, 0x40, 0x40};
    static const uint8_t m[5] = {0x7F, 0x02, 0x0C, 0x02, 0x7F};
    static const uint8_t n[5] = {0x7F, 0x04, 0x08, 0x10, 0x7F};
    static const uint8_t o[5] = {0x3E, 0x41, 0x41, 0x41, 0x3E};
    static const uint8_t p[5] = {0x7F, 0x09, 0x09, 0x09, 0x06};
    static const uint8_t r[5] = {0x7F, 0x09, 0x19, 0x29, 0x46};
    static const uint8_t s[5] = {0x46, 0x49, 0x49, 0x49, 0x31};
    static const uint8_t t[5] = {0x01, 0x01, 0x7F, 0x01, 0x01};
    static const uint8_t u[5] = {0x3F, 0x40, 0x40, 0x40, 0x3F};
    static const uint8_t v[5] = {0x1F, 0x20, 0x40, 0x20, 0x1F};
    static const uint8_t w[5] = {0x3F, 0x40, 0x38, 0x40, 0x3F};
    static const uint8_t x[5] = {0x63, 0x14, 0x08, 0x14, 0x63};
    static const uint8_t y[5] = {0x07, 0x08, 0x70, 0x08, 0x07};
    static const uint8_t z[5] = {0x61, 0x51, 0x49, 0x45, 0x43};

    if((ch >= 'a') && (ch <= 'z'))
    {
        ch = (char)(ch - 'a' + 'A');
    }

    switch(ch)
    {
        case ' ': return blank;
        case ':': return colon;
        case '-': return dash;
        case '0': return zero;
        case '1': return one;
        case '2': return two;
        case '3': return three;
        case '4': return four;
        case '5': return five;
        case '6': return six;
        case '7': return seven;
        case '8': return eight;
        case '9': return nine;
        case 'A': return a;
        case 'B': return b;
        case 'C': return c;
        case 'D': return d;
        case 'E': return e;
        case 'F': return f;
        case 'G': return g;
        case 'H': return h;
        case 'I': return i;
        case 'K': return k;
        case 'L': return l;
        case 'M': return m;
        case 'N': return n;
        case 'O': return o;
        case 'P': return p;
        case 'R': return r;
        case 'S': return s;
        case 'T': return t;
        case 'U': return u;
        case 'V': return v;
        case 'W': return w;
        case 'X': return x;
        case 'Y': return y;
        case 'Z': return z;
        default: return blank;
    }
}

void BSP_Disp_DrawChar(uint16_t x, uint16_t y, char ch, uint16_t color, uint16_t bg, uint8_t scale)
{
    const uint8_t *glyph = disp_font5x7(ch);
    uint8_t pixels[6U * 8U * 2U * 2U * 2U];
    uint16_t width;
    uint16_t height;
    uint16_t idx = 0;
    uint8_t col;
    uint8_t row;
    uint8_t sx;
    uint8_t sy;

    if(scale == 0)
    {
        scale = 1;
    }
    if(scale > 2)
    {
        scale = 2;
    }
    if((x >= BSP_DISP_WIDTH) || (y >= BSP_DISP_HEIGHT))
    {
        return;
    }

    width = (uint16_t)(6U * scale);
    height = (uint16_t)(8U * scale);
    if(((x + width) > BSP_DISP_WIDTH) || ((y + height) > BSP_DISP_HEIGHT))
    {
        return;
    }

    for(row = 0; row < 8; row++)
    {
        for(sy = 0; sy < scale; sy++)
        {
            for(col = 0; col < 6; col++)
            {
                uint8_t bits = (col < 5) ? glyph[col] : 0;
                uint16_t draw_color = (bits & (1U << row)) ? color : bg;
                for(sx = 0; sx < scale; sx++)
                {
                    pixels[idx++] = (uint8_t)(draw_color >> 8);
                    pixels[idx++] = (uint8_t)draw_color;
                }
            }
        }
    }

    lcd_set_window(x, y, (uint16_t)(x + width - 1U), (uint16_t)(y + height - 1U));
    lcd_write_data(pixels, (uint32_t)width * height * 2U);
}

void BSP_Disp_DrawString(uint16_t x, uint16_t y, const char *str, uint16_t color, uint16_t bg, uint8_t scale)
{
    uint16_t cursor_x = x;

    if(str == 0)
    {
        return;
    }
    if(scale == 0)
    {
        scale = 1;
    }

    while((*str != '\0') && (cursor_x < BSP_DISP_WIDTH))
    {
        BSP_Disp_DrawChar(cursor_x, y, *str, color, bg, scale);
        cursor_x = (uint16_t)(cursor_x + 6U * scale);
        str++;
    }
}

void BSP_Disp_DrawStatusFrame(void)
{
    BSP_Disp_FillColor(BSP_DISP_COLOR_BLACK);
    BSP_Disp_FillRect(0, 0, BSP_DISP_WIDTH, 34, BSP_DISP_COLOR_DARK);
    BSP_Disp_FillRect(0, 34, BSP_DISP_WIDTH, 2, BSP_DISP_COLOR_CYAN);
    BSP_Disp_DrawString(14, 10, "AIR MOUSE", BSP_DISP_COLOR_CYAN, BSP_DISP_COLOR_DARK, 2);
    BSP_Disp_DrawString(14, 48, "BLE", BSP_DISP_COLOR_GRAY, BSP_DISP_COLOR_BLACK, 2);
    BSP_Disp_DrawString(14, 78, "HID", BSP_DISP_COLOR_GRAY, BSP_DISP_COLOR_BLACK, 2);
    BSP_Disp_DrawString(14, 108, "IMU", BSP_DISP_COLOR_GRAY, BSP_DISP_COLOR_BLACK, 2);
    BSP_Disp_DrawString(14, 138, "BTN", BSP_DISP_COLOR_GRAY, BSP_DISP_COLOR_BLACK, 2);
    BSP_Disp_DrawString(14, 168, "MOVE", BSP_DISP_COLOR_GRAY, BSP_DISP_COLOR_BLACK, 2);
    BSP_Disp_DrawString(14, 198, "HR", BSP_DISP_COLOR_GRAY, BSP_DISP_COLOR_BLACK, 2);
    BSP_Disp_DrawString(14, 244, "PA3-L  PA6-R", BSP_DISP_COLOR_GRAY, BSP_DISP_COLOR_BLACK, 1);
}

void BSP_Disp_DrawStatusLine(uint8_t row, const char *label, const char *value, uint16_t color)
{
    uint16_t y;

    if(row >= 6)
    {
        return;
    }

    y = (uint16_t)(48U + (uint16_t)row * 30U);
    BSP_Disp_FillRect(76, y, 154, 18, BSP_DISP_COLOR_BLACK);
    if(label != 0)
    {
        BSP_Disp_FillRect(14, y, 58, 18, BSP_DISP_COLOR_BLACK);
        BSP_Disp_DrawString(14, y, label, BSP_DISP_COLOR_GRAY, BSP_DISP_COLOR_BLACK, 2);
    }
    BSP_Disp_DrawString(76, y, value, color, BSP_DISP_COLOR_BLACK, 2);
}
