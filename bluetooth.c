/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2026-05-22     user       the first version
 */
#include <rtthread.h>
#include <rtdevice.h>
#include "motor.h"

#define BT_UART_NAME    "uart5"
/*蓝牙参数*/
#define BT_THREAD_STACK  1024  //字节
#define BT_THREAD_PRIO   25    //优先级
#define BT_THREAD_TICK   10    //系统节拍

static rt_device_t bt_dev = RT_NULL;
static struct rt_semaphore rx_sem;//信号量

/*中断*/
static rt_err_t bt_rx_callback(rt_device_t dev, rt_size_t size)
{
    /* 有数据到达，释放信号量唤醒处理线程 */
    rt_sem_release(&rx_sem);
    return RT_EOK;
}

/* 蓝牙数据处理线程 */
static void bt_thread_entry(void *parameter)
{
    char buf[8];       // 缓冲区，一次读多个字节
    char ch;
    int n;
    int speed;

    while (1)
    {
        /* 等待信号量（超时1秒，避免蓝牙断连后永远卡住） */
        rt_err_t ret = rt_sem_take(&rx_sem, rt_tick_from_millisecond(3000));

        if (ret == -RT_ETIMEOUT)
        {
            /* N秒内没收到任何数据，自动停止（安全保护） */
            //all_stop();
            //rt_kprintf("BT Timeout! Auto stop.\n");
            continue;  // 回去继续等
        }

        /* 一次性读空串口缓冲区 */
        n = rt_device_read(bt_dev, 0, buf, sizeof(buf) - 1);

        if (n <= 0)
        {
            continue;  // 没读到数据，回去继续等
        }

        buf[n] = '\0';  // 加上字符串结束符

        /* 取第一个有效字符作为指令 */
        ch = buf[0];

        /* 调试：打印收到的数据 */
        rt_kprintf("BT CMD: %c\n", ch);

        /* 根据指令执行对应的电机动作 */
        switch (ch)
        {
            case 'F':   // Forward 前进
            case 'f':
                motor_a_set(50);
                motor_b_set(50);
                rt_kprintf("Action: Forward\n");
                break;

            case 'B':   // Backward 后退
            case 'b':
                motor_a_set(-50);
                motor_b_set(-50);
                rt_kprintf("Action: Backward\n");
                break;

            case 'L':   // Left 左转
            case 'l':
                motor_a_set(-50);
                motor_b_set(50);
                rt_kprintf("Action: Turn Left\n");
                break;

            case 'R':   // Right 右转
            case 'r':
                motor_a_set(50);
                motor_b_set(-50);
                rt_kprintf("Action: Turn Right\n");
                break;

            case 'S':   // Stop 停止
                all_stop();
                rt_kprintf("Action: Stop\n");
                break;


            case '+':
                servo_turn_right();
                rt_kprintf("Action: Up\n");
                break;

            case '-':
                servo_turn_left();
                rt_kprintf("Action: Down\n");
                break;


            case '\r':  // 忽略回车
            case '\n':  // 忽略换行
                break;

            default:
                rt_kprintf("Unknown command: 0x%02X\n", ch);
                break;
        }
    }
}

int bluetooth_init(void)
{
    rt_err_t ret;
    rt_thread_t tid;

    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;

    /* 改波特率 */
    config.baud_rate = BAUD_RATE_9600;

    /*查找 UART 设备 */
    bt_dev = rt_device_find(BT_UART_NAME);
    if (bt_dev == RT_NULL)
    {
        rt_kprintf("BT Error: UART device '%s' not found!\n", BT_UART_NAME);
        return -RT_ERROR;
    }

    /*在 open之前配置 */
    rt_device_control(bt_dev, RT_DEVICE_CTRL_CONFIG, &config);

    /*初始化信号量 */
    ret = rt_sem_init(&rx_sem, "bt_rx", 0, RT_IPC_FLAG_FIFO);
    if (ret != RT_EOK)
    {
        rt_kprintf("BT Error: sem init failed!\n");
        return -RT_ERROR;
    }

    /*以中断接收模式打开串口设备 */
    ret = rt_device_open(bt_dev, RT_DEVICE_FLAG_INT_RX | RT_DEVICE_FLAG_RDWR);
    if (ret != RT_EOK)
    {
        rt_kprintf("BT Error: open device '%s' failed!\n", BT_UART_NAME);
        return -RT_ERROR;
    }

    /*注册中断函数 */
    rt_device_set_rx_indicate(bt_dev, bt_rx_callback);

    /*创建数据处理线程 */
    tid = rt_thread_create("bt_remote",
                           bt_thread_entry,
                           RT_NULL,
                           BT_THREAD_STACK,
                           BT_THREAD_PRIO,
                           BT_THREAD_TICK);
    if (tid == RT_NULL)
    {
        rt_kprintf("BT Error: create thread failed!\n");
        return -RT_ERROR;
    }

    /* 启动线程 */
    rt_thread_startup(tid);

    rt_kprintf("BT init OK (9600)\n");

    return RT_EOK;
}
