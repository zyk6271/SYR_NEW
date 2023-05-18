/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-03-24     Rick       the first version
 */
#include <rtthread.h>
#include <stdio.h>
#include <stdbool.h>
#include <finsh.h>
#include <fal.h>

#include "uart_core.h"

#define DBG_TAG "wifi_ota"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

uint8_t wifi_ota_update_flag = 0;

#define DEFAULT_DOWNLOAD_PART "download"

rt_timer_t OTA_Timeout_Timer = RT_NULL;

static char* recv_partition = DEFAULT_DOWNLOAD_PART;
static size_t update_file_total_size, update_file_cur_size;
static const struct fal_partition * dl_part = RT_NULL;

void wifi_ota_begin(rt_uint32_t file_size)
{
    update_file_total_size = file_size;
    LOG_I("wifi_ota_begin file_size:%d\n", update_file_total_size);
    update_file_cur_size = 0;

    /* Get download partition information and erase download partition data */
    if ((dl_part = fal_partition_find(recv_partition)) == RT_NULL)
    {
        LOG_E("Firmware download failed! Partition (%s) find error!", recv_partition);
        return ;
    }

    if (update_file_total_size > dl_part->len)
    {
        LOG_E("Firmware is too large! File size (%d), '%s' partition size (%d)", update_file_total_size, recv_partition, dl_part->len);
        return ;
    }

    LOG_I("Start erase. Size (%d)", update_file_total_size);

    /* erase DL section */
    if (fal_partition_erase(dl_part, 0, update_file_total_size) < 0)
    {
        LOG_E("Firmware download failed! Partition (%s) erase error!", dl_part->name);
        return ;
    }
}

int wifi_ota_receive(rt_uint8_t *buf, rt_size_t offset, rt_size_t len)
{
    /* write data of application to DL partition  */
    if(update_file_cur_size != offset)//old
    {
        LOG_E("Firmware download failed! Frame repeat,Drop!\r\n");
        return RT_ERROR;
    }
    if (fal_partition_write(dl_part, update_file_cur_size, buf, len) < 0)
    {
        LOG_E("Firmware download failed! Partition (%s) write data error!", dl_part->name);
        return RT_ERROR;
    }
    update_file_cur_size += len;//new
    extern uint8_t st_progress;
    st_progress = update_file_cur_size * 100 / update_file_total_size;
    //LOG_D("Write success,offset (%d),recv size (%d),total size (%d)\r\n",offset,update_file_cur_size,update_file_total_size);
    return RT_EOK;
}
void wifi_ota_end(rt_uint8_t *buf, rt_size_t offset)
{
    Flash_Clear();
    LOG_I("update successfully,system will boot in 1 sec...\r\n");
}
void wifi_ota_request(uint8_t value)
{
    wifi_ota_update_flag = 1;
    unsigned short send_len = 0;
    send_len = set_wifi_uart_byte(send_len,value);
    wifi_uart_write_frame(UPDATE_CONTROL_CMD, MCU_TX_VER, send_len);
    wifi_ota_timer_refresh();
}
void OTA_Timeout_Timer_Callback(void* parameter)
{
    extern uint16_t ota_status;
    ota_status = Network_Fail;
}
void wifi_ota_init(void)
{
    OTA_Timeout_Timer = rt_timer_create("OTA_Timeout_Timer", OTA_Timeout_Timer_Callback, RT_NULL, 120000, RT_TIMER_FLAG_SOFT_TIMER|RT_TIMER_FLAG_PERIODIC);
}
void wifi_ota_timer_refresh(void)
{
    rt_timer_start(OTA_Timeout_Timer);
}
void wifi_ota_timer_stop(void)
{
    rt_timer_stop(OTA_Timeout_Timer);
}
