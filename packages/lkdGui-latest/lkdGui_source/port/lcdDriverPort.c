#include "rtthread.h"
#include "rtdevice.h"
#include "pin_config.h"
#include "lkdGui.h"
#include "12864.h"

uint8_t x_to_page[GUIXMAX][8];

void GuiUpdateDisplayAll(void)
{
    uint32_t x,y;
    uint8_t send_buf[128];
    for(y = 0; y < 8; y++)
    {
        lcd_goto_address(0,y);
        for(x = 0;x <128; x++)
        {
            send_buf[x] = x_to_page[x][y];
        }
        lcd_write_buffer(send_buf,128);
    }
}

void GuiRangeUpdateDisplay(lkdCoord beginx, lkdCoord beginy,lkdCoord endx, lkdCoord endy)
{

}

void GuiDrawPoint(lkdCoord x, lkdCoord y, lkdColour color)
{
  	if(color == CWHITLE)
    {
        x_to_page[x][y/8] &= ~(1 << (y % 8));
    }
    else
    {
        x_to_page[x][y/8] |= 1 << (y % 8);
    }
}

void GuiFlipPoint(lkdCoord x, lkdCoord y, lkdColour color)
{
  	if(color == CWHITLE)
    {
        x_to_page[y][x/8] &= ~(1 << (x % 8));
    }
    else
    {
        x_to_page[y][x/8] |= 1 << (x % 8);
    }
}

void GuiReadPoint(lkdCoord x, lkdCoord y, lkdColour *pColor)
{

}

