/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-05-09     Rick       the first version
 */
#include "rtthread.h"
#include "flashwork.h"
#include "lcd_display.h"
#include "uart_core.h"

#define DBG_TAG "SYR_API"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

extern uint32_t MCU_Version;
extern uint32_t SN;
extern uint8_t Reminder_Week;
extern uint8_t Reminder_Day;
extern uint8_t Reminder_Enable;
extern uint8_t Automatic_Week;
extern uint8_t Automatic_Day;
extern uint8_t Time_Range;
extern uint8_t Automatic_Enable;
extern uint8_t Deltapress_Enable;
extern uint32_t Counter_Manual;
extern uint32_t Counter_Automatic;
extern uint32_t Counter_Deltapress;
extern uint32_t Counter_Error;
extern uint8_t Setting_Deltapress;
extern uint32_t Setting_Hardness;
extern uint16_t Setting_Backwashtime;
extern uint8_t Setting_Language;
extern uint8_t TDS_CND_Value;
extern uint32_t RTC_Reminder_Time;
extern uint32_t RTC_Automatic_Time;
uint8_t syr_moto_get(void)
{
    uint8_t ret = Get_MotoValid();
    LOG_D("syr_moto_get %d\r\n",ret);
    return ret;
}
uint8_t syr_moto_set(void)
{
    OpenLcdDisplayNoBL();
    LcdtoBackwash();
    LOG_D("syr_moto_set start\r\n");
    return 0;
}
uint16_t syr_backwashtime_get(void)
{
    LOG_D("syr_backwashtime_get %d\r\n",Setting_Backwashtime);
    return Setting_Backwashtime;
}
void syr_backwashtime_set(uint16_t value)
{
    Setting_Backwashtime = value;
    LOG_D("syr_backwashtime_set %d\r\n",value);
    Flash_Set(14,Setting_Backwashtime);
}
uint16_t syr_tdslimit_get(void)
{
    LOG_D("syr_tdslimit_get %d\r\n",Setting_Hardness);
    return Setting_Hardness;
}
void syr_tdslimit_set(uint16_t value)
{
    Setting_Hardness = value;
    LOG_D("syr_tdslimit_set %d\r\n",value);
    Flash_Set(13,Setting_Hardness);
}
uint16_t syr_tdscalib_get(void)
{
    LOG_D("syr_tdscalib_get %d\r\n",TDS_CND_Value);
    return TDS_CND_Value;
}
void syr_tdscalib_set(uint16_t value)
{
    if(!value)value=1;
    TDS_CND_Value = value;
    LOG_D("syr_tdscalib_set %d\r\n",value);
    Flash_Set(21,TDS_CND_Value);
}
void syr_range_set(uint32_t value)
{
    Time_Range = value;
    Flash_Set(22,Time_Range);
    if(Time_Range==0 && Automatic_Week>9)
    {
        Automatic_Week=8;
        Flash_Set(4,Automatic_Week);
        Automatic_Day=5;
        Flash_Set(5,Automatic_Day);
        RTC_Automatic_Time=0;
        LOG_D("RTC_Automatic_Time Change to 0\r\n");
    }
    if(Time_Range==0 && Reminder_Week>9)
    {
        Reminder_Week=8;
        Flash_Set(1,Reminder_Week);
        Reminder_Day=5;
        Flash_Set(2,Reminder_Day);
        RTC_Reminder_Time=0;
        LOG_D("RTC_Reminder_Time Change to 0\r\n");
    }
}
uint32_t syr_range_get(void)
{
    LOG_D("syr_range_get %d\r\n",Time_Range);
    return Time_Range;
}
void syr_remind_time_set(uint32_t value)
{
    Reminder_Week = value/7;
    Flash_Set(1,Reminder_Week);
    Reminder_Day = value%7;
    Flash_Set(2,Reminder_Day);
}
uint32_t syr_remind_time_get(void)
{
    uint16_t total = Reminder_Week*7 + Reminder_Day;
    LOG_D("RSE now time is %d\r\n",total);
    return total;
}
void syr_auto_time_set(uint32_t value)
{
    Automatic_Week = value/7;
    Flash_Set(4,Automatic_Week);
    Automatic_Day = value%7;
    Flash_Set(5,Automatic_Day);
}
uint32_t syr_auto_time_get(void)
{
    uint16_t total = Automatic_Week*7 + Automatic_Day;
    LOG_D("RSA now time is %d\r\n",total);
    return total;
}
uint32_t syr_remain_auto_time_get(void)
{
    uint32_t value = Automatic_Week*7*24 + Automatic_Day*24  - RTC_Automatic_Time;
    LOG_D("Remain time of Automatic is %d\r\n",value);
    return value;
}
uint32_t syr_remain_remind_time_get(void)
{
    uint32_t value = Reminder_Week*7*24 + Reminder_Day*24  - RTC_Reminder_Time;
    LOG_D("Remain time of Remind is %d\r\n",value);
    return value;
}
uint8_t syr_remain_auto_time_set(uint32_t value)
{
    if(value > Automatic_Week*7*24 + Automatic_Day*24)return RT_ERROR;
    RTC_Automatic_Time =  Automatic_Week*7*24 + Automatic_Day*24  - value;
    LOG_D("Remain time of Automatic set to %d\r\n",value);
    return RT_EOK;
}
uint8_t syr_remain_remind_time_set(uint32_t value)
{
    if(value > Reminder_Week*7*24 + Reminder_Day*24)return RT_ERROR;
    RTC_Automatic_Time =  Reminder_Week*7*24 + Reminder_Day*24  - value;
    LOG_D("Remain time of Remain set to %d\r\n",value);
    return RT_EOK;
}
uint8_t syr_switch_remind_get(void)
{
    LOG_D("syr_switch_remind_get %d\r\n",Reminder_Enable);
    return Reminder_Enable;
}
void syr_switch_remind_set(uint8_t value)
{
    Reminder_Enable = value;
    LOG_D("syr_switch_remind_set %d\r\n",value);
    Flash_Set(3,Reminder_Enable);
    Refresh_Display();
}
uint8_t syr_switch_auto_get(void)
{
    LOG_D("syr_switch_auto_get %d\r\n",Automatic_Enable);
    return Automatic_Enable;
}
void syr_switch_auto_set(uint8_t value)
{
    Automatic_Enable = value;
    LOG_D("syr_switch_auto_set %d\r\n",value);
    Flash_Set(6,Automatic_Enable);
    Refresh_Display();
}
uint8_t syr_switch_delta_get(void)
{
    LOG_D("syr_switch_delta_get %d\r\n",Deltapress_Enable);
    return Deltapress_Enable;
}
void syr_switch_delta_set(uint8_t value)
{
    Deltapress_Enable = value;
    LOG_D("syr_switch_delta_set %d\r\n",value);
    Flash_Set(7,Deltapress_Enable);
    Refresh_Display();
}
uint8_t syr_language_get(void)
{
    LOG_D("syr_language_get %d\r\n",Setting_Language);
    return Setting_Language;
}
void syr_language_set(uint8_t value)
{
    Setting_Language = value;
    LOG_D("syr_language_set %d\r\n",Setting_Language);
    Flash_Set(15,Setting_Language);
    Refresh_Language(value);
}
uint8_t syr_count_manual_get(void)
{
    LOG_D("syr_count_manual_get %d\r\n",Counter_Manual);
    return Counter_Manual;
}
void syr_count_manual_set(uint8_t value)
{
    Counter_Manual = 0;
    LOG_D("syr_count_manual_set %d\r\n",Counter_Manual);
    Flash_Set(8,Counter_Manual);
}
uint8_t syr_count_auto_get(void)
{
    LOG_D("syr_count_auto_get %d\r\n",Counter_Automatic);
    return Counter_Automatic;
}
void syr_count_auto_set(uint8_t value)
{
    Counter_Automatic = value;
    LOG_D("syr_count_auto_set %d\r\n",Counter_Automatic);
    Flash_Set(9,Counter_Automatic);
}
uint8_t syr_count_delta_get(void)
{
    LOG_D("syr_count_delta_get %d\r\n",Counter_Deltapress);
    return Counter_Deltapress;
}
void syr_count_delta_set(uint8_t value)
{
    Counter_Deltapress = value;
    LOG_D("syr_count_delta_set %d\r\n",Counter_Deltapress);
    Flash_Set(10,Counter_Deltapress);
}
uint8_t syr_count_error_get(void)
{
    LOG_D("syr_count_error_get %d\r\n",Counter_Error);
    return Counter_Error;
}
void syr_count_error_set(uint8_t value)
{
    Counter_Error = value;
    LOG_D("syr_count_error_set %d\r\n",Counter_Error);
    Flash_Set(11,Counter_Error);
}
extern uint32_t Telemetry_Period;
void syr_telemetry_period_set(uint32_t value)
{
    Telemetry_Period = value;
    LOG_D("syr_telemetry_period_set %d\r\n",value);
    telemetry_timer_set(value);
    Flash_Set(24,value);
}
uint32_t syr_telemetry_period_get(void)
{
    LOG_D("syr_telemetry_period_get %d\r\n",Telemetry_Period);
    return Telemetry_Period;
}
extern uint32_t Telemetry_Timeout;
void syr_telemetry_timeout_set(uint32_t value)
{
    Telemetry_Timeout = value;
    LOG_D("syr_telemetry_timeout_set %d\r\n",value);
    Flash_Set(23,value);
}
uint32_t syr_telemetry_timeout_get(void)
{
    LOG_D("syr_telemetry_timeout_get %d\r\n",Telemetry_Timeout);
    return Telemetry_Timeout;
}
void syr_wifi_ap_enable_set(uint8_t value)
{
    extern uint8_t WIFI_AP_Enable;
    LOG_D("syr_wifi_ap_enable_set %d\r\n",value);
    WIFI_AP_Enable = value;
    Flash_Set(25, value);
    unsigned short send_len = 0;
    send_len = set_wifi_uart_byte(send_len,WIFI_AP_Enable);
    wifi_uart_write_frame(WIFI_AP_ENABLE_CMD, MCU_TX_VER, send_len);
}
