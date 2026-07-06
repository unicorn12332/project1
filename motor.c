/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2026-05-17     user       the first version
 */
#include <rtthread.h>
#include <drv_pwm.h>
#include <board.h>
#include "motor.h"

static struct rt_device_pwm *pwm_dev = RT_NULL;

/* 马达A控制 */
void motor_a_set(int speed)
{
    rt_uint32_t period = 500000;
    rt_uint32_t pulse;
/*限制输出*/
    if (speed > 100) speed = 100;
    if (speed < -100) speed = -100;

    if (speed > 0)
    {
        rt_pin_write(PIN_AIN1, PIN_HIGH);
        rt_pin_write(PIN_AIN2, PIN_LOW);
        pulse = speed * 5000;
        rt_kprintf("Motor A: Forward %d%%\n", speed);
    }
    else if (speed < 0)
    {
        rt_pin_write(PIN_AIN1, PIN_LOW);
        rt_pin_write(PIN_AIN2, PIN_HIGH);
        pulse = (-speed) * 5000;
        rt_kprintf("Motor A: Backward %d%%\n", -speed);
    }
    else
    {
        rt_pin_write(PIN_AIN1, PIN_LOW);
        rt_pin_write(PIN_AIN2, PIN_LOW);
        pulse = 0;
        rt_kprintf("Motor A: Stop\n");
    }

    if (pwm_dev)
    {
        rt_pwm_set(pwm_dev, PWM_MOTOR_A_CH, period, pulse);
        rt_pwm_enable(pwm_dev, PWM_MOTOR_A_CH);
    }
}

/* 马达B控制 */
void motor_b_set(int speed)
{
    rt_uint32_t period = 500000;
    rt_uint32_t pulse;

    if (speed > 100) speed = 100;
    if (speed < -100) speed = -100;

    if (speed > 0)
    {
        rt_pin_write(PIN_BIN1, PIN_HIGH);
        rt_pin_write(PIN_BIN2, PIN_LOW);
        pulse = speed * 5000;
        rt_kprintf("Motor B: Forward %d%%\n", speed);
    }
    else if (speed < 0)
    {
        rt_pin_write(PIN_BIN1, PIN_LOW);
        rt_pin_write(PIN_BIN2, PIN_HIGH);
        pulse = (-speed) * 5000;
        rt_kprintf("Motor B: Backward %d%%\n", -speed);
    }
    else
    {
        rt_pin_write(PIN_BIN1, PIN_LOW);
        rt_pin_write(PIN_BIN2, PIN_LOW);
        pulse = 0;
        rt_kprintf("Motor B: Stop\n");
    }

    if (pwm_dev)
    {
        rt_pwm_set(pwm_dev, PWM_MOTOR_B_CH, period, pulse);
        rt_pwm_enable(pwm_dev, PWM_MOTOR_B_CH);
    }
}

/* 两个马达都正转 */
void both_forward(void)
{
    motor_a_set(50);   /* 推进器50%速度前进 */
    motor_b_set(50);   /* 舵机50%速度正转 */
    rt_kprintf("Both motors: FORWARD!\n");
}

/* 全部停止 */
void all_stop(void)
{
    motor_a_set(0);
    motor_b_set(0);
    rt_kprintf("All motors: STOP\n");
}

/* 初始化 */
void motor_init(void)
{
    /* 初始化GPIO */
    rt_pin_mode(PIN_AIN1, PIN_MODE_OUTPUT);
    rt_pin_mode(PIN_AIN2, PIN_MODE_OUTPUT);
    rt_pin_mode(PIN_BIN1, PIN_MODE_OUTPUT);
    rt_pin_mode(PIN_BIN2, PIN_MODE_OUTPUT);
    rt_pin_mode(PIN_STBY, PIN_MODE_OUTPUT);

    /* 使能驱动板 */
    rt_pin_write(PIN_STBY, PIN_HIGH);

    /* 初始状态：停止 */
    rt_pin_write(PIN_AIN1, PIN_LOW);
    rt_pin_write(PIN_AIN2, PIN_LOW);
    rt_pin_write(PIN_BIN1, PIN_LOW);
    rt_pin_write(PIN_BIN2, PIN_LOW);

    /* 查找PWM设备 */
    pwm_dev = (struct rt_device_pwm *)rt_device_find(PWM_DEV_NAME);
    if (pwm_dev == RT_NULL)
    {
        rt_kprintf("ERROR: PWM device not found!\n");
    }
    else
    {
        rt_pwm_set(pwm_dev, PWM_MOTOR_A_CH, 500000, 0);
        rt_pwm_set(pwm_dev, PWM_MOTOR_B_CH, 500000, 0);
        rt_pwm_enable(pwm_dev, PWM_MOTOR_A_CH);
        rt_pwm_enable(pwm_dev, PWM_MOTOR_B_CH);
        rt_kprintf("Both motors initialized!\n");
    }
}

void servo_set_angle(int angle)
{
    rt_uint32_t period = 20000000;   // 20ms
    rt_uint32_t pulse;

    if (angle < 0)   angle = 0;
    if (angle > 270) angle = 270;

    pulse = 500000 + (angle * 2000000 / 270);

    if (pwm_dev)
    {
        rt_pwm_set(pwm_dev, PWM_SERVO_CH, period, pulse);
        rt_pwm_enable(pwm_dev, PWM_SERVO_CH);
    }
}

static int servo_angle = 135;   // 当前角度

void servo_turn_left(void)
{
    servo_angle -= 30;
    if (servo_angle < 0) servo_angle = 0;
    servo_set_angle(servo_angle);
}

void servo_turn_right(void)
{
    servo_angle += 30;
    if (servo_angle > 270) servo_angle = 270;
    servo_set_angle(servo_angle);
}
