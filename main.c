/*
 * Copyright (c) 2006-2023, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-06-29     Rbb666       first version
 */
#include <rtthread.h>
#include <rtdevice.h>
#include "drv_gpio.h"
#include "motor.h"
#include "bluetooth.h"
#include "driver/lcd.h"
#include "applications/driver/lcd_app.h"

#define LED_PIN     GET_PIN(0, 1)

#define BT_UART_NAME "uart5"



int main(void)
{
    /* 2. 初始化电机驱动（会设置STBY拉高、初始化PWM等)*/
    motor_init();
    bluetooth_init();

    rt_pin_mode(LED_PIN, PIN_MODE_OUTPUT);

    rt_kprintf("Unmanned Boat System Ready!\n");
    rt_kprintf("Commands:\n");
    rt_kprintf("  motor_a <speed>  - control left motor\n");
    rt_kprintf("  motor_b <speed>  - control right motor\n");
    rt_kprintf("  forward [speed]  - go forward\n");
    rt_kprintf("  backward [speed] - go backward\n");
    rt_kprintf("  stop             - stop all\n");

    rt_thread_t tid = rt_thread_create("lcd", lcd_thread_entry, RT_NULL, 2048, 20, 10);
    if (tid) {
        rt_thread_startup(tid);
        rt_kprintf("LCD thread created!\n");   // ← 加这行
    } else {
        rt_kprintf("LCD thread create failed!\n");  // ← 加这行
    }
    /* 3. 主循环：只保留LED闪烁作为心跳指示 */
    for (;;)
    {
        rt_pin_write(LED_PIN, PIN_HIGH);
        rt_thread_mdelay(1000);
        rt_pin_write(LED_PIN, PIN_LOW);
        rt_thread_mdelay(1000);
    }
}
