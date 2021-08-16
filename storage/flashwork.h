/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-07-13     Rick       the first version
 */
#ifndef APPLICATIONS_FLASHWORK_H_
#define APPLICATIONS_FLASHWORK_H_

#include "rtthread.h"

int Flash_Init(void);
uint32_t Flash_Get(uint8_t id);
void Flash_Set(uint8_t id,uint32_t value);
void Flash_Clear(void);

#endif /* APPLICATIONS_FLASHWORK_H_ */
