#include <drv_lcd.h>
/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author            Notes
 * 2018-08-14    flybreak           the first version
 * 2018-09-18    balanceTWK         add sleep mode function
 * 2018-09-27    ZYLX               optimized display speed
 * 2021-10-17    Meco Man           add lcd_fill_array()
 * 2021-10-17    Meco Man           fix the bug of lcd_fill_array()
 */
#include <rtthread.h>
#include <rtdevice.h>
#include <drv_spi.h>
#include "pin_config.h"

#define DBG_TAG    "drv.lcd"
#define DBG_LVL    DBG_INFO
#include <rtdbg.h>

static struct rt_spi_device *spi_dev_lcd;

static int rt_hw_lcd_config(void)
{
    if(spi_dev_lcd == RT_NULL)
    {
        rt_hw_soft_spi_device_attach("sspi1", "lcd", "PB7");

        spi_dev_lcd = (struct rt_spi_device *)rt_device_find("lcd");

        struct rt_spi_configuration cfg;
        cfg.data_width = 8;
        cfg.mode = RT_SPI_MASTER | RT_SPI_MODE_0 | RT_SPI_MSB;
        cfg.max_hz = 10 * 1000 * 1000; /* 1M,SPI max 42MHz,lcd 4-wire spi */

        rt_spi_configure(spi_dev_lcd, &cfg);
    }

    return RT_EOK;
}
INIT_APP_EXPORT(rt_hw_lcd_config);

rt_err_t lcd_write_cmd(const rt_uint8_t cmd)
{
    rt_size_t len;

    rt_pin_write(LCD_DC_PIN, PIN_LOW);

    len = rt_spi_send(spi_dev_lcd, &cmd, 1);

    if (len != 1)
    {
        LOG_I("lcd_write_cmd error. %d", len);
        return -RT_ERROR;
    }
    else
    {
        return RT_EOK;
    }
}

rt_err_t lcd_write_byte(const rt_uint8_t data)
{
    rt_size_t len;

    rt_pin_write(LCD_DC_PIN, PIN_HIGH);

    len = rt_spi_send(spi_dev_lcd, &data, 1);

    if (len != 1)
    {
        LOG_I("lcd_write_byte error. %d", len);
        return -RT_ERROR;
    }
    else
    {
        return RT_EOK;
    }
}

rt_err_t lcd_write_buffer(const rt_uint8_t* data,uint8_t size)
{
    rt_size_t len;

    rt_pin_write(LCD_DC_PIN, PIN_HIGH);

    len = rt_spi_send(spi_dev_lcd, data, size);

    if (len != size)
    {
        LOG_I("lcd_write_byte error. %d", len);
        return -RT_ERROR;
    }
    else
    {
        return RT_EOK;
    }
}
void lcd_gpio_init(void)
{
    rt_pin_mode(LCD_BK_PIN,PIN_MODE_OUTPUT);
    rt_pin_write(LCD_BK_PIN, PIN_HIGH);
    rt_pin_mode(LCD_PWR_PIN,PIN_MODE_OUTPUT);
    rt_pin_write(LCD_PWR_PIN, PIN_HIGH);
    rt_pin_mode(LCD_RST_PIN,PIN_MODE_INPUT);
    rt_pin_mode(LCD_DC_PIN,PIN_MODE_INPUT);
    rt_pin_mode(LCD_NSS_PIN,PIN_MODE_INPUT);

    rt_pin_mode(LCD_NSS_PIN, PIN_MODE_OUTPUT);

    rt_pin_mode(LCD_DC_PIN, PIN_MODE_OUTPUT);
    rt_pin_write(LCD_DC_PIN, PIN_HIGH);

    rt_pin_mode(LCD_RST_PIN, PIN_MODE_OUTPUT);
    rt_pin_write(LCD_RST_PIN, PIN_HIGH);

    rt_pin_write(LCD_RST_PIN, PIN_LOW);
    rt_thread_mdelay(5);               //Delay 5ms
    rt_pin_write(LCD_RST_PIN, PIN_HIGH);
    rt_thread_mdelay(5);               //Delay 5ms
}

int rt_hw_lcd_init(void)
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

    return RT_EOK;
}

void lcd_goto_address(char x,char y)
{
    lcd_write_cmd(0xB0 | y);
    lcd_write_cmd(0x10 | (x >> 4));
    lcd_write_cmd(0x00 | (x & 0xf));
}

