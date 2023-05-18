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
#include "Flashwork.h"
#include "cjson.h"

#define DBG_TAG "TELEMETRY"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

struct rt_lptimer rcp_timer;
rt_timer_t wti_timer = RT_NULL;

rt_sem_t telemetry_detect_sem = RT_NULL;
rt_sem_t telemetry_recv_sem = RT_NULL;

rt_thread_t telemetry_thread = RT_NULL;

extern rt_mq_t telemetry_queue;

void rcp_timer_set(uint32_t set_time)
{
    LOG_D("rcp_timer_set to %d\r\n",set_time);
    uint32_t set_tick = set_time*1000;
    rt_lptimer_control(&rcp_timer,RT_TIMER_CTRL_SET_TIME,&set_tick);
    rt_lptimer_start(&rcp_timer);
}

void rcp_timer_callback(void *parameter)
{
    LCD_Restart(0);
    LOG_D("Telemetry Wake Up Now\r\n");
    rt_pm_sleep_request(PM_PERIOD_ID, PM_SLEEP_MODE_NONE);
    rt_sem_release(telemetry_detect_sem);
}

void wti_timer_set(uint32_t set_time)
{
    LOG_D("rcp_timer_set to %d\r\n",set_time);
    uint32_t set_tick = set_time*1000;
    rt_timer_control(wti_timer,RT_TIMER_CTRL_SET_TIME,&set_tick);
    rt_timer_start(wti_timer);
}
void wti_timer_callback(void *parameter)
{
    LOG_D("wti_timer_callback\r\n");
    telemetry_upload();
}
void telemetry_interval_mode_set(uint8_t dc_level)
{
    if(dc_level)
    {
        rt_timer_start(wti_timer);
        rt_lptimer_stop(&rcp_timer);
    }
    else
    {
        rt_timer_stop(wti_timer);
        rt_lptimer_start(&rcp_timer);
    }
}
void telemetry_upload(void)
{
    unsigned char length = 0;

    cJSON *root = NULL;

    TDS_Work();

    root = cJSON_CreateObject();
    if(NULL == root) {
        //可在此添加提示信息，如：printf("xxx");
        return;
    }

    cJSON_AddNumberToObject(root, "net", Get_DC_Fix());
    cJSON_AddNumberToObject(root, "bat", Get_Bat_Fix());
    cJSON_AddNumberToObject(root, "com", syr_count_manual_get());
    cJSON_AddNumberToObject(root, "coa", syr_count_auto_get());
    cJSON_AddNumberToObject(root, "cod", syr_count_delta_get());
    uint8_t ala_hex[4];
    rt_sprintf(ala_hex, "%02x",get_error_value());
    cJSON_AddStringToObject(root, "ala", ala_hex);
    uint8_t not_hex[4];
    rt_sprintf(not_hex, "%02x",get_notify_value());
    cJSON_AddStringToObject(root, "not", not_hex);
    uint8_t wrn_hex[4];
    rt_sprintf(wrn_hex, "%02x",get_warning_value());
    cJSON_AddStringToObject(root, "wrn", wrn_hex);
    cJSON_AddNumberToObject(root, "cnd", GetTDS());

    char* out = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);

    if(NULL == out) {
        //可在此添加提示信息，如：printf("xxx");
        return;
    }
    length = set_wifi_uart_buffer(length, out, rt_strlen(out));
    wifi_uart_write_frame(TELEMETRY_CONTROL_CMD, MCU_TX_VER, length);
    rt_free(out);
}
void telemetry_thread_callback(void *parameter)
{
    while(1)
    {
        rt_sem_take(telemetry_detect_sem,RT_WAITING_FOREVER);
        rt_sem_control(telemetry_recv_sem, RT_IPC_CMD_RESET, RT_NULL);
        telemetry_upload();
        if(rt_sem_take(telemetry_recv_sem, Telemetry_Timeout*1000) == RT_EOK)
        {
            LOG_I("Telemetry Recv OK\r\n");
        }
        else
        {
            LOG_W("Telemetry Recv Timeout\r\n");
            rt_mq_control(telemetry_queue,RT_IPC_CMD_RESET,RT_NULL);
        }
        rt_pm_sleep_release(PM_PERIOD_ID,PM_SLEEP_MODE_NONE);
    }
}
void telemetry_flush(void)
{
    uint8_t alarm_buffer[64];
    while(rt_mq_recv(telemetry_queue,&alarm_buffer, 64, RT_WAITING_NO) == RT_EOK)
    {
        wifi_uart_write_data((unsigned char *)alarm_buffer, 64);
    }
}
void Telemetry_Init(void)
{
    telemetry_detect_sem = rt_sem_create("telemetry_detect_sem", 0, RT_IPC_FLAG_FIFO);
    telemetry_recv_sem = rt_sem_create("telemetry_recv_sem", 0, RT_IPC_FLAG_FIFO);
    telemetry_thread = rt_thread_create("telemetry_t", telemetry_thread_callback, RT_NULL, 2048, 10, 10);
    rt_thread_startup(telemetry_thread);

    rt_lptimer_init(&rcp_timer, "rcp_timer", rcp_timer_callback, RT_NULL, Report_Interval*60*1000,\
            RT_TIMER_FLAG_PERIODIC | RT_TIMER_FLAG_SOFT_TIMER);

    wti_timer = rt_timer_create("wti_timer",wti_timer_callback,RT_NULL,Telemetry_Interval*1000,\
            RT_TIMER_FLAG_PERIODIC | RT_TIMER_FLAG_SOFT_TIMER);
}
