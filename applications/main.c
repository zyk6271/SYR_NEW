/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-08-02     RT-Thread    first version
 */

#include <rtthread.h>
#include "flashwork.h"
#include "key.h"
#include "moto.h"
#include "adcwork.h"
#include "tds.h"
#include "low.h"
#include "lcd_display.h"
#include "led.h"
#include "delta.h"
#include "power.h"
#include "rtc.h"

#define DBG_TAG "main"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

int main(void)
{
    Flash_Init();
    ADC_Init();
    Button_Init();
    Led_Init();
    Moto_Init();
    TDS_Init();
    LCD_Init();
    Low_Init();
    RTC_Init();
    Power_Init();
    Delta_Init();
    while (1)
    {
        rt_thread_mdelay(1000);
    }

    return RT_EOK;
}
