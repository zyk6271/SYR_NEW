#ifndef __TDSWORK_H__
#define __TDSWORK_H__
#endif
#include "stdint.h"

void TDS_Init(void);
uint32_t TDS_Work(void);
uint32_t GetTDS(void);
void TDS_WarnSet(uint8_t value);
uint8_t TDS_WarnGet(void);
void TDS_GpioInit(void);
void TDS_GpioDeInit(void);
