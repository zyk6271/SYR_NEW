/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-05-10     Rick       the first version
 */
#include "rtthread.h"

#define DBG_TAG "wifi_connect"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

rt_timer_t ap_timeout_timer = RT_NULL;
rt_timer_t wifi_connect_timer = RT_NULL;
rt_timer_t wifi_connect_close_timer = RT_NULL;

extern uint32_t AP_Timeout;

void ap_timeout_control(uint8_t state)
{
    if(state)
    {
        LOG_D("ap_timeout enable\r\n");
        syr_wifi_ap_enable_set(1);
        if(AP_Timeout)//判断时长非0
        {
            rt_timer_start(ap_timeout_timer);
        }
    }
    else
    {
        LOG_D("ap_timeout disable\r\n");
        syr_wifi_ap_enable_set(0);
        rt_timer_stop(ap_timeout_timer);
    }
}
void ap_timeout_set(uint32_t time)
{
    AP_Timeout = time;
    if(time == 0)
    {
        rt_timer_stop(ap_timeout_timer);
    }
    else
    {
        rt_timer_control(ap_timeout_timer, RT_TIMER_CTRL_SET_TIME, &AP_Timeout);
    }
}
void ap_timeout_disable(void)
{
    LOG_D("ap_timeout is disable\r\n");
    syr_wifi_ap_enable_set(0);
}
void ap_timeout_timer_callback(void* parameter)
{
    LOG_D("ap_timeout is trigger\r\n");
    syr_wifi_ap_enable_set(0);
}

void wifi_connect_timer_callback(void* parameter)
{
    LOG_D("wifi_connect_timer_callback is trigger\r\n");
    wifi_connect_fail();
}

void wifi_connect_start(void)
{
    wifi_connect_win_add();
    rt_timer_start(wifi_connect_timer);
}
void wifi_connect_success(void)
{
    wifi_connect_show(2);
    rt_timer_start(wifi_connect_close_timer);
    rt_timer_stop(wifi_connect_timer);
}
void wifi_connect_fail(void)
{
    wifi_connect_show(1);
    rt_timer_start(wifi_connect_close_timer);
}
void wifi_connect_stop(void* parameter)
{
    wifi_connect_win_del();
}
void wifi_connect_init(void)
{
    ap_timeout_timer = rt_timer_create("ap_timeout", ap_timeout_timer_callback, RT_NULL, AP_Timeout*1000, RT_TIMER_FLAG_SOFT_TIMER | RT_TIMER_FLAG_ONE_SHOT);
    wifi_connect_timer = rt_timer_create("wifi_connect", wifi_connect_timer_callback, RT_NULL, 60*1000, RT_TIMER_FLAG_SOFT_TIMER | RT_TIMER_FLAG_ONE_SHOT);
    wifi_connect_close_timer = rt_timer_create("wifi_connect_close", wifi_connect_stop, RT_NULL, 3*1000, RT_TIMER_FLAG_SOFT_TIMER | RT_TIMER_FLAG_ONE_SHOT);
}
