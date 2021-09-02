/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-08-11     Rick       the first version
 */
#include <rtthread.h>
#include <rtdevice.h>
#include "delta.h"
#include "Pin_config.h"

#define DBG_TAG "DELTA"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

rt_thread_t Delta_t = RT_NULL;
rt_sem_t Delta_Sem = RT_NULL;
uint8_t Delta_Status;

extern uint8_t Deltapress_Enable;
extern uint8_t Setting_Deltapress;
extern uint32_t Counter_Deltapress;
extern uint8_t Delta_Wakeup_Flag;

void Delta_press_work(void *parameter)
{
    while(1)
    {
        //Delta_Status = rt_sem_take(Delta_Sem, 0);
        if(Delta_Status)
        {
            rt_thread_mdelay(500);
            Delta_Status = 0;
            LOG_D("Detected Water Flow\r\n");
            if(Deltapress_Enable)
            {
                Counter_Deltapress++;
                Flash_Set(10,Counter_Deltapress);
                JumptoAutomatic();
            }
        }
        rt_thread_mdelay(500);
    }
}
void Delta_Press_Callback(void *parameter)
{
    Delta_release();
}
void Delta_Init(void)
{
    Deltapress_Enable = Flash_Get(7);
    rt_pin_mode(WATER_FLOW, PIN_MODE_INPUT);
    rt_pin_attach_irq(WATER_FLOW, PIN_IRQ_MODE_FALLING, Delta_Press_Callback, RT_NULL);
    if(Deltapress_Enable)
    {
        LOG_I("Deltapress is Enable\r\n");
        rt_pin_irq_enable(WATER_FLOW, PIN_IRQ_ENABLE);
    }
    //Delta_Sem = rt_sem_create("delta_sem", 0, RT_IPC_FLAG_FIFO);
    Delta_t = rt_thread_create("delta_thread", Delta_press_work, RT_NULL, 2048, 10, 10);
    if(Delta_t != RT_NULL)
    {
        rt_thread_startup(Delta_t);
    }
}
void Delta_release(void)
{
    //rt_sem_release(Delta_Sem);
    Delta_Status = 1;
    Delta_Wakeup_Flag = 1;
}
void Delta_Open(void)
{
    rt_pin_irq_enable(WATER_FLOW, PIN_IRQ_ENABLE);
}
void Delta_Close(void)
{
    rt_pin_irq_enable(WATER_FLOW, PIN_IRQ_DISABLE);
}
void Delta_Reload(void)
{
    if(Deltapress_Enable)
    {
        rt_pin_attach_irq(WATER_FLOW, PIN_IRQ_MODE_FALLING, Delta_Press_Callback, RT_NULL);
        rt_pin_irq_enable(WATER_FLOW, Deltapress_Enable);
    }
    else
    {
        rt_pin_detach_irq(WATER_FLOW);
        rt_pin_irq_enable(WATER_FLOW, PIN_IRQ_DISABLE);
        rt_pin_mode(WATER_FLOW, PIN_MODE_INPUT);
    }
}
