/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-08-11     Rick       the first version
 */
#ifndef APPLICATIONS_LOW_H_
#define APPLICATIONS_LOW_H_

void Low_Init(void);
void SreenTimerInit(void);
void ScreenTimerStop(void);
void ScreenTimerRefresh(void);

#endif /* APPLICATIONS_LOW_H_ */
