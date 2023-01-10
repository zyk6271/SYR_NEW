/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-11-08     Rick       the first version
 */
#include "rtthread.h"
#include "uart_core.h"
#include "power.h"
#include "pm.h"

#define DBG_TAG "TELEMETRY"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

uint32_t Telemetry_Period;
uint32_t Telemetry_Timeout;

struct rt_lptimer telemetry_timer;
rt_thread_t telemetry_thread = RT_NULL;
rt_sem_t telemetry_detect_sem = RT_NULL;
rt_sem_t telemetry_recv_sem = RT_NULL;

void telemetry_timer_set(uint32_t set_time)
{
    LOG_D("telemetry_timer_set to %d\r\n",set_time);
    uint32_t set_tick = set_time*60*60*1000;
    rt_lptimer_control(&telemetry_timer,RT_TIMER_CTRL_SET_TIME,&set_tick);
    rt_lptimer_start(&telemetry_timer);
}
void telemetry_timer_callback(void *parameter)
{
    LCD_Restart(0);
    LOG_D("Telemetry Wake Up Now\r\n");
    rt_pm_sleep_request(PM_PERIOD_ID, PM_SLEEP_MODE_NONE);
    rt_sem_release(telemetry_detect_sem);
}
void telemetry_test(void)
{
    rt_sem_release(telemetry_detect_sem);
    LOG_D("Telemetry Test\r\n");
}
MSH_CMD_EXPORT(telemetry_test,telemetry_test);
void telemetry_thread_callback(void *parameter)
{
    while(1)
    {
        rt_sem_take(telemetry_detect_sem, RT_WAITING_FOREVER);
        rt_sem_control(telemetry_recv_sem, RT_IPC_CMD_RESET, RT_NULL);
        if(Power_State_Get())
        {
            telemetry_upload();
        }
        if(rt_sem_take(telemetry_recv_sem, Telemetry_Timeout*1000) == RT_EOK)
        {
            LOG_I("Telemetry Recv OK\r\n");
        }
        else
        {
            LOG_W("Telemetry Recv Timeout\r\n");
        }
        rt_pm_sleep_release(PM_PERIOD_ID,PM_SLEEP_MODE_NONE);
    }
}
void Telemetry_Init(void)
{
    telemetry_detect_sem = rt_sem_create("telemetry_detect_sem", 0, RT_IPC_FLAG_FIFO);
    telemetry_recv_sem = rt_sem_create("telemetry_recv_sem", 0, RT_IPC_FLAG_FIFO);
    telemetry_thread = rt_thread_create("telemetry_t", telemetry_thread_callback, RT_NULL, 1024, 10, 10);
    if(telemetry_thread != RT_NULL)
    {
        rt_thread_startup(telemetry_thread);
    }
    rt_lptimer_init(&telemetry_timer, "telemetry_timer", telemetry_timer_callback, RT_NULL, Telemetry_Period*60*60*1000,
    RT_TIMER_FLAG_PERIODIC | RT_TIMER_FLAG_SOFT_TIMER);
    rt_lptimer_start(&telemetry_timer);
}
