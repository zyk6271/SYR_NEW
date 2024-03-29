/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-08-25     Rick       the first version
 */
#ifndef APPLICATIONS_PIN_CONFIG_H_
#define APPLICATIONS_PIN_CONFIG_H_

//LED
#define LED_R_PIN                   21
#define LED_G_PIN                   22
#define LED_B_PIN                   20

//LCD
#define LCD_RST_PIN                 19
#define LCD_DC_PIN                  50
#define LCD_SCK_PIN                 44
#define LCD_SDA_PIN                 43
#define LCD_BK_PIN                  42
#define LCD_PWR_PIN                 15
#define LCD_NSS_PIN                 23

//Key
#define K0                          26
#define K1                          18
#define K2                          27

//MOTO.
#define MOTO_MODE                   5
#define MOTO_VCC                    4
#define MOTO_IN1                    7
#define MOTO_IN2                    6
#define MOTO_LEFT                   25
#define MOTO_RIGHT                  24

//Flash
#define FLASH_EN                    38
#define FLASH_MOSI                  31
#define FLASH_MISO                  30
#define FLASH_SCLK                  29
#define FLASH_CS                    28

//WIFI
#define WIFI_EN                     41
#define WIFI_RST                    8

//WATER FLOW
#define WATER_FLOW                  35

//ADC
#define ADC_BAT                     34
#define ADC_DC                      33
#define ADC_MOTO                    17

#endif /* APPLICATIONS_PIN_CONFIG_H_ */
