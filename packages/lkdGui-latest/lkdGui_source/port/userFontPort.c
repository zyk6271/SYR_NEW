/**
  * @file   userFontPort.c
  * @author  guoweilkd
  * @version V0.0.0
  * @date    2018/04/18
  * @brief  lkdGui���������ӿڣ�����ֲ����亯���ľ������ݡ�
  */

#include "lkdGui.h"

/* �������ʵ�� */
static lkdFont defaultFont;

/**
  *@brief ��ȡ������ģ
  *@param  code1,code2 ���ֱ���[H,L]
  *@param  pBuff ��ģbuff
  *@retval 0
  */
static uint8_t GetDfontData(uint8_t code1, uint8_t code2,uint8_t *pBuff)
{
    /* ����û����� */
    return 0;
}

/**
  *@brief ��ȡASCII��ģ
  *@param  code1, ����
  *@param  pBuff ��ģbuff
  *@retval 0
  */
static uint8_t GetSfontData(uint8_t code1, uint8_t *pBuff)
{
    extern uint8_t GetFontASCII_5_7(uint8_t code1, uint8_t *pBuff);
    GetFontASCII_5_7(code1, pBuff);
    return 0;
}

/**
  *@brief �����ʼ��
  *@param  None
  *@retval None
  */
void defaultFontInit(void)
{
    /* ��������Ҫ������Ӧ���޸� */

    /* ��buff�Ĵ�С�������ģ��С���� */
	static uint8_t dataBuff[12*2];

	defaultFont.name = "user";
	defaultFont.dhigh = 12;
	defaultFont.dwide = 12;
	defaultFont.shigh = 5;
	defaultFont.swide = 7;
	defaultFont.pZmBuff = dataBuff;
	defaultFont.getDfont = GetDfontData;
	defaultFont.getSfont = GetSfontData;

    /* ����ΪϵͳĬ������ */
    GuiFontSet(&defaultFont);
    GuiSetbackcolor(CWHITLE);
    GuiSetForecolor(CBLACK);
}
