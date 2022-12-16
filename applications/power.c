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

extern uint32_t BAT_Voltage;
extern uint32_t DC_Voltage;

rt_thread_t power_t = RT_NULL;
rt_sem_t power_check_sem = RT_NULL;

uint8_t Get_Bat_Level(void)
{
    return LowVoltageFlag;
}
void Power_Check(void)
{
    rt_sem_release(power_check_sem);
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
            rt_system_power_manager_pause();
        }
        else
        {
            ScreenTimerRefresh();
            rt_system_power_manager_resume();
        }
        LOG_I("Power_State_Change to State:%d\r\n",NowDcVol);
    }
}
void PowerCallback(void *parameter)
{
    LOG_D("Power Init OK\r\n");
    LowVoltageFlag = Flash_Get(19);
    while(1)
    {
        if(rt_sem_trytake(power_check_sem) == RT_EOK)
        {
            rt_sem_control(power_check_sem, RT_IPC_CMD_RESET, RT_NULL);
            if(Get_DC_Level() == 0)
            {
                Power_State_Change(0);
                PastBatVol = NowBatVol;
                NowBatVol = BAT_Voltage;
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
                    Power_State_Change(1);
                    if(LowVoltageFlag)
                    {
                        Refresh_Bat();
                        PowerSet(0);
                    }
                }
            }
        }
        rt_thread_mdelay(10);
    }
}
void Power_Init(void)
{
    power_check_sem = rt_sem_create("power_check_sem", 0, RT_IPC_FLAG_FIFO);
    power_t = rt_thread_create("power", PowerCallback, RT_NULL, 2048, 12, 10);
    if(power_t!=RT_NULL)rt_thread_startup(power_t);
}
