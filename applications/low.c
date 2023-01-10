/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-08-11     Rick       the first version
 */
#include <rtthread.h>
#include <rtdevice.h>
#include <stm32l4xx.h>
#include "low.h"
#include "12864.h"
#include "pin_config.h"
#include "lcd_display.h"
#include "lkdgui.h"
#include "pm.h"
#include "rthw.h"

#define DBG_TAG "LowPower"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

#define LCD_BACKLIGHT_TIME      60*1000
#define LCD_SLEEP_TIME          120*1000

rt_timer_t Screen_Backlight_Timer=RT_NULL;

uint8_t Button_Wakeup_Flag;
uint8_t Delta_Wakeup_Flag;

extern uint8_t LCD_Flag;
extern uint8_t Deltapress_Enable;

void LcdBacklightTimerCallback(void *parameter)
{
    LCD_Flag = 2;
    CloseLcdBacklight();
}
void SreenTimerInit(void)
{
    Screen_Backlight_Timer=rt_timer_create("Lcd_Backlight_Timer",LcdBacklightTimerCallback,RT_NULL,LCD_BACKLIGHT_TIME,RT_TIMER_FLAG_ONE_SHOT|RT_TIMER_FLAG_SOFT_TIMER);
    if(Screen_Backlight_Timer != RT_NULL)
    {
        rt_timer_start(Screen_Backlight_Timer);
    }
}
void ScreenTimerStop(void)
{
    rt_timer_stop(Screen_Backlight_Timer);
}
void ScreenTimerRefresh(void)
{
    rt_pm_module_delay_sleep(PM_LCD_ID,LCD_SLEEP_TIME);
    rt_timer_start(Screen_Backlight_Timer);
}
void button_wakeup(void *parameter)
{
    Button_Wakeup_Flag=1;
}
void delta_wakeup(void *parameter)
{
    Delta_Wakeup_Flag=1;
}
void FlashInit(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**SPI2 GPIO Configuration
    PB13     ------> SPI2_SCK
    PB14     ------> SPI2_MISO
    PB15     ------> SPI2_MOSI
    */
    GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    rt_pin_write(FLASH_EN,1);
    __HAL_RCC_SPI2_CLK_ENABLE();
    rt_pin_write(FLASH_CS,1);
}
void FlashDeInit(void)
{
    rt_pin_mode(FLASH_EN, PIN_MODE_OUTPUT);
    rt_pin_write(FLASH_EN,0);
    rt_pin_mode(FLASH_CS, PIN_MODE_OUTPUT);
    rt_pin_write(FLASH_CS,0);

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15);
    __HAL_RCC_SPI2_CLK_DISABLE();
}
void WiFi_Pin_Init(void)
{
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    rt_pin_mode(WIFI_RST, PIN_MODE_OUTPUT);
    rt_pin_mode(WIFI_EN, PIN_MODE_OUTPUT);
    rt_pin_write(WIFI_EN, PIN_HIGH);
    rt_pin_write(WIFI_RST, PIN_LOW);
}
void WiFi_Pin_DeInit(void)
{
    wifi_led(0);
    rt_pin_mode(WIFI_EN, PIN_MODE_OUTPUT);
    rt_pin_mode(WIFI_RST, PIN_MODE_OUTPUT);
    rt_pin_write(WIFI_EN,0);
    rt_pin_write(WIFI_RST,0);
}
void WiFi_Pin_Reset(void)
{
    rt_pin_mode(WIFI_EN, PIN_MODE_OUTPUT);
    rt_pin_mode(WIFI_RST, PIN_MODE_OUTPUT);
    rt_pin_write(WIFI_EN,0);
    rt_pin_write(WIFI_RST,1);
}
void NoneDeInit(void)
{
    rt_pin_mode(0,PIN_MODE_OUTPUT);
    rt_pin_write(0,0);
    rt_pin_mode(1,PIN_MODE_OUTPUT);
    rt_pin_write(1,0);
    rt_pin_mode(9,PIN_MODE_OUTPUT);
    rt_pin_write(9,0);
    rt_pin_mode(10,PIN_MODE_OUTPUT);
    rt_pin_write(10,0);
    rt_pin_mode(11,PIN_MODE_OUTPUT);
    rt_pin_write(11,0);
    rt_pin_mode(12,PIN_MODE_OUTPUT);
    rt_pin_write(12,0);
    rt_pin_mode(13,PIN_MODE_OUTPUT);
    rt_pin_write(13,0);
    rt_pin_mode(14,PIN_MODE_OUTPUT);
    rt_pin_write(14,0);
    rt_pin_mode(16,PIN_MODE_OUTPUT);
    rt_pin_write(16,0);
    rt_pin_mode(23,PIN_MODE_OUTPUT);
    rt_pin_write(23,0);
    rt_pin_mode(32,PIN_MODE_OUTPUT);
    rt_pin_write(32,0);
    rt_pin_mode(39,PIN_MODE_OUTPUT);
    rt_pin_write(39,0);
    rt_pin_mode(40,PIN_MODE_OUTPUT);
    rt_pin_write(40,0);
}

