/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-10-20     Rick       the first version
 */
#include "rtthread.h"
#include "rtdevice.h"
#include "pin_config.h"
#include "wifiuart.h"
#include "wifiwork.h"
#include "adcwork.h"
#include "string.h"
#include "lcd_display.h"
#include "moto.h"


#define DBG_TAG "WIFIWORK"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

extern uint8_t MotoWorkFlag;
extern uint8_t Reminder_Enable;
extern uint8_t Automatic_Enable;
extern uint8_t Deltapress_Enable;
extern uint32_t Counter_Manual;
extern uint32_t Counter_Automatic;
extern uint32_t Counter_Deltapress;
extern uint32_t Counter_Error;

void wifi_uart_parse(uint8_t command,uint8_t value)
{
    LOG_I("Recv Command is %d,value is %d",command,value);
    switch(command)
    {
    case 0:
        if(Get_MotoValid())
        {
            wifi_uart_send(1,0);
        }
        else
        {
            wifi_uart_send(1,1);
        }
        wifi_uart_send(2,Reminder_Enable);
        wifi_uart_send(3,Automatic_Enable);
        wifi_uart_send(4,Deltapress_Enable);
        counter_upload(1,Counter_Manual);
        counter_upload(2,Counter_Automatic);
        counter_upload(3,Counter_Deltapress);
        counter_upload(4,Counter_Error);
        break;
    case 1:
        Remote_Manual();
        break;
    case 2:
        Remote_Reminder(value);
        break;
    case 3:
        Remote_Automatic(value);
        break;
    case 4:
        Remote_Delta(value);
        break;
    }
}
void counter_upload(uint8_t id,uint32_t value)
{
    LOG_I("Counter_Upload %d,value is %d",id,value);
    switch(id)
    {
    case 1:
        wifi_uart_send(5,value);
        break;
    case 2:
        wifi_uart_send(6,value);
        break;
    case 3:
        wifi_uart_send(7,value);
        break;
    case 4:
        wifi_uart_send(8,value);
        break;
    }
}
