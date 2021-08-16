/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-07-01     Rick       the first version
 */
#ifndef APPLICATIONS_ADCWORK_H_
#define ADCWORK_H_

void ADC_Init(void);
void Enable_MotoINT(void);
void Disable_MotoINT(void);
uint8_t Get_DC_Level(void);
uint32_t Get_Bat_Value(void);
void ADC_Pin_Init(void);
void ADC_Pin_DeInit(void);

#endif /* APPLICATIONS_ADCWORK_H_ */
