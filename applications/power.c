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

rt_timer_t Power_timer = RT_NULL;

uint8_t Get_LowVoltageFlag(void)
{
    return LowVoltageFlag;
}
void Power_Check_Start(void)
{
    rt_timer_start(Power_timer);
}
void Power_Check_Stop(void)
{
    rt_timer_stop(Power_timer);
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
uint8_t Power_State_Get(void)
{
    return NowDcVol;
}
void Power_State_Change(uint8_t state)
{
    if(NowDcVol != state)
    {
        NowDcVol = state;
        if(state)
        {
            rt_pm_sleep_request(PM_DC_ID,PM_SLEEP_MODE_NONE);
        }
        else
        {
            ScreenTimerRefresh();
            rt_pm_sleep_release(PM_DC_ID,PM_SLEEP_MODE_NONE);
        }
        LOG_I("Power_State_Change to State:%d\r\n",NowDcVol);
    }
}
void PowerCallback(void *parameter)
{
    uint8_t DC_level = Get_DC_Level();
    Power_State_Change(DC_level);
    if(DC_level == 0)
    {
        PastBatVol = NowBatVol;
        NowBatVol = Get_Bat_Value();
        if(LowVoltageFlag)
        {
            if(NowBatVol>PastBatVol && NowBatVol>100 + PastBatVol)
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
                    LOG_D("New Battery is too low\r\n");
                    JumpToBatteryNew();
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
    else
    {
        if(NowDcVol == 0)
        {
            if(LowVoltageFlag)
            {
                Refresh_Bat();
                PowerSet(0);
            }
        }
    }
}
void Power_Init(void)
{
    LowVoltageFlag = Flash_Get(19);
    Power_timer = rt_timer_create("Power_timer", PowerCallback, RT_NULL, 3000, RT_TIMER_FLAG_SOFT_TIMER|RT_TIMER_FLAG_PERIODIC);
    rt_timer_start(Power_timer);
}
