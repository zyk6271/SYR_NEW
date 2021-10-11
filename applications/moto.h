#ifndef __MOTOCONTROL_H__
#define __MOTOCONTROL_H__
#endif

#define Event_Moto_Free             (1 << 0)
#define Event_Moto_Forward          (1 << 1)
#define Event_Moto_Back             (1 << 2)
#define Event_Moto_Over             (1 << 3)
#define Event_Moto_TDS              (1 << 4)

uint8_t Get_MotoValid(void);
void Moto_Init(void);
void Moto_Cycle(void);
void Moto_Overload(void);
void Moto_Forward(void);
void Moto_Back(void);
void Moto_Stop(void);
void Moto_Pin_Init(void);
void Moto_Pin_DeInit(void);
void Moto_Current_Detect(void);

