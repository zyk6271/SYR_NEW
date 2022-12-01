/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-07-13     Rick       the first version
 */
#include <rtthread.h>
#include <rtdevice.h>
#include <spi_flash.h>
#include <drv_spi.h>
#include <string.h>
#include <stdlib.h>
#include "pin_config.h"
#include "flashwork.h"
#include "fal.h"
#include "easyflash.h"

#define DBG_TAG "FLASH"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

rt_spi_flash_device_t fm25q16;
char read_value_temp[64]={0};

const char *Key_list[] =
{
    "boot_times",                         //0
    "Reminder_Week",                      //1
    "Reminder_Day",                       //2
    "Reminder_Enable",                    //3
    "Automatic_Week",                     //4
    "Automatic_Day",                      //5
    "Automatic_Enable",                   //6
    "Deltapress_Enable",                  //7
    "Counter_Manual",                     //8
    "Counter_Automatic",                  //9
    "Counter_Deltapress",                 //10
    "Counter_Error",                      //11
    "Setting_Deltapress",                 //12
    "Setting_Hardness",                   //13
    "Setting_Backwashtime",               //14
    "Setting_Language",                   //15
    "RTC_Reminder_Time",                  //16
    "RTC_Automatic_Time",                 //17
    "TDS_Offset",                         //18
    "BAT_State",                          //19
    "TDS_State",                          //20
    "TDS_CND",                            //21
    "Time_Range",                         //22
    "Telemetry_Timeout",                  //23
    "Telemetry_Period",                   //24
    "AP_Enable",                          //25
};
void Set_Default(void)
{
    Flash_Set(2,1);
    Flash_Set(4,8);
    Flash_Set(5,5);
    Flash_Set(6,1);
    Flash_Set(13,280);
    Flash_Set(14,21);
    Flash_Set(15,1);
    Flash_Set(21,11);
    Flash_Set(22,0);
    Flash_Set(23,30);
    Flash_Set(24,12);
    Flash_Set(25,1);
}
void Flash_Init(void)
{
    rt_pin_mode(FLASH_EN, 0);
    rt_pin_write(FLASH_EN, 1);
    rt_hw_spi_device_attach("spi2", "spi20", GPIOB, GPIO_PIN_12);
    fm25q16 = rt_sfud_flash_probe("norflash0", "spi20");
    if (RT_NULL == fm25q16)
    {
        LOG_E("sfud fail\r\n");
    };
    fal_init();
    easyflash_init();
    if(Flash_Get(0)==0)
    {
        Set_Default();
        Flash_Set(0,1);
    }
    Flash2Mem();
}
uint32_t Flash_Get(uint8_t id)
{
    rt_pm_module_request(PM_STORAGE_ID,PM_SLEEP_MODE_NONE);
    uint8_t read_len = 0;
    uint32_t read_value = 0;
    rt_memset(read_value_temp,0,64);
    read_len = ef_get_env_blob(Key_list[id], read_value_temp, 64, NULL);
    if(read_len>0)
    {
        read_value = atol(read_value_temp);
    }
    else
    {
        read_value = 0;
    }
    LOG_D("Reading Key %s value %ld \r\n", Key_list[id], read_value);//输出
    rt_pm_module_release(PM_STORAGE_ID,PM_SLEEP_MODE_NONE);
    return read_value;
}
void Flash_Set(uint8_t id,uint32_t value)
{
    rt_pm_module_request(PM_STORAGE_ID,PM_SLEEP_MODE_NONE);
    char Set_ValueBuf[64]={0};
    char Set_KeyBuf[64]={0};
    rt_sprintf(Set_ValueBuf,"%ld", value);
    rt_sprintf(Set_KeyBuf,"%s", Key_list[id]);
    ef_set_env(Set_KeyBuf,Set_ValueBuf);
    LOG_D("Writing %s to key %s \r\n", Set_ValueBuf,Set_KeyBuf);
    rt_pm_module_release(PM_STORAGE_ID,PM_SLEEP_MODE_NONE);
}
void Flash_Clear(void)
{
    rt_pm_module_request(PM_STORAGE_ID,PM_SLEEP_MODE_NONE);
    ef_env_set_default();
    LOG_D("Flash Set Clear");
    rt_thread_mdelay(1000);
    rt_hw_cpu_reset();
}
