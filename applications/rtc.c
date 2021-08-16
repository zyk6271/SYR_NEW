#include <rtthread.h>
#include <rtdevice.h>
#include <stm32l4xx.h>
#include "pin_config.h"
#include "Flashwork.h"
#include "lcd_display.h"
#include "adcwork.h"

#define DBG_TAG "RTC"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

rt_timer_t RTC_Timer=RT_NULL;
rt_thread_t RTC_Task=RT_NULL;
rt_thread_t RTC_Check_Thread=RT_NULL;
rt_sem_t RTC_Sem = RT_NULL;
rt_sem_t RTC_Check_Sem = RT_NULL;
RTC_HandleTypeDef RtcHandle;

extern uint8_t Reminder_Week;
extern uint8_t Reminder_Day;
extern uint8_t Reminder_Enable;
extern uint8_t Automatic_Week;
extern uint8_t Automatic_Day;
extern uint8_t Automatic_Enable;
extern uint8_t LCD_Flag;
extern uint8_t RTC_Wakeup_Flag;
extern uint8_t Low_Power_Flag;

uint32_t RTC_Reminder_Time = 0;
uint32_t RTC_Automatic_Time = 0;
uint32_t Reminder_Time = 0;
uint32_t Automatic_Time = 0;

void RTC_Timer_Entry(void)
{
    if(Reminder_Enable)
    {
        RTC_Reminder_Time++;
        LOG_D("RTC_Reminder_Time is %d\r\n",RTC_Reminder_Time);
    }
    if(Automatic_Enable)
    {
        RTC_Automatic_Time++;
        LOG_D("RTC_Automatic_Time is %d\r\n",RTC_Automatic_Time);
    }
}
void RTC_Check(void)
{
    rt_sem_release(RTC_Check_Sem);
}
void RTC_Clear(void)
{
    RTC_Reminder_Time = 0;
    RTC_Automatic_Time = 0;
}
uint8_t RTC_Event_Flag = 0;
void RTC_Check_Callback(void *parameter)
{
    while(1)
    {
        if(rt_sem_take(RTC_Check_Sem, 0) == RT_EOK)
        {
            LOG_D("RTC Check For Two Timers\r\n");
            RTC_Event_Flag = 0;
            if(Reminder_Enable)
            {
                if(RTC_Reminder_Time >= Reminder_Week*7*24+Reminder_Day*24)
                {
                    RTC_Reminder_Time = 0;
                    RTC_Event_Flag = 1;
                    Flash_Set(16,0);
                    LOG_D("Reminder_Enable\r\n");
                    if(Low_Power_Flag)
                    {
                        LcdInit();
                        LCD_Refresh();
                        ScreenTimerRefresh();
                    }
                    JumptoReminder();
                }
                else
                {
                    Flash_Set(16,RTC_Reminder_Time);
                }
            }
            if(Automatic_Enable)
            {
                if(RTC_Automatic_Time >= Automatic_Week*7*24+Automatic_Day*24 )
                {
                    RTC_Automatic_Time = 0;
                    RTC_Event_Flag = 1;
                    Flash_Set(17,0);
                    LOG_D("Automatic_Enable\r\n");
                    if(Low_Power_Flag)
                    {
                        LcdInit();
                        LCD_Refresh();
                        ScreenTimerRefresh();
                    }
                    JumptoAutomatic();
                }
                else
                {
                    Flash_Set(17,RTC_Automatic_Time);
                }
            }
            if(RTC_Event_Flag == 0 && Low_Power_Flag==1)
            {
                LOG_D("EnterLowPower\r\n");
                EnterLowPower();
            }
        }
        rt_thread_mdelay(10);
    }
}
void RTC_Check_Init(void)
{
    RTC_Check_Sem = rt_sem_create("RTC_Check_Sem", 0, RT_IPC_FLAG_FIFO);
    RTC_Check_Thread = rt_thread_create("RTC_Check", RTC_Check_Callback, RT_NULL, 2048, 10, 10);
    if(RTC_Check_Thread!=RT_NULL)rt_thread_startup(RTC_Check_Thread);
}

