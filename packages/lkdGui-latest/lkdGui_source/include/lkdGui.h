/**
  * @file   lkdGui.h
  * @author  guoweilkd
  * @version V1.1.0
  * @date    2019/06/20
  * @brief  lkdGuiʹ��ͷ�ļ�
  */

#ifndef __LKDGUI_H
#define	__LKDGUI_H

#include "lkdGuiTypedef.h"

/* ������y ������ x */
#define  GUIXMAX	GUI_LCM_XMAX
#define  GUIYMAX	GUI_LCM_YMAX

 /* ���� */
void GuiPoint(lkdCoord x, lkdCoord y, lkdColour color);
/* ��б�� */
void GuiBiasLine(lkdCoord x0, lkdCoord y0,lkdCoord x1, lkdCoord y1, lkdColour color);
/* ��ˮƽ�� */
void GuiHLine(lkdCoord x0, lkdCoord y0, lkdCoord x1, lkdColour color);
/* ����ֱ�� */
void GuiRLine(lkdCoord x0, lkdCoord y0, lkdCoord y1, lkdColour color);
/* ��ˮƽ���� */
void GuiHPointLine(lkdCoord x0, lkdCoord y0, lkdCoord x1, uint8_t interval, lkdColour color);
/* ����ֱ���� */
void GuiRPointLine(lkdCoord x0, lkdCoord y0, lkdCoord y1, uint8_t interval, lkdColour color);
/* ������ */
void GuiRect(lkdCoord x0, lkdCoord y0, lkdCoord x1,lkdCoord y1, lkdColour color);
/* �������� */
void GuiFillRect(lkdCoord x0, lkdCoord y0, lkdCoord x1,lkdCoord y1, lkdColour color);
/* ��λͼ */
void GuiBitMap(uint8_t *bitMap,lkdCoord x0, lkdCoord y0,uint32_t xlen,uint32_t ylen,uint8_t flag);
/* ���Զ���λͼ */
void GuiAreaBitMap(lkdBitmap *bitMap,lkdCoord x0, lkdCoord y0, lkdCoord endx, lkdCoord endy, uint8_t flag);
/* ���Զ���λͼ,Э��ʹ�� */
void GuiAreaBitMapNumber(lkdBitmap *bitMap,uint8_t number,\
	lkdCoord x0, lkdCoord y0, lkdCoord endx, lkdCoord endy, uint8_t flag);

/* 12x12���庯�� */
void GuiFont12(lkdCoord x, lkdCoord y, uint8_t *str);
/* 12x12���������ʾ���� */
void GuiFont12Align(lkdCoord x, lkdCoord y,uint16_t wide, FontFlag flag,uint8_t *str);
/* д�ı� */
void GuiText(fontTextInfo *tInfo, uint8_t *str);
/* д���ı� */
void GuiRowText(lkdCoord x, lkdCoord y,uint16_t wide, FontFlag flag,uint8_t *str);
/* д�»����ı� */
void GuiRowTextWithLine(lkdCoord x, lkdCoord y,uint16_t wide, uint16_t linesize,FontFlag flag,uint8_t *str);
/* ���õ�ǰҪʹ�õ�����ָ�� */
void GuiFontSet(lkdFont *pFont);
/* ��ȡ��ǰ���� */
lkdFont *GetCurrentFont(void);

/* ����ǰ��ɫ�ͱ���ɫ */
void GuiExchangeColor(void);
/* ��ȡǰ��ɫ */
lkdColour GuiGetForecolor(void);
/* ��ȡ����ɫ */
lkdColour GuiGetbackcolor(void);
/* ���ñ���ɫ */
void GuiSetbackcolor(lkdColour colour);
/* ����ǰ��ɫ */
void GuiSetForecolor(lkdColour colour);

/* ���ڳ�ʼ�� */
void GuiWinInit(void);
/* ��Ӵ��� */
int8_t GuiWinAdd(lkdWin *pWindow);
/* ��ʾ���� */
void GuiWinDisplay(void);
/* ɾ�����㴰�� */
void GuiWinDeleteTop(void);
/* ��ȡ���㴰�� */
lkdWin *GuiGetTopWin(void);

/* ��ֱ������ */
void GuiVScroll(lkdScroll *pScroll);
/* ˮƽ������ */
void GuiHScroll(lkdScroll *pScroll);
/* ��ť */
void GuiButton(lkdButton *pButton);
/* ������ */
void GuiProGress(lkdProgress *pProg);


/* �˵���ʼ�� */
void GuiMenuInit(lkdMenu *pM);
/* �˵�ѡ�нڵ������ƶ� */
void GuiMenuItemDownMove(lkdMenu *pM);
/* �˵�ѡ�нڵ������ƶ� */
void GuiMenuItemUpMove(lkdMenu *pM);
/* �˵���ǰ�ڵ�չ�� */
uint8_t GuiMenuCurrentNodeSonUnfold(lkdMenu *pM);
/* �˵���ǰ�ڵ����� */
uint8_t GuiMenuCurrentNodeHide(lkdMenu *pM);
/* �˵��ػ� */
void GuiMenuRedrawMenu(lkdMenu *pM);
/* ��ȡ��ǰѡ�еĽڵ� */
lkdMenuNode *GuiMenuGetCurrentpNode(lkdMenu *pM);
/* ��ȡ��ǰѡ�еĽڵ��ID */
uint16_t GuiMenuGetCurrentNodeID(lkdMenu *pM);
/* �˵��ֵܽڵ���� */
void GuiMenuBroNodeInsert(lkdMenuNode *bNode, lkdMenuNode *pNode);
/* �˵��ӽڵ���� */
void GuiMenuSonNodeInsert(lkdMenuNode *fNode, lkdMenuNode *pNode);
/* �˵��ڵ�ɾ�� */
uint8_t GuiMenuNodeDelete(lkdMenuNode *fbNode, lkdMenuNode *thisNode);

/* ��ȫ����Ļ */
#define GuiClearScreen(color) GuiFillRect(0, 0, GUIXMAX - 1,GUIXMAX - 1,color);
/* ˢ�»��浽lcd */
extern void GuiUpdateDisplayAll(void);
/* ˢ��ָ����Χ���浽lcd */
extern void GuiRangeUpdateDisplay(lkdCoord beginx, lkdCoord beginy,lkdCoord endx, lkdCoord endy);
/* ���㵽���� */
extern void GuiDrawPoint(lkdCoord x, lkdCoord y, lkdColour color);
/* ��Ӳ���豸���� */
extern void GuiReadPoint(lkdCoord x, lkdCoord y, lkdColour *pColor);

void GuiFlipPoint(lkdCoord x, lkdCoord y, lkdColour color);
void userAppPortInit(void);
void userAppPortRun(void);
#endif /* __LKDGUI_H */

/* END */
