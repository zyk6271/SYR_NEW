#ifndef __MOTOCONTROL_H__
#define __MOTOCONTROL_H__
#endif

#define MOTO_STOP       1<<0
#define MOTO_FORWARD    1<<1
#define MOTO_BACK       1<<2
#define MOTO_RESET      1<<3

uint8_t Get_MotoValid(void);
void Moto_Init(void);
uint8_t Moto_Cycle(void);
void Moto_Overload(void);
void Moto_Forward(void);
void Moto_Back(void);
void Moto_Stop(void);
void Moto_Pin_Init(void);
void Moto_Pin_DeInit(void);
void Moto_Current_Detect(void);
void Moto_Reset(void);
