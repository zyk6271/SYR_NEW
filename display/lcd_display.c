#include "rtthread.h"
#include <string.h>
#include "lkdGui.h"
#include "12864.h"
#include "lcd_display.h"
#include "Flashwork.h"
#include "Adcwork.h"
#include "led.h"
#include "tds.h"
#include "pin_config.h"
#include "delta.h"

#define DBG_TAG "Display"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

rt_thread_t lcd_task=RT_NULL;
rt_sem_t lcd_refresh_sem = RT_NULL;

volatile volatile const char *Manual="Manual";
volatile const char *Reminder="Reminder";
volatile const char *Automatic="Automatic";
volatile const char *Delta1="Delta Press";
volatile const char *Delta2="Delta Press(M)";
volatile const char *Delta3="Delta Press(A)";
volatile const char *Info="Info";
volatile const char *Down_arrow="}";
volatile const char *Back="Back             ";
volatile const char *SingleLeftBack="Back";
volatile const char *SingleYes="              Yes";
volatile const char *SingleSelect="                 ";
volatile const char *SingleRightBack="             Back";
volatile const char *LowSelect="<BAT LOW>        ";
volatile const char *YesOrNo="No            Yes";
volatile const char *Delta_Pressure="Delta Pressure";
volatile const char *Water_Hardness="Water Hardness";
volatile const char *Backwash_Time="Backwash Time";
volatile const char *Version="Version";
volatile const char *Factory="Factory";
volatile const char *Language="Language";
volatile const char *Update_FW="Update FW";
volatile const char *Smart_Link="Smart Link";
volatile const char *TDS_MODE="TDS Value";
volatile const char *AUTO_RANGE="Automatic Range";
volatile const char *TDS_CND="TDS CND";
volatile const char *Conductivity="Conductivity";
volatile const char *Password="Password";

static void UserMain1WinFun(void *param);
static void UserMain2WinFun(void *param);
static void UserMain3WinFun(void *param);
static void UserMain4WinFun(void *param);
static void UserMain5WinFun(void *param);
static void UserMain6WinFun(void *param);
static void UserMain7WinFun(void *param);
static void UserMain8WinFun(void *param);
static void UserMain9WinFun(void *param);
static void UserMain10WinFun(void *param);
static void UserMain11WinFun(void *param);
static void UserMain12WinFun(void *param);
static void UserMain13WinFun(void *param);
static void UserMain14WinFun(void *param);
static void UserMain15WinFun(void *param);
static void UserMain16WinFun(void *param);
static void UserMain17WinFun(void *param);
static void UserMain18WinFun(void *param);
static void UserMain19WinFun(void *param);
static void UserMain20WinFun(void *param);
static void UserMain21WinFun(void *param);
static void UserMain22WinFun(void *param);
static void UserMain23WinFun(void *param);
static void UserMain24WinFun(void *param);
static void UserMain25WinFun(void *param);
static void UserMain26WinFun(void *param);
static void UserMain27WinFun(void *param);
static void UserMain28WinFun(void *param);
static void UserMain29WinFun(void *param);

uint8_t Reminder_Week=0;
uint8_t Reminder_Day=0;
uint8_t Reminder_Enable=0;
uint8_t Automatic_Week=0;
uint8_t Automatic_Day=0;
uint8_t Automatic_Range=0;
uint8_t Automatic_Enable=0;
uint8_t Deltapress_Enable=0;
uint32_t Counter_Manual=0;
uint32_t Counter_Automatic=0;
uint32_t Counter_Deltapress=0;
uint32_t Counter_Error=0;
uint8_t Setting_Deltapress=0; //0:Report only  1:Automatic
uint32_t Setting_Hardness=0;
uint16_t Setting_Backwashtime=0;
uint8_t Setting_Language=0;
uint8_t TDS_CND_Value = 10;

void GotValue(void)
{
    Reminder_Week = Flash_Get(1);
    Reminder_Day = Flash_Get(2);
    Reminder_Enable = Flash_Get(3);
    Automatic_Week = Flash_Get(4);
    Automatic_Day = Flash_Get(5);
    Automatic_Enable = Flash_Get(6);
    Deltapress_Enable = Flash_Get(7);
    Counter_Manual = Flash_Get(8);
    Counter_Automatic = Flash_Get(9);
    Counter_Deltapress = Flash_Get(10);
    Counter_Error = Flash_Get(11);
    Setting_Deltapress = Flash_Get(12);
    Setting_Hardness = Flash_Get(13);
    if(Setting_Hardness==0||Setting_Backwashtime>10000)
    {
        Setting_Hardness = 100;
        Flash_Set(13,Setting_Hardness);
    }
    Setting_Backwashtime = Flash_Get(14);
    if(Setting_Backwashtime==0||Setting_Backwashtime>120)
    {
        Setting_Backwashtime = 30;
        Flash_Set(14,Setting_Backwashtime);
    }
    Setting_Language = Flash_Get(15);
    TDS_CND_Value = Flash_Get(21);
    if(TDS_CND_Value==0)
    {
        TDS_CND_Value = 10;
        Flash_Set(21,TDS_CND_Value);
    }
    Automatic_Range = Flash_Get(22);
}

extern rt_sem_t K0_Sem;
extern rt_sem_t K1_Sem;
extern rt_sem_t K2_Sem;
extern rt_sem_t K2_Long_Sem;

extern uint32_t TDS_Value;

static rt_err_t K0_Status;
static rt_err_t K1_Status;
static rt_err_t K2_Status;
static rt_err_t K2_Long_Status;

static uint8_t Jump_Flag=1;

lkdButton tButton[20];
lkdScroll tScroll[2];

char NowButtonId=0;
char NowSetting=0;
char FirstFlag[30]={0};
extern uint8_t LCD_Flag;
extern void defaultFontInit(void);

//以下为窗口参数配置
const lkdWin userMain1Win = {
    .x = 0,
    .y = 0,
    .wide = 128,
    .hight = 64,
    //.title = "Main Menu",
    .param = NULL,
    .WindowFunction = UserMain1WinFun,
        .firstflag = 0,
};
const lkdWin userMain2Win = {
    .x = 0,
    .y = 0,
    .wide = 128,
    .hight = 64,
    .title = "Manual",
    .param = NULL,
    .WindowFunction = UserMain2WinFun,
        .firstflag = 0,
};
const lkdWin userMain3Win = {
    .x = 0,
    .y = 0,
    .wide = 128,
    .hight = 64,
    //.title = "Main Window",
    .param = NULL,
    .WindowFunction = UserMain3WinFun,
        .firstflag = 0,
};
const lkdWin userMain4Win = {
    .x = 0,
    .y = 0,
    .wide = 128,
    .hight = 64,
    .title = "Reminder",
    .param = NULL,
    .WindowFunction = UserMain4WinFun,
    .firstflag = 0,
};
const lkdWin userMain5Win = {
    .x = 0,
    .y = 0,
    .wide = 128,
    .hight = 64,
    .title = "Automatic",
    .param = NULL,
    .WindowFunction = UserMain5WinFun,
    .firstflag = 0,
};
const lkdWin userMain6Win = {
    .x = 0,
    .y = 0,
    .wide = 128,
    .hight = 64,
    .title = "Delta Pressure",
    .param = NULL,
    .WindowFunction = UserMain6WinFun,
        .firstflag = 0,
};
const lkdWin userMain7Win = {
    .x = 0,
    .y = 0,
    .wide = 128,
    .hight = 64,
    .title = "Info",
    .param = NULL,
    .WindowFunction = UserMain7WinFun,
        .firstflag = 0,
};
const lkdWin userMain8Win = {
    .x = 0,
    .y = 0,
    .wide = 128,
    .hight = 64,
    .title = "Backwash Counter",
    .param = NULL,
    .WindowFunction = UserMain8WinFun,
        .firstflag = 0,
};
const lkdWin userMain9Win = {
    .x = 0,
    .y = 0,
    .wide = 128,
    .hight = 64,
    .title = "Backwash Counter",
    .param = NULL,
    .WindowFunction = UserMain9WinFun,
        .firstflag = 0,
};
const lkdWin userMain10Win = {
    .x = 0,
    .y = 0,
    .wide = 128,
    .hight = 64,
    .title = "Backwash Counter",
    .param = NULL,
    .WindowFunction = UserMain10WinFun,
        .firstflag = 0,
};
const lkdWin userMain11Win = {
    .x = 0,
    .y = 0,
    .wide = 128,
    .hight = 64,
    .title = "Backwash Counter",
    .param = NULL,
    .WindowFunction = UserMain11WinFun,
    .firstflag = 0,
};
const lkdWin userMain12Win = {
    .x = 0,
    .y = 0,
    .wide = 128,
    .hight = 64,
    .title = "Backwash Counter",
    .param = NULL,
    .WindowFunction = UserMain12WinFun,
        .firstflag = 0,
};
const lkdWin userMain13Win = {
    .x = 0,
    .y = 0,
    .wide = 128,
    .hight = 64,
    .title = "Info",
    .param = NULL,
    .WindowFunction = UserMain13WinFun,
        .firstflag = 0,
};
const lkdWin userMain14Win = {
    .x = 0,
    .y = 0,
    .wide = 128,
    .hight = 64,
    .title = "Settings",
    .param = NULL,
    .WindowFunction = UserMain14WinFun,
        .firstflag = 0,
};
const lkdWin userMain15Win = {
    .x = 0,
    .y = 0,
    .wide = 128,
    .hight = 64,
    .title = "Delta Set",
    .param = NULL,
    .WindowFunction = UserMain15WinFun,
        .firstflag = 0,
};
const lkdWin userMain16Win = {
    .x = 0,
    .y = 0,
    .wide = 128,
    .hight = 64,
    .title = "Conductivity Set",
    .param = NULL,
    .WindowFunction = UserMain16WinFun,
        .firstflag = 0,
};
const lkdWin userMain17Win = {
    .x = 0,
    .y = 0,
    .wide = 128,
    .hight = 64,
    .title = "Backwash Time",
    .param = NULL,
    .WindowFunction = UserMain17WinFun,
        .firstflag = 0,
};
const lkdWin userMain18Win = {
    .x = 0,
    .y = 0,
    .wide = 128,
    .hight = 64,
    .title = "Version",
    .param = NULL,
    .WindowFunction = UserMain18WinFun,
        .firstflag = 0,
};
const lkdWin userMain19Win = {
    .x = 0,
    .y = 0,
    .wide = 128,
    .hight = 64,
    .title = "Factory",
    .param = NULL,
    .WindowFunction = UserMain19WinFun,
        .firstflag = 0,
};
const lkdWin userMain20Win = {
    .x = 0,
    .y = 0,
    .wide = 128,
    .hight = 64,
    .title = "Language",
    .param = NULL,
    .WindowFunction = UserMain20WinFun,
        .firstflag = 0,
};
const lkdWin userMain21Win = {
    .x = 0,
    .y = 0,
    .wide = 128,
    .hight = 64,
    .title = "Password",
    .param = NULL,
    .WindowFunction = UserMain21WinFun,
        .firstflag = 0,
};
const lkdWin userMain22Win = {
    .x = 0,
    .y = 0,
    .wide = 128,
    .hight = 64,
    .title = "TDS Calibration",
    .param = NULL,
    .WindowFunction = UserMain22WinFun,
        .firstflag = 0,
};
const lkdWin userMain23Win = {
    .x = 0,
    .y = 0,
    .wide = 128,
    .hight = 64,
    .title = "Cuttent Value",
    .param = NULL,
    .WindowFunction = UserMain23WinFun,
        .firstflag = 0,
};
const lkdWin userMain24Win = {
    .x = 0,
    .y = 0,
    .wide = 128,
    .hight = 64,
    .title = "Calibration Value",
    .param = NULL,
    .WindowFunction = UserMain24WinFun,
        .firstflag = 0,
};
const lkdWin userMain25Win = {
    .x = 0,
    .y = 0,
    .wide = 128,
    .hight = 64,
    //.title = "Calibration Value",
    .param = NULL,
    .WindowFunction = UserMain25WinFun,
        .firstflag = 0,
};
const lkdWin userMain26Win = {
    .x = 0,
    .y = 0,
    .wide = 128,
    .hight = 64,
    .title = "Battery Empty",
    .param = NULL,
    .WindowFunction = UserMain26WinFun,
        .firstflag = 0,
};
const lkdWin userMain27Win = {
    .x = 0,
    .y = 0,
    .wide = 128,
    .hight = 64,
    .title = "Not New Battery",
    .param = NULL,
    .WindowFunction = UserMain27WinFun,
        .firstflag = 0,
};
const lkdWin userMain28Win = {
    .x = 0,
    .y = 0,
    .wide = 128,
    .hight = 64,
    .title = "Auto Range",
    .param = NULL,
    .WindowFunction = UserMain28WinFun,
    .firstflag = 0,
};
const lkdWin userMain29Win = {
    .x = 0,
    .y = 0,
    .wide = 128,
    .hight = 64,
    .title = "TDS_CND Value",
    .param = NULL,
    .WindowFunction = UserMain29WinFun,
        .firstflag = 0,
};
void Lcd_Event_Init(void)
{
    rt_event_init(&lcd_jump_event, "lcd_jump_event", RT_IPC_FLAG_FIFO);
}
//以下为显示启动以及测试部分
void userAppPortInit(void)
{
    /* 1. 初始化lcd */
    LcdInit();
    /* 2. 初始化用户字体 */
    defaultFontInit();

    /* 3. 初始化窗口,添加窗口 */
    GuiWinInit();
    GuiWinAdd(&userMain1Win);
}
void userAppPortRun(void)
{
    /* 窗口调度管理 */
    GuiWinDisplay();
}

void Jump_TDS(void)
{
    rt_event_send(&lcd_jump_event, TDS);
}
MSH_CMD_EXPORT(Jump_TDS,Jump_TDS);
void Jump_STALLING(void)
{
    Counter_Error++;
    Flash_Set(11,Counter_Error);
    rt_event_send(&lcd_jump_event, STALLING);
}
MSH_CMD_EXPORT(Jump_STALLING,Jump_STALLING);
void Jump_FINISH(void)
{
    rt_event_send(&lcd_jump_event, FINISH);
}
MSH_CMD_EXPORT(Jump_FINISH,Jump_FINISH);
void Jump_NOMOTO(void)
{
    Counter_Error++;
    Flash_Set(11,Counter_Error);
    rt_event_send(&lcd_jump_event, NOMOTO);
}
MSH_CMD_EXPORT(Jump_NOMOTO,Jump_NOMOTO);
void Jump_EXIT(void)
{
    rt_event_send(&lcd_jump_event, EXIT);
}
MSH_CMD_EXPORT(Jump_EXIT,Jump_EXIT);

void JumptoReminder(void)
{
    LcdtoReminder();
    OpenLcdDisplay();
    ScreenTimerRefresh();
}
MSH_CMD_EXPORT(JumptoReminder,JumptoReminder);
void JumptoAutomatic(void)
{
    Counter_Automatic++;
    Flash_Set(9,Counter_Automatic);
    LcdtoBackwash();
    OpenLcdDisplayNoBL();
}
MSH_CMD_EXPORT(JumptoAutomatic,JumptoAutomatic);
void JumpToBatteryEmpty(void)
{
    GuiClearScreen(0);
    GuiWinAdd(&userMain26Win);
    memset(FirstFlag,0,30);
    OpenLcdDisplay();
}
MSH_CMD_EXPORT(JumpToBatteryEmpty,JumpToBatteryEmpty);
void JumpToBatteryNew(void)
{
    GuiClearScreen(0);
    GuiWinAdd(&userMain27Win);
    memset(FirstFlag,0,30);
    OpenLcdDisplay();
}
MSH_CMD_EXPORT(JumpToBatteryNew,JumpToBatteryNew);
void JumptoMainWin(void)
{
    GuiClearScreen(0);
    GuiWinInit();
    GuiWinAdd(&userMain1Win);
    memset(FirstFlag,0,30);
}
void LcdtoReminder(void)
{
    if(FirstFlag[25]==0)
    {
        memset(FirstFlag,0,27);
        GuiClearScreen(0);
        GuiWinAdd(&userMain25Win);
    }
    else
    {
        FirstFlag[25]=0;
    }
}
void LcdtoBackwash(void)
{
    if(FirstFlag[3]==0)
    {
        memset(FirstFlag,0,27);
        GuiClearScreen(0);
        GuiWinAdd(&userMain3Win);
    }
    else
    {
        FirstFlag[3]=0;
    }
    Moto_Cycle();
}
void lcd_task_entry(void *parameter)
{
    GotValue();
    Lcd_Event_Init();
    userAppPortInit();
    LCD_Flag = 0;
    LOG_D("LCD Init Success\r\n");
    while(1)
    {
        GuiWinDisplay();
        if(rt_sem_take(lcd_refresh_sem, 0)==RT_EOK)
        {
            LOG_I("Lcd Refresh From Lowpower\r\n");
            rt_thread_mdelay(300);
            GuiUpdateDisplayAll();
            rt_thread_mdelay(300);
            GuiUpdateDisplayAll();
        }
        rt_thread_mdelay(10);
    }
}
void LCD_Init(void)
{
    lcd_refresh_sem = rt_sem_create("lcd_refresh", 0, RT_IPC_FLAG_FIFO);
    lcd_task=rt_thread_create("lcd_task",lcd_task_entry,RT_NULL,4096,15,20);
    if(lcd_task!=RT_NULL)rt_thread_startup(lcd_task);
    rt_pin_write(LCD_BL,1);
}
void LCD_Refresh(void)
{
    rt_sem_release(lcd_refresh_sem);
}

