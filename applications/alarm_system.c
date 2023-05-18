/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-05-05     Rick       the first version
 */
#ifndef APPLICATIONS_ALARM_SYSTEM_C_
#define APPLICATIONS_ALARM_SYSTEM_C_

#include "rtthread.h"
#include "uart_core.h"
#include "cjson.h"

#define DBG_TAG "ALARM"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

uint8_t error_value = 0xff;
uint8_t error_array[8] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
uint8_t warning_value = 0xff;
uint8_t warning_array[8] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
uint8_t notify_value = 0xff;
uint8_t notify_array[8] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

void insert_error_array(uint8_t data) {
    for (int i = 6; i >= 0; i--) {
        error_array[i + 1] = error_array[i];
    }
    error_array[0] = data;
    error_value = data;
    alarm_upload(data,2);
    put_error_array();
}
uint8_t get_error_value(void)
{
    return error_value;
}
void get_error_array(uint8_t** buffer)
{
    *buffer = error_array;
}
void put_error_array(void)
{
    uint8_t* array_ptr = RT_NULL;
    get_error_array(&array_ptr);
    uint8_t* alm_buffer = rt_malloc(64);
    rt_sprintf(alm_buffer, "%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x",       \
            array_ptr[0],array_ptr[1],array_ptr[2],array_ptr[3],\
            array_ptr[4],array_ptr[5],array_ptr[6],array_ptr[7]);
    device_info_put_buffer("alm",alm_buffer);
}

void insert_warning_array(uint8_t data) {
    for (int i = 6; i >= 0; i--) {
        warning_array[i + 1] = warning_array[i];
    }
    warning_array[0] = data;
    warning_value = data;
    alarm_upload(data,1);
    put_warning_array();
}
uint8_t get_warning_value(void)
{
    return warning_value;
}
void get_warning_array(uint8_t** buffer)
{
    *buffer = warning_array;
}
void put_warning_array(void)
{
    uint8_t* array_ptr = RT_NULL;
    get_warning_array(&array_ptr);
    uint8_t* alw_buffer = rt_malloc(64);
    rt_sprintf(alw_buffer, "%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x",       \
            array_ptr[0],array_ptr[1],array_ptr[2],array_ptr[3],\
            array_ptr[4],array_ptr[5],array_ptr[6],array_ptr[7]);
    device_info_put_buffer("alw",alw_buffer);
}

void insert_notify_array(uint8_t data) {
    for (int i = 6; i >= 0; i--) {
        notify_array[i + 1] = notify_array[i];
    }
    notify_array[0] = data;
    notify_value = data;
    alarm_upload(data,0);
    put_notify_array();
}
uint8_t get_notify_value(void)
{
    return notify_value;
}
void get_notify_array(uint8_t** buffer)
{
    *buffer = notify_array;
}
void put_notify_array(void)
{
    uint8_t* array_ptr = RT_NULL;
    get_notify_array(&array_ptr);
    uint8_t* aln_buffer = rt_malloc(64);
    rt_sprintf(aln_buffer, "%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x",       \
            array_ptr[0],array_ptr[1],array_ptr[2],array_ptr[3],\
            array_ptr[4],array_ptr[5],array_ptr[6],array_ptr[7]);
    device_info_put_buffer("aln",aln_buffer);
}

void alarm_upload(uint8_t code,uint8_t group)
{
    unsigned char length = 0;

    cJSON *root = NULL;

    root = cJSON_CreateObject();
    if(NULL == root) {
        //可在此添加提示信息，如：printf("xxx");
        return;
    }

    uint8_t code_hex[4];
    rt_sprintf(code_hex, "%02x",code);
    cJSON_AddStringToObject(root, "alarm", code_hex);
    switch(group)
    {
    case 0:
        cJSON_AddStringToObject(root, "group", "notification");
        break;
    case 1:
        cJSON_AddStringToObject(root, "group", "warning");
        break;
    case 2:
        cJSON_AddStringToObject(root, "group", "error");
        break;
    default:
        break;
    }

    char* out = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);

    if(NULL == out) {
        //可在此添加提示信息，如：printf("xxx");
        return;
    }
    LOG_I("%s\r\n",out);
    length = set_wifi_uart_buffer(length, out, rt_strlen(out));
    if(wifi_valid_check())
    {
        wifi_uart_write_frame(ALARM_CONTROL_CMD, MCU_TX_VER, length);
    }
    else
    {
        wifi_uart_write_queue(ALARM_CONTROL_CMD, MCU_TX_VER, length);
    }
    rt_free(out);
}
void event_upload(uint8_t status)
{
    unsigned char length = 0;

    cJSON *root = NULL;

    root = cJSON_CreateObject();
    if(NULL == root) {
        //可在此添加提示信息，如：printf("xxx");
        return;
    }

    if(status)
    {
        cJSON_AddStringToObject(root, "event", "backwash.started");
    }
    else
    {
        cJSON_AddStringToObject(root, "event", "backwash.ended");
    }

    char* out = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);

    if(NULL == out) {
        //可在此添加提示信息，如：printf("xxx");
        return;
    }
    LOG_I("%s\r\n",out);
    length = set_wifi_uart_buffer(length, out, rt_strlen(out));
    if(wifi_valid_check())
    {
        wifi_uart_write_frame(EVENT_CONTROL_CMD, MCU_TX_VER, length);
    }
    else
    {
        wifi_uart_write_queue(EVENT_CONTROL_CMD, MCU_TX_VER, length);
    }
    rt_free(out);
}
#endif /* APPLICATIONS_ALARM_SYSTEM_C_ */
