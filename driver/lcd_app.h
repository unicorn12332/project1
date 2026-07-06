/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2026-06-20     user       the first version
 */
#ifndef APPLICATIONS_DRIVER_LCD_APP_H_
#define APPLICATIONS_DRIVER_LCD_APP_H_
#include <rtthread.h>

/* 声明 LCD 线程的入口函数，供 main.c 调用*/
void lcd_thread_entry(void *param);


#endif /* APPLICATIONS_DRIVER_LCD_APP_H_ */
