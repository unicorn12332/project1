/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2026-05-17     user       the first version
 */
#ifndef APPLICATIONS_MOTOR_H_
#define APPLICATIONS_MOTOR_H_

/* 马达A引脚 */
#define PIN_AIN1    GET_PIN(0, 3)
#define PIN_AIN2    GET_PIN(0, 4)

/* 马达B引脚 */
#define PIN_BIN1    GET_PIN(5, 0)
#define PIN_BIN2    GET_PIN(5, 1)

/* 公共引脚 */
#define PIN_STBY    GET_PIN(5, 7)

/* PWM配置 */
#define PWM_DEV_NAME    "pwm0"
#define PWM_MOTOR_A_CH  7
#define PWM_MOTOR_B_CH  1
#define PWM_SERVO_CH    2    // 新通道号
/* 函数声明 */
void motor_init(void);
void motor_a_set(int speed);   /* 马达A控制，-100~100 */
void motor_b_set(int speed);   /* 马达B控制，-100~100 */
void both_forward(void);       /* 两个马达都正转 */
void all_stop(void);           /* 全部停止 */

void servo_turn_left(void);   // 左转 10°
void servo_turn_right(void);  // 右转 10°

void servo_set_angle(int angle);

#endif /* APPLICATIONS_MOTOR_H_ */
