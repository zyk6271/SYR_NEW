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

static void UserMain1WinFun(void *param);
static void UserMain1InputFun(void *param);
static void UserMain2WinFun(void *param);
static void UserMain2InputFun(void *param);
static void UserMain3WinFun(void *param);
static void UserMain3InputFun(void *param);
static void UserMain4WinFun(void *param);
static void UserMain4InputFun(void *param);
static void UserMain5WinFun(void *param);
static void UserMain5InputFun(void *param);
static void UserMain6WinFun(void *param);
static void UserMain6InputFun(void *param);
static void UserMain7WinFun(void *param);
static void UserMain7InputFun(void *param);
static void UserMain8WinFun(void *param);
static void UserMain8InputFun(void *param);
static void UserMain9WinFun(void *param);
static void UserMain9InputFun(void *param);
static void UserMain10WinFun(void *param);
static void UserMain10InputFun(void *param);
static void UserMain11WinFun(void *param);
static void UserMain11InputFun(void *param);
static void UserMain12WinFun(void *param);
static void UserMain12InputFun(void *param);
static void UserMain13WinFun(void *param);
static void UserMain13InputFun(void *param);
static void UserMain14WinFun(void *param);
static void UserMain14InputFun(void *param);
static void UserMain16WinFun(void *param);
static void UserMain16InputFun(void *param);
static void UserMain17WinFun(void *param);
static void UserMain17InputFun(void *param);
static void UserMain18WinFun(void *param);
static void UserMain18InputFun(void *param);
static void UserMain19WinFun(void *param);
static void UserMain19InputFun(void *param);
static void UserMain20WinFun(void *param);
static void UserMain20InputFun(void *param);
static void UserMain23WinFun(void *param);
static void UserMain23InputFun(void *param);
static void UserMain25WinFun(void *param);
static void UserMain25InputFun(void *param);
static void UserMain26WinFun(void *param);
static void UserMain26InputFun(void *param);
static void UserMain27WinFun(void *param);
static void UserMain27InputFun(void *param);
static void UserMain28WinFun(void *param);
static void UserMain28InputFun(void *param);
static void UserMain29WinFun(void *param);
static void UserMain29InputFun(void *param);
static void UserMain30WinFun(void *param);
static void UserMain30InputFun(void *param);
static void UserMain31WinFun(void *param);
static void UserMain31InputFun(void *param);
static void UserMain32WinFun(void *param);
static void UserMain32InputFun(void *param);


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

