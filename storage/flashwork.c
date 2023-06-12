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
#include "sfud.h"

#define DBG_TAG "FLASH"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

rt_spi_flash_device_t fm25q16;
char read_value_temp[64]={0};

uint8_t Reminder_Week=0;
uint8_t Reminder_Day=0;
uint8_t Reminder_Enable=0;
uint8_t Automatic_Week=0;
uint8_t Automatic_Day=0;
uint8_t Time_Range=0;
uint8_t Automatic_Enable=0;
uint8_t Deltapress_Enable=0;
uint32_t Counter_Manual=0;
uint32_t Counter_Automatic=0;
uint32_t Counter_Deltapress=0;
uint32_t Counter_Error=0;
uint32_t Setting_Hardness = 0;
uint16_t Setting_Backwashtime = 0;
uint8_t Setting_Language=0;
uint8_t TDS_CND_Value = 10;
uint32_t AP_Timeout = 0;
uint32_t RTC_Reminder_Time = 0;
uint32_t RTC_Automatic_Time = 0;
uint8_t LowVoltageFlag = 0;
uint32_t Telemetry_Interval = 0;
uint32_t Telemetry_Timeout = 0;
uint32_t Report_Interval = 0;

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
    "BAT_State",                          //18
    "TDS_CND",                            //19
    "Time_Range",                         //20
    "Telemetry_Timeout",                  //21
    "Telemetry_Interval",                 //22
    "Report_Interval",                    //23
    "AP_Timeout",                         //24
};
void Set_Default(void)
{
    Flash_Set(2,1);
    Flash_Set(4,8);
    Flash_Set(5,5);
    Flash_Set(6,1);
    Flash_Set(13,30);
    Flash_Set(14,21);
    Flash_Set(15,1);
    Flash_Set(19,19);
    Flash_Set(20,0);
    Flash_Set(21,30);
    Flash_Set(22,1800);
    Flash_Set(23,24);
    Flash_Set(24,0);
}
void Flash2Mem(void)
{
    Reminder_Week = Flash_Get(1);
    Reminder_Day = Flash_Get(2);
    Reminder_Enable = Flash_Get(3);
    Automatic_Week = Flash_Get(4);
    Automatic_Day = Flash_Get(5);
    Automatic_Enable = Flash_Get(6);
    Deltapress_Enable = Flash_Get(7);
    Counter_Manual = Flash_Get(8);
    Counter_Automatic = Flash_Get(9);
    Counter_Deltapress = Flash_Get(10);
    Counter_Error = Flash_Get(11);
    Setting_Hardness = Flash_Get(13);
    Setting_Backwashtime = Flash_Get(14);
    Setting_Language = Flash_Get(15);
    RTC_Reminder_Time = Flash_Get(16);
    RTC_Automatic_Time = Flash_Get(17);
    LowVoltageFlag = Flash_Get(18);
    TDS_CND_Value = Flash_Get(19);
    Time_Range = Flash_Get(20);
    Telemetry_Timeout = Flash_Get(21);
    Telemetry_Interval = Flash_Get(22);
    Report_Interval = Flash_Get(23);
    AP_Timeout = Flash_Get(24);
    if(Setting_Language)
    {
        SetEnglish();
    }
    else
    {
        SetGerman();
    }
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
    if(easyflash_init() != EF_NO_ERR)
    {
        static sfud_flash_t sfud_dev = NULL;
        sfud_dev = rt_sfud_flash_find_by_dev_name(FAL_USING_NOR_FLASH_DEV_NAME);
        sfud_erase(sfud_dev,0,1048576);
    }
    if(Flash_Get(0)==0)
    {
        Flash_Set(0,1);
        Set_Default();
    }
    Flash2Mem();
}
uint32_t Flash_Get(uint8_t id)
{
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
    return read_value;
}
void Flash_Set(uint8_t id,uint32_t value)
{
    rt_pm_sleep_request(PM_STORAGE_ID,PM_SLEEP_MODE_NONE);
    char Set_ValueBuf[64]={0};
    char Set_KeyBuf[64]={0};
    rt_sprintf(Set_ValueBuf,"%ld", value);
    rt_sprintf(Set_KeyBuf,"%s", Key_list[id]);
    ef_set_env(Set_KeyBuf,Set_ValueBuf);
    LOG_D("Writing %s to key %s \r\n", Set_ValueBuf,Set_KeyBuf);
    rt_pm_sleep_release(PM_STORAGE_ID,PM_SLEEP_MODE_NONE);
}
void Flash_Clear(void)
{
    rt_pm_sleep_request(PM_STORAGE_ID,PM_SLEEP_MODE_NONE);
    Set_Default();
    LOG_I("env clear success");
    rt_thread_mdelay(3000);
    rt_hw_cpu_reset();
    rt_pm_sleep_release(PM_STORAGE_ID,PM_SLEEP_MODE_NONE);
}
