#include <rtthread.h>
#include <rtdevice.h>
#include <rthw.h>
#include "12864.h"
#include "pin_config.h"
#include "lkdgui.h"

uint8_t LCD_Flag=0;//0为亮屏可操作，1为亮屏不可操作，2为息屏有显示，3为彻底关断

uint8_t GetLCD(void)
{
    return LCD_Flag;
}
void OpenLcdDisplay(void)
{
    switch(LCD_Flag)
    {
    case 1:
        LCD_Flag=0;
        rt_pin_write(LCD_BK_PIN, PIN_HIGH);
        break;
    case 2:
        LCD_Flag=1;
        rt_pin_write(LCD_BK_PIN, PIN_HIGH);
        break;
    case 3:
        LCD_Flag=1;
        LcdInit();
        GuiUpdateDisplayAll();
        break;
    default:
        break;
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
    default:
        break;
    }
}
void LCD_Restart(uint8_t backlight)
{
    ScreenTimerRefresh();
    switch(LCD_Flag)
    {
    case 3:
        lcd_gpio_init();
        if(backlight)
        {
            rt_pin_write(LCD_BK_PIN, PIN_HIGH);
        }
        LCD_Refresh();
        break;
    default:
        break;
    }
}
void CloseLcdVcc(void)
{
    LCD_Flag = 3;
    rt_pin_write(LCD_PWR_PIN,0);
    rt_pin_write(LCD_DC_PIN,0);
    rt_pin_write(LCD_RST_PIN,0);
    rt_pin_write(LCD_NSS_PIN,0);
}
void OpenLcdVcc(void)
{
    rt_pin_mode(LCD_PWR_PIN, PIN_MODE_OUTPUT);
    rt_pin_write(LCD_PWR_PIN,PIN_HIGH);
}
void CloseLcdBacklight(void)
{
    LCD_Flag = 2;
    rt_pin_write(LCD_BK_PIN, PIN_LOW);
}
void LcdInit(void)
{
    lcd_gpio_init();
    //************* Start Initial Sequence **********//
    lcd_write_cmd(0x11);                   //Sleep out
    lcd_write_cmd(0xe2);
    rt_thread_mdelay(1);               //Delay 5ms
    lcd_write_cmd(0x2c);
    rt_thread_mdelay(1);               //Delay 5ms
    lcd_write_cmd(0x2e);
    rt_thread_mdelay(1);               //Delay 5ms
    lcd_write_cmd(0x2f);
    rt_thread_mdelay(1);               //Delay 5ms
    lcd_write_cmd(0x24);
    lcd_write_cmd(0x81);
    lcd_write_cmd(0x15);
    lcd_write_cmd(0xa2);
    lcd_write_cmd(0xc8);
    lcd_write_cmd(0xa0);
    lcd_write_cmd(0x40);
    lcd_write_cmd(0xaf);
}
