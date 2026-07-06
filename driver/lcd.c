/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2026-05-31     user       the first version
 */
#include "lcd.h"
#include "stdlib.h"
#include <rtthread.h>
#include "board.h"
#include "lcdfont.h"


static struct rt_spi_device *spi_dev = RT_NULL;

/* ==================== 基础 SPI 操作 ==================== */

static void lcd_write_cmd(u8 cmd)
{
    if (spi_dev == RT_NULL) return;

    LCD_DC_Clr();
    //LCD_CS_Clr();
    rt_spi_send(spi_dev, &cmd, 1);
    //LCD_CS_Set();
}

static void lcd_write_data(u8 *buf, int len)
{
    if (spi_dev == RT_NULL) return;
    if (buf == RT_NULL) return;
    if (len <= 0) return;

    LCD_DC_Set();
    //LCD_CS_Clr();
    rt_spi_send(spi_dev, buf, len);
    //LCD_CS_Set();
}

static void lcd_write_data8(u8 dat)
{
    if (spi_dev == RT_NULL) return;

    LCD_DC_Set();
    //LCD_CS_Clr();
    rt_spi_send(spi_dev, &dat, 1);
    //LCD_CS_Set();
}

static void lcd_write_data16(u16 dat)
{
    u8 buf[2];
    buf[0] = dat >> 8;
    buf[1] = dat & 0xFF;
    lcd_write_data(buf, 2);
}

/* ==================== 地址设置 ==================== */

static void lcd_address_set(u16 x1, u16 y1, u16 x2, u16 y2)
{
    if (x1 >= LCD_WIDTH) x1 = LCD_WIDTH - 1;
    if (x2 >= LCD_WIDTH) x2 = LCD_WIDTH - 1;
    if (y1 >= LCD_HEIGHT) y1 = LCD_HEIGHT - 1;
    if (y2 >= LCD_HEIGHT) y2 = LCD_HEIGHT - 1;

    if(USE_HORIZONTAL == 0 || USE_HORIZONTAL == 1)
    {
        lcd_write_cmd(0x2a);
        lcd_write_data16(x1+2);
        lcd_write_data16(x2+2);
        lcd_write_cmd(0x2b);
        lcd_write_data16(y1+1);
        lcd_write_data16(y2+1);
        lcd_write_cmd(0x2c);
    }
    else
    {
        lcd_write_cmd(0x2a);
        lcd_write_data16(x1+1);
        lcd_write_data16(x2+1);
        lcd_write_cmd(0x2b);
        lcd_write_data16(y1+2);
        lcd_write_data16(y2+2);
        lcd_write_cmd(0x2c);
    }

}

/* ==================== 清屏/填充 ==================== */

void LCD_Fill(u16 xsta, u16 ysta, u16 xend, u16 yend, u16 color)
{
    u32 total = (xend - xsta) * (yend - ysta);
    static u8 lcd_buf[128];
    u32 i;
    u32 send_pixels;

    if (spi_dev == RT_NULL) return;
    if (xsta > xend || ysta > yend) return;

    // 填充缓冲区
    for(i = 0; i < 64; i++)
    {
        lcd_buf[2*i] = color >> 8;
        lcd_buf[2*i+1] = color;
    }

    lcd_address_set(xsta, ysta, xend, yend);
    LCD_DC_Set();
    //LCD_CS_Clr();

    while(total)
    {
        send_pixels = total > 64 ? 64 : total;
        rt_spi_send(spi_dev, lcd_buf, send_pixels * 2);
        total -= send_pixels;
    }

    //LCD_CS_Set();
}

void LCD_Clear(u16 color)
{
    LCD_Fill(0, 0, LCD_WIDTH, LCD_HEIGHT, color);
}

/* ==================== 显示单个字符 ==================== */

