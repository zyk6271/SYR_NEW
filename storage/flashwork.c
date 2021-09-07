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
#define DBG_LVL DBG_LOG
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
    "Auto_Range"                          //22
};
void Set_Default(void)
{
    Flash_Set(2,1);
    Flash_Set(4,8);
    Flash_Set(5,5);
    Flash_Set(6,1);
    Flash_Set(13,280);
    Flash_Set(14,1);
    Flash_Set(15,1);
    Flash_Set(21,11);
    Flash_Set(22,0);
}
int Flash_Init(void)
{
    rt_err_t status;
    extern rt_spi_flash_device_t rt_sfud_flash_probe(const char *spi_flash_dev_name, const char *spi_dev_name);
    rt_pin_mode(FLASH_EN, 0);
    rt_pin_write(FLASH_EN, 1);
    rt_hw_spi_device_attach("spi2", "spi20", GPIOB, GPIO_PIN_12);
    fm25q16 = rt_sfud_flash_probe("norflash0", "spi20");
    if (RT_NULL == fm25q16)
    {
        LOG_E("sfud fail\r\n");
        return RT_ERROR;
    };
    status = fal_init();
    if (status == 0)
    {
        LOG_E("fal_init fail\r\n");
        return RT_ERROR;
    };
    status = easyflash_init();
    if (status != EF_NO_ERR)
    {
        LOG_E("easyflash_init fail\r\n");
        return RT_ERROR;
    };
    LOG_I("Storage Init Success\r\n");
    return RT_EOK;
}
uint32_t Flash_Get(uint8_t id)
{
    uint8_t read_len = 0;
    uint32_t read_value = 0;
    memset(read_value_temp,0,64);
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
    char Temp_ValueBuf[10]={0};//申请临时buffer空间
    char Temp_KeyBuf[40]={0};//申请临时buffer空间
    sprintf(Temp_ValueBuf,"%ld", value);
    sprintf(Temp_KeyBuf,"%s", Key_list[id]);
    ef_set_env(Temp_KeyBuf,Temp_ValueBuf);
    LOG_D("Writing %s to key %s \r\n", Temp_ValueBuf,Temp_KeyBuf);
}

void Flash_Clear(void)
{
    ef_env_set_default();
    LOG_D("Flash Set Clear");
    Set_Default();
    rt_thread_mdelay(2000);
    rt_hw_cpu_reset();
}