void RTC_AlarmConfig(void)
{
    RTC_DateTypeDef  sdatestructure;
    RTC_TimeTypeDef  stimestructure;
    RTC_AlarmTypeDef salarmstructure;

    sdatestructure.Year = 0x14;
    sdatestructure.Month = RTC_MONTH_FEBRUARY;
    sdatestructure.Date = 0x18;
    sdatestructure.WeekDay = RTC_WEEKDAY_TUESDAY;

    if(HAL_RTC_SetDate(&RtcHandle,&sdatestructure,RTC_FORMAT_BIN) != HAL_OK)
    {
      /* Initialization Error */
    }

    stimestructure.Hours = 0;
    stimestructure.Minutes = 0;
    stimestructure.Seconds = 0;
    stimestructure.TimeFormat = RTC_HOURFORMAT12_AM;
    stimestructure.DayLightSaving = RTC_DAYLIGHTSAVING_NONE ;
    stimestructure.StoreOperation = RTC_STOREOPERATION_RESET;

    if(HAL_RTC_SetTime(&RtcHandle,&stimestructure,RTC_FORMAT_BIN) != HAL_OK)
    {
      /* Initialization Error */
    }

    salarmstructure.Alarm = RTC_ALARM_A;
    salarmstructure.AlarmDateWeekDay = RTC_WEEKDAY_MONDAY;
    salarmstructure.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_WEEKDAY;
    salarmstructure.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY;
    salarmstructure.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
    salarmstructure.AlarmTime.TimeFormat = RTC_HOURFORMAT12_AM;
    salarmstructure.AlarmTime.Hours = 0;
    salarmstructure.AlarmTime.Minutes = 1;
    salarmstructure.AlarmTime.Seconds = 0;
    salarmstructure.AlarmTime.SubSeconds = 0;

    if(HAL_RTC_SetAlarm_IT(&RtcHandle,&salarmstructure,RTC_FORMAT_BIN) == HAL_OK)
    {
        LOG_D("RTC Alarm Set Ok\r\n");
    }
}
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *RtcHandle)
{
    RTC_TimeTypeDef sTime = {0};
    sTime.Hours = 0;
    sTime.Minutes = 0;
    sTime.Seconds = 0;
    sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sTime.StoreOperation = RTC_STOREOPERATION_RESET;
    HAL_RTC_SetTime(RtcHandle, &sTime, RTC_FORMAT_BIN);

    RTC_Timer_Entry();
    RTC_Check();
}
void RTC_Init(void)
{
    RTC_Reminder_Time = Flash_Get(16);
    RTC_Automatic_Time = Flash_Get(17);
    Reminder_Time = Reminder_Week*7*24+Reminder_Day*24;
    Automatic_Time = Automatic_Week*7*24+Automatic_Day*24;

    __HAL_RCC_RTC_ENABLE();
    HAL_NVIC_SetPriority(RTC_Alarm_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);

    RtcHandle.Instance = RTC;
    RtcHandle.Init.HourFormat = RTC_HOURFORMAT_24;
    RtcHandle.Init.AsynchPrediv = 127;
    RtcHandle.Init.SynchPrediv = 255;
    RtcHandle.Init.OutPut = RTC_OUTPUT_DISABLE;
    RtcHandle.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
    RtcHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    RtcHandle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
    if (HAL_RTC_Init(&RtcHandle) != HAL_OK)
    {

    }
    RTC_AlarmConfig();
    RTC_Check_Init();
    LOG_D("RTC Init is Success\r\n");
}
MSH_CMD_EXPORT(RTC_Init,RTC_Init);
void RTC_Alarm_IRQHandler(void)
{
    if(Low_Power_Flag)
    {
        SystemClock_Config();
        AfterWake();
        LOG_I("RTC Wake Up\r\n");
    }
    HAL_RTC_AlarmIRQHandler(&RtcHandle);
}

