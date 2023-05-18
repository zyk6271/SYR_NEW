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
#include "Flashwork.h"

#define DBG_TAG "POWER"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

uint8_t Now_DC_level = 0;
uint32_t Now_BAT_Voltage = 0;
uint32_t Past_BAT_Voltage = 0;

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
void PowerSet(uint8_t value)
{
    if(value != LowVoltageFlag)
    {
        LowVoltageFlag = value;
        Flash_Set(19, LowVoltageFlag);
        device_info_put("sup",value);
    }
}
uint8_t Power_State_Get(void)
{
    return Now_DC_level;
}
void Power_State_Change(uint8_t state)
{
    if(Now_DC_level != state)
    {
        Now_DC_level = state;
        if(state)
        {
            rt_pm_sleep_request(PM_DC_ID,PM_SLEEP_MODE_NONE);
        }
        else
        {
            ScreenTimerRefresh();
            rt_pm_sleep_release(PM_DC_ID,PM_SLEEP_MODE_NONE);
        }
        LOG_I("Power_State_Change to State:%d\r\n",Now_DC_level);
        telemetry_interval_mode_set(state);
    }
}

void PowerCallback(void *parameter)
{
    if(Get_DC_Level() == 0)//No DC
    {
        Power_State_Change(0);
        Past_BAT_Voltage = Now_BAT_Voltage;
        Now_BAT_Voltage = Get_Bat_Value();
        if(LowVoltageFlag)
        {
            if(Now_BAT_Voltage>Past_BAT_Voltage && Now_BAT_Voltage>100 + Past_BAT_Voltage)
            {
                if(Now_BAT_Voltage>3100)//5.2
                {
                    PowerSet(0);
                    LOG_D("BatteryOK\r\n");
                    ReloadMainWin();
                    insert_warning_array(0xff);
                }
                else if(Now_BAT_Voltage<=3100)//4.8
                {
                    PowerSet(1);
                    LOG_D("New Battery is too low\r\n");
                    JumpToBatteryNew();
                }
            }
        }
        else
        {
            if(Now_BAT_Voltage<=2860)//4.8
            {
                PowerSet(1);
                LOG_D("BatteryLow in Normal\r\n");
                JumpToBatteryEmpty();
                insert_warning_array(0x08);
            }
        }
    }
    else
    {
        if(Now_DC_level == 0)//DC input ok is low battery
        {
            Power_State_Change(1);
            if(LowVoltageFlag)
            {
                PowerSet(0);
                ReloadMainWin();
            }
            insert_warning_array(0xff);
        }
    }
}
void Power_Init(void)
{
    Power_timer = rt_timer_create("Power_timer", PowerCallback, RT_NULL, 2000, RT_TIMER_FLAG_SOFT_TIMER|RT_TIMER_FLAG_PERIODIC);
    rt_timer_start(Power_timer);
}
