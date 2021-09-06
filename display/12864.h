#ifndef __12864_H
#define	__12864_H
#include "rtthread.h"
#include "rtdevice.h"

#define LCD_EN_HIGH()       rt_pin_write(LCD_EN,1);
#define LCD_EN_LOW()        rt_pin_write(LCD_EN,0);

#define LCD_CS_HIGH()       rt_pin_write(LCD_NSS,1);
#define LCD_CS_LOW()        rt_pin_write(LCD_NSS,0);

#define LCD_RS_HIGH() 	    rt_pin_write(LCD_RS,1);
#define LCD_RS_LOW() 		rt_pin_write(LCD_RS,0);

#define LCD_RST_HIGH() 	    rt_pin_write(LCD_RST,1);
#define LCD_RST_LOW()	 	rt_pin_write(LCD_RST,0);

#define LCD_CLK_HIGH() 	    rt_pin_write(LCD_CLK,1);
#define LCD_CLK_LOW() 	    rt_pin_write(LCD_CLK,0);

#define LCD_SDA_HIGH()  	rt_pin_write(LCD_SDA,1);
#define LCD_SDA_LOW() 	    rt_pin_write(LCD_SDA,0);

#define LCD_BL_HIGH() 	    rt_pin_write(LCD_BL,1);
#define LCD_BL_LOW() 		rt_pin_write(LCD_BL,0);

#define OPEN_LCDDISPLAY 	0xAD
#define CLOSE_LCDDISPLAY	0xAC

void LcdInit(void);
uint8_t GetLCD(void);
void SetLcdRow(uint8_t row);
void SetLcdCol(uint8_t col);
void WriteLcdData(uint8_t data);
void WriteLcdCommand(uint8_t cmd);
void OpenLcdDisplay(void);
void OpenLcdDisplayNoBL(void);
void OpenLcdDisplayNoBL(void);
void CloseLcdDisplay(void);
void Write8Dots(uint8_t Data);
void lcd_address(char page,char column);
void clear_screen(void);
void lcd_y_address(char page);
void lcd_x_address(char column);
void LcdDeinit(void);
void CloseLcdVcc(void);
void CloseLcdBacklight(void);
void BlinkBL(void);
#endif

