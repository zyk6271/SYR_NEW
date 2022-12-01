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

struct rt_event Moto_Event;

rt_thread_t Moto_t = RT_NULL;
rt_timer_t Moto_Cycle_Timer = RT_NULL;
rt_timer_t Moto_Detect_Timer = RT_NULL;
rt_timer_t Moto_Current_Timer = RT_NULL;
rt_timer_t Moto_TDS_Timer = RT_NULL;

uint8_t MotoWorkFlag;
extern uint16_t Setting_Backwashtime;
extern uint32_t Setting_Hardness;
extern uint32_t Moto_Current;

#define MOTO_STOP 0
#define MOTO_FORWARD 1
#define MOTO_BACK 2
#define MOTO_RESET 3

uint8_t Get_MotoValid(void)
{
    if(MotoWorkFlag == MOTO_STOP)return 1;
    else return 0;
}
void Moto_Forward(void)
{
    rt_event_send(&Moto_Event, Event_Moto_Forward);
}
void Moto_Back(void)
{
    rt_event_send(&Moto_Event, Event_Moto_Back);
}
void Moto_Stop(void)
{
    rt_pin_write(MOTO_IN1,0);
    rt_pin_write(MOTO_IN2,0);
    rt_event_send(&Moto_Event, Event_Moto_Free);
}
void Moto_Overload(void)
{
    rt_pin_write(MOTO_IN1,0);
    rt_pin_write(MOTO_IN2,0);
    rt_event_send(&Moto_Event, Event_Moto_Over);
}
uint8_t Moto_Cycle(void)
{
    RTC_Clear();
    TDS_GpioInit();
    if(Get_Bat_Level()==0)
    {
        if(MotoWorkFlag == MOTO_STOP || MotoWorkFlag == MOTO_RESET)
        {
            ScreenTimerStop();
            rt_pm_module_request(PM_MOTO_ID,PM_SLEEP_MODE_NONE);
            uint32_t Setting_Backwashtime_MileSecond=0;
            Setting_Backwashtime_MileSecond = (Setting_Backwashtime-20)*1000;
            rt_timer_control(Moto_Cycle_Timer,RT_TIMER_CTRL_SET_TIME,&Setting_Backwashtime_MileSecond);
            Setting_Backwashtime_MileSecond += 60*1000;
            rt_timer_control(Moto_Detect_Timer,RT_TIMER_CTRL_SET_TIME,&Setting_Backwashtime_MileSecond);
            LOG_D("Start Backwash with Timer %d\r\n",(Setting_Backwashtime-20));
            rt_timer_start(Moto_Detect_Timer);
            Moto_Forward();
            led_select(2);
            return RT_EOK;
        }
        else
        {
            LOG_D("Moto is Working Now");
            return RT_EBUSY;
        }
    }
    else
    {
        LOG_D("Moto Not Work(Low Voltage)");
        Jump_EXIT();
        return RT_ERROR;
    }
}
void Moto_Cycle_Timer_Callback(void *parameter)
{
    if(MotoWorkFlag == MOTO_FORWARD)
    {
        LOG_D("Moto Start Back\r\n");
        rt_event_send(&Moto_Event, Event_Moto_Back);
        rt_timer_start(Moto_Detect_Timer);
    }
}
void Moto_TDS_Timer_Callback(void *parameter)
{
    Jump_TDS();
}
void Moto_Current_Detect(void)
{
    rt_timer_start(Moto_Current_Timer);
    LOG_D("Moto_Current_Timer is Start\r\n");
}
void Moto_Current_Timer_Callback(void *parameter)
{
    if(Moto_Current>=45)
    {
        LOG_E("Moto Current Still Overload\r\n");
        Moto_Overload();
    }
    else
    {
        Enable_MotoINT();
    }
}
void Moto_Detect_Timer_Callback(void *parameter)
{
    if(MotoWorkFlag == MOTO_FORWARD)
    {
        if(rt_pin_read(MOTO_RIGHT)==1)
        {
            LOG_D("No Moto,Start to Free\r\n");
            rt_event_send(&Moto_Event, Event_Moto_Free);
            MotoWorkFlag=MOTO_STOP;
            ScreenTimerRefresh();
            rt_pm_module_release(PM_MOTO_ID,PM_SLEEP_MODE_NONE);
            Jump_NOMOTO();
            Moto_Reset();
        }
    }
    else if(MotoWorkFlag == MOTO_BACK)
    {
        if(rt_pin_read(MOTO_LEFT)==0)
        {
            LOG_D("Moto Cycle Done\r\n");
            rt_event_send(&Moto_Event, Event_Moto_Free);
            MotoWorkFlag = MOTO_STOP;
            ScreenTimerRefresh();
            rt_pm_module_release(PM_MOTO_ID,PM_SLEEP_MODE_NONE);
            Jump_FINISH();
        }
        else
        {
            LOG_D("No Moto,Start to Free\r\n");
            MotoWorkFlag = MOTO_STOP;
            rt_event_send(&Moto_Event, Event_Moto_Free);
            ScreenTimerRefresh();
            rt_pm_module_release(PM_MOTO_ID,PM_SLEEP_MODE_NONE);
            Jump_NOMOTO();
            Moto_Reset();
        }
    }
}
void MotoLeft_Callback(void *parameter)
{
    if(MotoWorkFlag == MOTO_BACK)
    {
        Moto_Stop();
        MotoWorkFlag = MOTO_STOP;
        rt_timer_stop(Moto_Detect_Timer);
        rt_event_send(&Moto_Event, Event_Moto_Free);
        if(TDS_WarnGet())
        {
            if(GetTDS() < Setting_Hardness)
            {
                TDS_WarnSet(0);
            }
            Jump_FINISH();
        }
        else
        {
            if(GetTDS() > Setting_Hardness)
            {
                TDS_WarnSet(1);
                rt_timer_start(Moto_TDS_Timer);
            }
            else
            {
                Jump_FINISH();
            }
        }
        ScreenTimerRefresh();
        rt_pm_module_release(PM_MOTO_ID,PM_SLEEP_MODE_NONE);
        LOG_D("Moto Cycle Done,TDS Value is %d\r\n",GetTDS());
    }
    else if(MotoWorkFlag == MOTO_STOP)
    {
        rt_event_send(&Moto_Event, Event_Moto_Free);
    }
    else if(MotoWorkFlag == MOTO_RESET)
    {
        MotoWorkFlag = MOTO_STOP;
        Moto_Stop();
    }
}
void MotoRight_Callback(void *parameter)
{
    if(MotoWorkFlag == MOTO_FORWARD)
    {
        Moto_Stop();
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
    rt_pin_mode(MOTO_MODE, PIN_MODE_OUTPUT);
    rt_pin_write(MOTO_MODE,0);
    rt_pin_mode(MOTO_IN1, PIN_MODE_OUTPUT);
    rt_pin_write(MOTO_IN1,0);
    rt_pin_mode(MOTO_IN2, PIN_MODE_OUTPUT);
    rt_pin_write(MOTO_IN2,0);
    rt_pin_mode(MOTO_VCC, PIN_MODE_OUTPUT);
    rt_pin_write(MOTO_VCC,0);
    rt_pin_detach_irq(MOTO_LEFT);
    rt_pin_detach_irq(MOTO_RIGHT);
    rt_pin_irq_enable(MOTO_LEFT, PIN_IRQ_DISABLE);
    rt_pin_irq_enable(MOTO_RIGHT, PIN_IRQ_DISABLE);
    rt_pin_mode(MOTO_LEFT, PIN_MODE_OUTPUT);
    rt_pin_write(MOTO_LEFT,0);
    rt_pin_mode(MOTO_RIGHT, PIN_MODE_OUTPUT);
    rt_pin_write(MOTO_RIGHT,0);
}
void Moto_Callback(void *parameter)
{
    rt_uint32_t e;
    LOG_D("Moto Init Success\r\n");
    Moto_Reset();
    while(1)
    {
        if (rt_event_recv(&Moto_Event, (Event_Moto_Free | Event_Moto_Forward| Event_Moto_Back| Event_Moto_Over),RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,0, &e) == RT_EOK)
        {
            switch(e)
            {
            case Event_Moto_Free:
                wifi_ras_update();
                rt_pin_write(MOTO_IN1,0);
                rt_pin_write(MOTO_IN2,0);
                Disable_MotoINT();
                rt_timer_stop(Moto_Detect_Timer);
                LOG_I("Moto is Free\r\n");
                break;
            case Event_Moto_Forward:
                MotoWorkFlag = MOTO_FORWARD;
                if(rt_pin_read(MOTO_RIGHT)==1 || rt_pin_read(MOTO_LEFT)==0)
                {
                    wifi_ras_update();
                    rt_pin_write(MOTO_IN1,0);
                    rt_pin_write(MOTO_IN2,1);
                    rt_thread_mdelay(500);
                    Enable_MotoINT();
                    LOG_I("Moto is Forward\r\n");
                }
                else
                {
                    LOG_I("Moto State is already Forward\r\n");
                }
                break;
            case Event_Moto_Back:
                MotoWorkFlag = MOTO_BACK;
                if(rt_pin_read(MOTO_RIGHT)==0 || rt_pin_read(MOTO_LEFT)==1)
                {
                    rt_pin_write(MOTO_IN1,1);
                    rt_pin_write(MOTO_IN2,0);
                    rt_thread_mdelay(500);
                    Enable_MotoINT();
                    LOG_I("Moto is Back\r\n");
                }
                else
                {
                    LOG_I("Moto State is already Back\r\n");
                }
                break;
            case Event_Moto_Over:
                rt_timer_stop(Moto_Detect_Timer);
                rt_pin_write(MOTO_IN1,0);
                rt_pin_write(MOTO_IN2,0);
                Disable_MotoINT();
                MotoWorkFlag = MOTO_STOP;
                ScreenTimerRefresh();
                rt_pm_module_release(PM_MOTO_ID,PM_SLEEP_MODE_NONE);
                LOG_D("Moto Event Overload\r\n");
                Jump_STALLING();
                Moto_Reset();
                break;
            }
        }
        rt_thread_mdelay(10);
    }
}
void Moto_Reset(void)
{
    if(rt_pin_read(MOTO_RIGHT)==0 || rt_pin_read(MOTO_LEFT)==1)
    {
        MotoWorkFlag = MOTO_RESET;
        rt_pin_write(MOTO_IN1,1);
        rt_pin_write(MOTO_IN2,0);
        Disable_MotoINT();
        LOG_I("Moto is Back\r\n");
    }
}
void Moto_Init(void)
{
    Moto_Pin_Init();
    rt_event_init(&Moto_Event, "Moto_Event", RT_IPC_FLAG_FIFO);
    Moto_Cycle_Timer = rt_timer_create("Moto_Cycle_Timer",Moto_Cycle_Timer_Callback,RT_NULL,15*1000,RT_TIMER_FLAG_ONE_SHOT|RT_TIMER_FLAG_SOFT_TIMER);
    Moto_Detect_Timer = rt_timer_create("Moto_Detect_Timer",Moto_Detect_Timer_Callback,RT_NULL,60*1000,RT_TIMER_FLAG_ONE_SHOT|RT_TIMER_FLAG_SOFT_TIMER);
    Moto_TDS_Timer = rt_timer_create("Moto_TDS_Timer",Moto_TDS_Timer_Callback,RT_NULL,100,RT_TIMER_FLAG_ONE_SHOT|RT_TIMER_FLAG_SOFT_TIMER);
    Moto_Current_Timer = rt_timer_create("Moto_Current_Timer",Moto_Current_Timer_Callback,RT_NULL,500,RT_TIMER_FLAG_ONE_SHOT|RT_TIMER_FLAG_SOFT_TIMER);
    Moto_t = rt_thread_create("Moto",Moto_Callback,RT_NULL,2048,10,10);
    if(Moto_t != RT_NULL)
    {
        rt_thread_startup(Moto_t);
    }
}
