/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-10-20     Rick       the first version
 */
#ifndef WIFI_WIFIWORK_H_
#define WIFI_WIFIWORK_H_

void wifi_uart_parse(uint8_t command,uint8_t value);
void counter_upload(uint8_t id,uint32_t value);

#endif /* WIFI_WIFIWORK_H_ */
