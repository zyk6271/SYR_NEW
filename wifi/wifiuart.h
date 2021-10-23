#ifndef __WIFIUART_H__
#define __WIFIUART_H__
#endif
#include "stdint.h"

void WIFI_Uart_Init(void);
void wifi_uart_send(uint8_t command,uint32_t value);
