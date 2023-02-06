/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-08-04     Rick       the first version
 */
#include "rtthread.h"
#include "agile_led.h"
#include "pin_config.h"
#include "led.h"
#include <stm32l4xx.h>

static agile_led_t *LED_R = RT_NULL;
static agile_led_t *LED_G = RT_NULL;
static agile_led_t *LED_B = RT_NULL;

static uint8_t led_state  = 0;
extern uint8_t WIFI_AP_Enable;

uint8_t wifi_status;

void Led_Init(void)
{
    if(LED_R == RT_NULL)
    {
        LED_R = agile_led_create(LED_R_PIN, PIN_LOW, "200,200", -1);
    }
    if(LED_G == RT_NULL)
    {
        LED_G = agile_led_create(LED_G_PIN, PIN_LOW, "200,200", -1);
    }
    if(LED_B == RT_NULL)
    {
        LED_B = agile_led_create(LED_B_PIN, PIN_LOW, "200,200", -1);
    }
}
void led_select(uint8_t type)//red:1,green:2
{
    led_state = type;
    switch(type)
    {
    case 0://全关
        agile_led_stop(LED_R);
        agile_led_stop(LED_G);
        break;
    case 1://红灯闪烁
        agile_led_stop(LED_G);
        agile_led_set_light_mode(LED_R, "500,500", -1);
        agile_led_start(LED_R);
        break;
    case 2://绿灯常亮
        agile_led_stop(LED_R);
        agile_led_stop(LED_G);
        agile_led_on(LED_G);
        break;
    case 3://绿灯熄灭
        agile_led_off(LED_G);
        break;
    }
}
void Led_GpioInit(void)
{
    if(led_state == 0 )
    {
        led_select(0);
    }
    agile_led_resume(LED_R);
    agile_led_resume(LED_G);
    agile_led_resume(LED_B);
}

void Led_GpioDeInit(void)
{
    agile_led_pause(LED_R);
    agile_led_pause(LED_G);
    agile_led_pause(LED_B);
    rt_pin_mode(LED_G_PIN, PIN_MODE_OUTPUT);
    rt_pin_write(LED_G_PIN, 1);
    rt_pin_mode(LED_R_PIN, PIN_MODE_OUTPUT);
    rt_pin_write(LED_R_PIN, 1);
    rt_pin_mode(LED_B_PIN, PIN_MODE_OUTPUT);
    rt_pin_write(LED_B_PIN, 1);
}

void wifi_led(uint8_t type)
{
    wifi_status = type;
    switch(type)
    {
    case 0://熄灭全部
        agile_led_stop(LED_B);
        break;
    case 1://AP模式
        if(WIFI_AP_Enable)
        {
            agile_led_set_light_mode(LED_B, "1000,1000", -1);
            agile_led_start(LED_B);
        }
        else
        {
            agile_led_stop(LED_B);
        }
        break;
    case 2://连接路由器
        agile_led_set_light_mode(LED_B, "150,200,150,500", -1);
        agile_led_start(LED_B);
        break;
    case 3://连接微软云
        agile_led_set_light_mode(LED_B, "150,150", -1);
        agile_led_start(LED_B);
        break;
    case 4://连接成功
        agile_led_set_light_mode(LED_B, "200,0", -1);
        agile_led_start(LED_B);
        break;
    default:
        break;
    }
}
