/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2026-05-31     user       the first version
 */
#ifndef APPLICATIONS_DRIVER_LCD_H_
#define APPLICATIONS_DRIVER_LCD_H_

#include <rtthread.h>
#include <rtdevice.h>

/* 数据类型映射 */
typedef rt_uint8_t   u8;
typedef rt_uint16_t  u16;
typedef rt_uint32_t  u32;

/* ==================== 引脚定义（请根据你的实际接线修改）==================== */
#define PIN_LCD_SCL     GET_PIN(10, 2)   // SCL 时钟
#define PIN_LCD_SDA     GET_PIN(10, 0)   // SDA 数据
#define PIN_LCD_CS      GET_PIN(10, 3)   // CS 片选
#define PIN_LCD_DC      GET_PIN(10, 4)   // DC 数据/命令
#define PIN_LCD_RST     GET_PIN(11, 4)   // RST 复位
#define PIN_LCD_BLK     GET_PIN(11, 5)   // BLK 背光

/* ==================== GPIO 控制宏 ==================== */
//#define LCD_CS_Clr()    rt_pin_write(PIN_LCD_CS, PIN_LOW)
//#define LCD_CS_Set()    rt_pin_write(PIN_LCD_CS, PIN_HIGH)
#define LCD_DC_Clr()    rt_pin_write(PIN_LCD_DC, PIN_LOW)
#define LCD_DC_Set()    rt_pin_write(PIN_LCD_DC, PIN_HIGH)
#define LCD_RES_Clr()   rt_pin_write(PIN_LCD_RST, PIN_LOW)
#define LCD_RES_Set()   rt_pin_write(PIN_LCD_RST, PIN_HIGH)
#define LCD_BLK_Set()   rt_pin_write(PIN_LCD_BLK, PIN_HIGH)

/* ==================== 屏幕方向配置 ==================== */
#define USE_HORIZONTAL 1  // 0或1为竖屏，2或3为横屏

/* ==================== 屏幕尺寸 ==================== */
#if USE_HORIZONTAL==0||USE_HORIZONTAL==1
#define LCD_W 128
#define LCD_H 160
#else
#define LCD_W 160
#define LCD_H 128
#endif

#define LCD_WIDTH   LCD_W
#define LCD_HEIGHT  LCD_H

/* ==================== 颜色定义 ==================== */
#define WHITE         0xFFFF
#define BLACK         0x0000
#define BLUE          0x001F
#define BRED          0XF81F
#define GRED          0XFFE0
#define GBLUE         0X07FF
#define RED           0xF800
#define MAGENTA       0xF81F
#define GREEN         0x07E0
#define CYAN          0x7FFF
#define YELLOW        0xFFE0
#define BROWN         0XBC40
#define BRRED         0XFC07
#define GRAY          0X8430
#define DARKBLUE      0X01CF
#define LIGHTBLUE     0X7D7C
#define GRAYBLUE      0X5458
#define LIGHTGREEN    0X841F
#define LGRAY         0XC618
#define LGRAYBLUE     0XA651
#define LBBLUE        0X2B12

/* ==================== 函数声明 ==================== */
//void LCD_GPIO_Init(void);
void LCD_Writ_Bus(u8 dat);
void LCD_WR_DATA8(u8 dat);
void LCD_WR_DATA(u16 dat);
void LCD_WR_REG(u8 dat);
void LCD_Address_Set(u16 x1, u16 y1, u16 x2, u16 y2);
void LCD_Init(void);

void LCD_Clear(u16 color);

void LCD_Fill(u16 xsta, u16 ysta, u16 xend, u16 yend, u16 color);
void LCD_DrawPoint(u16 x, u16 y, u16 color);
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2, u16 color);
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2, u16 color);
void Draw_Circle(u16 x0, u16 y0, u8 r, u16 color);

void LCD_ShowChinese(u16 x, u16 y, u8 *s, u16 fc, u16 bc, u8 sizey, u8 mode);
void LCD_ShowChinese12x12(u16 x, u16 y, u8 *s, u16 fc, u16 bc, u8 sizey, u8 mode);
void LCD_ShowChinese16x16(u16 x, u16 y, u8 *s, u16 fc, u16 bc, u8 sizey, u8 mode);
void LCD_ShowChinese24x24(u16 x, u16 y, u8 *s, u16 fc, u16 bc, u8 sizey, u8 mode);
void LCD_ShowChinese32x32(u16 x, u16 y, u8 *s, u16 fc, u16 bc, u8 sizey, u8 mode);

void LCD_ShowChar(u16 x, u16 y, u8 num, u16 fc, u16 bc, u8 sizey, u8 mode);
void LCD_ShowString(u16 x, u16 y, const u8 *p, u16 fc, u16 bc, u8 sizey, u8 mode);
//u32 mypow(u8 m, u8 n);
void LCD_ShowIntNum(u16 x, u16 y, u16 num, u8 len, u16 fc, u16 bc, u8 sizey);
void LCD_ShowFloatNum1(u16 x, u16 y, float num, u8 len, u16 fc, u16 bc, u8 sizey);
void LCD_ShowPicture(u16 x, u16 y, u16 length, u16 width, const u8 pic[]);

#endif /* APPLICATIONS_DRIVER_LCD_H_ */