//以下为显示窗口部分
uint8_t Win1PageID=0;
extern uint8_t LowVoltageFlag;
static void UserMain1WinFun(void *param)
{
     if(FirstFlag[1] == 0)//绘图等第一次进入窗口的相关执行动作
    {
        switch(Win1PageID)
        {
            case 0:
                FirstFlag[1] = 1;
                NowButtonId=0;
                tScroll[0].max = 5;
                tScroll[0].x = 119;
                tScroll[0].y = 2;
                tScroll[0].hight = 48;
                tScroll[0].lump = 0;/* 进度快控制 */
                GuiVScroll(&tScroll[0]);/* 垂直进度条 */

                tButton[0].x = 0;
                tButton[0].y = 0;
                tButton[0].wide = 117;
                tButton[0].high = 15;
                tButton[0].name = Manual;
                tButton[0].linesize = 0;
                tButton[0].flag = 1;/* 抬起状态 */
                GuiButton(&tButton[0]);

                tButton[1].x = 0;
                tButton[1].y = 11;
                tButton[1].wide = 117;
                tButton[1].high = 15;
                tButton[1].name = Reminder;
                tButton[1].linesize = 60-!Reminder_Enable*60;
                tButton[1].flag = 0;/* 抬起状态 */
                GuiButton(&tButton[1]);

                tButton[2].x = 0;
                tButton[2].y = 24;
                tButton[2].wide = 117;
                tButton[2].high = 15;
                tButton[2].name = Automatic;
                tButton[2].linesize = 70-!Automatic_Enable*70;
                tButton[2].flag = 0;/* 抬起状态 */
                GuiButton(&tButton[2]);

                tButton[3].x = 0;
                tButton[3].y = 36;
                tButton[3].high = 15;
                tButton[3].wide = 117;
                tButton[3].name = Delta_Pressure;
                tButton[3].linesize = 70-!Deltapress_Enable*70;
                if(Deltapress_Enable)
                {
                    if(Setting_Deltapress)
                    {
                        tButton[3].wide = 110;
                        tButton[3].linesize = 100-!Deltapress_Enable*100;
                        tButton[3].name = Delta3;
                    }
                    else
                    {
                        tButton[3].wide = 110;
                        tButton[3].linesize = 100-!Deltapress_Enable*100;
                        tButton[3].name = Delta2;
                    }
                }
                else
                {
                    tButton[3].linesize = 0;
                    tButton[3].wide = 117;
                    tButton[3].name = Delta1;
                }
                tButton[3].flag = 0;/* 抬起状态 */
                GuiButton(&tButton[3]);

                tButton[4].x = 0;
                tButton[4].y = 51;
                tButton[4].wide = 128;
                tButton[4].high = 15;
                if(LowVoltageFlag==1)
                {
                    tButton[4].name = LowSelect;
                }
                else if(LowVoltageFlag==0)
                {
                    tButton[4].name = SingleSelect;
                }
                tButton[4].flag = 1;/* 按下状态 */
                GuiButton(&tButton[4]);
                GuiUpdateDisplayAll();
                break;
            case 1:
                FirstFlag[1] = 1;
                GuiClearScreen(0);
                tScroll[0].max = 5;
                tScroll[0].x = 119;
                tScroll[0].y = 2;
                tScroll[0].hight = 48;
                tScroll[0].lump = NowButtonId;/* 进度快控制 */
                GuiVScroll(&tScroll[0]);/* 垂直进度条 */

                tButton[0].x = 0;
                tButton[0].y = 0;
                tButton[0].wide = 117;
                tButton[0].high = 15;
                tButton[0].name = Info;
                tButton[0].linesize = 0;
                tButton[0].flag = 1;/* 抬起状态 */
                GuiButton(&tButton[0]);

                tButton[4].x = 0;
                tButton[4].y = 51;
                tButton[4].wide = 128;
                tButton[4].high = 15;
                if(LowVoltageFlag==1)
                {
                    tButton[4].name = LowSelect;
                }
                else if(LowVoltageFlag==0)
                {
                    tButton[4].name = SingleSelect;
                }
                tButton[4].flag = 1;/* 按下状态 */
                GuiButton(&tButton[4]);

                GuiUpdateDisplayAll();
                break;
            case 2:
                NowButtonId=3;
                FirstFlag[1] = 1;
                GuiClearScreen(0);
                tScroll[0].max = 5;
                tScroll[0].x = 119;
                tScroll[0].y = 2;
                tScroll[0].hight = 48;
                tScroll[0].lump = NowButtonId;/* 进度快控制 */
                GuiVScroll(&tScroll[0]);/* 垂直进度条 */

                tButton[0].x = 0;
                tButton[0].y = 0;
                tButton[0].wide = 117;
                tButton[0].high = 15;
                tButton[0].name = Manual;
                tButton[0].linesize = 0;
                tButton[0].flag = 0;/* 抬起状态 */
                GuiButton(&tButton[0]);

                tButton[1].x = 0;
                tButton[1].y = 11;
                tButton[1].wide = 117;
                tButton[1].high = 15;
                tButton[1].name = Reminder;
                tButton[1].linesize = 60-!Reminder_Enable*60;
                tButton[1].flag = 0;/* 抬起状态 */
                GuiButton(&tButton[1]);

                tButton[2].x = 0;
                tButton[2].y = 24;
                tButton[2].wide = 117;
                tButton[2].high = 15;
                tButton[2].name = Automatic;
                tButton[2].linesize = 70-!Automatic_Enable*70;
                tButton[2].flag = 0;/* 抬起状态 */
                GuiButton(&tButton[2]);

                tButton[3].x = 0;
                tButton[3].y = 36;
                tButton[3].high = 15;
                tButton[3].wide = 117;
                tButton[3].name = Delta_Pressure;
                tButton[3].linesize = 70-!Deltapress_Enable*70;
                if(Deltapress_Enable)
                {
                    if(Setting_Deltapress)
                    {
                        tButton[3].wide = 110;
                        tButton[3].linesize = 100-!Deltapress_Enable*100;
                        tButton[3].name = Delta3;
                    }
                    else
                    {
                        tButton[3].wide = 110;
                        tButton[3].linesize = 100-!Deltapress_Enable*100;
                        tButton[3].name = Delta2;
                    }
                }
                else
                {
                    tButton[3].linesize = 0;
                    tButton[3].wide = 117;
                    tButton[3].name = Delta1;
                }
                tButton[3].flag = 1;/* 抬起状态 */
                GuiButton(&tButton[3]);

                tButton[4].x = 0;
                tButton[4].y = 51;
                tButton[4].wide = 128;
                tButton[4].high = 15;
                if(LowVoltageFlag==1)
                {
                    tButton[4].name = LowSelect;
                }
                else if(LowVoltageFlag==0)
                {
                    tButton[4].name = SingleSelect;
                }
                tButton[4].flag = 1;/* 按下状态 */
                GuiButton(&tButton[4]);
                GuiUpdateDisplayAll();
                break;
            }

     }
     else
    {
        if(LowVoltageFlag==1)
        {
         tButton[4].name = LowSelect;
        }
        else if(LowVoltageFlag==0)
        {
         tButton[4].name = SingleSelect;
        }
        GuiButton(&tButton[4]);
        K0_Status = rt_sem_take(K0_Sem, 0);
        K1_Status = rt_sem_take(K1_Sem, 0);
        K2_Status = rt_sem_take(K2_Sem, 0);
        K2_Long_Status = rt_sem_take(K2_Long_Sem, 0);
        if(K2_Long_Status==RT_EOK)
        {
            FirstFlag[1]=0;
            NowButtonId = 0;
            Win1PageID = 0;
            GuiClearScreen(0);
            GuiWinAdd(&userMain14Win);
        }
        if(K0_Status==RT_EOK)
        {
            switch(NowButtonId)
            {
                case 0:
                    NowButtonId=4;
                    Win1PageID = 1;
                    FirstFlag[1]  = 0;
                    break;
                case 4:
                    NowButtonId--;
                    Win1PageID = 2;
                    FirstFlag[1]  = 0;
                    break;
                default:
                    tButton[NowButtonId].flag=0;
                    GuiButton(&tButton[NowButtonId]);
                    if(NowButtonId>=1)NowButtonId--;
                    else NowButtonId = 4;
                    tButton[NowButtonId].flag=1;
                    GuiButton(&tButton[NowButtonId]);
                    break;
            }
                tScroll[0].lump = NowButtonId;/* 进度快控制 */
                GuiVScroll(&tScroll[0]);/* 垂直进度条 */
                GuiUpdateDisplayAll();
        }
        if(K1_Status==RT_EOK)
        {
            switch(NowButtonId)
            {
                case 3:
                    Win1PageID = 1;
                    FirstFlag[1]  = 0;
                    NowButtonId++;
                    break;
                case 4:
                    NowButtonId++;
                    Win1PageID = 0;
                    FirstFlag[1]  = 0;
                    break;
                default:
                    tButton[NowButtonId].flag=0;
                    GuiButton(&tButton[NowButtonId]);
                    NowButtonId++;
                    tButton[NowButtonId].flag=1;
                    GuiButton(&tButton[NowButtonId]);
                    break;
            }
                tScroll[0].lump = NowButtonId;/* 进度快控制 */
                GuiVScroll(&tScroll[0]);/* 垂直进度条 */
                GuiUpdateDisplayAll();
        }
        if(K2_Status==RT_EOK)
        {
            Win1PageID = 0;
            GuiClearScreen(0);
            switch(NowButtonId)
            {
                case 0:GuiWinAdd(&userMain2Win);break;//Manual
                case 1:GuiWinAdd(&userMain4Win);break;//Reminder
                case 2:GuiWinAdd(&userMain5Win);break;//Automatic
                case 3:GuiWinAdd(&userMain6Win);break;//Delta Press
                case 4:GuiWinAdd(&userMain7Win);break;//Info
            }
            FirstFlag[1]=0;
        }
    }
}
static void UserMain2WinFun(void *param)
{
     if(FirstFlag[2] == 0)
     {
            FirstFlag[2] = 1;
            NowButtonId=0;

            tButton[0].x = 20;
            tButton[0].y = 15;
            tButton[0].wide = 90;
            tButton[0].high = 15;
            tButton[0].name = "Backwash now";
            tButton[0].linesize = 0;
            tButton[0].flag = 1;/* 按下状态 */
            GuiButton(&tButton[0]);

            tButton[1].x = 45;
            tButton[1].y = 30;
            tButton[1].wide = 36;
            tButton[1].high = 15;
            tButton[1].name = "Exit";
            tButton[1].linesize = 0;
            tButton[1].flag = 0;/* 按下状态 */
            GuiButton(&tButton[1]);

            tButton[2].x = 0;
            tButton[2].y = 50;
            tButton[2].wide = 128;
            tButton[2].high = 15;
            tButton[2].name = SingleSelect;
            tButton[2].linesize = 0;
            tButton[2].flag = 1;/* 按下状态 */
            GuiButton(&tButton[2]);
            GuiUpdateDisplayAll();
     }
         else
         {
                K0_Status = rt_sem_take(K0_Sem, 0);
                K1_Status = rt_sem_take(K1_Sem, 0);
                K2_Status = rt_sem_take(K2_Sem, 0);
                K2_Long_Status = rt_sem_take(K2_Long_Sem, 0);
                if(K2_Long_Status==RT_EOK)
                {

                }
                if(K0_Status==RT_EOK)
                {
                    switch(NowButtonId)
                    {
                          case 0:
                              NowButtonId=1;
                              tButton[0].flag=0;
                              tButton[1].flag=1;
                              GuiButton(&tButton[0]);
                              GuiButton(&tButton[1]);
                              break;
                          case 1:
                              NowButtonId=0;
                              tButton[0].flag=1;
                              tButton[1].flag=0;
                              GuiButton(&tButton[0]);
                              GuiButton(&tButton[1]);
                              break;
                    }
                    GuiUpdateDisplayAll();
                }
                if(K1_Status==RT_EOK)
                {
                    switch(NowButtonId)
                    {
                      case 0:
                          NowButtonId=1;
                          tButton[0].flag=0;
                          tButton[1].flag=1;
                          GuiButton(&tButton[0]);
                          GuiButton(&tButton[1]);
                          break;
                      case 1:
                          NowButtonId=0;
                          tButton[0].flag=1;
                          tButton[1].flag=0;
                          GuiButton(&tButton[0]);
                          GuiButton(&tButton[1]);
                          break;
                    }
                    GuiUpdateDisplayAll();
                }
                if(K2_Status==RT_EOK)
                {
                   if(NowButtonId==0)
                   {
                        FirstFlag[2]=0;
                        Counter_Manual++;
                        Flash_Set(8,Counter_Manual);
                        Moto_Cycle();
                        GuiClearScreen(0);
                        GuiWinAdd(&userMain3Win);
                   }
                   else
                   {
                       GuiClearScreen(0);
                       GuiWinDeleteTop();
                       FirstFlag[2]=0;
                   }
                }
            }
}
rt_timer_t SemJump_Timer=RT_NULL;
void K2_Setjump_Sem_Release(void *parameter)
{
         rt_sem_release(K2_Sem);
}
void SemJump (void)
{
    if(SemJump_Timer == RT_NULL)
    {
        SemJump_Timer = rt_timer_create("SemJump_Timer",K2_Setjump_Sem_Release,RT_NULL,4000,RT_TIMER_FLAG_ONE_SHOT|RT_TIMER_FLAG_SOFT_TIMER);
    }
    rt_timer_start(SemJump_Timer);
}
static void UserMain3WinFun(void *param)
{
    rt_uint32_t e;
    if(FirstFlag[3] == 0)
    {
        FirstFlag[3] = 1;
        Jump_Flag=0;
        GuiClearScreen(0);
        GuiRowText(35,15,57,0,"Backwash");
        GuiRowText(25,27,80,0,"In progress");
        GuiRowText(25,39,85,0,"-->-->-->-->");
        GuiRowText(106,56,30,0,"   ");
        GuiUpdateDisplayAll();
    }
     else
     {
        if(rt_event_recv(&lcd_jump_event, (TDS|STALLING|FINISH|NOMOTO|EXIT),RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,RT_WAITING_NO, &e) == RT_EOK)
        {
            switch(e)
            {
            case TDS:
                GuiClearScreen(0);
                GuiRowText(0,1,125,0,"Increased water");
                GuiRowText(0,13,125,0,"hardness");
                GuiRowText(0,25,127,0,"The use of a water");
                GuiRowText(0,37,125,0,"soft system");
                GuiRowText(0,49,125,0,"is recommended");
                GuiRowText(106,56,30,0,"Yes");
                led_select(1);
                Jump_Flag=1;
                break;
            case STALLING:
                GuiClearScreen(0);
                GuiRowText(50,15,127,0,"Error");
                GuiRowText(15,30,127,0,"Motor Stalling");
                GuiRowText(106,56,30,0,"Yes");
                led_select(1);
                Jump_Flag=1;
                break;
            case FINISH:
                GuiClearScreen(0);
                GuiRowText(35,15,57,0,"Backwash");
                GuiRowText(40,27,70,0,"Finish");
                GuiRowText(106,56,30,0,"Yes");
                Jump_Flag=1;
                SemJump();
                break;
            case NOMOTO:
                GuiClearScreen(0);
                GuiRowText(50,15,127,0,"Error");
                GuiRowText(40,30,127,0,"No Motor");
                GuiRowText(106,56,30,0,"Yes");
                led_select(1);
                Jump_Flag=1;
                break;
            case EXIT:
                FirstFlag[3]=0;
                GuiClearScreen(0);
                GuiWinInit();
                GuiWinAdd(&userMain1Win);
                led_select(0);
                Jump_Flag=1;
                break;
            }
            GuiUpdateDisplayAll();
        }
        K0_Status = rt_sem_take(K0_Sem, 0);
        K1_Status = rt_sem_take(K1_Sem, 0);
        K2_Status = rt_sem_take(K2_Sem, 0);
        K2_Long_Status = rt_sem_take(K2_Long_Sem, 0);
        if(K2_Long_Status==RT_EOK)
        {

        }
        if(K2_Status==RT_EOK&&Jump_Flag==1)
        {
            FirstFlag[3]=0;
            led_select(0);
            GuiClearScreen(0);
            GuiWinInit();
            GuiWinAdd(&userMain1Win);
        }
     }
}

