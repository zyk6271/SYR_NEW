#ifndef __UART_API_H
#define __UART_API_H

void rst_set_cb(uint32_t value);//��λ
void def_set_cb(uint32_t value);//��������
void ras_set_cb(uint32_t value);//��ϴ��ʼ
void ras_get_cb(void);//��ϴ��ѯ
void cnd_get_cb(void);//TDS��ȡ
void net_get_cb(void);//DC��ѹ��ȡ
void bat_get_cb(void);//��ص�ѹ��ȡ
void ala_get_cb(void);//�Զ���ϴʣ��ʱ���ȡ
void ala_set_cb(uint32_t value);//�Զ���ϴʣ��ʱ������
void alr_get_cb(void);//�Զ�����ʣ��ʱ���ȡ
void alr_set_cb(uint32_t value);//�Զ�����ʣ��ʱ���ȡ
void sup_get_cb(void);//��Դ���ӷ�ʽ
void rse_set_cb(uint32_t value);//�Զ�����ʱ������
void rse_get_cb(void);//�Զ�����ʱ���ȡ
void rsa_set_cb(uint32_t value);//�Զ���ϴʱ������
void rsa_get_cb(void);//�Զ���ϴʱ���ȡ
void rsi_set_cb(uint32_t value);//ʱ�䷶Χ����
void rsi_get_cb(void);//ʱ�䷶Χ��ȡ
void rsd_set_cb(uint32_t value);//��ϴʱ������
void rsd_get_cb(void);//��ϴʱ���ȡ
void cnf_set_cb(uint32_t value);//TDSУ׼ֵ����
void cnf_get_cb(void);//TDSУ׼ֵ��ȡ
void cnl_set_cb(uint32_t value);//TDS��ֵ����
void cnl_get_cb(void);//TDS��ֵ��ȡ
void sse_set_cb(uint32_t value);//���ѿ�������
void sse_get_cb(void);//���ѿ��ػ�ȡ
void ssa_set_cb(uint32_t value);//�Զ���������
void ssa_get_cb(void);//�Զ����ػ�ȡ
void ssd_set_cb(uint32_t value);//ѹ�������
void ssd_get_cb(void);//ѹ��ػ�ȡ
void lng_set_cb(uint32_t value);//��������
void lng_get_cb(void);//���Ի�ȡ
void srn_get_cb(void);//���кŻ�ȡ
void ver_get_cb(void);//�汾�Ż�ȡ
void com_set_cb(uint32_t value);//Manual���������
void com_get_cb(void);//Manual��������ȡ
void coa_set_cb(uint32_t value);//Auto���������
void coa_get_cb(void);//Auto��������ȡ
void cod_set_cb(uint32_t value);//Delta���������
void cod_get_cb(void);//Delta��������ȡ
void coe_set_cb(uint32_t value);//Error���������
void coe_get_cb(void);//Error��������ȡ

#endif
