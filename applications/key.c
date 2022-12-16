#include <rtthread.h>
#include <rtdevice.h>
#include "Key.h"
#include "pin_config.h"
#include "button.h"
#include "Flashwork.h"
#include "moto.h"
#include "12864.h"

#define DBG_TAG "DeviceInfo"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

rt_thread_t button_task = RT_NULL;

rt_sem_t K0_Sem = RT_NULL;
rt_sem_t K0_Long_Sem = RT_NULL;
rt_sem_t K1_Sem = RT_NULL;
rt_sem_t K1_Long_Sem = RT_NULL;
rt_sem_t K2_Sem = RT_NULL;
rt_sem_t K2_Long_Sem = RT_NULL;

uint8_t K0_OnceFlag=0;
uint8_t K1_OnceFlag=0;
uint8_t K2_OnceFlag=0;
uint8_t K0_Long_Sem_Counter=0;
uint8_t K1_Long_Sem_Counter=0;
uint8_t K2_Long_Sem_Counter=0;

void Light_BackLight(void)
{
	rt_pin_mode(LCD_BL, PIN_MODE_OUTPUT);
	rt_pin_write(LCD_BL,1);
}
void Key_Sem_init(void)
{
	K0_Sem=rt_sem_create("K0", 0, RT_IPC_FLAG_FIFO);
	K0_Long_Sem=rt_sem_create("K0_Long", 0, RT_IPC_FLAG_FIFO);
	K1_Sem=rt_sem_create("K1", 0, RT_IPC_FLAG_FIFO);
	K1_Long_Sem=rt_sem_create("K1_Long", 0, RT_IPC_FLAG_FIFO);
	K2_Sem=rt_sem_create("K2", 0, RT_IPC_FLAG_FIFO);
	K2_Long_Sem = rt_sem_create("K2_Long", 0, RT_IPC_FLAG_FIFO);
}
void K0_Sem_Release(void *parameter)
{
    OpenLcdDisplay();
    ScreenTimerRefresh();
    LOG_D("Up Key is Press\r\n");
    if(GetLCD()==0)
    {
        rt_sem_release(K0_Sem);
    }
}
void K1_Sem_Release(void *parameter)
{
    OpenLcdDisplay();
    ScreenTimerRefresh();
    LOG_D("Down Key is Press\r\n");
    if(GetLCD()==0)
    {
        rt_sem_release(K1_Sem);
    }
}
void K2_Sem_Release(void *parameter)
{
    OpenLcdDisplay();
    ScreenTimerRefresh();
    LOG_D("Enter Key is Press\r\n");
    if(GetLCD()==0)
    {
        rt_sem_release(K2_Sem);
    }
}
void K0_LongSem_Release(void *parameter)
{
    OpenLcdDisplay();
    ScreenTimerRefresh();
    if(K0_OnceFlag==0)
    {
        if(K0_Long_Sem_Counter>0)
        {
            K0_OnceFlag=1;
            rt_sem_release(K0_Long_Sem);
            LOG_D("K0 is Long\r\n");
        }
        else
        {
            K0_Long_Sem_Counter++;
            LOG_D("K0 Long Counter is %d",K0_Long_Sem_Counter);
        }
    }
    else {
        rt_sem_release(K0_Sem);
    }
}
void K1_LongSem_Release(void *parameter)
{
    OpenLcdDisplay();
    ScreenTimerRefresh();
    if(K1_OnceFlag==0)
    {
        if(K1_Long_Sem_Counter>0)
        {
            K1_OnceFlag=1;
            rt_sem_release(K1_Long_Sem);
            LOG_D("K1 is Long\r\n");
        }
        else
        {
            K1_Long_Sem_Counter++;
            LOG_D("K1 Long Counter is %d",K1_Long_Sem_Counter);
        }
    }
    else
    {
        rt_sem_release(K1_Sem);
    }
}
void K2_LongSem_Release(void *parameter)
{
    if(K2_OnceFlag==0)
    {
        OpenLcdDisplay();
        ScreenTimerRefresh();
        if(K2_Long_Sem_Counter>6)
        {
            K2_OnceFlag=1;
            rt_sem_release(K2_Long_Sem);
            LOG_D("K2 is Long\r\n");
        }
        else
        {
            K2_Long_Sem_Counter++;
            LOG_D("K2 Long Counter is %d",K2_Long_Sem_Counter);
        }
    }
}
void K0_LongFree_Release(void *parameter)
{
    K0_OnceFlag=0;
    K0_Long_Sem_Counter=0;
    OpenLcdDisplay();
    ScreenTimerRefresh();
    LOG_D("Up Key is Long Free\r\n");
}
void K1_LongFree_Release(void *parameter)
{
    K1_OnceFlag=0;
    K1_Long_Sem_Counter=0;
    OpenLcdDisplay();
    ScreenTimerRefresh();
    LOG_D("Down Key is Long Free\r\n");
}
void K2_LongFree_Release(void *parameter)
{
    K2_OnceFlag=0;
    K2_Long_Sem_Counter=0;
    OpenLcdDisplay();
    ScreenTimerRefresh();
    LOG_D("Enter Key is Long Free\r\n");
}
extern uint8_t Deltapress_Enable;
extern uint32_t Counter_Deltapress;
void Delta_Release(void *parameter)
{
    LOG_D("Detected Water Flow\r\n");
    if(Deltapress_Enable)
    {
        if(Get_MotoValid())
        {
            Flash_Set(10,++Counter_Deltapress);
            wifi_cod_update();
            JumptoDelta();
        }
    }
}
uint8_t Read_K0_Level(void)
{
  return rt_pin_read(K0);
}
uint8_t Read_K1_Level(void)
{
  return rt_pin_read(K1);
}
uint8_t Read_K2_Level(void)
{
  return rt_pin_read(K2);
}
uint8_t Read_Delta_Level(void)
{
  return rt_pin_read(WATER_FLOW);
}
void Key_Init(void)
{
    rt_pin_mode(K0, PIN_MODE_INPUT);
    rt_pin_mode(K1, PIN_MODE_INPUT);
    rt_pin_mode(K2, PIN_MODE_INPUT);
    rt_pin_mode(MOTO_LEFT, PIN_MODE_INPUT);
    rt_pin_mode(MOTO_RIGHT, PIN_MODE_INPUT);
    rt_pin_mode(WATER_FLOW, PIN_MODE_INPUT);
}
void button_task_entry(void *parameter)
{
    Key_Sem_init();
    Key_Init();
    Button_t Key0;
    Button_t Key1;
    Button_t Key2;
    Button_t Delta;
    Button_Create("Key0",&Key0,Read_K0_Level,0);
    Button_Create("Key1",&Key1,Read_K1_Level,0);
    Button_Create("Key2",&Key2,Read_K2_Level,0);
    Button_Create("Delta",&Delta,Read_Delta_Level,0);
    Button_Attach(&Key0,BUTTON_DOWM,K0_Sem_Release);
    Button_Attach(&Key0,BUTTON_LONG,K0_LongSem_Release);
    Button_Attach(&Key0,BUTTON_LONG_FREE,K0_LongFree_Release);
    Button_Attach(&Key1,BUTTON_DOWM,K1_Sem_Release);
    Button_Attach(&Key1,BUTTON_LONG,K1_LongSem_Release);
    Button_Attach(&Key1,BUTTON_LONG_FREE,K1_LongFree_Release);
    Button_Attach(&Key2,BUTTON_DOWM,K2_Sem_Release);
    Button_Attach(&Key2,BUTTON_LONG,K2_LongSem_Release);
    Button_Attach(&Key2,BUTTON_LONG_FREE,K2_LongFree_Release);
    Button_Attach(&Delta,BUTTON_DOWM,Delta_Release);
    while(1)
    {
        Button_Process();
        rt_thread_mdelay(10);
    }
}
void Button_Init(void)
{
    button_task=rt_thread_create("button_task",button_task_entry,RT_NULL,2048,5,20);
    if(button_task!=RT_NULL)rt_thread_startup(button_task);
}
