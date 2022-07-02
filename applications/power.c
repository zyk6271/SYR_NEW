/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-08-09     Rick       the first version
 */
#include <rtthread.h>
#include <rtdevice.h>
#include "pin_config.h"
#include "power.h"
#include "lcd_display.h"
#include "adcwork.h"

#define DBG_TAG "POWER"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

uint32_t NowDcVol;
uint32_t NowBatVol;
uint32_t PastBatVol;
uint8_t LowVoltageFlag;

extern uint8_t Low_Power_Flag;
extern uint8_t RTC_Wakeup_Flag;
extern uint32_t BAT_Voltage;
extern uint32_t DC_Voltage;

rt_thread_t power_t = RT_NULL;

uint8_t Get_Bat_Level(void)
{
    return LowVoltageFlag;
}
void PowerSet(uint8_t Flag)
{
    if(Flag != LowVoltageFlag)
    {
        LowVoltageFlag = Flag;
        Flash_Set(19, LowVoltageFlag);
        wifi_sup_update();
    }
}
void PowerCallback(void *parameter)
{
    LOG_D("Power Init OK\r\n");
    rt_thread_mdelay(1000);
    LowVoltageFlag = Flash_Get(19);
    NowDcVol = Get_DC_Level();
    while(1)
    {
        if(Get_DC_Level() == 0 && Low_Power_Flag==0)
        {
            NowDcVol = 0;
            PastBatVol = NowBatVol;
            NowBatVol = BAT_Voltage;
            if(LowVoltageFlag)
            {
                if(NowBatVol>PastBatVol && NowBatVol>20 + PastBatVol)
                {
                    if(NowBatVol>3100)//5.2
                    {
                        PowerSet(0);
                        LOG_D("BatteryOK\r\n");
                        Refresh_Bat();
                    }
                    else if(NowBatVol<=3100)//4.8
                    {
                        PowerSet(1);
                        LOG_D("BatteryLow in New Bat\r\n");
                        JumpToBatteryEmpty();
                    }
                }
            }
            else
            {
                if(NowBatVol<=2860)//4.8
                {
                    PowerSet(1);
                    LOG_D("BatteryLow in Normal\r\n");
                    JumpToBatteryEmpty();
                }
            }
        }
        else if(Get_DC_Level())
        {
            if(!NowDcVol)
            {
                NowDcVol = 1;
                Refresh_Bat();
            }
            PowerSet(0);
        }
        rt_thread_mdelay(5000);
    }
}
void Power_Init(void)
{
    power_t = rt_thread_create("power", PowerCallback, RT_NULL, 2048, 12, 10);
    if(power_t!=RT_NULL)rt_thread_startup(power_t);
}
