/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2026-06-20     user       the first version
 */
#include <rtthread.h>
#include <rtdevice.h>
#include "applications/driver/lcd_app.h" // 包含自己的头文件
#include "lcd.h"     // 包含屏幕驱动
#include <stdio.h>   // 包含 sprintf

/* 定义浊度传感器配置 */
#define TURBIDITY_ADC_NAME  "adc1"
#define TURBIDITY_CHANNEL   0

/* LCD 实时刷新线程 */
void lcd_thread_entry(void *param)
{
    struct rt_adc_device *adc_dev;
    rt_uint32_t adc_value;
    float voltage;
    char display_buf[30];

    /* 1. 初始化屏幕 */
    LCD_Init();
    LCD_Clear(WHITE);
    LCD_ShowString(5, 5, "Unmanned Boat", BLUE, WHITE, 16, 0);
    LCD_ShowString(5, 25, "Transparency:", BLACK, WHITE, 16, 0);

    /* 2. 初始化 ADC */
    adc_dev = (struct rt_adc_device *)rt_device_find(TURBIDITY_ADC_NAME);
    if (adc_dev == RT_NULL) {
        LCD_ShowString(5, 50, "ADC Error!", RED, WHITE, 16, 0);
        rt_kprintf("error can not find ADC0 ！\n");
        return;
    }
    else rt_kprintf("ADC Success!\n");
    rt_adc_enable(adc_dev, TURBIDITY_CHANNEL);

    /* 3. 实时刷新循环 */
    while (1)
    {
        adc_value = rt_adc_read(adc_dev, TURBIDITY_CHANNEL);
        if (adc_value > 4095) adc_value = 4095;
        voltage = adc_value * 3.3f / 4096.0f;

        if (voltage > 3.3f) voltage = 3.3f;
        if (voltage < 0.0f) voltage = 0.0f;

        int v_int = (int)(voltage * 100);    // 2.35 × 100 = 235
        int a = v_int / 100;          // 整数：2
        int b = v_int % 100;          // 小数：35
        // 格式化并擦除/重写屏幕
        rt_sprintf(display_buf, "V:%d.%02dV", a, b);
        LCD_Fill(5, 45, 5 + 100, 45 + 16, WHITE);
        LCD_ShowString(5, 45, display_buf, BLACK, WHITE, 16, 0);

        LCD_Fill(5, 65, 5 + 100, 65 + 16, WHITE);
        if (voltage > 0.9f) {
            LCD_ShowString(5, 65, "State:Clean", GREEN, WHITE, 16, 0);
        } else if (voltage > 0.4f) {
            LCD_ShowString(5, 65, "State:Mid", YELLOW, WHITE, 16, 0);
        } else {
            LCD_ShowString(5, 65, "State:Dirty", RED, WHITE, 16, 0);
        }

        rt_thread_mdelay(500);
    }
}

