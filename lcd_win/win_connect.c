/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-04-19     Rick       the first version
 */
#include "rtthread.h"
#include "lkdgui.h"

static void win_wifi_win_callback(void *parameter);
static void win_wifi_input_callback(void *param);

extern uint8_t Setting_Language;

lkdWin win_wifi_connect = {
    .x = 0,
    .y = 0,
    .wide = 128,
    .hight = 64,
    .param = NULL,
    .WindowFunction = win_wifi_win_callback,
    .InputFunction = win_wifi_input_callback,
};

static void win_wifi_win_callback(void *parameter)
{
    if(!Setting_Language)
    {
        GuiRowText(0,15,128,FONT_MID,"Wlan Verbindung");
        GuiRowText(0,27,128,FONT_MID,"wird aufgebaut");
        GuiRowText(0,40,128,FONT_MID,"-->-->-->-->");
    }
    else
    {
        GuiRowText(0,15,128,FONT_MID,"Connecting");
        GuiRowText(0,27,128,FONT_MID,"wifi");
        GuiRowText(0,40,128,FONT_MID,"-->-->-->-->");
    }
    GuiUpdateDisplayAll();
}
static void win_wifi_input_callback(void *param)
{
    if(Get_Up_Click() == RT_EOK)
    {

    }
    else if(Get_Down_Click() == RT_EOK)
    {

    }
    else if(Get_Enter_Click() == RT_EOK)
    {

    }
    else if(Get_Long_Click() == RT_EOK)
    {

    }
}

void wifi_connect_show(uint8_t state)
{
    if(GuiGetTopWin() == &win_wifi_connect)
    {
        GuiClearScreen(0);
        switch(state)
        {
        case 0:
            if(!Setting_Language)
            {
                GuiRowText(0,15,128,FONT_MID,"Wlan Verbindung");
                GuiRowText(0,27,128,FONT_MID,"wird aufgebaut");
                GuiRowText(0,40,128,FONT_MID,"-->-->-->-->");
            }
            else
            {
                GuiRowText(0,15,128,FONT_MID,"Connecting");
                GuiRowText(0,27,128,FONT_MID,"wifi");
                GuiRowText(0,40,128,FONT_MID,"-->-->-->-->");
            }
            break;
        case 1:
            if(!Setting_Language)
            {
                GuiRowText(0,15,128,FONT_MID,"Wlan Verbindung");
                GuiRowText(0,27,128,FONT_MID,"fehlgeschlagen");
            }
            else
            {
                GuiRowText(0,15,128,FONT_MID,"Wifi connection");
                GuiRowText(0,27,128,FONT_MID,"failed");
            }
            break;
        case 2:
            if(!Setting_Language)
            {
                GuiRowText(0,15,128,FONT_MID,"Wlan Verbindung");
                GuiRowText(0,27,128,FONT_MID,"erfolgreich");
            }
            else
            {
                GuiRowText(0,15,128,FONT_MID,"Wifi connection");
                GuiRowText(0,27,128,FONT_MID,"success");
            }
            break;
        default:
            break;
        }
        GuiUpdateDisplayAll();
    }
}
void wifi_connect_win_add(void)
{
    lcd_mutex_lock();
    if(GuiGetTopWin() != &win_wifi_connect)
    {
        GuiWinAdd(&win_wifi_connect);
    }
    wifi_connect_show(0);
    lcd_mutex_unlock();
}
void wifi_connect_win_del(void)
{
    if(GuiGetTopWin() == &win_wifi_connect)
    {
        lcd_mutex_lock();
        GuiWinDel(&win_wifi_connect);
        lcd_mutex_unlock();
    }
}
