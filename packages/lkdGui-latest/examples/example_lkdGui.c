#include <rtthread.h>
#if defined(RT_USING_FINSH)
#include <finsh.h>
#endif

#include "lkdGui.h"

//void test(int argc, char** argv)
//{

//    /* 1.begin ��ֲ���
//     * 1.1 ����û����壺������������ѡ����������ļ���ӵ���Ŀ����(�����ļ�
//     *     λ��/lkdGui_source/port/fontDriver/,���û�к���������Ҫ�û���
//     *     ����д)����ѡ��ASCII_6_12.c��GB2312_12_12.c����Ŀ���̡�
//     * 1.2 ��userFontPort.c�������غ��������ʾ���������ע��1.1:userFontPort.c��䡣
//     * 1.3 ���LCD��ʾ����:����Һ���ͺ�ѡ����ʵ�Һ����������Ŀ���̡�(Һ������
//     *     λ��/lkdGui_source/port/lcdDriver/,���û�к���������Ҫ�û���
//     *     ����д)����ѡ��UC1689Driver.c����Ŀ����,�����ͷ�ļ�����Ŀ·����
//     * 1.4 ��lcdDriverPort.c�������غ��������ʾ���������ע��1.2:lcdDriverPort.c��䡣
//     * 1.end ��ֲ����
//     */

//    /* 2.begin lkdGuiʹ��
//     * 2.1 lkdGui��ʼ��:userAppPort.c���û���ӿڡ������Ѿ�д���˳�ʼ��ģ�塣
//     *     �û�ֻ��Ҫ������ʾ���userAppPortInit()������
//     * 2.2 ��main�����ĳ�ʼ�������е���userAppPortInit()�����lkdGui��ʼ����
//     * 2.3 lkdGui����:userAppPortRun()������Gui�ĵ������к�������Ҫ����main
//     *     ��������ѭ����ʹ��һֱ����(����Ƶ���û��Լ�����,����10msִ��һ��)��
//     * 2.4 �û��������λ��:lkdGuiʹ�ô��ڹ����û��ڴ��ڵĻص������������س���
//     *     ��userAppPort.c�Ѿ���������������,��ص�����ΪUserMainWinFun().
//     *     �û����Դ˺�����ִ���κγ��򡣹��ڴ��ڵľ���ʹ�ÿ��Բο�doc�ļ�����
//     *     ��program���̡����ʾ���������ע��2.1:UserMainWinFun()��䡣
//     */

//    extern void userAppPortInit(void);
//    extern void userAppPortRun(void);
//    userAppPortInit();

//    while(1){
//        userAppPortRun();
////				GuiClearScreen(0);
////				GuiRowText(10,20,40,0,"AAAAA");
////				GuiRowText(10,40,40,0,"kkkkk");
////				GuiDrawPoint(0,0,1);
////				GuiDrawPoint(0,63,1);
////				GuiDrawPoint(62,30,1);
////				GuiDrawPoint(62,31,1);
////				GuiDrawPoint(63,30,1);
////				GuiDrawPoint(63,31,1);
////				GuiDrawPoint(127,0,1);
////				GuiDrawPoint(127,63,1);
////				

//	
//				GuiUpdateDisplayAll();
//			
//        //msleep(10);
//    };

//}
//#if defined(RT_USING_FINSH)
//MSH_CMD_EXPORT(test, lkdGui API test);
//#endif


/* ע��1.1:userFontPort.c��� */
//1. ����ȡ���ֺ���
// static uint8_t GetDfontData(uint8_t code1, uint8_t code2,uint8_t *pBuff)
// {
//     /* ����û����� */
//     extern uint8_t GetFontGb2312_12_12(uint8_t codeH, uint8_t codeL, uint8_t *pBuff);
//     GetFontGb2312_12_12(code1,code2,pBuff);
//     return 0;
// }
// //2. ����ȡASCII����
// static uint8_t GetSfontData(uint8_t code1, uint8_t *pBuff)
// {
//     /* ����û����� */
//     extern uint8_t GetFontASCII_6_12(uint8_t code1, uint8_t *pBuff);
//     GetFontASCII_6_12(code1, pBuff);
//     return 0;
// }

// //2. ����ʼ������
// void defaultFontInit(void)
// {
//     /* ��������Ҫ������Ӧ���޸� */

//     /* ��buff�Ĵ�С�������ģ��С���� */
// 	static uint8_t dataBuff[12*2];

// 	defaultFont.name = "������ģΪ12*12��GB2312,ASCII��ģΪ6*12";
// 	defaultFont.dhigh = 12;
// 	defaultFont.dwide = 12;
// 	defaultFont.shigh = 6;
// 	defaultFont.swide = 12;
// 	defaultFont.pZmBuff = dataBuff;
// 	defaultFont.getDfont = GetDfontData;
// 	defaultFont.getSfont = GetSfontData;

//     /* ����ΪϵͳĬ������ */
// 	GuiFontSet(&defaultFont);
//   GuiSetbackcolor(CWHITLE);
//   GuiSetForecolor(CBLACK);
// }

/* ע��1.2:lcdDriverPort.c��� */
// //1. �������˺���:��lkdGui����������ˢ�µ�lcd.���Բο�doc/program�е���䷽ʽ
// void GuiUpdateDisplayAll(void)
// {
//   /* ����û����� */
// }
// //2. �������˺���:gui������еĻ�ͼ����������ô˺�����
// //   �˺���������ͼ���ݵĴ�š�һ�㶨��һ�������š����Բο�doc/program�е�ʹ�÷�ʽ��
// void GuiDrawPoint(lkdCoord x, lkdCoord y, lkdColour color)
// {
//   /* ����û����� */
// }

// //3. ��ѡ���˺���:��lcd��ʾ
// void OpenLcdDisplay(void)
// {
//   /* ����û����� */
// }

// //4. ���ļ������ຯ��ҲΪ��ѡ���˺���

/* ע��2.1:UserMainWinFun()��� */
/* static void UserMainWinFun(void *param)
{
     static flag;

     if(flag == 0){//��ͼ�ȵ�һ�ν��봰�ڵ����ִ�ж���
        GuiRowText(10,10,100,"lkdGui");
        flag = 1;
     }

     switch(keyIs){//���ݰ���ֵ����Ӧ������
         case UP  :xxx;break;
         case DOWN:xxx;break;
         case LEFT:xxx;break;
         case RIGHT:xxx;break;
         case OK   :xxx;break;//��Ӵ��ڵ���ز���
         case CANCEL:
            flag = 0;break;//��ȡ������ִ��ʱ,���ٴ��ڣ��ָ�������ر�����
     }
} */
