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
void led_select(uint8_t type)//red:1,green:2,blue:3
{
    led_state = type;
    switch(type)
    {
    case 0:
        agile_led_stop(LED_R);
        agile_led_stop(LED_G);
        break;
    case 1:
        agile_led_stop(LED_G);
        agile_led_set_light_mode(LED_R, "500,500", -1);
        agile_led_start(LED_R);
        break;
    case 2:
        agile_led_stop(LED_R);
        agile_led_set_light_mode(LED_G, "100,0", -1);
        agile_led_start(LED_G);
        break;
    case 3:
        agile_led_stop(LED_R);
        agile_led_stop(LED_G);
        agile_led_set_light_mode(LED_B, "100,0", -1);
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
    switch(type)
    {
    case 0://熄灭全部
        wifi_status = type;
        agile_led_stop(LED_B);
        break;
    case 1://尝试连接wifi
        wifi_status = type;
        agile_led_set_light_mode(LED_B, "150,200,150,500", -1);
        agile_led_start(LED_B);
        break;
    case 2://尝试连接azure
        wifi_status = type;
        agile_led_set_light_mode(LED_B, "150,150", -1);
        agile_led_start(LED_B);
        break;
    case 3://连接成功
        wifi_status = type;
        agile_led_set_light_mode(LED_B, "200,0", -1);
        agile_led_start(LED_B);
        break;
    case 4://AP关
        if(wifi_status == 5)
        {
            wifi_status = 4;
            agile_led_stop(LED_B);
        }
        break;
    case 5://AP开
        if(wifi_status == 0 || wifi_status == 4)
        {
            wifi_status = 5;
            agile_led_set_light_mode(LED_B, "1000,1000", -1);
            agile_led_start(LED_B);
        }
        break;
    default:
        break;
    }
}
