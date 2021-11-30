/**
  * @file   lkdGuiFont.c
  * @author  guoweilkd
  * @version V1.1.0
  * @date    2018/03/27
  * @brief  gwGui�����ļ�
  */

#include "lkdGui.h"

/* lkd���� */
static lkdFont *thisFont;

/**
  *@brief  ���õ�ǰҪʹ�õ�����ָ��
  *@param  pFont Ҫʹ�õ�����ָ��
  *@retval None
  */
void GuiFontSet(lkdFont *pFont)
{
	thisFont = pFont;
}

/**
  *@brief  ��ȡ��ǰ����
  *@param  None
  *@retval ��ǰ����ָ��
  */
lkdFont *GetCurrentFont(void)
{
	return thisFont;
}

/**
  *@brief  д�ı�
  *@param  tInfo д�ı������Ϣ
  *@param  str �ı�
  *@retval None
  */
void GuiText(fontTextInfo *tInfo, uint8_t *str)
{
	uint16_t endx = tInfo->x + tInfo->wide;
	uint16_t endy = tInfo->y + tInfo->high;
	lkdCoord tx = tInfo->x + tInfo->beginOffset;
	uint16_t fontWide;
	while(*str != '\0'){
		/* ���㵱ǰ����Ŀ�� */
		if(*str > 0x7f){
			fontWide = thisFont->dwide;
		}
		else{
			fontWide = thisFont->swide;
		}
		/* ���㵱ǰx��ӡ���� */
		if(tx + fontWide + tInfo->wInterval > endx){
			tx = tInfo->x;
			tInfo->y += (thisFont->dhigh + tInfo->hInterval);
		}
		/* �Ƿ�ﵽ�������� */
		if(tInfo->y + thisFont->dhigh + tInfo->hInterval > endy){
			break;
		}
		if(*str > 0x7f){
			thisFont->getDfont(*str,*(str+1),thisFont->pZmBuff);
			GuiBitMap(thisFont->pZmBuff,tx,tInfo->y,\
				thisFont->dwide,thisFont->dhigh, tInfo->flag);
			str += 2;
			tx += (thisFont->dwide + tInfo->wInterval);
		}
		else{
			thisFont->getSfont(*str,thisFont->pZmBuff);
			GuiBitMap(thisFont->pZmBuff,tx,tInfo->y,\
				thisFont->swide,thisFont->shigh, tInfo->flag);
			str ++;
			tx += (thisFont->swide + tInfo->wInterval);
		}
	}
}

/**
  *@brief  ��ȡ�ı���ռ���س���
  *@param  str �ı�
  *@retval ���س���
  */
uint16_t GetTextPixLen(uint8_t *str)
{
	uint16_t len = 0;
	while(*str != '\0'){
		if(*str > 0x7f){
			len += thisFont->dwide;
			str += 2;
		}
		else{
			len += thisFont->swide;
			str ++;
		}
	}
	return len;
}

/**
  *@brief  д�����ı�
  *@param  x,y ��ʼ����
  *@param  wide �����ı����
  *@param  flag ��������־
  *@param  str �ı�
  *@retval ���س���
  */
void GuiRowText(lkdCoord x, lkdCoord y,uint16_t wide, FontFlag flag,uint8_t *str)
{
	uint16_t pixLen;
	fontTextInfo tInfo;
	lkdColour backcolor;

	backcolor = GuiGetbackcolor();

	pixLen = GetTextPixLen(str);
	if(pixLen <= wide){
		if(flag == FONT_MID){
			tInfo.beginOffset = (wide - pixLen) / 2;
		}
		else if(flag == FONT_RIGHT){
			tInfo.beginOffset = wide - pixLen;
		}
		else{
			tInfo.beginOffset = 0;
		}
	}
	else{
		tInfo.beginOffset = 0;
	}
	tInfo.hInterval = 0;
	tInfo.wInterval = 0;
	tInfo.x = x;
	tInfo.y = y;
	tInfo.wide = wide;
	tInfo.high = 12;
	if(backcolor == CWHITLE){
		tInfo.flag = 0;
	}
	else{//����
		tInfo.flag = 1;
	}
	GuiText(&tInfo, str);
}
/**
  *@brief  д�����»����ı�
  *@param  x,y ��ʼ����
  *@param  wide �����ı����
  *@param  flag ��������־
  *@param  str �ı�
  *@retval ���س���
  */
void GuiRowTextWithLine(lkdCoord x, lkdCoord y,uint16_t wide, uint16_t linesize,FontFlag flag,uint8_t *str)
{
	uint16_t pixLen;
	fontTextInfo tInfo;
	lkdColour backcolor;

	backcolor = GuiGetbackcolor();

	pixLen = GetTextPixLen(str);
	if(pixLen <= wide){
		if(flag == FONT_MID){
			tInfo.beginOffset = (wide - pixLen) / 2;
		}
		else if(flag == FONT_RIGHT){
			tInfo.beginOffset = wide - pixLen;
		}
		else{
			tInfo.beginOffset = 0;
		}
	}
	else{
		tInfo.beginOffset = 0;
	}
	tInfo.hInterval = 0;
	tInfo.wInterval = 0;
	tInfo.x = x;
	tInfo.y = y;
	tInfo.wide = wide;
	tInfo.high = 12;
	if(backcolor == CWHITLE){
		tInfo.flag = 0;
	}
	else{//����
		tInfo.flag = 1;
	}
	GuiHLine(x,y+8,linesize,1);
	GuiText(&tInfo, str);
}

void GuiFont12Align(lkdCoord x, lkdCoord y,uint16_t wide, FontFlag flag,uint8_t *str)
{
	GuiRowText(x, y,wide,flag,str);
}

void GuiFont12(lkdCoord x, lkdCoord y, uint8_t *str)
{
	GuiRowText(x, y,159,FONT_LEFT,str);
}
/* END */
