#include "rtthread.h"
#include "rtdevice.h"
#include <stm32l4xx.h>
#include "moto.h"
#include "pin_config.h"
#include "adcwork.h"
#include "rtc.h"
#include "tds.h"
#include "lcd_display.h"
#include "led.h"
#include "power.h"
#include "adcwork.h"
#include "low.h"

#define DBG_TAG "MOTO"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>


rt_thread_t Moto_t = RT_NULL;

struct rt_event Moto_Event;
rt_timer_t Moto_Cycle_Timer = RT_NULL;
rt_timer_t Moto_Detect_Timer = RT_NULL;

uint8_t MotoWorkFlag;
extern uint16_t Setting_Backwashtime;
extern uint32_t Setting_Hardness;
extern uint8_t TDS_CND_Value;

void Moto_Forward(void)
{
    rt_event_send(&Moto_Event, Event_Moto_Forward);
}
MSH_CMD_EXPORT(Moto_Forward,Moto_Forward);
void Moto_Back(void)
{
    rt_event_send(&Moto_Event, Event_Moto_Back);
}
MSH_CMD_EXPORT(Moto_Back,Moto_Back);
void Moto_Stop(void)
{
    rt_event_send(&Moto_Event, Event_Moto_Free);
}
MSH_CMD_EXPORT(Moto_Stop,Moto_Stop);
void Moto_Overload(void)
{
    rt_event_send(&Moto_Event, Event_Moto_Over);
}
void Moto_Cycle(void)
{
    RTC_Clear();
    if(Get_Bat_Level()==0)
    {
        if(MotoWorkFlag == 0)
        {
            ScreenTimerStop();
            uint32_t Setting_Backwashtime_MileSecond=0;
            Setting_Backwashtime_MileSecond = Setting_Backwashtime*1000;
            rt_timer_control(Moto_Cycle_Timer,RT_TIMER_CTRL_SET_TIME,&Setting_Backwashtime_MileSecond);
            LOG_D("Start Backwash with Timer %d\r\n",Setting_Backwashtime);
            rt_timer_start(Moto_Cycle_Timer);
            Moto_Forward();
            led_select(2);
        }
        else
        {
            LOG_D("Moto is Working Now");
        }
    }
    else
    {
        LOG_D("Moto Not Work(Low Voltage)");
        Jump_EXIT();
        JumpToBatteryEmpty();
    }
}
MSH_CMD_EXPORT(Moto_Cycle,Moto_Cycle);
void Moto_Cycle_Timer_Callback(void *parameter)
{
    if(MotoWorkFlag==1)
    {
        if(rt_pin_read(MOTO_RIGHT)==0)
        {
            LOG_D("Moto Start Back\r\n");
            rt_event_send(&Moto_Event, Event_Moto_Back);
            rt_timer_start(Moto_Cycle_Timer);
        }
        else
        {
            LOG_D("No Moto,Start to Free\r\n");
            rt_event_send(&Moto_Event, Event_Moto_Free);
            ScreenTimerRefresh();
            MotoWorkFlag=0;
            Jump_NOMOTO();
        }
    }
    else if(MotoWorkFlag == 2)
    {
        if(rt_pin_read(MOTO_LEFT)==0)
        {
            LOG_D("Moto Cycle Done\r\n");
            rt_event_send(&Moto_Event, Event_Moto_Free);
            ScreenTimerRefresh();
            MotoWorkFlag=0;
            Jump_FINISH();
        }
        else
        {
            LOG_D("No Moto,Start to Free\r\n");
            MotoWorkFlag=0;
            rt_event_send(&Moto_Event, Event_Moto_Free);
            ScreenTimerRefresh();
            Jump_NOMOTO();
        }
    }
}
void MotoLeft_Callback(void *parameter)
{
    MotoWorkFlag=0;
    rt_timer_stop(Moto_Cycle_Timer);
    rt_event_send(&Moto_Event, Event_Moto_Free);
    if(TDS_WarnGet())
    {
        if(GetTDS() < Setting_Hardness*TDS_CND_Value*0.1)
        {
            TDS_WarnSet(0);
        }
        Jump_FINISH();
    }
    else
    {
        if(GetTDS() > Setting_Hardness*TDS_CND_Value*0.1)
        {
            TDS_WarnSet(1);
            Jump_TDS();
        }
        else
        {
            Jump_FINISH();
        }
    }
    ScreenTimerRefresh();
    LOG_D("Moto Cycle Done,TDS Value is %d\r\n",GetTDS());
}
void MotoRight_Callback(void *parameter)
{
    Moto_Stop();
    TDS_Work();
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
//    rt_pin_mode(MOTO_LEFT, PIN_MODE_INPUT);
//    rt_pin_mode(MOTO_RIGHT, PIN_MODE_INPUT);
}
void Moto_Callback(void *parameter)
{
    rt_uint32_t e;
    LOG_D("Moto Init Success\r\n");
    while(1)
    {
        if (rt_event_recv(&Moto_Event, (Event_Moto_Free | Event_Moto_Forward| Event_Moto_Back| Event_Moto_Over),RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,0, &e) == RT_EOK)
        {
            switch(e)
            {
            case Event_Moto_Free:
                rt_pin_write(MOTO_IN1,0);
                rt_pin_write(MOTO_IN2,0);
                Disable_MotoINT();
                LOG_I("Moto is Free\r\n");
                break;
            case Event_Moto_Forward:
                MotoWorkFlag=1;
                if(rt_pin_read(MOTO_RIGHT)==1 || rt_pin_read(MOTO_LEFT)==0)
                {
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
                MotoWorkFlag=2;
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
                rt_pin_write(MOTO_IN1,0);
                rt_pin_write(MOTO_IN2,0);
                Disable_MotoINT();
                MotoWorkFlag=0;
                ScreenTimerRefresh();
                LOG_I("Moto is Overload\r\n");
                Jump_STALLING();
            }
        }
        rt_thread_mdelay(10);
    }
}
void Moto_Init(void)
{
    Moto_Pin_Init();
    rt_event_init(&Moto_Event, "Moto_Event", RT_IPC_FLAG_FIFO);
    Moto_Cycle_Timer = rt_timer_create("Moto_Cycle_Timer",Moto_Cycle_Timer_Callback,RT_NULL,15*1000,RT_TIMER_FLAG_ONE_SHOT|RT_TIMER_FLAG_SOFT_TIMER);
    Moto_t = rt_thread_create("Moto",Moto_Callback,RT_NULL,2048,10,10);
    if(Moto_t != RT_NULL)
    {
        rt_thread_startup(Moto_t);
    }
}
MSH_CMD_EXPORT(Moto_Init,Moto_Init);