uint8_t Reminder_Week_Temp;
uint8_t Reminder_Day_Temp;
uint8_t Reminder_Enable_Temp;
extern uint32_t RTC_Reminder_Time ;
char ReminderWeekstring[10]={0};
char ReminderDaystring[10]={0};
static void UserMain4WinFun(void *param)
{
    if(FirstFlag[4] == 0)
    {
        Reminder_Week_Temp=Reminder_Week;
        Reminder_Day_Temp=Reminder_Day;
        Reminder_Enable_Temp=Reminder_Enable;
        FirstFlag[4] = 1;

        tButton[0].x = 0;
        tButton[0].y = 50;
        tButton[0].wide = 128;
        tButton[0].high = 15;
        tButton[0].name = SingleSelect;
        tButton[0].linesize = 0;
        tButton[0].flag = 1;/* 按下状态 */
        GuiButton(&tButton[0]);

        sprintf(ReminderWeekstring,"Weeks:%02d",Reminder_Week);
        tButton[1].x = 33;
        tButton[1].y = 12;
        tButton[1].wide = 60;
        tButton[1].high = 15;
        tButton[1].name = ReminderWeekstring;
        tButton[1].linesize = 0;
        tButton[1].flag = 1;/* 按下状态 */
        GuiButton(&tButton[1]);


        sprintf(ReminderDaystring,"Days:%02d",Reminder_Day);
        tButton[2].x = 35;
        tButton[2].y = 24;
        tButton[2].wide = 55;
        tButton[2].high = 15;
        tButton[2].name = ReminderDaystring;
        tButton[2].linesize = 0;
        tButton[2].flag = 0;/* 按下状态 */
        GuiButton(&tButton[2]);

        tButton[3].x = 33;
        tButton[3].y = 36;
        tButton[3].wide = 60;
        tButton[3].high = 15;
        if(Reminder_Enable_Temp){tButton[3].name="Enabled";}
        else{tButton[3].name="Disabled";}
        tButton[3].linesize = 0;
        tButton[3].flag = 0;/* 按下状态 */
        GuiButton(&tButton[3]);
        GuiUpdateDisplayAll();
    }
    else
    {
        K0_Status = rt_sem_take(K0_Sem, 0);
        K1_Status = rt_sem_take(K1_Sem, 0);
        K2_Status = rt_sem_take(K2_Sem, 0);
        K2_Long_Status = rt_sem_take(K2_Long_Sem, 0);
        if(K2_Long_Status==RT_EOK)
        {

        }
        if(K1_Status==RT_EOK)
        {
            switch(NowSetting)
            {
                case 0:
                    if(Reminder_Week_Temp--<=0)Reminder_Week_Temp=51;
                    sprintf(ReminderWeekstring,"Weeks:%02d",Reminder_Week_Temp);
                    tButton[1].name = ReminderWeekstring;
                    GuiButton(&tButton[1]);
                    break;
                case 1:
                    if(Reminder_Day_Temp--<=0)Reminder_Day_Temp=6;
                    sprintf(ReminderDaystring,"Days:%02d",Reminder_Day_Temp);
                    tButton[2].name = ReminderDaystring;
                    GuiButton(&tButton[2]);
                    break;
                case 2:
                    if(Reminder_Enable_Temp>0)Reminder_Enable_Temp--;else Reminder_Enable_Temp=2;
                    if(Reminder_Enable_Temp==0){tButton[3].name="Disable";GuiButton(&tButton[3]);}
                    else if(Reminder_Enable_Temp==1){tButton[3].name="Enable";GuiButton(&tButton[3]);}
                    else if(Reminder_Enable_Temp==2){tButton[3].name=" Exit";GuiButton(&tButton[3]);}
                    break;
            }
            GuiUpdateDisplayAll();
        }
        if(K0_Status==RT_EOK)
        {
            switch(NowSetting)
            {
                case 0:
                    if(Reminder_Week_Temp++>=51)Reminder_Week_Temp=0;
                    sprintf(ReminderWeekstring,"Weeks:%02d",Reminder_Week_Temp);
                    tButton[1].name = ReminderWeekstring;
                    GuiButton(&tButton[1]);
                    break;
                case 1:
                    if(Reminder_Day_Temp++>=6)Reminder_Day_Temp=0;
                    sprintf(ReminderDaystring,"Days:%02d",Reminder_Day_Temp);
                    tButton[2].name = ReminderDaystring;
                    GuiButton(&tButton[2]);
                    break;
                case 2:
                    if(Reminder_Enable_Temp<2)Reminder_Enable_Temp++;else Reminder_Enable_Temp=0;
                    if(Reminder_Enable_Temp==0){tButton[3].name="Disabled";GuiButton(&tButton[3]);}
                    else if(Reminder_Enable_Temp==1){tButton[3].name="Enabled";GuiButton(&tButton[3]);}
                    else if(Reminder_Enable_Temp==2){tButton[3].name=" Exit";GuiButton(&tButton[3]);}
                    break;
            }
            GuiUpdateDisplayAll();
        }
        if(K2_Status==RT_EOK)
        {
            if(Reminder_Enable_Temp==2)
            {
                NowSetting=0;
                GuiClearScreen(0);
                GuiWinDeleteTop();
                GuiWinDisplay();
                FirstFlag[4]=0;
                LOG_D("Reminder Exit\r\n");
                return;
            }
            switch(NowSetting)
            {
                case 0:
                    tButton[1].flag = 0;/* 抬起状态 */
                    GuiButton(&tButton[1]);
                    tButton[2].flag = 1;/* 按下状态 */
                    GuiButton(&tButton[2]);
                    tButton[3].flag = 0;/* 抬起状态 */
                    GuiButton(&tButton[3]);
                    NowSetting++;
                    break;
                case 1:
                    tButton[1].flag = 0;/* 抬起状态 */
                    GuiButton(&tButton[1]);
                    tButton[2].flag = 0;/* 抬起状态 */
                    GuiButton(&tButton[2]);
                    tButton[3].flag = 1;/* 抬起状态 */
                    GuiButton(&tButton[3]);
                    NowSetting++;
                    break;
                case 2:
                    FirstFlag[4]=0;
                    NowSetting=0;
                    RTC_Reminder_Time=0;
                    LOG_D("RTC_Reminder_Time Change to 0\r\n");
                    if(Reminder_Week_Temp==0&&Reminder_Day_Temp==0)
                    {
                        Reminder_Day_Temp=1;
                    }
                    Reminder_Week=Reminder_Week_Temp;
                    Flash_Set(1,Reminder_Week_Temp);
                    Reminder_Day=Reminder_Day_Temp;
                    Flash_Set(2,Reminder_Day_Temp);
                    Reminder_Enable=Reminder_Enable_Temp;
                    Flash_Set(3,Reminder_Enable_Temp);
                    GuiClearScreen(0);
                    GuiWinDeleteTop();
                    break;
            }
            GuiUpdateDisplayAll();
        }
    }
}
uint8_t Automatic_Week_Temp;
uint8_t Automatic_Day_Temp;
uint8_t Automatic_Enable_Temp;
extern uint32_t RTC_Automatic_Time ;
char AutoWeekstring[10]={0};
char AutoDaystring[10]={0};
static void UserMain5WinFun(void *param)
{
    if(FirstFlag[5] == 0)
    {
        Automatic_Week_Temp=Automatic_Week;
        Automatic_Day_Temp=Automatic_Day;
        Automatic_Enable_Temp=Automatic_Enable;
        FirstFlag[5] = 1;

        tButton[0].x = 0;
        tButton[0].y = 50;
        tButton[0].wide = 128;
        tButton[0].high = 15;
        tButton[0].name = SingleSelect;
        tButton[0].linesize = 0;
        tButton[0].flag = 1;/* 按下状态 */
        GuiButton(&tButton[0]);

        sprintf(AutoWeekstring,"Weeks:%02d",Automatic_Week);
        tButton[1].x = 33;
        tButton[1].y = 12;
        tButton[1].wide = 60;
        tButton[1].high = 15;
        tButton[1].name = AutoWeekstring;
        tButton[1].linesize = 0;
        tButton[1].flag = 1;/* 按下状态 */
        GuiButton(&tButton[1]);


        sprintf(AutoDaystring,"Days:%02d",Automatic_Day);
        tButton[2].x = 35;
        tButton[2].y = 24;
        tButton[2].wide = 55;
        tButton[2].high = 15;
        tButton[2].name = AutoDaystring;
        tButton[2].linesize = 0;
        tButton[2].flag = 0;/* 按下状态 */
        GuiButton(&tButton[2]);

        tButton[3].x = 36;
        tButton[3].y = 36;
        tButton[3].wide = 60;
        tButton[3].high = 15;
        if(Automatic_Enable_Temp){tButton[3].name="Enabled";}
        else{tButton[3].name="Disabled";}
        tButton[3].linesize = 0;
        tButton[3].flag = 0;/* 按下状态 */
        GuiButton(&tButton[3]);
        GuiUpdateDisplayAll();
    }
    else
    {
        K0_Status = rt_sem_take(K0_Sem, 0);
        K1_Status = rt_sem_take(K1_Sem, 0);
        K2_Status = rt_sem_take(K2_Sem, 0);
        K2_Long_Status = rt_sem_take(K2_Long_Sem, 0);
        if(K2_Long_Status==RT_EOK)
        {

        }
        if(K1_Status==RT_EOK)
        {
            switch(NowSetting)
            {
                case 0:
                    if(Automatic_Range)
                    {
                        if(Automatic_Week_Temp--<=9)
                        {
                            Automatic_Week_Temp=52;
                        }
                    }
                    else
                    {
                        if(Automatic_Week_Temp--<=0)
                        {
                            Automatic_Week_Temp=9;
                        }
                    }
                    sprintf(AutoWeekstring,"Weeks:%02d",Automatic_Week_Temp);
                    tButton[1].name = AutoWeekstring;
                    GuiButton(&tButton[1]);
                    break;
                case 1:
                    if(Automatic_Day_Temp--<=0)
                    {
                        Automatic_Day_Temp=6;
                    }
                    sprintf(AutoDaystring,"Days:%02d",Automatic_Day_Temp);
                    tButton[2].name = AutoDaystring;
                    GuiButton(&tButton[2]);
                    break;
                case 2:
                    if(Automatic_Enable_Temp>0)
                    {
                        Automatic_Enable_Temp--;
                    }
                    else
                    {
                        Automatic_Enable_Temp=2;
                    }
                    if(Automatic_Enable_Temp==0){tButton[3].name="Disable";GuiButton(&tButton[3]);}
                    else if(Automatic_Enable_Temp==1){tButton[3].name="Enable";GuiButton(&tButton[3]);}
                    else if(Automatic_Enable_Temp==2){tButton[3].name=" Exit";GuiButton(&tButton[3]);}
                    break;
            }
            GuiUpdateDisplayAll();
        }
        if(K0_Status==RT_EOK)
        {
            switch(NowSetting)
            {
                case 0:
                    if(Automatic_Range)
                    {
                        if(Automatic_Week_Temp++>=52)
                        {
                            Automatic_Week_Temp=9;
                        }
                    }
                    else
                    {
                        if(Automatic_Week_Temp++>=9)
                        {
                            Automatic_Week_Temp=0;
                        }
                    }
                    sprintf(AutoWeekstring,"Weeks:%02d",Automatic_Week_Temp);
                    tButton[1].name = AutoWeekstring;
                    GuiButton(&tButton[1]);
                    break;
                case 1:
                    if(Automatic_Day_Temp++>=6)Automatic_Day_Temp=0;
                    sprintf(AutoDaystring,"Days:%02d",Automatic_Day_Temp);
                    tButton[2].name = AutoDaystring;
                    GuiButton(&tButton[2]);
                    break;
                case 2:
                    if(Automatic_Enable_Temp<2)Automatic_Enable_Temp++;else Automatic_Enable_Temp=0;
                    if(Automatic_Enable_Temp==0){tButton[3].name="Disabled";GuiButton(&tButton[3]);}
                    else if(Automatic_Enable_Temp==1){tButton[3].name="Enabled";GuiButton(&tButton[3]);}
                    else if(Automatic_Enable_Temp==2){tButton[3].name=" Exit";GuiButton(&tButton[3]);}
                    break;
            }
            GuiUpdateDisplayAll();
        }
        if(K2_Status==RT_EOK)
        {
            if(Automatic_Enable_Temp==2)
            {
                NowSetting=0;
                GuiClearScreen(0);
                GuiWinDeleteTop();
                GuiWinDisplay();
                FirstFlag[5]=0;
                LOG_D("Automatic Exit\r\n");
                return;
            }
            switch(NowSetting)
            {
                case 0:
                    tButton[1].flag = 0;/* 抬起状态 */
                    GuiButton(&tButton[1]);
                    tButton[2].flag = 1;/* 按下状态 */
                    GuiButton(&tButton[2]);
                    tButton[3].flag = 0;/* 抬起状态 */
                    GuiButton(&tButton[3]);
                    NowSetting++;
                    break;
                case 1:
                    tButton[1].flag = 0;/* 抬起状态 */
                    GuiButton(&tButton[1]);
                    tButton[2].flag = 0;/* 抬起状态 */
                    GuiButton(&tButton[2]);
                    tButton[3].flag = 1;/* 抬起状态 */
                    GuiButton(&tButton[3]);
                    NowSetting++;
                    break;
                case 2:
                    FirstFlag[5]=0;
                    NowSetting=0;
                    if(Automatic_Week_Temp==0&&Automatic_Day_Temp==0)Automatic_Day_Temp=1;
                    Automatic_Week=Automatic_Week_Temp;
                    Flash_Set(4,Automatic_Week_Temp);
                    Automatic_Day=Automatic_Day_Temp;
                    Flash_Set(5,Automatic_Day_Temp);
                    RTC_Automatic_Time=0;
                    LOG_D("RTC_Automatic_Time Change to 0\r\n");
                    Automatic_Enable=Automatic_Enable_Temp;
                    Flash_Set(6,Automatic_Enable_Temp);
                    GuiClearScreen(0);
                    GuiWinDeleteTop();
                    break;
                }
            GuiUpdateDisplayAll();
        }
    }
}
uint8_t Deltapress_Enable_Temp=0;
static void UserMain6WinFun(void *param)
{
    if(FirstFlag[6] == 0)
    {
        FirstFlag[6] = 1;
        Deltapress_Enable_Temp=Deltapress_Enable;
        GuiRowText(25,15,100,0,"Deltapress is");

        tButton[0].x = 33;
        tButton[0].y = 23;
        tButton[0].wide = 60;
        tButton[0].high = 15;
        tButton[0].name = "Disabled";
        tButton[0].linesize = 0;
        tButton[0].flag = !Deltapress_Enable_Temp;/* 按下状态 */
        GuiButton(&tButton[0]);

        tButton[1].x = 35;
        tButton[1].y = 35;
        tButton[1].wide = 60;
        tButton[1].high = 15;
        tButton[1].name = "Enabled";
        tButton[1].linesize = 0;
        tButton[1].flag = Deltapress_Enable_Temp;/* 抬起状态 */
        GuiButton(&tButton[1]);

        tButton[3].x = 0;
        tButton[3].y = 50;
        tButton[3].wide = 128;
        tButton[3].high = 15;
        tButton[3].name = SingleSelect;
        tButton[3].linesize = 0;
        tButton[3].flag = 1;/* 按下状态 */
        GuiButton(&tButton[3]);
        GuiUpdateDisplayAll();
    }
    else
    {
        K0_Status = rt_sem_take(K0_Sem, 0);
        K1_Status = rt_sem_take(K1_Sem, 0);
        K2_Status = rt_sem_take(K2_Sem, 0);
        K2_Long_Status = rt_sem_take(K2_Long_Sem, 0);
        if(K2_Long_Status==RT_EOK)
        {

        }
        if(K0_Status==RT_EOK)
        {
            Deltapress_Enable_Temp=!Deltapress_Enable_Temp;
            tButton[0].flag = !Deltapress_Enable_Temp;
            tButton[1].flag = Deltapress_Enable_Temp;
            GuiButton(&tButton[0]);
            GuiButton(&tButton[1]);
            GuiUpdateDisplayAll();
        }
        if(K1_Status==RT_EOK)
        {
            Deltapress_Enable_Temp=!Deltapress_Enable_Temp;
            tButton[0].flag = !Deltapress_Enable_Temp;
            tButton[1].flag = Deltapress_Enable_Temp;
            GuiButton(&tButton[0]);
            GuiButton(&tButton[1]);
            GuiUpdateDisplayAll();
        }
        if(K2_Status==RT_EOK)
        {
            FirstFlag[6]=0;
            if(Deltapress_Enable != Deltapress_Enable_Temp)
            {
                Deltapress_Enable=Deltapress_Enable_Temp;
                if(Deltapress_Enable)
                {
                    Delta_Open();
                }
                else
                {
                    Delta_Close();
                }
                Flash_Set(7,Deltapress_Enable_Temp);
            }
            GuiClearScreen(0);
            GuiWinDeleteTop();
        }
    }
}
static void UserMain7WinFun(void *param)
{
    if(FirstFlag[7] == 0)
    {
        FirstFlag[7] = 1;
        NowButtonId=0;

        tScroll[0].max = 4;
        tScroll[0].x = 119;
        tScroll[0].y = 13;
        tScroll[0].hight = 38;
        tScroll[0].lump = 0;/* 进度快控制 */
        GuiVScroll(&tScroll[0]);/* 垂直进度条 */

        tButton[0].x = 0;
        tButton[0].y = 11;
        tButton[0].wide = 117;
        tButton[0].high = 15;
        tButton[0].linesize = 0;
        tButton[0].name = "Backwash Counter";
        tButton[0].flag = 1;/* 按下状态 */
        GuiButton(&tButton[0]);

        tButton[1].x = 0;
        tButton[1].y = 24;
        tButton[1].wide = 117;
        tButton[1].high = 15;
        tButton[1].linesize = 0;
        tButton[1].name = "Error Counter";
        tButton[1].flag = 0;/* 抬起状态 */
        GuiButton(&tButton[1]);

        tButton[2].x = 0;
        tButton[2].y = 37;
        tButton[2].wide = 117;
        tButton[2].high = 15;
        tButton[2].linesize = 0;
        tButton[2].name = "Battery Status";
        tButton[2].flag = 0;/* 抬起状态 */
        GuiButton(&tButton[2]);

        tButton[3].x = 0;
        tButton[3].y = 50;
        tButton[3].wide = 40;
        tButton[3].high = 15;
        tButton[3].name = "Back";
        tButton[3].linesize = 0;
        tButton[3].flag = 0;/* 按下状态 */
        GuiButton(&tButton[3]);
        GuiUpdateDisplayAll();
    }
    else
    {
        K0_Status = rt_sem_take(K0_Sem, 0);
        K1_Status = rt_sem_take(K1_Sem, 0);
        K2_Status = rt_sem_take(K2_Sem, 0);
        K2_Long_Status = rt_sem_take(K2_Long_Sem, 0);
        if(K2_Long_Status==RT_EOK)
        {

        }
        if(K0_Status==RT_EOK)
        {
            tButton[NowButtonId].flag=0;
            GuiButton(&tButton[NowButtonId]);
            if(NowButtonId>0){NowButtonId--;}
            else NowButtonId=3;
            tButton[NowButtonId].flag=1;
            GuiButton(&tButton[NowButtonId]);
            tScroll[0].lump = NowButtonId;/* 进度快控制 */
            GuiVScroll(&tScroll[0]);/* 垂直进度条 */
            GuiUpdateDisplayAll();
        }
        if(K1_Status==RT_EOK)
        {
            tButton[NowButtonId].flag=0;
            GuiButton(&tButton[NowButtonId]);
            NowButtonId++;
            if(NowButtonId==4){NowButtonId=0;}
            tButton[NowButtonId].flag=1;
            GuiButton(&tButton[NowButtonId]);
            tScroll[0].lump = NowButtonId;/* 进度快控制 */
            GuiVScroll(&tScroll[0]);/* 垂直进度条 */
            GuiUpdateDisplayAll();
        }
        if(K2_Status==RT_EOK)
        {
            GuiClearScreen(0);
            switch(NowButtonId)
            {
                GuiClearScreen(0);
                case 0:GuiWinAdd(&userMain8Win);break;//Manual
                case 1:GuiWinAdd(&userMain12Win);break;//Error
                case 2:GuiWinAdd(&userMain13Win);break;//Battery
                case 3:
                    GuiClearScreen(0);
                    GuiWinDeleteTop();
                    break;
            }
            FirstFlag[7]=0;
        }
    }
}
static void UserMain8WinFun(void *param)
{
    if(FirstFlag[8] == 0)
    {
        FirstFlag[8] = 1;
        NowButtonId=0;

        tScroll[0].max = 4;
        tScroll[0].x = 119;
        tScroll[0].y = 13;
        tScroll[0].hight = 38;
        tScroll[0].lump = 0;/* 进度快控制 */
        GuiVScroll(&tScroll[0]);/* 垂直进度条 */

        tButton[0].x = 0;
        tButton[0].y = 11;
        tButton[0].wide = 117;
        tButton[0].high = 15;
        tButton[0].linesize = 0;
        tButton[0].name = "Manual";
        tButton[0].flag = 1;/* 按下状态 */
        GuiButton(&tButton[0]);

        tButton[1].x = 0;
        tButton[1].y = 24;
        tButton[1].wide = 117;
        tButton[1].high = 15;
        tButton[1].linesize = 0;
        tButton[1].name = "Automatic";
        tButton[1].flag = 0;/* 抬起状态 */
        GuiButton(&tButton[1]);

        tButton[2].x = 0;
        tButton[2].y = 37;
        tButton[2].wide = 117;
        tButton[2].high = 15;
        tButton[2].linesize = 0;
        tButton[2].name = "Delta Pressure";
        tButton[2].flag = 0;/* 抬起状态 */
        GuiButton(&tButton[2]);

        tButton[3].x = 0;
        tButton[3].y = 50;
        tButton[3].wide = 40;
        tButton[3].high = 15;
        tButton[3].name = "Back";
        tButton[3].linesize = 0;
        tButton[3].flag = 0;/* 按下状态 */
        GuiButton(&tButton[3]);

        GuiUpdateDisplayAll();
    }
    else
    {
        K0_Status = rt_sem_take(K0_Sem, 0);
        K1_Status = rt_sem_take(K1_Sem, 0);
        K2_Status = rt_sem_take(K2_Sem, 0);
        K2_Long_Status = rt_sem_take(K2_Long_Sem, 0);
        if(K2_Long_Status==RT_EOK)
        {

        }
        if(K0_Status==RT_EOK)
        {
            tButton[NowButtonId].flag=0;
            GuiButton(&tButton[NowButtonId]);
            if(NowButtonId>0){NowButtonId--;}
            else NowButtonId=3;
            tButton[NowButtonId].flag=1;
            GuiButton(&tButton[NowButtonId]);
            tScroll[0].lump = NowButtonId;/* 进度快控制 */
            GuiVScroll(&tScroll[0]);/* 垂直进度条 */
            GuiUpdateDisplayAll();
        }
        if(K1_Status==RT_EOK)
        {
            tButton[NowButtonId].flag=0;
            GuiButton(&tButton[NowButtonId]);
            NowButtonId++;
            if(NowButtonId==4){NowButtonId=0;}
            tButton[NowButtonId].flag=1;
            GuiButton(&tButton[NowButtonId]);
            tScroll[0].lump = NowButtonId;/* 进度快控制 */
            GuiVScroll(&tScroll[0]);/* 垂直进度条 */
            GuiUpdateDisplayAll();
        }
        if(K2_Status==RT_EOK)
        {
            GuiClearScreen(0);
            switch(NowButtonId)
            {
                case 0:GuiWinAdd(&userMain9Win);break;//Manual
                case 1:GuiWinAdd(&userMain10Win);break;//Automatic
                case 2:GuiWinAdd(&userMain11Win);break;//Delta
                case 3:GuiWinDeleteTop();break;//Home
            }
            FirstFlag[8]=0;
        }
    }
}
static void UserMain9WinFun(void *param)
{
    uint8_t ManualString[20];
    if(FirstFlag[9] == 0)
    {
        FirstFlag[9] = 1;
        NowButtonId=1;

        sprintf(ManualString,"Manual:%06d",Counter_Manual);
        tButton[0].x = 20;
        tButton[0].y = 10;
        tButton[0].wide = 100;
        tButton[0].high = 15;
        tButton[0].linesize = 0;
        tButton[0].name = ManualString;
        tButton[0].flag = 0;/* 按下状态 */
        GuiButton(&tButton[0]);


        tButton[1].x = 40;
        tButton[1].y = 22;
        tButton[1].wide = 50;
        tButton[1].high = 15;
        tButton[1].name = "Reset?";
        tButton[1].linesize = 0;
        tButton[1].flag = 1;/* 按下状态 */
        GuiButton(&tButton[1]);

        tButton[2].x = 45;
        tButton[2].y = 34;
        tButton[2].wide = 36;
        tButton[2].high = 15;
        tButton[2].name = "Back";
        tButton[2].linesize = 0;
        tButton[2].flag = 0;/* 按下状态 */
        GuiButton(&tButton[2]);

        tButton[3].x = 0;
        tButton[3].y = 50;
        tButton[3].wide = 128;
        tButton[3].high = 15;
        tButton[3].name = SingleSelect;
        tButton[3].linesize = 0;
        tButton[3].flag = 1;/* 按下状态 */
        GuiButton(&tButton[3]);

        GuiUpdateDisplayAll();
    }
    else
    {
        K0_Status = rt_sem_take(K0_Sem, 0);
        K1_Status = rt_sem_take(K1_Sem, 0);
        K2_Status = rt_sem_take(K2_Sem, 0);
        K2_Long_Status = rt_sem_take(K2_Long_Sem, 0);
        if(K2_Long_Status==RT_EOK)
        {

        }
        if(K0_Status==RT_EOK)
        {
            tButton[NowButtonId].flag=0;
            GuiButton(&tButton[NowButtonId]);
            if(NowButtonId>1){NowButtonId--;}
            else NowButtonId=2;
            tButton[NowButtonId].flag=1;
            GuiButton(&tButton[NowButtonId]);
            GuiUpdateDisplayAll();
        }
        if(K1_Status==RT_EOK)
        {
            tButton[NowButtonId].flag=0;
            GuiButton(&tButton[NowButtonId]);
            NowButtonId++;
            if(NowButtonId==3){NowButtonId=1;}
            tButton[NowButtonId].flag=1;
            GuiButton(&tButton[NowButtonId]);
            GuiUpdateDisplayAll();
        }
        if(K2_Status==RT_EOK)
        {
            GuiClearScreen(0);
            switch(NowButtonId)
            {
                case 1:
                    Counter_Manual=0;
                    Flash_Set(8,0);
                    GuiWinDeleteTop();
                    break;
                case 2:
                    GuiWinDeleteTop();
                    break;
            }
            FirstFlag[9]=0;
        }
    }
}
static void UserMain10WinFun(void *param)
{
    uint8_t AutomaticString[15];
    if(FirstFlag[10] == 0)
     {
         FirstFlag[10] = 1;
         NowButtonId=1;

         sprintf(AutomaticString,"Auto:%06d",Counter_Automatic);
         tButton[0].x = 22;
         tButton[0].y = 10;
         tButton[0].wide = 105;
         tButton[0].high = 15;
         tButton[0].linesize = 0;
         tButton[0].name = AutomaticString;
         tButton[0].flag = 0;/* 按下状态 */
         GuiButton(&tButton[0]);


         tButton[1].x = 40;
         tButton[1].y = 22;
         tButton[1].wide = 50;
         tButton[1].high = 15;
         tButton[1].name = "Reset?";
         tButton[1].linesize = 0;
         tButton[1].flag = 1;/* 按下状态 */
         GuiButton(&tButton[1]);

         tButton[2].x = 45;
         tButton[2].y = 34;
         tButton[2].wide = 36;
         tButton[2].high = 15;
         tButton[2].name = "Back";
         tButton[2].linesize = 0;
         tButton[2].flag = 0;/* 按下状态 */
         GuiButton(&tButton[2]);

         tButton[3].x = 0;
         tButton[3].y = 50;
         tButton[3].wide = 128;
         tButton[3].high = 15;
         tButton[3].name = SingleSelect;
         tButton[3].linesize = 0;
         tButton[3].flag = 1;/* 按下状态 */
         GuiButton(&tButton[3]);

         GuiUpdateDisplayAll();
     }
     else
     {
         K0_Status = rt_sem_take(K0_Sem, 0);
         K1_Status = rt_sem_take(K1_Sem, 0);
         K2_Status = rt_sem_take(K2_Sem, 0);
         K2_Long_Status = rt_sem_take(K2_Long_Sem, 0);
         if(K2_Long_Status==RT_EOK)
         {

         }
         if(K0_Status==RT_EOK)
         {
             tButton[NowButtonId].flag=0;
             GuiButton(&tButton[NowButtonId]);
             if(NowButtonId>1){NowButtonId--;}
             else NowButtonId=2;
             tButton[NowButtonId].flag=1;
             GuiButton(&tButton[NowButtonId]);
             GuiUpdateDisplayAll();
         }
         if(K1_Status==RT_EOK)
         {
             tButton[NowButtonId].flag=0;
             GuiButton(&tButton[NowButtonId]);
             NowButtonId++;
             if(NowButtonId==3){NowButtonId=1;}
             tButton[NowButtonId].flag=1;
             GuiButton(&tButton[NowButtonId]);
             GuiUpdateDisplayAll();
         }
         if(K2_Status==RT_EOK)
         {
             GuiClearScreen(0);
             switch(NowButtonId)
             {
                 case 1:
                     Counter_Automatic=0;
                     Flash_Set(9,0);
                     GuiWinDeleteTop();
                     break;
                 case 2:
                     GuiWinDeleteTop();
                     break;
             }
             FirstFlag[10]=0;
         }
     }
}
static void UserMain11WinFun(void *param)
{
    uint8_t DeltaString[15];
    if(FirstFlag[11] == 0)
     {
         FirstFlag[11] = 1;
         NowButtonId=1;

         sprintf(DeltaString,"Delta:%06d",Counter_Deltapress);
         tButton[0].x = 22;
         tButton[0].y = 10;
         tButton[0].wide = 128;
         tButton[0].high = 15;
         tButton[0].linesize = 0;
         tButton[0].name = DeltaString;
         tButton[0].flag = 0;/* 按下状态 */
         GuiButton(&tButton[0]);


         tButton[1].x = 40;
         tButton[1].y = 22;
         tButton[1].wide = 50;
         tButton[1].high = 15;
         tButton[1].name = "Reset?";
         tButton[1].linesize = 0;
         tButton[1].flag = 1;/* 按下状态 */
         GuiButton(&tButton[1]);

         tButton[2].x = 45;
         tButton[2].y = 34;
         tButton[2].wide = 36;
         tButton[2].high = 15;
         tButton[2].name = "Back";
         tButton[2].linesize = 0;
         tButton[2].flag = 0;/* 按下状态 */
         GuiButton(&tButton[2]);

         tButton[3].x = 0;
         tButton[3].y = 50;
         tButton[3].wide = 128;
         tButton[3].high = 15;
         tButton[3].name = SingleSelect;
         tButton[3].linesize = 0;
         tButton[3].flag = 1;/* 按下状态 */
         GuiButton(&tButton[3]);
         GuiUpdateDisplayAll();
     }
     else
     {
         K0_Status = rt_sem_take(K0_Sem, 0);
         K1_Status = rt_sem_take(K1_Sem, 0);
         K2_Status = rt_sem_take(K2_Sem, 0);
         K2_Long_Status = rt_sem_take(K2_Long_Sem, 0);
         if(K2_Long_Status==RT_EOK)
         {

         }
         if(K0_Status==RT_EOK)
         {
             tButton[NowButtonId].flag=0;
             GuiButton(&tButton[NowButtonId]);
             if(NowButtonId>1){NowButtonId--;}
             else NowButtonId=2;
             tButton[NowButtonId].flag=1;
             GuiButton(&tButton[NowButtonId]);
             GuiUpdateDisplayAll();
         }
         if(K1_Status==RT_EOK)
         {
             tButton[NowButtonId].flag=0;
             GuiButton(&tButton[NowButtonId]);
             NowButtonId++;
             if(NowButtonId==3){NowButtonId=1;}
             tButton[NowButtonId].flag=1;
             GuiButton(&tButton[NowButtonId]);
             GuiUpdateDisplayAll();
         }
         if(K2_Status==RT_EOK)
         {
             GuiClearScreen(0);
             switch(NowButtonId)
             {
                 case 1:
                     Counter_Deltapress=0;
                     Flash_Set(10,0);
                     GuiWinDeleteTop();
                     break;
                 case 2:
                     GuiWinDeleteTop();
                     break;
             }
             FirstFlag[11]=0;
         }
     }
}
static void UserMain12WinFun(void *param)
{
    uint8_t ErrorString[15];
    if(FirstFlag[12] == 0)
     {
         FirstFlag[12] = 1;
         NowButtonId=1;

         sprintf(ErrorString,"Error:%06d",Counter_Error);
         tButton[0].x = 22;
         tButton[0].y = 10;
         tButton[0].wide = 128;
         tButton[0].high = 15;
         tButton[0].linesize = 0;
         tButton[0].name = ErrorString;
         tButton[0].flag = 0;/* 按下状态 */
         GuiButton(&tButton[0]);


         tButton[1].x = 40;
         tButton[1].y = 22;
         tButton[1].wide = 50;
         tButton[1].high = 15;
         tButton[1].name = "Reset?";
         tButton[1].linesize = 0;
         tButton[1].flag = 1;/* 按下状态 */
         GuiButton(&tButton[1]);

         tButton[2].x = 45;
         tButton[2].y = 34;
         tButton[2].wide = 36;
         tButton[2].high = 15;
         tButton[2].name = "Back";
         tButton[2].linesize = 0;
         tButton[2].flag = 0;/* 按下状态 */
         GuiButton(&tButton[2]);

         tButton[3].x = 0;
         tButton[3].y = 50;
         tButton[3].wide = 128;
         tButton[3].high = 15;
         tButton[3].name = SingleSelect;
         tButton[3].linesize = 0;
         tButton[3].flag = 1;/* 按下状态 */
         GuiButton(&tButton[3]);
         GuiUpdateDisplayAll();
     }
     else
     {
         K0_Status = rt_sem_take(K0_Sem, 0);
         K1_Status = rt_sem_take(K1_Sem, 0);
         K2_Status = rt_sem_take(K2_Sem, 0);
         K2_Long_Status = rt_sem_take(K2_Long_Sem, 0);
         if(K2_Long_Status==RT_EOK)
         {

         }
         if(K0_Status==RT_EOK)
         {
             tButton[NowButtonId].flag=0;
             GuiButton(&tButton[NowButtonId]);
             if(NowButtonId>1){NowButtonId--;}
             else NowButtonId=2;
             tButton[NowButtonId].flag=1;
             GuiButton(&tButton[NowButtonId]);
             GuiUpdateDisplayAll();
         }
         if(K1_Status==RT_EOK)
         {
             tButton[NowButtonId].flag=0;
             GuiButton(&tButton[NowButtonId]);
             NowButtonId++;
             if(NowButtonId==3){NowButtonId=1;}
             tButton[NowButtonId].flag=1;
             GuiButton(&tButton[NowButtonId]);
             GuiUpdateDisplayAll();
         }
         if(K2_Status==RT_EOK)
         {
             GuiClearScreen(0);
             switch(NowButtonId)
             {
                 case 1:
                     Counter_Error=0;
                     Flash_Set(11,0);
                     GuiWinDeleteTop();
                     break;
                 case 2:
                     GuiWinDeleteTop();
                     break;
             }
             FirstFlag[12]=0;
         }
     }
}
extern uint8_t LowVoltageFlag;
static void UserMain13WinFun(void *param)
{
    if(FirstFlag[13] == 0)
    {
        FirstFlag[13] = 1;

        tButton[0].x = 13;
        tButton[0].y = 13;
        tButton[0].wide = 115;
        tButton[0].high = 15;
        tButton[0].linesize = 0;
        tButton[0].name = "Battery Status";
        tButton[0].flag = 0;/* 按下状态 */
        GuiButton(&tButton[0]);

        tButton[1].x = 48;
        tButton[1].y = 25;
        tButton[1].wide = 40;
        tButton[1].high = 15;
        if(LowVoltageFlag==1)
        {
            tButton[1].name = "LOW";
        }
        else if(LowVoltageFlag==2)
        {
            tButton[1].name = "Empty";
        }
        else if(LowVoltageFlag==0)
        {
            tButton[1].name = "OK";
        }
        tButton[1].linesize = 0;
        tButton[1].flag = 1;/* 按下状态 */
        GuiButton(&tButton[1]);


        tButton[2].x = 0;
        tButton[2].y = 50;
        tButton[2].wide = 128;
        tButton[2].high = 15;
        tButton[2].name = SingleRightBack;
        tButton[2].linesize = 0;
        tButton[2].flag = 1;/* 按下状态 */
        GuiButton(&tButton[2]);
        GuiUpdateDisplayAll();
    }
    else
    {
        K0_Status = rt_sem_take(K0_Sem, 0);
        K1_Status = rt_sem_take(K1_Sem, 0);
        K2_Status = rt_sem_take(K2_Sem, 0);
        K2_Long_Status = rt_sem_take(K2_Long_Sem, 0);
        if(K2_Long_Status==RT_EOK)
        {

        }
        if(K0_Status==RT_EOK)
        {

        }
        if(K1_Status==RT_EOK)
        {

        }
        if(K2_Status==RT_EOK)
        {
            GuiClearScreen(0);
            GuiWinDeleteTop();
            FirstFlag[13]=0;
        }
    }
}
uint8_t volatile Win14PageID=0;
uint8_t Password_Page;
static void UserMain14WinFun(void *param)
{
    if(FirstFlag[14]  == 0)
    {
     switch(Win14PageID)
     {
     case 0:
         NowButtonId=0;

         tScroll[0].max = 13;
         tScroll[0].x = 119;
         tScroll[0].y = 13;
         tScroll[0].hight = 38;
         tScroll[0].lump = 0;/* 进度快控制 */
         GuiVScroll(&tScroll[0]);/* 垂直进度条 */

         tButton[0].x = 0;
         tButton[0].y = 11;
         tButton[0].wide = 117;
         tButton[0].high = 15;
         tButton[0].name = Delta_Pressure;
         tButton[0].linesize = 0;
         tButton[0].flag = 1;/* 抬起状态 */
         GuiButton(&tButton[0]);

         tButton[1].x = 0;
         tButton[1].y = 24;
         tButton[1].wide = 117;
         tButton[1].high = 15;
         tButton[1].name = Conductivity;
         tButton[1].linesize = 0;
         tButton[1].flag = 0;/* 抬起状态 */
         GuiButton(&tButton[1]);

         tButton[2].x = 0;
         tButton[2].y = 37;
         tButton[2].wide = 117;
         tButton[2].high = 15;
         tButton[2].name = Backwash_Time;
         tButton[2].linesize = 0;
         tButton[2].flag = 0;/* 抬起状态 */
         GuiButton(&tButton[2]);

         tButton[3].x = 0;
         tButton[3].y = 50;
         tButton[3].wide = 128;
         tButton[3].high = 15;
         tButton[3].name = SingleSelect;
         tButton[3].linesize = 0;
         tButton[3].flag = 1;/* 按下状态 */
         GuiButton(&tButton[3]);

         FirstFlag[14] = 1;
         break;
     case 1:
         NowButtonId=0;

         tScroll[0].max = 13;
         tScroll[0].x = 119;
         tScroll[0].y = 13;
         tScroll[0].hight = 38;
         tScroll[0].lump = 3;/* 进度快控制 */
         GuiVScroll(&tScroll[0]);/* 垂直进度条 */


         tButton[0].x = 0;
         tButton[0].y = 11;
         tButton[0].wide = 117;
         tButton[0].high = 15;
         tButton[0].name = Version;
         tButton[0].linesize = 0;
         tButton[0].flag = 1;/* 抬起状态 */
         GuiButton(&tButton[0]);

         tButton[1].x = 0;
         tButton[1].y = 24;
         tButton[1].wide = 117;
         tButton[1].high = 15;
         tButton[1].name = Factory;
         tButton[1].linesize = 0;
         tButton[1].flag = 0;/* 抬起状态 */
         GuiButton(&tButton[1]);

         tButton[2].x = 0;
         tButton[2].y = 37;
         tButton[2].wide = 117;
         tButton[2].high = 15;
         tButton[2].name = Language;
         tButton[2].linesize = 0;
         tButton[2].flag = 0;/* 抬起状态 */
         GuiButton(&tButton[2]);

         tButton[3].x = 0;
         tButton[3].y = 50;
         tButton[3].wide = 128;
         tButton[3].high = 15;
         tButton[3].name = SingleSelect;
         tButton[3].linesize = 0;
         tButton[3].flag = 1;/* 按下状态 */
         GuiButton(&tButton[3]);

         FirstFlag[14] = 1;
         break;
     case 2:
         NowButtonId=0;

         tScroll[0].max = 13;
         tScroll[0].x = 119;
         tScroll[0].y = 13;
         tScroll[0].hight = 38;
         tScroll[0].lump = 6;/* 进度快控制 */
         GuiVScroll(&tScroll[0]);/* 垂直进度条 */

         tButton[0].x = 0;
         tButton[0].y = 11;
         tButton[0].wide = 117;
         tButton[0].high = 15;
         tButton[0].name = Update_FW;
         tButton[0].linesize = 0;
         tButton[0].flag = 1;/* 抬起状态 */
         GuiButton(&tButton[0]);

         tButton[1].x = 0;
         tButton[1].y = 24;
         tButton[1].wide = 117;
         tButton[1].high = 15;
         tButton[1].name = Smart_Link;
         tButton[1].linesize = 0;
         tButton[1].flag = 0;/* 抬起状态 */
         GuiButton(&tButton[1]);

         tButton[2].x = 0;
         tButton[2].y = 37;
         tButton[2].wide = 117;
         tButton[2].high = 15;
         tButton[2].name = TDS_MODE;
         tButton[2].linesize = 0;
         tButton[2].flag = 0;/* 抬起状态 */
         GuiButton(&tButton[2]);

         tButton[3].x = 0;
         tButton[3].y = 50;
         tButton[3].wide = 128;
         tButton[3].high = 15;
         tButton[3].name = SingleSelect;
         tButton[3].linesize = 0;
         tButton[3].flag = 1;/* 按下状态 */
         GuiButton(&tButton[3]);

         FirstFlag[14] = 1;
         break;
     case 3:
        NowButtonId=0;

        tScroll[0].max = 13;
        tScroll[0].x = 119;
        tScroll[0].y = 13;
        tScroll[0].hight = 38;
        tScroll[0].lump = 9;/* 进度快控制 */
        GuiVScroll(&tScroll[0]);/* 垂直进度条 */

        tButton[0].x = 0;
        tButton[0].y = 11;
        tButton[0].wide = 117;
        tButton[0].high = 15;
        tButton[0].name = "Automatic Range";
        tButton[0].linesize = 0;
        tButton[0].flag = 1;/* 抬起状态 */
        GuiButton(&tButton[0]);

        tButton[1].x = 0;
        tButton[1].y = 24;
        tButton[1].wide = 117;
        tButton[1].high = 15;
        tButton[1].name ="TDS_CND";
        tButton[1].linesize = 0;
        tButton[1].flag = 0;/* 抬起状态 */
        GuiButton(&tButton[1]);

        tButton[2].x = 0;
        tButton[2].y = 37;
        tButton[2].wide =117;
        tButton[2].high = 15;
        tButton[2].name = "Back";
        tButton[2].linesize = 0;
        tButton[2].flag = 0;/* 抬起状态 */
        GuiButton(&tButton[2]);

        tButton[3].x = 0;
        tButton[3].y = 50;
        tButton[3].wide = 128;
        tButton[3].high = 15;
        tButton[3].name = SingleSelect;
        tButton[3].linesize = 0;
        tButton[3].flag = 1;/* 按下状态 */
        GuiButton(&tButton[3]);

        FirstFlag[14] = 1;
        break;
     case 4:
         NowButtonId=2;

         tScroll[0].max =11;
         tScroll[0].x = 119;
         tScroll[0].y = 13;
         tScroll[0].hight = 38;
         tScroll[0].lump = 2;/* 进度快控制 */
         GuiVScroll(&tScroll[0]);/* 垂直进度条 */

         tButton[0].x = 0;
         tButton[0].y = 11;
         tButton[0].wide = 117;
         tButton[0].high = 15;
         tButton[0].name = Delta_Pressure;
         tButton[0].linesize = 0;
         tButton[0].flag = 0;/* 抬起状态 */
         GuiButton(&tButton[0]);

         tButton[1].x = 0;
         tButton[1].y = 24;
         tButton[1].wide = 117;
         tButton[1].high = 15;
         tButton[1].name = Conductivity;
         tButton[1].linesize = 0;
         tButton[1].flag = 0;/* 抬起状态 */
         GuiButton(&tButton[1]);

         tButton[2].x = 0;
         tButton[2].y = 37;
         tButton[2].wide = 117;
         tButton[2].high = 15;
         tButton[2].name = Backwash_Time;
         tButton[2].linesize = 0;
         tButton[2].flag = 1;/* 抬起状态 */
         GuiButton(&tButton[2]);

         tButton[3].x = 0;
         tButton[3].y = 50;
         tButton[3].wide = 128;
         tButton[3].high = 15;
         tButton[3].name = SingleSelect;
         tButton[3].linesize = 0;
         tButton[3].flag = 1;/* 按下状态 */
         GuiButton(&tButton[3]);

         FirstFlag[14] = 1;
         break;
     case 5:
         NowButtonId=2;

         tScroll[0].max = 13;
         tScroll[0].x = 119;
         tScroll[0].y = 13;
         tScroll[0].hight = 38;
         tScroll[0].lump = 5;/* 进度快控制 */
         GuiVScroll(&tScroll[0]);/* 垂直进度条 */


         tButton[0].x = 0;
         tButton[0].y = 11;
         tButton[0].wide = 117;
         tButton[0].high = 15;
         tButton[0].name = Version;
         tButton[0].linesize = 0;
         tButton[0].flag = 0;/* 抬起状态 */
         GuiButton(&tButton[0]);

         tButton[1].x = 0;
         tButton[1].y = 24;
         tButton[1].wide = 117;
         tButton[1].high = 15;
         tButton[1].name = Factory;
         tButton[1].linesize = 0;
         tButton[1].flag = 0;/* 抬起状态 */
         GuiButton(&tButton[1]);

         tButton[2].x = 0;
         tButton[2].y = 37;
         tButton[2].wide = 117;
         tButton[2].high = 15;
         tButton[2].name = Language;
         tButton[2].linesize = 0;
         tButton[2].flag = 1;/* 抬起状态 */
         GuiButton(&tButton[2]);

         tButton[3].x = 0;
         tButton[3].y = 50;
         tButton[3].wide = 128;
         tButton[3].high = 15;
         tButton[3].name = SingleSelect;
         tButton[3].linesize = 0;
         tButton[3].flag = 1;/* 按下状态 */
         GuiButton(&tButton[3]);

         FirstFlag[14] = 1;
         break;
     case 6:
         NowButtonId=2;

         tScroll[0].max = 13;
         tScroll[0].x = 119;
         tScroll[0].y = 13;
         tScroll[0].hight = 38;
         tScroll[0].lump = 8;/* 进度快控制 */
         GuiVScroll(&tScroll[0]);/* 垂直进度条 */

         tButton[0].x = 0;
         tButton[0].y = 11;
         tButton[0].wide = 117;
         tButton[0].high = 15;
         tButton[0].name = Update_FW;
         tButton[0].linesize = 0;
         tButton[0].flag = 0;/* 抬起状态 */
         GuiButton(&tButton[0]);

         tButton[1].x = 0;
         tButton[1].y = 24;
         tButton[1].wide = 117;
         tButton[1].high = 15;
         tButton[1].name = Smart_Link;
         tButton[1].linesize = 0;
         tButton[1].flag = 0;/* 抬起状态 */
         GuiButton(&tButton[1]);

         tButton[2].x = 0;
         tButton[2].y = 37;
         tButton[2].wide = 117;
         tButton[2].high = 15;
         tButton[2].name = TDS_MODE;
         tButton[2].linesize = 0;
         tButton[2].flag = 1;/* 抬起状态 */
         GuiButton(&tButton[2]);

         tButton[3].x = 0;
         tButton[3].y = 50;
         tButton[3].wide = 128;
         tButton[3].high = 15;
         tButton[3].name = SingleSelect;
         tButton[3].linesize = 0;
         tButton[3].flag = 1;/* 按下状态 */
         GuiButton(&tButton[3]);
         FirstFlag[14] = 1;
         break;

     case 7:
        NowButtonId=2;

        tScroll[0].max = 13;
        tScroll[0].x = 119;
        tScroll[0].y = 13;
        tScroll[0].hight = 38;
        tScroll[0].lump = 11;/* 进度快控制 */
        GuiVScroll(&tScroll[0]);/* 垂直进度条 */

        tButton[0].x = 0;
        tButton[0].y = 11;
        tButton[0].wide = 117;
        tButton[0].high = 15;
        tButton[0].name = "Automatic Range";
        tButton[0].linesize = 0;
        tButton[0].flag = 0;/* 抬起状态 */
        GuiButton(&tButton[0]);

        tButton[1].x = 0;
        tButton[1].y = 24;
        tButton[1].wide = 117;
        tButton[1].high = 15;
        tButton[1].name = "TDS_CND";
        tButton[1].linesize = 0;
        tButton[1].flag = 0;/* 抬起状态 */
        GuiButton(&tButton[1]);

        tButton[2].x = 0;
        tButton[2].y = 37;
        tButton[2].wide = 117;
        tButton[2].high = 15;
        tButton[2].name = "Back";
        tButton[2].linesize = 0;
        tButton[2].flag = 1;/* 抬起状态 */
        GuiButton(&tButton[2]);

        tButton[3].x = 0;
        tButton[3].y = 50;
        tButton[3].wide = 128;
        tButton[3].high = 15;
        tButton[3].name = SingleSelect;
        tButton[3].linesize = 0;
        tButton[3].flag = 1;/* 按下状态 */
        GuiButton(&tButton[3]);

        FirstFlag[14] = 1;
        break;
     }
     GuiUpdateDisplayAll();
    }
    else
    {
        K0_Status = rt_sem_take(K0_Sem, 0);
        K1_Status = rt_sem_take(K1_Sem, 0);
        K2_Status = rt_sem_take(K2_Sem, 0);
        K2_Long_Status = rt_sem_take(K2_Long_Sem, 0);
        if(K2_Long_Status==RT_EOK)
        {

        }
        if(K0_Status==RT_EOK)
        {
            switch(NowButtonId)
            {
            case 0:
                switch(Win14PageID)
                {
                    case 0:Win14PageID = 7;break;
                    case 1:Win14PageID = 4;break;
                    case 2:Win14PageID = 5;break;
                    case 3:Win14PageID = 6;break;

                    case 4:Win14PageID = 7;break;
                    case 5:Win14PageID = 4;break;
                    case 6:Win14PageID = 5;break;
                    case 7:Win14PageID = 6;break;
                }
                FirstFlag[14]  = 0;
                break;
            default:
                tButton[NowButtonId].flag=0;
                GuiButton(&tButton[NowButtonId]);//当前行数反显取消
                if(NowButtonId>0)NowButtonId--;
                tButton[NowButtonId].flag=1;
                GuiButton(&tButton[NowButtonId]);//按下后的反显开
                /*进度条*/
                if(Win14PageID>3){tScroll[0].lump = 3*(Win14PageID-4)+NowButtonId;}/* 进度快控制 */
                else {tScroll[0].lump = NowButtonId+3*Win14PageID;}/* 进度快控制 */
                GuiVScroll(&tScroll[0]);/* 垂直进度条 */
                break;
            }
            GuiUpdateDisplayAll();
        }
        if(K1_Status==RT_EOK)
        {
            switch(NowButtonId)
            {
                case 2:
                    switch(Win14PageID)
                    {
                        case 0:Win14PageID = 1;break;
                        case 1:Win14PageID = 2;break;
                        case 2:Win14PageID = 3;break;
                        case 3:Win14PageID = 0;break;

                        case 4:Win14PageID = 1;break;
                        case 5:Win14PageID = 2;break;
                        case 6:Win14PageID = 3;break;
                        case 7:Win14PageID = 0;break;
                    }
                    FirstFlag[14]  = 0;
                    break;
                default:
                    tButton[NowButtonId].flag=0;
                     GuiButton(&tButton[NowButtonId]);
                     NowButtonId++;
                     tButton[NowButtonId].flag=1;
                     GuiButton(&tButton[NowButtonId]);
                     if(Win14PageID>3){tScroll[0].lump = 3*(Win14PageID-4)+NowButtonId;}/* 进度快控制 */
                     else {tScroll[0].lump = NowButtonId+3*Win14PageID;}/* 进度快控制 */
                     GuiVScroll(&tScroll[0]);/* 垂直进度条 */
                     break;
            }
            GuiUpdateDisplayAll();
        }
        if(K2_Status==RT_EOK)
        {
            FirstFlag[14]=0;
            GuiClearScreen(0);
            switch(Win14PageID)
            {
                case 0:
                    switch(NowButtonId)
                    {
                        case 0:GuiWinAdd(&userMain15Win);break;//password
                        case 1:GuiWinAdd(&userMain16Win);break;//Conductivity
                        case 2:GuiWinAdd(&userMain17Win);break;//Backwash Time
                    }
                    break;
                case 1:
                    switch(NowButtonId)
                    {
                        case 0:GuiWinAdd(&userMain18Win);break;//Version
                        case 1:GuiWinAdd(&userMain19Win);break;//Factory
                        case 2:GuiWinAdd(&userMain20Win);break;//Language
                    }
                    break;
                case 2:
                    switch(NowButtonId)
                    {
                        //case 0:break;//up data
                        //case 1:break;//smart_link
                        case 2:Password_Page=1;GuiWinAdd(&userMain21Win);break;//Password
                    }
                    break;
                case 3:
                    switch(NowButtonId)
                    {
                        case 0:GuiWinAdd(&userMain28Win);break;//AutoRange
                        case 1:GuiWinAdd(&userMain29Win);break;
                        case 2:GuiClearScreen(0);GuiWinDeleteTop();break;//GuiWinAdd(&userMain1Win);GuiWinDisplay();break;
                    }
                    break;
                case 4:
                    switch(NowButtonId)
                    {
                        case 0:GuiWinAdd(&userMain15Win);break;//password
                        case 1:GuiWinAdd(&userMain16Win);break;//Conductivity
                        case 2:GuiWinAdd(&userMain17Win);break;//Backwash Time
                    }
                    break;
                case 5:
                    switch(NowButtonId)
                    {
                        case 0:GuiWinAdd(&userMain18Win);break;//Version
                        case 1:GuiWinAdd(&userMain19Win);break;//Factory
                        case 2:GuiWinAdd(&userMain20Win);break;//Language
                    }
                    break;
                case 6:
                    switch(NowButtonId)
                    {
                        case 2:GuiWinAdd(&userMain21Win);break;//Password
                    }
                    break;
                case 7:
                    switch(NowButtonId)
                    {
                        case 0:GuiWinAdd(&userMain28Win);break;//AutoRange
                        case 1:GuiWinAdd(&userMain29Win);break;
                        case 2:GuiClearScreen(0);GuiWinDeleteTop();break;//GuiWinAdd(&userMain1Win); GuiWinDisplay();break;
                    }
                    break;
            }
            GuiUpdateDisplayAll();
            Win14PageID = 0;
            NowButtonId = 0;
        }
    }
}
uint8_t Set_Delta_Temp;
static void UserMain15WinFun(void *param)
{
    if(FirstFlag[15] == 0)
    {
        FirstFlag[15] = 1;
        Set_Delta_Temp=Setting_Deltapress;

        tButton[0].x = 20;
        tButton[0].y = 12;
        tButton[0].wide = 85;
        tButton[0].high = 15;
        tButton[0].name = "Report Only";
        tButton[0].linesize = 100 - Set_Delta_Temp*100;
        tButton[0].flag = !Set_Delta_Temp;/* 按下状态 */
        GuiButton(&tButton[0]);

        tButton[1].x = 27;
        tButton[1].y = 24;
        tButton[1].wide = 72;
        tButton[1].high = 15;
        tButton[1].name = "Automatic";
        tButton[1].linesize = 94 - !Set_Delta_Temp*94;
        tButton[1].flag = Set_Delta_Temp;/* 抬起状态 */
        GuiButton(&tButton[1]);

        tButton[3].x = 0;
        tButton[3].y = 50;
        tButton[3].wide = 128;
        tButton[3].high = 15;
        tButton[3].name = SingleSelect;
        tButton[3].linesize = 0;
        tButton[3].flag = 1;/* 按下状态 */
        GuiButton(&tButton[3]);
        GuiUpdateDisplayAll();
    }
    else
    {
        K0_Status = rt_sem_take(K0_Sem, 0);
        K1_Status = rt_sem_take(K1_Sem, 0);
        K2_Status = rt_sem_take(K2_Sem, 0);
        K2_Long_Status = rt_sem_take(K2_Long_Sem, 0);
        if(K2_Long_Status==RT_EOK)
        {

        }
        if(K0_Status==RT_EOK)
        {
            Set_Delta_Temp=!Set_Delta_Temp;
            tButton[0].flag = !Set_Delta_Temp;
            GuiButton(&tButton[0]);
            tButton[1].flag = Set_Delta_Temp;
            GuiButton(&tButton[1]);
            GuiUpdateDisplayAll();
        }
        if(K1_Status==RT_EOK)
        {
            Set_Delta_Temp=!Set_Delta_Temp;
            tButton[0].flag = !Set_Delta_Temp;
            GuiButton(&tButton[0]);
            tButton[1].flag = Set_Delta_Temp;
            GuiButton(&tButton[1]);
            GuiUpdateDisplayAll();
        }
        if(K2_Status==RT_EOK)
        {
            Setting_Deltapress=Set_Delta_Temp;
            Flash_Set(12,Set_Delta_Temp);
            GuiClearScreen(0);
            GuiWinDeleteTop();
            FirstFlag[15]=0;
        }
    }
}
uint16_t Set_Hardness_Temp;
uint8_t HardnessString[15];
static void UserMain16WinFun(void *param)
{
    if(FirstFlag[16] == 0)
    {
        FirstFlag[16] = 1;
        Set_Hardness_Temp=Setting_Hardness;
        NowButtonId=0;

        sprintf(HardnessString,"Value:%04duS",Setting_Hardness);

        tButton[0].x = 20;
        tButton[0].y = 12;
        tButton[0].wide = 90;
        tButton[0].high = 15;
        tButton[0].name = HardnessString;
        tButton[0].linesize = 0;
        tButton[0].flag = 1;/* 按下状态 */
        GuiButton(&tButton[0]);

        tButton[1].x = 45;
        tButton[1].y = 24;
        tButton[1].wide = 40;
        tButton[1].high = 15;
        tButton[1].name = "Save";
        tButton[1].linesize = 0;
        tButton[1].flag = 0;/* 按下状态 */
        GuiButton(&tButton[1]);

        tButton[2].x = 45;
        tButton[2].y = 36;
        tButton[2].wide = 40;
        tButton[2].high = 15;
        tButton[2].name = "Back";
        tButton[2].linesize = 0;
        tButton[2].flag = 0;/* 按下状态 */
        GuiButton(&tButton[2]);

        tButton[3].x = 0;
        tButton[3].y = 50;
        tButton[3].wide = 128;
        tButton[3].high = 15;
        tButton[3].name = SingleSelect;
        tButton[3].linesize = 0;
        tButton[3].flag = 1;/* 按下状态 */
        GuiButton(&tButton[3]);
        GuiUpdateDisplayAll();
    }
    else
    {
        K0_Status = rt_sem_take(K0_Sem, 0);
        K1_Status = rt_sem_take(K1_Sem, 0);
        K2_Status = rt_sem_take(K2_Sem, 0);
        K2_Long_Status = rt_sem_take(K2_Long_Sem, 0);
        if(K2_Long_Status==RT_EOK)
        {

        }
        if(K1_Status==RT_EOK)
        {
            switch(NowButtonId)
            {
                case 0:
                    if(Set_Hardness_Temp>100)Set_Hardness_Temp-=10;
                    sprintf(HardnessString,"Value:%04duS",Set_Hardness_Temp);
                    tButton[0].name = HardnessString;
                    GuiButton(&tButton[0]);
                    break;
                case 1:
                    NowButtonId = 2;
                    tButton[1].flag=0;
                    GuiButton(&tButton[1]);
                    tButton[2].flag=1;
                    GuiButton(&tButton[2]);
                    break;
                case 2:
                    NowButtonId = 1;
                    tButton[1].flag=1;
                    GuiButton(&tButton[1]);
                    tButton[2].flag=0;
                    GuiButton(&tButton[2]);
                    break;
            }
            GuiUpdateDisplayAll();
        }
        if(K0_Status==RT_EOK)
        {
            switch(NowButtonId)
            {
                case 0:
                    Set_Hardness_Temp+=10;
                    sprintf(HardnessString,"Value:%04duS",Set_Hardness_Temp);
                    tButton[0].name = HardnessString;
                    GuiButton(&tButton[0]);
                    break;
                case 1:
                    NowButtonId = 2;
                    tButton[1].flag=0;
                    GuiButton(&tButton[1]);
                    tButton[2].flag=1;
                    GuiButton(&tButton[2]);
                    break;
                case 2:
                    NowButtonId = 1;
                    tButton[1].flag=1;
                    GuiButton(&tButton[1]);
                    tButton[2].flag=0;
                    GuiButton(&tButton[2]);
                    break;
            }
            GuiUpdateDisplayAll();
        }
        if(K2_Status==RT_EOK)
        {
            switch(NowButtonId)
            {
                case 0:
                    NowButtonId = 1;
                    tButton[0].flag=0;
                    GuiButton(&tButton[0]);
                    tButton[1].flag=1;
                    GuiButton(&tButton[1]);
                    break;
                case 1:
                    Setting_Hardness=Set_Hardness_Temp;
                    Flash_Set(13,Set_Hardness_Temp);
                    GuiClearScreen(0);
                    GuiWinDeleteTop();
                    FirstFlag[16]=0;
                    break;
                case 2:
                    GuiClearScreen(0);
                    GuiWinDeleteTop();
                    FirstFlag[16]=0;
                    break;
            }
            GuiUpdateDisplayAll();
        }
    }
}
uint16_t Set_Backwash_Temp;
uint8_t BackwashString[15];
static void UserMain17WinFun(void *param)
{
    if(FirstFlag[17] == 0)
    {
        FirstFlag[17] = 1;
        Set_Backwash_Temp=Setting_Backwashtime;
        NowButtonId=0;

        sprintf(BackwashString,"Time:%02dS",Setting_Backwashtime);

        tButton[0].x = 32;
        tButton[0].y = 12;
        tButton[0].wide = 73;
        tButton[0].high = 15;
        tButton[0].name = BackwashString;
        tButton[0].linesize = 0;
        tButton[0].flag = 1;/* 按下状态 */
        GuiButton(&tButton[0]);

        tButton[1].x = 45;
        tButton[1].y = 24;
        tButton[1].wide = 40;
        tButton[1].high = 15;
        tButton[1].name = "Save";
        tButton[1].linesize = 0;
        tButton[1].flag = 0;/* 按下状态 */
        GuiButton(&tButton[1]);

        tButton[2].x = 45;
        tButton[2].y = 36;
        tButton[2].wide = 40;
        tButton[2].high = 15;
        tButton[2].name = "Back";
        tButton[2].linesize = 0;
        tButton[2].flag = 0;/* 按下状态 */
        GuiButton(&tButton[2]);

        tButton[3].x = 0;
        tButton[3].y = 50;
        tButton[3].wide = 128;
        tButton[3].high = 15;
        tButton[3].name = SingleSelect;
        tButton[3].linesize = 0;
        tButton[3].flag = 1;/* 按下状态 */
        GuiButton(&tButton[3]);
        GuiUpdateDisplayAll();
    }
    else
    {
        K0_Status = rt_sem_take(K0_Sem, 0);
        K1_Status = rt_sem_take(K1_Sem, 0);
        K2_Status = rt_sem_take(K2_Sem, 0);
        K2_Long_Status = rt_sem_take(K2_Long_Sem, 0);
        if(K2_Long_Status==RT_EOK)
        {

        }
        if(K0_Status==RT_EOK)
        {
            switch(NowButtonId)
            {
                case 0:
                    if(Set_Backwash_Temp>30)Set_Backwash_Temp-=1;
                    sprintf(BackwashString,"Time:%02dS",Set_Backwash_Temp);
                    tButton[0].name = BackwashString;
                    GuiButton(&tButton[0]);
                    break;
                case 1:
                    NowButtonId = 2;
                    tButton[1].flag=0;
                    GuiButton(&tButton[1]);
                    tButton[2].flag=1;
                    GuiButton(&tButton[2]);
                    break;
                case 2:
                    NowButtonId = 1;
                    tButton[1].flag=1;
                    GuiButton(&tButton[1]);
                    tButton[2].flag=0;
                    GuiButton(&tButton[2]);
                    break;
            }
            GuiUpdateDisplayAll();
        }
        if(K1_Status==RT_EOK)
        {
            switch(NowButtonId)
            {
                case 0:
                    if(Set_Backwash_Temp<100)Set_Backwash_Temp+=1;
                    sprintf(BackwashString,"Time:%02dS",Set_Backwash_Temp);
                    tButton[0].name = BackwashString;
                    GuiButton(&tButton[0]);
                    break;
                case 1:
                    NowButtonId = 2;
                    tButton[1].flag=0;
                    GuiButton(&tButton[1]);
                    tButton[2].flag=1;
                    GuiButton(&tButton[2]);
                    break;
                case 2:
                    NowButtonId = 1;
                    tButton[1].flag=1;
                    GuiButton(&tButton[1]);
                    tButton[2].flag=0;
                    GuiButton(&tButton[2]);
                    break;
            }
            GuiUpdateDisplayAll();
        }
        if(K2_Status==RT_EOK)
        {
            switch(NowButtonId)
            {
                case 0:
                    NowButtonId = 1;
                    tButton[0].flag=0;
                    GuiButton(&tButton[0]);
                    tButton[1].flag=1;
                    GuiButton(&tButton[1]);
                    break;
                case 1:
                    Setting_Backwashtime=Set_Backwash_Temp;
                    Flash_Set(14,Set_Backwash_Temp);
                    GuiClearScreen(0);
                    GuiWinDeleteTop();
                    GuiWinDisplay();
                    FirstFlag[17]=0;
                    break;
                case 2:
                    GuiClearScreen(0);
                    GuiWinDeleteTop();
                    GuiWinDisplay();
                    FirstFlag[17]=0;
                    break;
            }
            GuiUpdateDisplayAll();
        }
    }
}
static void UserMain18WinFun(void *param)
{
    if(FirstFlag[18] == 0)
    {
        FirstFlag[18] = 1;

        GuiRowText(19,30,80,0,"SYR BFC:");
        GuiRowText(76,30,80,0,"0.0.4");

        tButton[0].x = 0;
        tButton[0].y = 50;
        tButton[0].wide = 128;
        tButton[0].high = 15;
        tButton[0].name = SingleRightBack;
        tButton[0].linesize = 0;
        tButton[0].flag = 1;/* 按下状态 */
        GuiButton(&tButton[0]);
        GuiUpdateDisplayAll();
    }
    else
    {
        K0_Status = rt_sem_take(K0_Sem, 0);
        K1_Status = rt_sem_take(K1_Sem, 0);
        K2_Status = rt_sem_take(K2_Sem, 0);
        K2_Long_Status = rt_sem_take(K2_Long_Sem, 0);
        if(K2_Long_Status==RT_EOK)
        {

        }
        if(K0_Status==RT_EOK)
        {

        }
        if(K1_Status==RT_EOK)
        {

        }
        if(K2_Status==RT_EOK)
        {
            GuiClearScreen(0);
            GuiWinDeleteTop();
            FirstFlag[18]=0;
        }
    }
}
static void UserMain19WinFun(void *param)
{
    if(FirstFlag[19] == 0)
    {
        FirstFlag[19] = 1;
        NowButtonId = 0;

        GuiRowText(10,15,120,0,"Reset BFC Config");

        tButton[0].x = 47;
        tButton[0].y = 23;
        tButton[0].wide = 38;
        tButton[0].high = 15;
        tButton[0].name = "Reset";
        tButton[0].linesize = 0;
        tButton[0].flag = 1;/* 按下状态 */
        GuiButton(&tButton[0]);

        tButton[1].x = 47;
        tButton[1].y = 36;
        tButton[1].wide = 38;
        tButton[1].high = 15;
        tButton[1].name = "Back";
        tButton[1].linesize = 0;
        tButton[1].flag = 0;/* 按下状态 */
        GuiButton(&tButton[1]);

        tButton[2].x = 0;
        tButton[2].y = 50;
        tButton[2].wide = 128;
        tButton[2].high = 15;
        tButton[2].name = SingleSelect;
        tButton[2].linesize = 0;
        tButton[2].flag = 1;/* 按下状态 */
        GuiButton(&tButton[2]);
        GuiUpdateDisplayAll();
    }
    else
    {
        K0_Status = rt_sem_take(K0_Sem, 0);
        K1_Status = rt_sem_take(K1_Sem, 0);
        K2_Status = rt_sem_take(K2_Sem, 0);
        K2_Long_Status = rt_sem_take(K2_Long_Sem, 0);
        if(K2_Long_Status==RT_EOK)
        {

        }
        if(K0_Status==RT_EOK)
        {
            switch(NowButtonId)
            {
                case 0:
                    NowButtonId = 1;
                    tButton[1].flag=0;
                    GuiButton(&tButton[0]);
                    tButton[2].flag=1;
                    GuiButton(&tButton[1]);
                    break;
                case 1:
                    NowButtonId = 0;
                    tButton[1].flag=1;
                    GuiButton(&tButton[0]);
                    tButton[2].flag=0;
                    GuiButton(&tButton[1]);
                    break;
            }
            GuiUpdateDisplayAll();
        }
        if(K1_Status==RT_EOK)
        {
            switch(NowButtonId)
            {
                case 0:
                    NowButtonId = 1;
                    tButton[0].flag=0;
                    GuiButton(&tButton[0]);
                    tButton[1].flag=1;
                    GuiButton(&tButton[1]);
                    break;
                case 1:
                    NowButtonId = 0;
                    tButton[0].flag=1;
                    GuiButton(&tButton[0]);
                    tButton[1].flag=0;
                    GuiButton(&tButton[1]);
                    break;
            }
            GuiUpdateDisplayAll();
        }
        if(K2_Status==RT_EOK)
        {
            switch(NowButtonId)
            {
                case 0:
                    Flash_Clear();
                    break;
                case 1:
                    GuiClearScreen(0);
                    GuiWinDeleteTop();
                    FirstFlag[19]=0;
                    break;
            }
        }
    }
}
uint8_t Set_Language_Temp;
static void UserMain20WinFun(void *param)
{
    if(FirstFlag[20] == 0)
    {
        FirstFlag[20] = 1;
        NowButtonId=0;
        Set_Language_Temp=Setting_Language;

        tButton[0].x = 37;
        tButton[0].y = 12;
        tButton[0].wide = 56;
        tButton[0].high = 15;
        tButton[0].name = "English";
        tButton[0].linesize = 90 - Set_Language_Temp*90;
        tButton[0].flag = 1;/* 按下状态 */
        GuiButton(&tButton[0]);

        tButton[1].x = 41;
        tButton[1].y = 24;
        tButton[1].wide = 48;
        tButton[1].high = 15;
        tButton[1].name = "Deutsh";
        tButton[1].linesize = 85 - !Set_Language_Temp*85;
        tButton[1].flag = 0;/* 抬起状态 */
        GuiButton(&tButton[1]);

        tButton[2].x = 45;
        tButton[2].y = 36;
        tButton[2].wide = 37;
        tButton[2].high = 15;
        tButton[2].name = "Back";
        tButton[2].linesize = 0;
        tButton[2].flag = 0;/* 抬起状态 */
        GuiButton(&tButton[2]);

        tButton[3].x = 0;
        tButton[3].y = 50;
        tButton[3].wide = 128;
        tButton[3].high = 15;
        tButton[3].name = SingleSelect;
        tButton[3].linesize = 0;
        tButton[3].flag = 1;/* 按下状态 */
        GuiButton(&tButton[3]);
        GuiUpdateDisplayAll();
    }
    else
    {
        K0_Status = rt_sem_take(K0_Sem, 0);
        K1_Status = rt_sem_take(K1_Sem, 0);
        K2_Status = rt_sem_take(K2_Sem, 0);
        K2_Long_Status = rt_sem_take(K2_Long_Sem, 0);
        if(K2_Long_Status==RT_EOK)
        {

        }
        if(K1_Status==RT_EOK)
        {
            tButton[NowButtonId].flag=0;
            GuiButton(&tButton[NowButtonId]);
            NowButtonId++;
            if(NowButtonId==3){NowButtonId=0;}
            tButton[NowButtonId].flag=1;
            GuiButton(&tButton[NowButtonId]);
            GuiUpdateDisplayAll();
        }
        if(K0_Status==RT_EOK)
        {
            tButton[NowButtonId].flag=0;
            GuiButton(&tButton[NowButtonId]);
            if(NowButtonId>0){NowButtonId--;}else NowButtonId=2;
            tButton[NowButtonId].flag=1;
            GuiButton(&tButton[NowButtonId]);
            GuiUpdateDisplayAll();
        }
        if(K2_Status==RT_EOK)
        {
            switch(NowButtonId)
            {
                case 0:
                    Setting_Language = 0;
                    Flash_Set(15,Setting_Language);
                    GuiClearScreen(0);
                    GuiWinDeleteTop();
                    FirstFlag[20]=0;
                    break;
                case 1:
                    Setting_Language = 1;
                    Flash_Set(15,Setting_Language);
                    GuiClearScreen(0);
                    GuiWinDeleteTop();
                    FirstFlag[20]=0;
                    break;
                case 2:
                    GuiClearScreen(0);
                    GuiWinDeleteTop();
                    FirstFlag[20]=0;
                    break;
            }
        }
    }
}
uint8_t Tds_Password=3;
uint8_t Tds_Password_Temp;
static void UserMain21WinFun(void *param)//password
{
    uint8_t Tds_PasswordString[4];
    if(FirstFlag[21] == 0)
    {
        FirstFlag[21] = 1;
        Tds_Password_Temp=0;

        GuiRowText(15,20,100,0,"Input Password");
        sprintf(Tds_PasswordString,"%02d",Tds_Password_Temp);
        GuiRowText(57,32,85,0,Tds_PasswordString);

        tButton[3].x = 0;
        tButton[3].y = 50;
        tButton[3].wide = 128;
        tButton[3].high = 15;
        tButton[3].name = SingleSelect;
        tButton[3].linesize = 0;
        tButton[3].flag = 1;/* 按下状态 */
        GuiButton(&tButton[3]);
        GuiUpdateDisplayAll();
    }
    else
    {
        K0_Status = rt_sem_take(K0_Sem, 0);
        K1_Status = rt_sem_take(K1_Sem, 0);
        K2_Status = rt_sem_take(K2_Sem, 0);
        K2_Long_Status = rt_sem_take(K2_Long_Sem, 0);
        if(K2_Long_Status==RT_EOK)
        {

        }
        if(K0_Status==RT_EOK)
        {
            if(Tds_Password_Temp>0)Tds_Password_Temp--;
            sprintf(Tds_PasswordString,"%02d",Tds_Password_Temp);
            GuiRowText(57,32,85,0,Tds_PasswordString);
            GuiUpdateDisplayAll();
        }
        if(K1_Status==RT_EOK)
        {
            Tds_Password_Temp++;
            sprintf(Tds_PasswordString,"%02d",Tds_Password_Temp);
            GuiRowText(57,32,85,0,Tds_PasswordString);
            GuiUpdateDisplayAll();
        }
        if(K2_Status==RT_EOK)
        {
            GuiClearScreen(0);
            if(Tds_Password_Temp==Tds_Password)
            {
                GuiWinAdd(&userMain23Win);
            }
            else
            {
                GuiWinDeleteTop();
            }
            FirstFlag[21]=0;
        }
    }
}
static void UserMain22WinFun(void *param)//password
{
    if(FirstFlag[22] == 0)
    {
        FirstFlag[22] = 1;
        NowButtonId=0;

        tButton[0].x = 0;
        tButton[0].y = 11;
        tButton[0].wide = 128;
        tButton[0].high = 15;
        tButton[0].name = "Current Value";
        tButton[0].linesize = 0;
        tButton[0].flag = 1;/* 按下状态 */
        GuiButton(&tButton[0]);


        tButton[3].x = 0;
        tButton[3].y = 50;
        tButton[3].wide = 128;
        tButton[3].high = 15;
        tButton[3].name = Back;
        tButton[3].linesize = 0;
        tButton[3].flag = 1;/* 按下状态 */
        GuiButton(&tButton[3]);
        GuiUpdateDisplayAll();
    }
    else
    {
        K0_Status = rt_sem_take(K0_Sem, 0);
        K1_Status = rt_sem_take(K1_Sem, 0);
        K2_Status = rt_sem_take(K2_Sem, 0);
        K2_Long_Status = rt_sem_take(K2_Long_Sem, 0);
        if(K2_Long_Status==RT_EOK)
        {

        }
        if(K0_Status==RT_EOK)
        {
            GuiClearScreen(0);
            GuiWinDeleteTop();
            FirstFlag[22]=0;
        }
        if(K1_Status==RT_EOK)
        {
//            tButton[NowButtonId].flag=0;
//            GuiButton(&tButton[NowButtonId]);
//            NowButtonId++;
//            if(NowButtonId==1){NowButtonId=0;}
//            tButton[NowButtonId].flag=1;
//            GuiButton(&tButton[NowButtonId]);
        }
        if(K2_Status==RT_EOK)
        {
            FirstFlag[22]=0;
            GuiClearScreen(0);
            if(NowButtonId==0)
            {
                GuiWinAdd(&userMain23Win);
            }
        }
    }
}
double TdsValueOffset=0;
uint16_t TdsValueZeroOffset=0;
uint16_t TdsValueOffsetTemp;
static void UserMain23WinFun(void *param)//password
{
    uint8_t CurrentTdsString[10]={0};
    uint32_t TdsValue,result=0;
    if(FirstFlag[23] == 0)
    {
        FirstFlag[23] = 1;

        sprintf(CurrentTdsString,"%05dus/cm",0);
        GuiRowText(30,25,110,0,CurrentTdsString);

        tButton[3].x = 0;
        tButton[3].y = 50;
        tButton[3].wide = 128;
        tButton[3].high = 15;
        tButton[3].name = SingleRightBack;
        tButton[3].linesize = 0;
        tButton[3].flag = 1;/* 按下状态 */
        GuiButton(&tButton[3]);
        GuiUpdateDisplayAll();
    }
    else
    {
        TdsValue = TDS_Work();
        sprintf(CurrentTdsString,"%05dus/cm",TdsValue);
        GuiRowText(30,25,110,0,CurrentTdsString);
        rt_thread_mdelay(1000);
        K0_Status = rt_sem_take(K0_Sem, 0);
        K1_Status = rt_sem_take(K1_Sem, 0);
        K2_Status = rt_sem_take(K2_Sem, 0);
        K2_Long_Status = rt_sem_take(K2_Long_Sem, 0);
        if(K2_Long_Status==RT_EOK)
        {

        }
        if(K0_Status==RT_EOK)
        {

        }
        if(K1_Status==RT_EOK)
        {

        }
        if(K2_Status==RT_EOK)
        {
            GuiClearScreen(0);
            GuiWinDeleteTop();
            GuiWinDeleteTop();
            FirstFlag[23]=0;
        }
    }
}

