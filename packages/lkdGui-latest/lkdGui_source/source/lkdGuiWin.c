/**
  * @file   lkdGuiWin.c
  * @author  guoweilkd
  * @version V0.0.0
  * @date    2018/06/03
  * @brief  lkdGui���ڹ���
  */

#include "lkdGui.h"

/* ���ڹ���ṹ */
static struct GuiWinManage winManage;

/**
  *@brief  ���ƴ���
  *@param  pWindow ����ָ��
  *@retval None
  */
void GuiWinDraw(lkdWin *pWindow)
{
    GuiClearScreen(0);
    if(pWindow->x + pWindow->wide - 1 > GUIXMAX || pWindow->y + pWindow->hight - 1 > GUIYMAX ||\
         pWindow->x < 0 || pWindow->y < 0 || pWindow->wide == 0 || pWindow->hight == 0){
           return;
    }
    if(pWindow->title != NULL){
        GuiSetForecolor(1);
        GuiSetbackcolor(0);
        GuiFillRect(pWindow->x, pWindow->y, pWindow->x + pWindow->wide - 1,\
        11, 1);
        GuiSetForecolor(0);
        GuiSetbackcolor(1);
        GuiRowText(pWindow->x + 4, pWindow->y+2, pWindow->wide - 4,\
            FONT_MID,pWindow->title);
        GuiSetForecolor(1);
        GuiSetbackcolor(0);
    }
    //GuiUpdateDisplayAll();
}

/**
  *@brief  ��Ӵ���
  *@param  pWindow ����ָ��
  *@retval 0 �ɹ� -1 ʧ��
  */
int8_t GuiWinAdd(lkdWin *pWindow)
{
	GuiWinDraw(pWindow);
	if(winManage.pWin >= GUIWINMANAGE_NUM){
		return -1;
	}
	winManage.winStack[winManage.pWin] = pWindow;
	winManage.pWin ++;
    pWindow->WindowFunction(pWindow->param);
	return 0;
}

/**
  *@brief  ��Ӵ���
  *@param  pWindow ����ָ��
  *@retval 0 �ɹ� -1 ʧ��
  */
int8_t GuiWinReload(lkdWin *pWindow)
{
    GuiClearScreen(0);
    GuiWinDraw(pWindow);
    pWindow->WindowFunction(pWindow->param);
    return 0;
}
/**
  *@brief  ��Ӵ���
  *@param  pWindow ����ָ��
  *@retval 0 �ɹ� -1 ʧ��
  */
int8_t GuiWinDel(lkdWin *pWindow)
{
    if(GuiGetTopWin() == pWindow)
    {
        GuiWinDeleteTop();
    }
    return 0;
}
/**
  *@brief  ��Ӵ���
  *@param  pWindow ����ָ��
  *@retval 0 �ɹ� -1 ʧ��
  */
int8_t GuiWinRefresh(lkdWin *pWindow)
{
    GuiWinDraw(pWindow);
    return 0;
}
/**
  *@brief  ִ�ж��㴰��
  *@param  None
  *@retval None
  */
void GuiWinDisplay(void)
{
	if(winManage.pWin == 0 || winManage.pWin > GUIWINMANAGE_NUM){
		return;
	}
	lkdWin *pThis = winManage.winStack[winManage.pWin - 1];
	pThis->InputFunction(pThis->param);
	//pThis->WindowFunction(pThis);
}

/**
  *@brief  ɾ����ǰ����
  *@param  None
  *@retval None
  */
void GuiWinDeleteTop(void)
{
    GuiClearScreen(0);
	if(winManage.pWin <= 1 || winManage.pWin > GUIWINMANAGE_NUM){
		return;
	}
	winManage.pWin --;
	GuiWinDraw(winManage.winStack[winManage.pWin - 1]);
	winManage.winStack[winManage.pWin - 1]->WindowFunction(winManage.winStack[winManage.pWin - 1]->param);
}

/**
  *@brief  ��ȡ���㴰��
  *@param  None
  *@retval ���㴰��
  */
lkdWin *GuiGetTopWin(void)
{
	lkdWin *pTopWin = NULL;

	if(winManage.pWin >= 1){
		pTopWin = winManage.winStack[winManage.pWin - 1];
	}
	return pTopWin;
}

void GuiWinInit(void)
{
	winManage.pWin = 0;
}
/* END */
