#ifndef __UART_API_H
#define __UART_API_H

void rst_set_cb(const unsigned char value[], unsigned short length);//��λ
void def_set_cb(const unsigned char value[], unsigned short length);//��������
void ras_set_cb(const unsigned char value[], unsigned short length);//��ϴ��ʼ
void ras_get_cb(const unsigned char value[], unsigned short length);//��ϴ��ѯ
void cnd_get_cb(const unsigned char value[], unsigned short length);//TDS��ȡ
void net_get_cb(const unsigned char value[], unsigned short length);//DC��ѹ��ȡ
void bat_get_cb(const unsigned char value[], unsigned short length);//��ص�ѹ��ȡ
void ala_get_cb(const unsigned char value[], unsigned short length);//�Զ���ϴʣ��ʱ���ȡ
void ala_set_cb(const unsigned char value[], unsigned short length);//�Զ���ϴʣ��ʱ������
void alr_get_cb(const unsigned char value[], unsigned short length);//�Զ�����ʣ��ʱ���ȡ
void alr_set_cb(const unsigned char value[], unsigned short length);//�Զ�����ʣ��ʱ���ȡ
void sup_get_cb(const unsigned char value[], unsigned short length);//��Դ���ӷ�ʽ
void rse_set_cb(const unsigned char value[], unsigned short length);//�Զ�����ʱ������
void rse_get_cb(const unsigned char value[], unsigned short length);//�Զ�����ʱ���ȡ
void rsa_set_cb(const unsigned char value[], unsigned short length);//�Զ���ϴʱ������
void rsa_get_cb(const unsigned char value[], unsigned short length);//�Զ���ϴʱ���ȡ
void rsi_set_cb(const unsigned char value[], unsigned short length);//ʱ�䷶Χ����
void rsi_get_cb(const unsigned char value[], unsigned short length);//ʱ�䷶Χ��ȡ
void rsd_set_cb(const unsigned char value[], unsigned short length);//��ϴʱ������
void rsd_get_cb(const unsigned char value[], unsigned short length);//��ϴʱ���ȡ
void cnf_set_cb(const unsigned char value[], unsigned short length);//TDSУ׼ֵ����
void cnf_get_cb(const unsigned char value[], unsigned short length);//TDSУ׼ֵ��ȡ
void cnl_set_cb(const unsigned char value[], unsigned short length);//TDS��ֵ����
void cnl_get_cb(const unsigned char value[], unsigned short length);//TDS��ֵ��ȡ
void sse_set_cb(const unsigned char value[], unsigned short length);//���ѿ�������
void sse_get_cb(const unsigned char value[], unsigned short length);//���ѿ��ػ�ȡ
void ssa_set_cb(const unsigned char value[], unsigned short length);//�Զ���������
void ssa_get_cb(const unsigned char value[], unsigned short length);//�Զ����ػ�ȡ
void ssd_set_cb(const unsigned char value[], unsigned short length);//ѹ�������
void ssd_get_cb(const unsigned char value[], unsigned short length);//ѹ��ػ�ȡ
void lng_set_cb(const unsigned char value[], unsigned short length);//��������
void lng_get_cb(const unsigned char value[], unsigned short length);//���Ի�ȡ
void com_set_cb(const unsigned char value[], unsigned short length);//Manual���������
void com_get_cb(const unsigned char value[], unsigned short length);//Manual��������ȡ
void coa_set_cb(const unsigned char value[], unsigned short length);//Auto���������
void coa_get_cb(const unsigned char value[], unsigned short length);//Auto��������ȡ
void cod_set_cb(const unsigned char value[], unsigned short length);//Delta���������
void cod_get_cb(const unsigned char value[], unsigned short length);//Delta��������ȡ
void coe_set_cb(const unsigned char value[], unsigned short length);//Error���������
void coe_get_cb(const unsigned char value[], unsigned short length);//Error��������ȡ
void wst_set_cb(const unsigned char value[], unsigned short length);//WIFI״̬��ȡ
void rcp_set_cb(const unsigned char value[], unsigned short length);//ң��������
void rcp_get_cb(const unsigned char value[], unsigned short length);//ң������ȡ
void emr_set_cb(const unsigned char value[], unsigned short length);//ң�ⳬʱ����
void emr_get_cb(const unsigned char value[], unsigned short length);//ң�ⳬʱ��ȡ

#endif