static void UserMain24WinFun(void *param)//password
{
    uint8_t CurrentTdsString[10]={0};
    if(FirstFlag[24] == 0)
    {
        FirstFlag[24] = 1;
        TdsValueOffsetTemp=0;

        sprintf(CurrentTdsString,"%05d",0);
        GuiRowText(30,25,110,0,CurrentTdsString);
        GuiRowText(70,25,110,0,"us/cm");

        tButton[3].x = 0;
        tButton[3].y = 50;
        tButton[3].wide = 128;
        tButton[3].high = 15;
        tButton[3].name = Back;
        tButton[3].linesize = 0;
        tButton[3].flag = 1;/* 按下状态 */
        GuiButton(&tButton[3]);
        GuiUpdateDisplayAll();
    }
    else
    {
        K0_Status = rt_sem_take(K0_Sem, 0);
        K1_Status = rt_sem_take(K1_Sem, 0);
        K2_Status = rt_sem_take(K2_Sem, 0);
        K2_Long_Status = rt_sem_take(K2_Long_Sem, 0);
        if(K2_Long_Status==RT_EOK)
        {

        }
        if(K0_Status==RT_EOK)
        {
            GuiClearScreen(0);
            GuiWinDeleteTop();
            FirstFlag[24]=0;
        }
        if(K1_Status==RT_EOK)
        {
            TdsValueOffsetTemp+=100;
            sprintf(CurrentTdsString,"%05d",TdsValueOffsetTemp);
            GuiRowText(30,25,110,0,CurrentTdsString);
            GuiUpdateDisplayAll();
        }
        if(K2_Status==RT_EOK)
        {
            if(TdsValueOffsetTemp==0)
            {
                //TdsValueZeroOffset=Tds_Work();
                rt_kprintf("Zero Offset is %d\r\n",TdsValueZeroOffset);
            }
            else

            {
                //TdsValueOffset = (double)TdsValueOffsetTemp / (double)(Tds_Work()-TdsValueZeroOffset);
                printf("Offset is %5.3f\r\n",TdsValueOffset);
            }
            GuiClearScreen(0);
            GuiWinDeleteTop();
            FirstFlag[24]=0;
        }
    }
}
static void UserMain25WinFun(void *param)
{
    if(FirstFlag[25] == 0)
    {
        FirstFlag[25] = 1;

        GuiRowText(16,15,110,0,"Backwash Filter");
        GuiRowText(53,30,57,0,"Now");

        tButton[3].x = 0;
        tButton[3].y = 50;
        tButton[3].wide = 128;
        tButton[3].high = 15;
        tButton[3].name = YesOrNo;
        tButton[3].linesize = 0;
        tButton[3].flag = 1;/* 按下状态 */
        GuiButton(&tButton[3]);
        LOG_I("Jump OK\r\n");
        GuiUpdateDisplayAll();
    }
    else
    {
        K0_Status = rt_sem_take(K0_Sem, 0);
        K1_Status = rt_sem_take(K1_Sem, 0);
        K2_Status = rt_sem_take(K2_Sem, 0);
        K2_Long_Status = rt_sem_take(K2_Long_Sem, 0);
        if(K2_Long_Status==RT_EOK)
        {

        }
        if(K0_Status==RT_EOK)
        {
            FirstFlag[25]=0;
            GuiClearScreen(0);
            GuiWinInit();
            GuiWinAdd(&userMain1Win);
        }
        if(K1_Status==RT_EOK)
        {


        }
        if(K2_Status==RT_EOK)
        {
            JumptoAutomatic();
        }
    }
}
static void UserMain26WinFun(void *param)
{
    if(FirstFlag[26] == 0)
    {
        FirstFlag[26] = 1;

        GuiRowText(25,20,110,0,"Low Voltage");
        GuiRowText(20,35,110,0,"Renew Battery");

        tButton[0].x = 0;
        tButton[0].y = 50;
        tButton[0].wide = 128;
        tButton[0].high = 15;
        tButton[0].name = SingleYes;
        tButton[0].linesize = 0;
        tButton[0].flag = 1;/* 按下状态 */
        GuiButton(&tButton[0]);
        GuiUpdateDisplayAll();
    }
    else
    {
        K0_Status = rt_sem_take(K0_Sem, 0);
        K1_Status = rt_sem_take(K1_Sem, 0);
        K2_Status = rt_sem_take(K2_Sem, 0);
        K2_Long_Status = rt_sem_take(K2_Long_Sem, 0);
        if(K2_Long_Status==RT_EOK)
        {
        }
        if(K0_Status==RT_EOK)
        {
        }
        if(K1_Status==RT_EOK)
        {
        }
        if(K2_Status==RT_EOK)
        {
            GuiClearScreen(0);
            GuiWinDeleteTop();
            FirstFlag[26]=0;
        }
    }
}
static void UserMain27WinFun(void *param)
{
    if(FirstFlag[27] == 0)
    {
        FirstFlag[27] = 1;

        GuiRowText(13,20,115,0,"Abnormal Voltage");
        GuiRowText(20,35,110,0,"Renew Battery");

        tButton[0].x = 0;
        tButton[0].y = 50;
        tButton[0].wide = 128;
        tButton[0].high = 15;
        tButton[0].name = SingleYes;
        tButton[0].linesize = 0;
        tButton[0].flag = 1;/* 按下状态 */
        GuiButton(&tButton[0]);
        GuiUpdateDisplayAll();
    }
    else
    {
        K0_Status = rt_sem_take(K0_Sem, 0);
        K1_Status = rt_sem_take(K1_Sem, 0);
        K2_Status = rt_sem_take(K2_Sem, 0);
        K2_Long_Status = rt_sem_take(K2_Long_Sem, 0);
        if(K2_Long_Status==RT_EOK)
        {
        }
        if(K0_Status==RT_EOK)
        {
        }
        if(K1_Status==RT_EOK)
        {
        }
        if(K2_Status==RT_EOK)
        {
            GuiClearScreen(0);
            GuiWinDeleteTop();
            FirstFlag[27]=0;
        }
    }
}
uint8_t Automatic_Range_Temp;
static void UserMain28WinFun(void *param)
{
    if(FirstFlag[28] == 0)
    {
        FirstFlag[28] = 1;
        Automatic_Range_Temp = Automatic_Range;
        NowButtonId=0;

        tButton[0].x = 32;
        tButton[0].y = 12;
        tButton[0].wide = 75;
        tButton[0].high = 15;
        if(Automatic_Range_Temp)
        {
            tButton[0].name = "weeks:9-52";
        }
        else {
            tButton[0].name = "weeks:0-9";
        }
        tButton[0].flag = 1;//按下状态
        GuiButton(&tButton[0]);

        tButton[1].x = 45;
        tButton[1].y = 24;
        tButton[1].wide = 35;
        tButton[1].high = 15;
        tButton[1].name = "Save";
        tButton[1].linesize = 0;
        tButton[1].flag = 0;//按下状态
        GuiButton(&tButton[1]);

        tButton[2].x = 45;
        tButton[2].y = 36;
        tButton[2].wide = 35;
        tButton[2].high = 15;
        tButton[2].name = "Back";
        tButton[2].linesize = 0;
        tButton[2].flag = 0;
        GuiButton(&tButton[2]);

        tButton[3].x = 0;
        tButton[3].y = 50;
        tButton[3].wide = 128;
        tButton[3].high = 15;
        tButton[3].name = SingleSelect;
        tButton[3].linesize = 0;
        tButton[3].flag = 1;
        GuiButton(&tButton[3]);
        GuiUpdateDisplayAll();
    }
    else
    {
        K0_Status = rt_sem_take(K0_Sem, 0);
        K1_Status = rt_sem_take(K1_Sem, 0);
        K2_Status = rt_sem_take(K2_Sem, 0);
        K2_Long_Status = rt_sem_take(K2_Long_Sem, 0);
        if(K2_Long_Status==RT_EOK)
        {

        }
        if(K1_Status==RT_EOK)
        {
            switch(NowButtonId)
            {
                case 0:
                    if(Automatic_Range_Temp)
                    {
                        Automatic_Range_Temp=0;
                        tButton[0].name = "weeks:0-9";
                    }
                    else
                    {
                        Automatic_Range_Temp=1;
                        tButton[0].name = "weeks:9-52";
                    }
                    GuiButton(&tButton[0]);
                    break;
                case 1:
                    NowButtonId = 2;
                    tButton[1].flag=0;
                    GuiButton(&tButton[1]);
                    tButton[2].flag=1;
                    GuiButton(&tButton[2]);
                    break;
                case 2:
                    NowButtonId = 1;
                    tButton[1].flag=1;
                    GuiButton(&tButton[1]);
                    tButton[2].flag=0;
                    GuiButton(&tButton[2]);
                    break;
            }
            GuiUpdateDisplayAll();
        }
        if(K0_Status==RT_EOK)
        {
            switch(NowButtonId)
            {
            case 0:
                if(Automatic_Range_Temp)
                {
                    Automatic_Range_Temp=0;
                    tButton[0].name = "weeks:0-9";
                }
                else
                {
                    Automatic_Range_Temp=1;
                    tButton[0].name = "weeks:9-52";
                }
                GuiButton(&tButton[0]);
                break;
            case 1:
                NowButtonId = 2;
                tButton[1].flag=0;
                GuiButton(&tButton[1]);
                tButton[2].flag=1;
                GuiButton(&tButton[2]);
                break;
            case 2:
                NowButtonId = 1;
                tButton[1].flag=1;
                GuiButton(&tButton[1]);
                tButton[2].flag=0;
                GuiButton(&tButton[2]);
                break;
            }
            GuiUpdateDisplayAll();
        }
        if(K2_Status==RT_EOK)
        {
            switch(NowButtonId)
            {
                case 0:
                    NowButtonId = 1;
                     tButton[0].flag=0;
                     GuiButton(&tButton[0]);
                     tButton[1].flag=1;
                     GuiButton(&tButton[1]);
                     tButton[2].flag=0;
                     GuiButton(&tButton[2]);
                     break;
                case 1:
                    Automatic_Range=Automatic_Range_Temp;
                    Flash_Set(22,Automatic_Range);
                    if(Automatic_Range==0 && Automatic_Week>9)
                    {
                        Automatic_Week=8;
                        Flash_Set(4,Automatic_Week);
                        Automatic_Day=5;
                        Flash_Set(5,Automatic_Day);
                        RTC_Automatic_Time=0;
                        LOG_D("RTC_Automatic_Time Change to 0\r\n");
                    }
                   else if(Automatic_Range==1 && Automatic_Week<9)
                    {
                        Automatic_Week=9;
                        Flash_Set(4,Automatic_Week);
                        Automatic_Day=0;
                        Flash_Set(5,Automatic_Day);
                        RTC_Automatic_Time=0;
                        LOG_D("RTC_Automatic_Time Change to 0\r\n");
                    }
                    GuiClearScreen(0);
                    GuiWinDeleteTop();
                    GuiWinDisplay();
                    FirstFlag[28]=0;
                    break;
                case 2:
                    GuiClearScreen(0);
                    GuiWinDeleteTop();
                    GuiWinDisplay();
                    FirstFlag[28]=0;
                    break;
              }
            GuiUpdateDisplayAll();
        }
    }
}
rt_uint8_t TDS_CND_Value_Temp=0;
uint8_t TdsCNDString[10]={0};
rt_uint8_t a,b;
static void UserMain29WinFun(void *param)
{
    if(FirstFlag[29] == 0)
    {
        TDS_CND_Value_Temp = TDS_CND_Value;
        FirstFlag[29] = 1;
        a = TDS_CND_Value_Temp/10;
        b = TDS_CND_Value_Temp%10;
        sprintf(TdsCNDString,"CND:%d.%d",a,b);

        tButton[0].x = 37;
        tButton[0].y = 12;
        tButton[0].wide = 55;
        tButton[0].high = 15;
        tButton[0].name = TdsCNDString;
        tButton[0].linesize = 0;
        tButton[0].flag = 1;/* 按下状态 */
        GuiButton(&tButton[0]);

        tButton[1].x = 45;
        tButton[1].y = 24;
        tButton[1].wide = 40;
        tButton[1].high = 15;
        tButton[1].name = "Save";
        tButton[1].linesize = 0;
        tButton[1].flag = 0;/* 按下状态 */
        GuiButton(&tButton[1]);

        tButton[2].x = 45;
        tButton[2].y = 36;
        tButton[2].wide = 40;
        tButton[2].high = 15;
        tButton[2].name = "Back";
        tButton[2].linesize = 0;
        tButton[2].flag = 0;/* 按下状态 */
        GuiButton(&tButton[2]);

        tButton[3].x = 0;
        tButton[3].y = 50;
        tButton[3].wide = 128;
        tButton[3].high = 15;
        tButton[3].name = SingleSelect;
        tButton[3].linesize = 0;
        tButton[3].flag = 1;/* 按下状态 */
        GuiButton(&tButton[3]);
        GuiUpdateDisplayAll();
    }
    else
    {
        K0_Status = rt_sem_take(K0_Sem, 0);
        K1_Status = rt_sem_take(K1_Sem, 0);
        K2_Status = rt_sem_take(K2_Sem, 0);
        K2_Long_Status = rt_sem_take(K2_Long_Sem, 0);
        if(K2_Long_Status==RT_EOK)
        {

        }
        if(K0_Status==RT_EOK)
        {
           switch(NowButtonId)
            {
               case 0:
                   if(TDS_CND_Value_Temp>=50)TDS_CND_Value_Temp=5;
                   else TDS_CND_Value_Temp+=1;
                   a = TDS_CND_Value_Temp/10;
                   b = TDS_CND_Value_Temp%10;
                   sprintf(TdsCNDString,"CND:%d.%d",a,b);
                   tButton[0].name = TdsCNDString;
                   GuiButton(&tButton[0]);
                   break;
               case 1:
                   NowButtonId = 2;
                   tButton[1].flag=0;
                   GuiButton(&tButton[1]);
                   tButton[2].flag=1;
                   GuiButton(&tButton[2]);
                   break;
               case 2:
                   NowButtonId = 1;
                   tButton[1].flag=1;
                   GuiButton(&tButton[1]);
                   tButton[2].flag=0;
                   GuiButton(&tButton[2]);
                   break;
             }
           GuiUpdateDisplayAll();
        }
       if(K1_Status==RT_EOK)
       {
           switch(NowButtonId)
           {
               case 0:
                   if(TDS_CND_Value_Temp<=5)TDS_CND_Value_Temp=50;
                   else TDS_CND_Value_Temp-=1;
                   a = TDS_CND_Value_Temp/10;
                   b = TDS_CND_Value_Temp%10;
                   sprintf(TdsCNDString,"CND:%d.%d",a,b);
                   tButton[0].name = TdsCNDString;
                   GuiButton(&tButton[0]);
                   break;
               case 1:
                   NowButtonId = 2;
                   tButton[1].flag=0;
                   GuiButton(&tButton[1]);
                   tButton[2].flag=1;
                   GuiButton(&tButton[2]);
                   break;
               case 2:
                   NowButtonId = 1;
                   tButton[1].flag=1;
                   GuiButton(&tButton[1]);
                   tButton[2].flag=0;
                   GuiButton(&tButton[2]);
                   break;
           }
           GuiUpdateDisplayAll();
       }
       if(K2_Status==RT_EOK)
       {
           switch(NowButtonId)
           {
               case 0:
                   NowButtonId = 1;
                   tButton[0].flag=0;
                   GuiButton(&tButton[0]);
                   tButton[1].flag=1;
                   GuiButton(&tButton[1]);
                   break;
               case 1:
                   if(TDS_CND_Value_Temp==0)TDS_CND_Value_Temp=1;
                   TDS_CND_Value=TDS_CND_Value_Temp;
                   Flash_Set(21,TDS_CND_Value_Temp);
                   GuiClearScreen(0);
                   GuiWinDeleteTop();
                   FirstFlag[29]=0;
                   break;
               case 2:
                   GuiClearScreen(0);
                   GuiWinDeleteTop();
                   FirstFlag[29]=0;
                   break;
           }
           GuiUpdateDisplayAll();
       }
    }
}
