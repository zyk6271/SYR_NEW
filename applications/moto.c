#include "rtthread.h"
#include "rtdevice.h"
#include <stm32l4xx.h>
#include "moto.h"
#include "pin_config.h"
#include "adcwork.h"
#include "rtc.h"
#include "tds_service.h"
#include "lcd_display.h"
#include "led.h"
#include "power.h"
#include "adcwork.h"
#include "low.h"

#define DBG_TAG "MOTO"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

rt_timer_t Moto_Cycle_Timer = RT_NULL;
rt_timer_t Moto_Detect_Timer = RT_NULL;
rt_timer_t Moto_Current_Timer = RT_NULL;

uint8_t MotoWorkStatus = 0;
uint8_t MotoWorkFlag = MOTO_STOP;

extern uint16_t Setting_Backwashtime;
extern uint32_t Setting_Hardness;

void Moto_Pause(void)
{
    MotoWorkStatus = 1;
    LOG_D("Moto is Pause\r\n");
}
void Moto_Resume(void)
{
    MotoWorkStatus = 0;
    LOG_D("Moto is Resume\r\n");
}
uint8_t Get_MotoValid(void)
{
    if(MotoWorkFlag == MOTO_STOP)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
void Moto_Run(uint8_t state)
{
    switch(state)
    {
    case MOTO_STOP:
        MotoWorkFlag = MOTO_STOP;
        rt_pin_write(MOTO_IN1,0);
        rt_pin_write(MOTO_IN2,0);
        Disable_MotoINT();
        rt_timer_stop(Moto_Detect_Timer);
        LOG_I("Moto is Stop\r\n");
        break;
    case MOTO_FORWARD:
        MotoWorkFlag = MOTO_FORWARD;
        if(rt_pin_read(MOTO_RIGHT)==1)
        {
            rt_pin_write(MOTO_IN1,0);
            rt_pin_write(MOTO_IN2,1);
            Enable_MotoINT();
            rt_timer_start(Moto_Detect_Timer);
            LOG_I("Moto is Forward\r\n");
        }
        else
        {
            LOG_I("Moto State is already Forward\r\n");
            rt_timer_start(Moto_Cycle_Timer);
        }
        break;
    case MOTO_WAITBACK:
        MotoWorkFlag = MOTO_WAITBACK;
        rt_pin_write(MOTO_IN1,0);
        rt_pin_write(MOTO_IN2,0);
        Disable_MotoINT();
        rt_timer_stop(Moto_Detect_Timer);
        LOG_I("Moto is MOTO_WAITBACK\r\n");
        break;
    case MOTO_BACK:
        MotoWorkFlag = MOTO_BACK;
        if(rt_pin_read(MOTO_LEFT)==1)
        {
            rt_pin_write(MOTO_IN1,1);
            rt_pin_write(MOTO_IN2,0);
            Enable_MotoINT();
            rt_timer_start(Moto_Detect_Timer);
            LOG_I("Moto is Back\r\n");
        }
        else
        {
            LOG_I("Moto State is already Back\r\n");
        }
        break;
    case MOTO_RESET:
        MotoWorkFlag = MOTO_STOP;
        if(rt_pin_read(MOTO_LEFT)==1)
        {
            rt_pin_write(MOTO_IN1,1);
            rt_pin_write(MOTO_IN2,0);
            Disable_MotoINT();
            LOG_I("Moto is Reset back\r\n");
        }
        break;
    }
}
uint8_t Moto_Cycle(void)
{
    RTC_Clear();
    if(Get_LowVoltageFlag())
    {
        LOG_W("Moto Not Work(Low Voltage)");
        Jump_EXIT();
        return RT_ERROR;
    }
    if(MotoWorkStatus)
    {
        LOG_W("Moto Not Work(Pause)");
        Jump_EXIT();
        return RT_ERROR;
    }
    if(MotoWorkFlag != MOTO_STOP && MotoWorkFlag != MOTO_RESET)
    {
        LOG_W("Moto Not Work(Working now)");
        Jump_EXIT();
        return RT_ERROR;
    }
    if(MotoWorkFlag == MOTO_STOP)
    {
        rt_pm_sleep_request(PM_MOTO_ID,PM_SLEEP_MODE_NONE);
        ScreenTimerStop();

        uint32_t Setting_Backwashtime_MileSecond = (Setting_Backwashtime-20)*1000;
        rt_timer_control(Moto_Cycle_Timer,RT_TIMER_CTRL_SET_TIME,&Setting_Backwashtime_MileSecond);
        Setting_Backwashtime_MileSecond += 60*1000;
        rt_timer_control(Moto_Detect_Timer,RT_TIMER_CTRL_SET_TIME,&Setting_Backwashtime_MileSecond);
        LOG_D("Start Backwash with Timer %d\r\n",(Setting_Backwashtime-20));

        Moto_Run(MOTO_FORWARD);
        led_select(2);
        return RT_EOK;
    }
    else
    {
        LOG_I("Moto is working now\r\n");
        return RT_EBUSY;
    }
}
void Moto_Cycle_Timer_Callback(void *parameter)
{
    LOG_D("Moto Start Back\r\n");
    Moto_Run(MOTO_BACK);
}
void Moto_Current_Detect(void)
{
    rt_timer_start(Moto_Current_Timer);
    LOG_D("Moto_Current delay detect\r\n");
}
void Moto_Current_Timer_Callback(void *parameter)
{
    if(Get_Moto_Current()>=45)//延迟检测
    {
        LOG_E("Moto Current Overload\r\n");//发生过流
        led_select(3);
        Moto_Run(MOTO_STOP);
        ScreenTimerRefresh();
        Jump_STALLING();
        Moto_Run(MOTO_RESET);
        rt_pm_sleep_release(PM_MOTO_ID,PM_SLEEP_MODE_NONE);
    }
    else
    {
        LOG_D("Moto Current Not Overload\r\n");//没发生过流
        Enable_MotoINT();//重启电流中断
    }
}
void Moto_Detect_Timer_Callback(void *parameter)
{
    if(MotoWorkFlag == MOTO_FORWARD)
    {
        if(rt_pin_read(MOTO_RIGHT)==1)
        {
            LOG_W("No Moto,Start to Free\r\n");
            led_select(3);
            Moto_Run(MOTO_RESET);
            Jump_NOMOTO();
        }
    }
    else if(MotoWorkFlag == MOTO_BACK)
    {
        if(rt_pin_read(MOTO_LEFT)==1)
        {
            LOG_W("No Moto,Start to Free\r\n");
            led_select(3);
            Moto_Run(MOTO_RESET);
            Jump_NOMOTO();
        }
    }
    ScreenTimerRefresh();
    rt_pm_sleep_release(PM_MOTO_ID,PM_SLEEP_MODE_NONE);
}
void MotoLeft_Callback(void *parameter)
{
    if(MotoWorkFlag == MOTO_BACK)
    {
        led_select(3);
        Moto_Run(MOTO_STOP);
        if(GetTDS() >= Setting_Hardness)
        {
            if(GetTDSWarn())
            {
                Jump_FINISH();
            }
            else
            {
                SetTDSWarn(1);
                Jump_TDS();
            }
        }
        else
        {
            SetTDSWarn(0);
            Jump_FINISH();
        }
        LOG_I("Moto Cycle Done,TDS Value is %d\r\n",GetTDS());
        ScreenTimerRefresh();
        rt_pm_sleep_release(PM_MOTO_ID,PM_SLEEP_MODE_NONE);
    }
    else if(MotoWorkFlag == MOTO_RESET)
    {
        Moto_Run(MOTO_STOP);
    }
}
void MotoRight_Callback(void *parameter)
{
    if(MotoWorkFlag == MOTO_FORWARD)
    {
        Moto_Run(MOTO_WAITBACK);
        TDS_Work();
        rt_timer_start(Moto_Cycle_Timer);
    }
}
void Moto_Pin_Init(void)
{
    rt_pin_mode(MOTO_MODE, PIN_MODE_OUTPUT);
    rt_pin_write(MOTO_MODE,0);
    rt_pin_mode(MOTO_IN1, PIN_MODE_OUTPUT);
    rt_pin_write(MOTO_IN1,0);
    rt_pin_mode(MOTO_IN2, PIN_MODE_OUTPUT);
    rt_pin_write(MOTO_IN2,0);
    rt_pin_mode(MOTO_VCC, PIN_MODE_OUTPUT);
    rt_pin_write(MOTO_VCC,1);

    rt_pin_mode(MOTO_LEFT, PIN_MODE_INPUT);
    rt_pin_attach_irq(MOTO_LEFT, PIN_IRQ_MODE_FALLING, MotoLeft_Callback, RT_NULL);
    rt_pin_irq_enable(MOTO_LEFT, PIN_IRQ_ENABLE);

    rt_pin_mode(MOTO_RIGHT, PIN_MODE_INPUT);
    rt_pin_attach_irq(MOTO_RIGHT, PIN_IRQ_MODE_FALLING, MotoRight_Callback, RT_NULL);
    rt_pin_irq_enable(MOTO_RIGHT, PIN_IRQ_ENABLE);
}
void Moto_Pin_DeInit(void)
{
    rt_pin_write(MOTO_MODE,0);
    rt_pin_write(MOTO_IN1,0);
    rt_pin_write(MOTO_IN2,0);
    rt_pin_write(MOTO_VCC,0);

    rt_pin_irq_enable(MOTO_LEFT, PIN_IRQ_DISABLE);
    rt_pin_irq_enable(MOTO_RIGHT, PIN_IRQ_DISABLE);
    rt_pin_detach_irq(MOTO_LEFT);
    rt_pin_detach_irq(MOTO_RIGHT);

    rt_pin_mode(MOTO_LEFT, PIN_MODE_OUTPUT);
    rt_pin_write(MOTO_LEFT,0);
    rt_pin_mode(MOTO_RIGHT, PIN_MODE_OUTPUT);
    rt_pin_write(MOTO_RIGHT,0);
}
void Moto_Init(void)
{
    Moto_Pin_Init();
    Moto_Cycle_Timer = rt_timer_create("Moto_Cycle_Timer",Moto_Cycle_Timer_Callback,RT_NULL,15*1000,RT_TIMER_FLAG_ONE_SHOT|RT_TIMER_FLAG_SOFT_TIMER);
    Moto_Detect_Timer = rt_timer_create("Moto_Detect_Timer",Moto_Detect_Timer_Callback,RT_NULL,60*1000,RT_TIMER_FLAG_ONE_SHOT|RT_TIMER_FLAG_SOFT_TIMER);
    Moto_Current_Timer = rt_timer_create("Moto_Current_Timer",Moto_Current_Timer_Callback,RT_NULL,200,RT_TIMER_FLAG_ONE_SHOT|RT_TIMER_FLAG_SOFT_TIMER);
    Moto_Run(MOTO_RESET);
}