void LCD_ShowChar(u16 x, u16 y, u8 num, u16 fc, u16 bc, u8 sizey, u8 mode)
{
    if (sizey != 12 && sizey != 16 && sizey != 24 && sizey != 32) {
           rt_kprintf("LCD: 不支持的字体大小 %d\n", sizey);
           return;
       }

    u8 temp, sizex, t, m = 0;
    u16 i, TypefaceNum;
    u16 x0 = x;

    sizex = sizey / 2;
    TypefaceNum = (sizex/8 + ((sizex%8)?1:0)) * sizey;
    if (num < ' ' || num > '~') num = ' ';
    num = num - ' ';

    lcd_address_set(x, y, x+sizex-1, y+sizey-1);
    lcd_write_cmd(0x2c);

    for(i=0; i<TypefaceNum; i++)
    {
        if(sizey == 12)      temp = ascii_1206[num][i];
        else if(sizey == 16) temp = ascii_1608[num][i];
        else if(sizey == 24) temp = ascii_2412[num][i];
        else if(sizey == 32) temp = ascii_3216[num][i];


        for(t=0; t<8; t++)
        {
            if(!mode)  // 非叠加模式
            {
                if(temp & (0x01<<t)) lcd_write_data16(fc);
                else lcd_write_data16(bc);
                m++;
                if(m % sizex == 0)
                {
                    m = 0;
                    break;
                }
            }
            else  // 叠加模式
            {
                if(temp & (0x01<<t)) lcd_write_data16(fc);
                x++;
                if((x - x0) == sizex)
                {
                    x = x0;
                    y++;
                    break;
                }
            }
        }
    }
}

/* ==================== 显示字符串 ==================== */

void LCD_ShowString(u16 x, u16 y, const u8 *p, u16 fc, u16 bc, u8 sizey, u8 mode)
{
    u16 cur_x = x;
    u16 cur_y = y;
    u8 char_width = sizey / 2;

    while(*p != '\0')
    {
        if (*p == '\n') {
        cur_x = x;
        cur_y += sizey;
        p++;
        continue;
        }

    if (cur_x + char_width > LCD_WIDTH) {
       cur_x = x;
       cur_y += sizey;
                }

    if (cur_y + sizey > LCD_HEIGHT) break;

        LCD_ShowChar(cur_x, cur_y, *p, fc, bc, sizey, mode);
        cur_x += char_width;
        p++;
    }
}

/* ==================== 显示整数 ==================== */

static u32 mypow(u8 m, u8 n)
{
    u32 result = 1;
    while(n--) result *= m;
    return result;
}

void LCD_ShowIntNum(u16 x, u16 y, u16 num, u8 len, u16 fc, u16 bc, u8 sizey)
{
    u8 t, temp;
    u8 enshow = 0;
    u8 sizex = sizey/2;

    for(t=0; t<len; t++)
    {
        temp = (num / mypow(10, len-t-1)) % 10;
        if(enshow == 0 && t < (len-1))
        {
            if(temp == 0)
            {
                LCD_ShowChar(x+t*sizex, y, ' ', fc, bc, sizey, 0);
                continue;
            }
            else enshow = 1;
        }
        LCD_ShowChar(x+t*sizex, y, temp+48, fc, bc, sizey, 0);
    }
}

/* ==================== GPIO初始化 ==================== */

static void LCD_GPIO_Init(void)
{
    //rt_pin_mode(PIN_LCD_CS,  PIN_MODE_OUTPUT);
    rt_pin_mode(PIN_LCD_DC,  PIN_MODE_OUTPUT);
    rt_pin_mode(PIN_LCD_RST, PIN_MODE_OUTPUT);
    rt_pin_mode(PIN_LCD_BLK, PIN_MODE_OUTPUT);

    //rt_pin_write(PIN_LCD_CS,  PIN_HIGH);
    rt_pin_write(PIN_LCD_DC,  PIN_HIGH);
    rt_pin_write(PIN_LCD_RST, PIN_HIGH);
    rt_pin_write(PIN_LCD_BLK, PIN_HIGH);
}

/* ==================== SPI初始化 ==================== */

