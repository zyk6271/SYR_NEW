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
    }
}
void PowerCallback(void *parameter)
{
    LOG_D("Power Init OK\r\n");
    LowVoltageFlag = Flash_Get(19);
    rt_thread_mdelay(2000);
    while(1)
    {
        if(Get_DC_Level() == 0 && Low_Power_Flag==0)
        {
            PastBatVol = NowBatVol;
            NowBatVol = Get_Bat_Value();
            if(LowVoltageFlag == 1)
            {
                if(NowBatVol>PastBatVol && NowBatVol>100 + PastBatVol)
                {
                    if(NowBatVol>3500)//5.2
                    {
                        PowerSet(0);
                        LOG_D("BatteryOK\r\n");
                    }
                    else if(NowBatVol<=3500 && NowBatVol>2900)//5.2
                    {
//                        PowerSet(3);
//                        LOG_D("BatteryBAD\r\n");
//                        JumpToBatteryNew();
                        PowerSet(1);
                        LOG_D("BatteryLow in New Bat\r\n");
                        JumpToBatteryEmpty();
                    }
                    else if(NowBatVol<=2900)//5.2
                    {
                        PowerSet(1);
                        LOG_D("BatteryLow in New Bat\r\n");
                        JumpToBatteryEmpty();
                    }
                }
            }
            else if(LowVoltageFlag == 3)
            {
                if(NowBatVol<=3500)//4.8
                {
                    PowerSet(1);
                    LOG_D("BatteryLow in Bad\r\n");
                    JumpToBatteryEmpty();
                }
                else
                {
                    PowerSet(0);
                    LOG_D("BatteryOK in Bad\r\n");
                }
            }
            else if(LowVoltageFlag == 0)
            {
                if(NowBatVol<=2900)//4.8
                {
                    PowerSet(1);
                    LOG_D("BatteryLow in Normal\r\n");
                    JumpToBatteryEmpty();
                }
            }
        }
        else if(Get_DC_Level() == 0 && Low_Power_Flag==1)
        {

        }
        else if(Get_DC_Level())
        {
            PowerSet(0);
        }
        rt_thread_mdelay(5000);
    }
}
void Power_Init(void)
{
    power_t = rt_thread_create("power", PowerCallback, RT_NULL, 2048, 30, 10);
    if(power_t!=RT_NULL)rt_thread_startup(power_t);
}
