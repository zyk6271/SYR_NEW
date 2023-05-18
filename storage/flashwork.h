/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-07-13     Rick       the first version
 */
#ifndef APPLICATIONS_FLASHWORK_H_
#define APPLICATIONS_FLASHWORK_H_

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
extern uint32_t Setting_Hardness;
extern uint16_t Setting_Backwashtime;
extern uint8_t Setting_Language;
extern uint8_t TDS_CND_Value;
extern uint32_t AP_Timeout;
extern uint32_t RTC_Reminder_Time;
extern uint32_t RTC_Automatic_Time;
extern uint8_t LowVoltageFlag;
extern uint32_t Telemetry_Interval;
extern uint32_t Telemetry_Timeout;
extern uint32_t Report_Interval;

#include "rtthread.h"

void Flash_Init(void);
uint32_t Flash_Get(uint8_t id);
void Flash2Mem(void);
void Flash_Set(uint8_t id,uint32_t value);
void Flash_Clear(void);

#endif /* APPLICATIONS_FLASHWORK_H_ */
