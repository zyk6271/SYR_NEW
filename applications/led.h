/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-08-04     Rick       the first version
 */
#ifndef APPLICATIONS_LED_H_
#define APPLICATIONS_LED_H_

void Led_Init(void);
void led_select(uint8_t type);
void Led_GpioInit(void);
void Led_GpioDeInit(void);

#endif /* APPLICATIONS_LED_H_ */