static void LCD_SPI_Init(void)
{
    // 直接查找SPI设备（根据你的实际设备名修改）
    spi_dev = (struct rt_spi_device *)rt_device_find("spi10");
    if (spi_dev == RT_NULL) {
        rt_kprintf(" LCD SPI10 device not found!\n");
        return;
    }

    rt_kprintf("[LCD] find spi10 OK\n");

    struct rt_spi_configuration cfg;
    cfg.data_width = 8;
    cfg.mode = RT_SPI_MASTER | RT_SPI_MODE_0 | RT_SPI_MSB;
    cfg.max_hz = 10 * 1000 * 1000;

    if (rt_spi_configure(spi_dev, &cfg) != RT_EOK) {
        rt_kprintf("[LCD] SPI failed\n");
        return;
     }

    rt_kprintf("[LCD] SPI start\n");

}
/* ==================== LCD初始化 ==================== */

void LCD_Init(void)
{
    rt_kprintf("[LCD] Init start\n");
    LCD_GPIO_Init();
    LCD_SPI_Init();

    // 硬件复位
        LCD_RES_Clr();
        rt_thread_mdelay(100);
        LCD_RES_Set();
        rt_thread_mdelay(100);

        LCD_BLK_Set();
        rt_thread_mdelay(100);


    // ST7735S 初始化序列
    lcd_write_cmd(0x11); rt_thread_mdelay(120);
    lcd_write_cmd(0xB1); lcd_write_data8(0x05); lcd_write_data8(0x3C); lcd_write_data8(0x3C);
    lcd_write_cmd(0xB2); lcd_write_data8(0x05); lcd_write_data8(0x3C); lcd_write_data8(0x3C);
    lcd_write_cmd(0xB3); lcd_write_data8(0x05); lcd_write_data8(0x3C); lcd_write_data8(0x3C);
    lcd_write_cmd(0xB4); lcd_write_data8(0x03);
    lcd_write_cmd(0xC0); lcd_write_data8(0x28); lcd_write_data8(0x08); lcd_write_data8(0x04);
    lcd_write_cmd(0xC1); lcd_write_data8(0xC0);
    lcd_write_cmd(0xC2); lcd_write_data8(0x0D); lcd_write_data8(0x00);
    lcd_write_cmd(0xC3); lcd_write_data8(0x8D); lcd_write_data8(0x2A);
    lcd_write_cmd(0xC4); lcd_write_data8(0x8D); lcd_write_data8(0xEE);
    lcd_write_cmd(0xC5); lcd_write_data8(0x1A);

    lcd_write_cmd(0x36);
    if(USE_HORIZONTAL == 0) lcd_write_data8(0x00);
    else if(USE_HORIZONTAL == 1) lcd_write_data8(0xC0);
    else if(USE_HORIZONTAL == 2) lcd_write_data8(0x70);
    else lcd_write_data8(0xA0);

    lcd_write_cmd(0xE0);
    lcd_write_data8(0x04); lcd_write_data8(0x22); lcd_write_data8(0x07);
    lcd_write_data8(0x0A); lcd_write_data8(0x2E); lcd_write_data8(0x30);
    lcd_write_data8(0x25); lcd_write_data8(0x2A); lcd_write_data8(0x28);
    lcd_write_data8(0x26); lcd_write_data8(0x2E); lcd_write_data8(0x3A);
    lcd_write_data8(0x00); lcd_write_data8(0x01); lcd_write_data8(0x03);
    lcd_write_data8(0x13);

    lcd_write_cmd(0xE1);
    lcd_write_data8(0x04); lcd_write_data8(0x16); lcd_write_data8(0x06);
    lcd_write_data8(0x0D); lcd_write_data8(0x2D); lcd_write_data8(0x26);
    lcd_write_data8(0x23); lcd_write_data8(0x27); lcd_write_data8(0x27);
    lcd_write_data8(0x25); lcd_write_data8(0x2D); lcd_write_data8(0x3B);
    lcd_write_data8(0x00); lcd_write_data8(0x01); lcd_write_data8(0x04);
    lcd_write_data8(0x13);

    lcd_write_cmd(0x3A); lcd_write_data8(0x05);
    lcd_write_cmd(0x29); // Display on

    rt_kprintf("LCD init success!");
/*
    LCD_Clear(0xFFFF);  // 白屏测试
    rt_thread_mdelay(500);
    LCD_Clear(0x0000);  // 黑屏
    */
}

