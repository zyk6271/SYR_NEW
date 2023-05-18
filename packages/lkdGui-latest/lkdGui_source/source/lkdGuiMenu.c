/**
  * @file   lkdGuiMenu.c
  * @author  guoweilkd
  * @version V0.0.0
  * @date    2018/06/05
  * @brief  lkdGui�˵�����
  */

#include "lkdGui.h"

/**
  *@brief  �˵��ֵܽڵ����
  *@param  bNode ǰһ���ֵܽڵ�
  *@param  pNode Ҫ����Ľڵ�
  *@retval None
  */
void GuiMenuBroNodeInsert(lkdMenuNode *bNode, lkdMenuNode *pNode)
{
	lkdMenuNode *tNode;
	tNode = bNode->pBro;
	bNode->pBro = pNode;
	pNode->pBro = tNode;
}

/**
  *@brief  �˵��ӽڵ����
  *@param  fNode ���ڵ�
  *@param  pNode Ҫ����Ľڵ�
  *@retval None
  */
void GuiMenuSonNodeInsert(lkdMenuNode *fNode, lkdMenuNode *pNode)
{
	lkdMenuNode *tNode;
	tNode = fNode->pSon;
	fNode->pSon = pNode;
	pNode->pBro = tNode;
}

/**
  *@brief  �˵��ڵ�ɾ��
  *@param  pMenu
  *@param  node Ҫɾ���Ľڵ�ID
  *@retval 0 ɾ���ɹ� 1 fbNode��thisNodeû�й�ϵ
  */
uint8_t GuiMenuNodeDelete(lkdMenuNode *fbNode, lkdMenuNode *thisNode)
{
	if(fbNode->pSon == thisNode){
		fbNode->pSon = thisNode->pSon;
	}
	else if(fbNode->pBro == thisNode){
		fbNode->pBro = thisNode->pBro;
	}
	else{
		return 1;
	}
	return 0;
}

/**
  *@brief  ��ȡ�ֵܽڵ���
  *@param  pbeginNode ��ʼ�ڵ�
  *@retval �ڵ���
  */
static uint8_t GetbNodesNum(const lkdMenuNode *pbeginNode)
{
	uint8_t nodeNum = 0;
	while(pbeginNode != NULL){
		nodeNum ++;
		pbeginNode = pbeginNode->pBro;
	}
	return nodeNum;
}

/**
  *@brief  ��ȡ��ǰ�ڵ��ָ��
  *@param  pM �˵����
  *@retval NULL û�ҵ� lkdMenuNode ��ǰ�ڵ��ָ��
  */
lkdMenuNode *GetpNodeFromIndex(lkdMenu *pM)
{
	lkdMenuNode *tpNode = pM->pDraw;
	uint8_t nodeNum = 0;
	while(tpNode != NULL){
		if(nodeNum == pM->index){
			return tpNode;
		}
		nodeNum ++;
		tpNode = tpNode->pBro;
	}
	return NULL;
}
/**
  *@brief  ��ȡѡ�нڵ�Ļ���y����
  *@param  pM
  *@param  index ����
  *@param  cy y��ʼ��������
  *@retval y����
  */
static lkdCoord GetCheckedNodeDrawy(lkdMenu *pM, uint8_t index, lkdCoord cy)
{
	uint8_t cNum;
	cNum = (pM->hight - 1 - (cy - pM->y)) / pM->Itemshigh;
	if(index >= cNum){
		return (lkdCoord)(cy + (cNum - 1)*pM->Itemshigh);
	}
	else{
		return (lkdCoord)(cy + index*pM->Itemshigh);
	}
}

/**
  *@brief  ��ȡӦ�û��ƵĽڵ���Ŀ�����޸Ļ�������ο�
  *@param  pM �˵����
  *@param  pDraw ���ƽڵ㿪ʼ��
  *@param  drawy ��ʼ��������ο�
  *@retval ���ƽڵ���
  */
static uint8_t GetDrawNodes(const lkdMenu *pM,const lkdMenuNode *pDraw,lkdCoord *drawy)
{
	uint8_t nodeNum,DrawNodeNum;
	int16_t ty = *drawy;
	nodeNum = GetbNodesNum(pDraw);
	DrawNodeNum = (pM->hight - 1 - (*drawy - pM->y)) / pM->Itemshigh;
	if(DrawNodeNum >= nodeNum){//�ɻ��ƵĽڵ�����ܽڵ�
		return nodeNum;
	}
	/* �ɻ��ƵĽڵ�С���ܽڵ� */
	while(1){
		DrawNodeNum ++;
		ty -= pM->Itemshigh;
		if(ty < pM->y){
			ty += pM->Itemshigh;
			DrawNodeNum --;
			break;
		}
		if(DrawNodeNum == nodeNum){
			break;
		}
	}
	*drawy = ty;
	return DrawNodeNum;
}

