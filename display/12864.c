#include <rtthread.h>
#include <rtdevice.h>
#include <rthw.h>
#include "12864.h"
#include "pin_config.h"
#include "lkdgui.h"

uint8_t LCD_Flag=0;//0为亮屏可操作，1为亮屏不可操作，2为息屏有显示，3为彻底关断
extern const unsigned char ASCII_5_7[95][5];
uint8_t GetLCD(void)
{
    return LCD_Flag;
}
void delay(int i)
{
    int j,k;
    for(j=0;j<i;j++);
    for(k=0;k<110;k++);
}
void delay_us(int i)
{
    int j,k;
    for(j=0;j<i;j++);
    for(k=0;k<1;k++);
}
static void LcdGpioConfig(void)
{
    rt_pin_mode(LCD_NSS, PIN_MODE_OUTPUT);
    rt_pin_write(LCD_NSS, 1);
    rt_pin_mode(LCD_BL, PIN_MODE_OUTPUT);
    rt_pin_mode(LCD_RS, PIN_MODE_OUTPUT);
    rt_pin_mode(LCD_RST, PIN_MODE_OUTPUT);
    rt_pin_write(LCD_RST, 1);
    rt_pin_mode(LCD_SDA, PIN_MODE_OUTPUT);
    rt_pin_mode(LCD_CLK, PIN_MODE_OUTPUT);
    rt_pin_mode(LCD_EN, PIN_MODE_OUTPUT);
}
void WriteLcdCommand(uint8_t cmd)
{
    LCD_CS_LOW();
    LCD_RS_LOW();
    for(uint8_t i=0;i<8;i++)
    {
        LCD_CLK_LOW();
        if(cmd&0x80)
        {
            LCD_SDA_HIGH();
        }
        else
        {
            LCD_SDA_LOW();
        }
        LCD_CLK_HIGH();
        delay_us(2);
        cmd<<=1;
    }
    LCD_CS_HIGH();
}
void WriteLcdData(uint8_t data)
{
    LCD_CS_LOW();
    LCD_RS_HIGH();
    for(uint8_t i=0;i<8;i++)
    {
        LCD_CLK_LOW();
        if(data&0x80)
        {
            LCD_SDA_HIGH();
        }
        else
        {
            LCD_SDA_LOW();
        }
        LCD_CLK_HIGH();
        delay_us(2);
        data<<=1;
    }
    LCD_CS_HIGH();
}
void SetLcdRow(uint8_t row)
{
    WriteLcdCommand(0xb0+row);
}
void SetLcdCol(uint8_t col)
{
    WriteLcdCommand(((col>>4)&0x0f)+0x10);
    WriteLcdCommand(col&0x0f);
}
void lcd_address(char page,char column)
{
	WriteLcdCommand(0xb0+page);
	WriteLcdCommand(((column>>4)&0x0f)+0x10);
	WriteLcdCommand(column&0x0f);
}
void lcd_x_address(char column)
{
	WriteLcdCommand(((column>>4)&0x0f)+0x10);
	WriteLcdCommand(column&0x0f);
}
void lcd_y_address(char page)
{
	WriteLcdCommand(0xb0+page);
}
void clear_screen(void)
{
    uint8_t i,j;
    for(i=0;i<8;i++)
    {
        lcd_address(i,0);
        for(j=0;j<128;j++)
        {
            WriteLcdData(0);
        }
    }
}
MSH_CMD_EXPORT(clear_screen,clear_screen);
void full_screen(void)
{
    uint8_t i,j;
    for(i=0;i<8;i++)
    {
        lcd_address(i,0);
        for(j=0;j<128;j++)
        {
            WriteLcdData(0xff);
        }
    }
}
MSH_CMD_EXPORT(full_screen,full_screen);
void OpenLcdDisplay(void)
{
    switch(LCD_Flag)
    {
        case 1:
            LCD_Flag=0;
            LCD_BL_HIGH();
            break;
        case 2:
            LCD_Flag=1;
            LCD_BL_HIGH();
            break;
        case 3:
            LCD_Flag=1;
            LcdInit();
            break;
        default:break;
    }
}
void OpenLcdDisplayNoBL(void)
{
    switch(LCD_Flag)
    {
        case 3:
            LCD_Flag=2;
            LcdInit();
            break;
        default:break;
    }
}
void LcdDeinit(void)
{
    rt_pin_mode(LCD_EN, PIN_MODE_OUTPUT);
    rt_pin_write(LCD_EN,0);
}
void CloseLcdVcc(void)
{
    LCD_Flag = 3;
    rt_pin_write(LCD_EN,0);
    rt_pin_write(LCD_RS,0);
    rt_pin_write(LCD_RST,0);
    rt_pin_mode(LCD_SDA, PIN_MODE_OUTPUT);
    rt_pin_write(LCD_SDA,0);
    rt_pin_write(LCD_CLK,0);
    rt_pin_write(LCD_NSS,0);
}
void OpenLcdVcc(void)
{
    rt_pin_mode(LCD_EN, PIN_MODE_OUTPUT);
    rt_pin_write(LCD_EN,1);
}
void CloseLcdBacklight(void)
{
    LCD_Flag = 2;
    LCD_BL_LOW();
}
void LcdRst(void)
{
    LcdGpioConfig();
    LCD_RST_LOW();
    delay_us(5);
    LCD_RST_HIGH();
    delay_us(5);
}
void LcdInit(void)
{
	LcdGpioConfig();
	LCD_RST_LOW();
    delay(100);
    LCD_EN_HIGH();
    rt_thread_mdelay(10);
	LCD_RST_HIGH();
	delay(100);
	WriteLcdCommand(0xe2);
	delay(5);
	WriteLcdCommand(0x2c);
	delay(5);
	WriteLcdCommand(0x2e);
	delay(5);
	WriteLcdCommand(0x2f);
	delay(5);
	WriteLcdCommand(0x24);
	WriteLcdCommand(0x81);
	WriteLcdCommand(0x15);
	WriteLcdCommand(0xa2);
	WriteLcdCommand(0xc8);
	WriteLcdCommand(0xa0);
	WriteLcdCommand(0x40);
	WriteLcdCommand(0xaf);
}
void LcdRefresh(void)
{
    WriteLcdCommand(0x2f);
    rt_thread_mdelay(5);
    WriteLcdCommand(0x24);
    WriteLcdCommand(0x81);
    WriteLcdCommand(0x13);
    WriteLcdCommand(0xa2);
    WriteLcdCommand(0xc8);
    WriteLcdCommand(0xa0);
    WriteLcdCommand(0x40);
    WriteLcdCommand(0xaf);
}
