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
    switch(type)
    {
    case 0:
        agile_led_stop(LED_R);
        agile_led_stop(LED_G);
        agile_led_stop(LED_B);
        break;
    case 1:
        agile_led_stop(LED_G);
        agile_led_stop(LED_B);
        agile_led_set_light_mode(LED_R, "500,500", -1);
        agile_led_start(LED_R);
        break;
    case 2:
        agile_led_stop(LED_R);
        agile_led_stop(LED_B);
        agile_led_set_light_mode(LED_G, "100,0", -1);
        agile_led_start(LED_G);
        break;
    case 3:
        agile_led_stop(LED_R);
        agile_led_stop(LED_G);
        agile_led_set_light_mode(LED_B, "100,0", -1);
        agile_led_start(LED_B);
        break;
    }
}
void Led_GpioInit(void)
{
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