static uint8_t GetDrawNodesNum(const lkdMenu *pM,const lkdMenuNode *pDraw,lkdCoord drawy)
{
	uint8_t nodeNum,DrawNodeNum;
	nodeNum = GetbNodesNum(pDraw);
	DrawNodeNum = (pM->hight - 1 - (drawy - pM->y)) / pM->Itemshigh;
	if(DrawNodeNum >= nodeNum){//�ɻ��ƵĽڵ�����ܽڵ�
		DrawNodeNum = nodeNum;
	}
	return DrawNodeNum;
}
/**
  *@brief  ���ƵĽڵ�
  *@param  pM �˵����
  *@param  pthisNode ���ƽڵ㿪ʼ��
  *@param  beginy ��ʼ��������
  *@param  index ����
  *@param  cNum �ɻ��ƽڵ���
  *@retval None
  */
static void DrawNodes(lkdMenu *pM,lkdMenuNode *pthisNode,lkdCoord x,\
	lkdCoord beginy,uint8_t index,uint8_t cNum)
{
	uint8_t indexOffset = 0;
	lkdColour forecolor,backcolor; 

    GuiSetForecolor(1);
    GuiSetbackcolor(0);

	forecolor = GuiGetForecolor();
	backcolor=  GuiGetbackcolor();

	/* ȷ�����ĸ��ڵ㿪ʼ�� */
	if(cNum <= index){
		indexOffset = index - cNum + 1;
		for(uint8_t i = 0; i < indexOffset; i ++){
			pthisNode = pthisNode->pBro;
		}
	}
	GuiFillRect(x,beginy,x + pM->ItemsWide - 1,beginy + cNum*pM->Itemshigh, backcolor);
	for(uint8_t i = 0; i < cNum; i ++){
		if(indexOffset + i == index){
			GuiFillRect(x + 1,beginy + 1 + i*pM->Itemshigh,\
				x + pM->ItemsWide - 1, beginy + (i+1)*pM->Itemshigh - 1, forecolor);
	        GuiSetForecolor(0);
	        GuiSetbackcolor(1);
			GuiRowText(x + 2,beginy + 2 + i*pM->Itemshigh,pM->ItemsWide - 3,FONT_LEFT,pthisNode->name);
			if(pthisNode->pSon != NULL){
				GuiRowText(x + pM->ItemsWide - 8,beginy + 2 + i*pM->Itemshigh,6,FONT_LEFT,">");
			}
	        GuiSetForecolor(1);
	        GuiSetbackcolor(0);
		}
		else{
			GuiRowText(x + 2,beginy + 2 + i*pM->Itemshigh,pM->ItemsWide - 3,FONT_LEFT,pthisNode->name);
			if(pthisNode->pSon != NULL){
				GuiRowText(x + pM->ItemsWide - 8,beginy + 2 + i*pM->Itemshigh,6,FONT_LEFT,">");
			}
		}
		GuiHLine(x + 1,beginy + (i+1)*pM->Itemshigh,x + pM->ItemsWide - 1, forecolor);
		pthisNode = pthisNode->pBro;
	}
	GuiHLine(x + 1,beginy,x + pM->ItemsWide - 1, forecolor);
	if(beginy + cNum*pM->Itemshigh + 1 <= pM->y + pM->hight - 1){
		GuiHLine(x + 1,beginy + cNum*pM->Itemshigh + 1,x + pM->ItemsWide - 1, forecolor);
	}
	GuiRLine(x, beginy,beginy + cNum*pM->Itemshigh + 1, forecolor);
	GuiRLine(x + pM->ItemsWide, beginy,beginy + cNum*pM->Itemshigh + 1, forecolor);
	if(pM->x + pM->wide > x + pM->ItemsWide){
		GuiRLine(x + pM->ItemsWide + 1, beginy,beginy + cNum*pM->Itemshigh + 1, forecolor);
	}
}

/**
  *@brief  ���˵�
  *@param  pM
  *@retval None
  */
