#ifndef __LCD_DISPLAY_H
#define	__LCD_DISPLAY_H
#endif

struct rt_event lcd_jump_event;
#define BACK (1 << 0)
#define TDS (1 << 1)
#define STALLING (1 << 2)
#define FINISH (1 << 3)
#define NOMOTO (1 << 4)
#define EXIT (1 << 5)

void LCD_Init(void);
void JumptoReminder(void);
void JumptoAutomatic(void);
void JumptoDelta(void);
void JumpToBatteryEmpty(void);
void JumpToBatteryNew(void);
void Jump_TDS(void);
void Jump_STALLING(void);
void Jump_FINISH(void);
void Jump_NOMOTO(void);
void Jump_EXIT(void);
void JumptoBackwash(void);
void userAppPortInit(void);
void LcdtoBackwash(void);
void JumptoMainWin(void);
void LCD_Refresh(void);

