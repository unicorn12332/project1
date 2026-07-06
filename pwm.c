/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2026-05-15     user       the first version
 */

#include<rtthread.h>
#include<drv_pwm.h>
#include<stdlib.h>
#include"motor.h"

#define PWM_DEV_NAME   "pwm0"


struct rt_device_pwm *pwm_dev;

/* 控制 A 电机的命令 */
static int motor_a_cmd(int argc, char *argv[])
{
    int speed;

    /* 只需要一个参数：速度值 */
    if (argc != 2)
    {
        rt_kprintf("Usage: motor_a <speed>\n");
        rt_kprintf("  speed: -100 ~ 100\n");
        rt_kprintf("  positive = forward, negative = backward\n");
        rt_kprintf("Example:\n");
        rt_kprintf("  motor_a 50   - forward 50%%\n");
        rt_kprintf("  motor_a -30  - backward 30%%\n");
        rt_kprintf("  motor_a 0    - stop\n");
        return RT_ERROR;
    }

    speed = atoi(argv[1]);
    motor_a_set(speed);

    return RT_EOK;
}
MSH_CMD_EXPORT(motor_a_cmd, control motor A: motor_a <speed>);

/* 控制 B 电机的命令 */
static int motor_b_cmd(int argc, char *argv[])
{
    int speed;

    if (argc != 2)
    {
        rt_kprintf("Usage: motor_b <speed>\n");
        rt_kprintf("  speed: -100 ~ 100\n");
        rt_kprintf("  positive = forward, negative = backward\n");
        rt_kprintf("Example:\n");
        rt_kprintf("  motor_b 50   - forward 50%%\n");
        rt_kprintf("  motor_b -30  - backward 30%%\n");
        rt_kprintf("  motor_b 0    - stop\n");
        return RT_ERROR;
    }

    speed = atoi(argv[1]);
    motor_b_set(speed);

    return RT_EOK;
}
MSH_CMD_EXPORT(motor_b_cmd, control motor B: motor_b <speed>);