static void DrawMenu(lkdMenu *pM)
{
	int8_t tCount;
	lkdColour backcolor; 

	backcolor = GuiGetbackcolor();

	/* �˵���Χ����ʾ�ļ��� */
	tCount = pM->wide / pM->ItemsWide;
	if(tCount - pM->count >= 0){
		/* ������һ�����㱾��������Ϣ */
		pM->cy = GetCheckedNodeDrawy(pM,pM->stack[pM->count - 1].index,\
			pM->stack[pM->count - 1].cy);
		pM->cNum = GetDrawNodes(pM, pM->pDraw, &pM->cy);
		pM->cx = (pM->count - 1)*pM->ItemsWide + pM->x;
		DrawNodes(pM, pM->pDraw, pM->cx,pM->cy, pM->index, pM->cNum);
	}
	else{
		uint8_t index,cNum;
		int16_t cy;
		GuiFillRect(pM->x, pM->y, pM->x + pM->wide - 1, pM->y + pM->hight - 1, backcolor);
		for(uint8_t i = 1; i < tCount; i++){/* ��ȫ�ϼ��˵� */
			index = pM->stack[pM->count - tCount + i].index;
			cy = pM->stack[pM->count - tCount + i].cy;
			cNum = GetDrawNodesNum(pM,pM->stack[pM->count - tCount + i].pNode,cy);
			DrawNodes(pM, pM->stack[pM->count - tCount + i].pNode,\
				(i-1)*pM->ItemsWide + pM->x, cy, index, cNum);
		}
		/* �����²˵� */
		pM->cy = GetCheckedNodeDrawy(pM,pM->stack[pM->count - 1].index,pM->stack[pM->count - 1].cy);
		pM->cNum = GetDrawNodes(pM, pM->pDraw, &pM->cy);
		pM->cx = (tCount - 1)*pM->ItemsWide + pM->x;
		DrawNodes(pM, pM->pDraw, pM->cx, pM->cy, pM->index, pM->cNum);
	}
}

/**
  *@brief  ���˵�
  *@param  pM
  *@retval None
  */
static void DrawAllMenu(lkdMenu *pM)
{
	int8_t tCount;
	lkdColour backcolor; 

	backcolor = GuiGetbackcolor();

	/* �˵���Χ����ʾ�ļ��� */
	tCount = pM->wide / pM->ItemsWide;
	if(tCount - pM->count >= 0){
		lkdCoord tcy;
		for(uint8_t i = 1; i < pM->count; i ++){
			tcy = GetCheckedNodeDrawy(pM,pM->stack[i- 1].index,pM->stack[i-1].cy);
			pM->cNum = GetDrawNodes(pM, pM->stack[i].pNode, &tcy);
			pM->cx = (i - 1)*pM->ItemsWide + pM->x;
			DrawNodes(pM, pM->stack[i].pNode, pM->cx,tcy, pM->stack[i].index, pM->cNum);
		}
		pM->cNum = GetDrawNodes(pM, pM->pDraw, &pM->cy);
		pM->cx = (pM->count - 1)*pM->ItemsWide + pM->x;
		DrawNodes(pM, pM->pDraw, pM->cx,pM->cy, pM->index, pM->cNum);
	}
	else{
		uint8_t index,cNum;
		int16_t cy;
		GuiFillRect(pM->x, pM->y, pM->x + pM->wide - 1, pM->y + pM->hight - 1, backcolor);
		for(uint8_t i = 1; i < tCount; i++){/* ��ȫ�ϼ��˵� */
			index = pM->stack[pM->count - tCount + i].index;
			cy = pM->stack[pM->count - tCount + i].cy;
			cNum = GetDrawNodesNum(pM,pM->stack[pM->count - tCount + i].pNode,cy);
			DrawNodes(pM, pM->stack[pM->count - tCount + i].pNode,\
				(i-1)*pM->ItemsWide + pM->x, cy, index, cNum);
		}
		/* �����²˵� */
		pM->cy = GetCheckedNodeDrawy(pM,pM->stack[pM->count - 1].index,pM->stack[pM->count - 1].cy);
		pM->cNum = GetDrawNodes(pM, pM->pDraw, &pM->cy);
		pM->cx = (tCount - 1)*pM->ItemsWide + pM->x;
		DrawNodes(pM, pM->pDraw, pM->cx, pM->cy, pM->index, pM->cNum);
	}
}

/**
  *@brief  �ػ�˵�
  *@param  pM
  *@retval None
  */
static void RedrawMenu(lkdMenu *pM)
{
	int8_t tCount,offset;
	lkdColour backcolor; 
	
	backcolor = GuiGetbackcolor();

	/* �˵���Χ����ʾ�ļ��� */
	tCount = pM->wide / pM->ItemsWide;
	if(tCount - pM->count >= 0){
		offset = 1;
		tCount = pM->count;
	}
	else{
		offset = pM->count - tCount + 1;
	}
	GuiFillRect(pM->x,pM->y,pM->x + pM->wide - 1,pM->y + pM->hight - 1, backcolor);
	for(uint8_t i = 0; i < tCount; i++){
		pM->cy = pM->stack[i + offset].cy;
		pM->cNum = GetDrawNodesNum(pM,pM->stack[i + offset].pNode,pM->cy);
		pM->cx = i*pM->ItemsWide + pM->x;
		pM->index = pM->stack[i + offset].index;
		DrawNodes(pM, pM->stack[i + offset].pNode, pM->cx, pM->cy, pM->index, pM->cNum);
	}
}