void DebugInit(void)
{
    __HAL_RCC_USART3_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOC_CLK_ENABLE();
    /**USART3 GPIO Configuration
    PC4     ------> USART3_TX
    PC5     ------> USART3_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}
void IRQ_Bund(void)
{
    rt_pin_attach_irq(K0, PIN_IRQ_MODE_FALLING, button_wakeup, RT_NULL);
    rt_pin_attach_irq(K1, PIN_IRQ_MODE_FALLING, button_wakeup, RT_NULL);
    rt_pin_attach_irq(K2, PIN_IRQ_MODE_FALLING, button_wakeup, RT_NULL);
    rt_pin_irq_enable(K0, PIN_IRQ_ENABLE);
    rt_pin_irq_enable(K1, PIN_IRQ_ENABLE);
    rt_pin_irq_enable(K2, PIN_IRQ_ENABLE);
    if(Deltapress_Enable)
    {
        rt_pin_attach_irq(WATER_FLOW, PIN_IRQ_MODE_FALLING, delta_wakeup, RT_NULL);
        rt_pin_irq_enable(WATER_FLOW, Deltapress_Enable);
    }
}
void IRQ_DeBund(void)
{
    rt_pin_detach_irq(K0);
    rt_pin_detach_irq(K1);
    rt_pin_detach_irq(K2);
    rt_pin_detach_irq(WATER_FLOW);
    rt_pin_irq_enable(K0, PIN_IRQ_DISABLE);
    rt_pin_irq_enable(K1, PIN_IRQ_DISABLE);
    rt_pin_irq_enable(K2, PIN_IRQ_DISABLE);
    rt_pin_irq_enable(WATER_FLOW, PIN_IRQ_DISABLE);
    rt_pin_mode(K0, PIN_MODE_INPUT);
    rt_pin_mode(K1, PIN_MODE_INPUT);
    rt_pin_mode(K2, PIN_MODE_INPUT);
    rt_pin_mode(WATER_FLOW, PIN_MODE_INPUT);
}
void BeforeSleep(void)
{
    //RGB
    Led_GpioDeInit();
    //MOTO
    Moto_Pin_DeInit();
    //Flash
    FlashDeInit();
    //ADC
    ADC_DMA_DeInit();
    //WIFI
    WiFi_Pin_DeInit();
    //LCD
    CloseLcdVcc();
    //TDS
    TDS_GpioDeInit();
    //None Pin
    NoneDeInit();
    //IRQ_Bund
    IRQ_Bund();
    //Power timer stop
    Power_Check_Stop();
}
void AfterWake(void)
{
    //WIFI
    WiFi_Pin_Init();
    //RGB
    Led_GpioInit();
    //MOTO
    Moto_Pin_Init();
    //Flash
    FlashInit();
    //LCD
    OpenLcdVcc();
    //TDS
    TDS_GpioInit();
    //Debug Uart
    DebugInit();
    //IRQ_DeBund
    IRQ_DeBund();
    //ADC
    ADC_DMA_Init();
}
void user_notify(rt_uint8_t event, rt_uint8_t mode, void *data)
{
    if (event == RT_PM_ENTER_SLEEP)
    {
        Delta_Wakeup_Flag = 0;
        Button_Wakeup_Flag=0;
        LOG_D("Go to Sleep\r\n");
        BeforeSleep();
    }
    else
    {
        Power_Check_Start();
        if(Button_Wakeup_Flag)
        {
            Button_Wakeup_Flag = 0;
            LCD_Restart(1);
            LOG_D("Button Wake Up Now\r\n");
        }
        else if(Delta_Wakeup_Flag)
        {
            Delta_Wakeup_Flag = 0;
            LCD_Restart(0);
            LOG_D("Delta Wake Up Now\r\n");
        }
    }
}
void Low_Init(void)
{
    rt_pm_notify_set(user_notify, RT_NULL);
    SreenTimerInit();
    ScreenTimerRefresh();
    rt_pm_request(PM_SLEEP_MODE_DEEP);
    rt_pm_release(PM_SLEEP_MODE_NONE);
}