/**
  *@brief  �˵��ڵ������ӽڵ����ʾ
  *@param  pM
  *@param  fNode ���ڵ�
  *@retval 0 ��ʾ��ӳɹ� 1 ջ��������
  */
static uint8_t GuiMenuNodesDisplay(lkdMenu *pM,lkdMenuNode *node)
{
	if(pM->count < pM->stackNum){//�ж�ջ�Ƿ����
		/* ѹջ��һ���ڵ�������Ϣ */
		pM->stack[pM->count].index = pM->index;
		pM->stack[pM->count].pNode = pM->pDraw;
		pM->stack[pM->count].cy = pM->cy;
		
		/* ��ʼ���˼��ڵ��й���Ϣ */
		pM->count ++;
		pM->index = 0;
		pM->pDraw = node;
		pM->nodeNum = GetbNodesNum(pM->pDraw);
		DrawMenu(pM);
		return 0;
	}
	return 1;
}

/**
  *@brief  �˵���ǰ�ڵ�����
  *@param  pM
  *@retval 0 ��ʾ��ӳɹ� 1 û�п����ص�
  */
uint8_t GuiMenuCurrentNodeHide(lkdMenu *pM)
{
	if(pM->count >= 1){
		pM->count --;
	}
	if(pM->count < pM->stackNum){//�ж�ջ�Ƿ�Ϊ��
		RedrawMenu(pM);
		pM->index = pM->stack[pM->count].index;
		pM->pDraw = pM->stack[pM->count].pNode;
		pM->cy = pM->stack[pM->count].cy;
		pM->nodeNum = GetbNodesNum(pM->pDraw);
		GuiUpdateDisplayAll();
		return 0;
	}
	return 1;
}

/**
  *@brief  �˵���ǰ�ڵ�չ��
  *@param  pM
  *@retval 0 �ɹ� 1 ʧ��
  */
uint8_t GuiMenuCurrentNodeSonUnfold(lkdMenu *pM)
{
	lkdMenuNode *thisNode;
	thisNode = GetpNodeFromIndex(pM);
	if(thisNode != NULL && thisNode->pSon != NULL){
		GuiMenuNodesDisplay(pM,thisNode->pSon);
		GuiUpdateDisplayAll();
		return 0;
	}
	return 1;
}

/**
  *@brief  �ػ�˵�
  *@param  pM �˵����
  *@retval None
  */
void GuiMenuRedrawMenu(lkdMenu *pM)
{
	if(pM->count >= 1){
		DrawAllMenu(pM);
		GuiUpdateDisplayAll();
	}
}
/**
  *@brief  �˵��������ƶ�
  *@param  pM
  *@retval None
  */
void GuiMenuItemUpMove(lkdMenu *pM)
{
	if(pM->index - 1 < 0){
		pM->index = pM->nodeNum - 1;
	}
	else{
		pM->index --;
	}
	DrawNodes(pM, pM->pDraw, pM->cx, pM->cy, pM->index, pM->cNum);
	GuiUpdateDisplayAll();
}

/**
  *@brief  �˵��������ƶ�
  *@param  pM
  *@retval None
  */
void GuiMenuItemDownMove(lkdMenu *pM)
{
	pM->index ++;
	if(pM->index >= pM->nodeNum){
		pM->index = 0;
	}
	DrawNodes(pM, pM->pDraw, pM->cx, pM->cy, pM->index, pM->cNum);
	GuiUpdateDisplayAll();
}

/**
  *@brief  ��ȡ��ǰ�ڵ�ID
  *@param  pM �˵����
  *@retval nodeId
  */
uint16_t GuiMenuGetCurrentNodeID(lkdMenu *pM)
{
	lkdMenuNode *pNode;
	pNode = GetpNodeFromIndex(pM);
	return pNode->nodeId;
}
/**
  *@brief  ��ȡ��ǰ�ڵ�ָ��
  *@param  pM �˵����
  *@retval �ڵ�ָ��
  */
lkdMenuNode *GuiMenuGetCurrentpNode(lkdMenu *pM)
{
	return GetpNodeFromIndex(pM);
}

/**
  *@brief  �˵���ʼ��
  *@param  pM �˵����
  *@retval NULL
  */
void GuiMenuInit(lkdMenu *pM)
{
	pM->count = 0;
	pM->index = 0;
	pM->cx = pM->x;
	pM->cy = pM->y;
	pM->pDraw = pM->Root;
}

/* END */
