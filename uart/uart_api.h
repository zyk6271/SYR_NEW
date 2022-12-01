#ifndef __UART_API_H
#define __UART_API_H

void rst_set_cb(const unsigned char value[], unsigned short length);//复位
void def_set_cb(const unsigned char value[], unsigned short length);//出厂设置
void ras_set_cb(const unsigned char value[], unsigned short length);//冲洗开始
void ras_get_cb(const unsigned char value[], unsigned short length);//冲洗查询
void cnd_get_cb(const unsigned char value[], unsigned short length);//TDS获取
void net_get_cb(const unsigned char value[], unsigned short length);//DC电压获取
void bat_get_cb(const unsigned char value[], unsigned short length);//电池电压获取
void ala_get_cb(const unsigned char value[], unsigned short length);//自动冲洗剩余时间获取
void ala_set_cb(const unsigned char value[], unsigned short length);//自动冲洗剩余时间设置
void alr_get_cb(const unsigned char value[], unsigned short length);//自动提醒剩余时间获取
void alr_set_cb(const unsigned char value[], unsigned short length);//自动提醒剩余时间获取
void sup_get_cb(const unsigned char value[], unsigned short length);//电源连接方式
void rse_set_cb(const unsigned char value[], unsigned short length);//自动提醒时间设置
void rse_get_cb(const unsigned char value[], unsigned short length);//自动提醒时间获取
void rsa_set_cb(const unsigned char value[], unsigned short length);//自动冲洗时间设置
void rsa_get_cb(const unsigned char value[], unsigned short length);//自动冲洗时间获取
void rsi_set_cb(const unsigned char value[], unsigned short length);//时间范围设置
void rsi_get_cb(const unsigned char value[], unsigned short length);//时间范围获取
void rsd_set_cb(const unsigned char value[], unsigned short length);//冲洗时间设置
void rsd_get_cb(const unsigned char value[], unsigned short length);//冲洗时间获取
void cnf_set_cb(const unsigned char value[], unsigned short length);//TDS校准值设置
void cnf_get_cb(const unsigned char value[], unsigned short length);//TDS校准值获取
void cnl_set_cb(const unsigned char value[], unsigned short length);//TDS阈值设置
void cnl_get_cb(const unsigned char value[], unsigned short length);//TDS阈值获取
void sse_set_cb(const unsigned char value[], unsigned short length);//提醒开关设置
void sse_get_cb(const unsigned char value[], unsigned short length);//提醒开关获取
void ssa_set_cb(const unsigned char value[], unsigned short length);//自动开关设置
void ssa_get_cb(const unsigned char value[], unsigned short length);//自动开关获取
void ssd_set_cb(const unsigned char value[], unsigned short length);//压差开关设置
void ssd_get_cb(const unsigned char value[], unsigned short length);//压差开关获取
void lng_set_cb(const unsigned char value[], unsigned short length);//语言设置
void lng_get_cb(const unsigned char value[], unsigned short length);//语言获取
void com_set_cb(const unsigned char value[], unsigned short length);//Manual计数器清空
void com_get_cb(const unsigned char value[], unsigned short length);//Manual计数器获取
void coa_set_cb(const unsigned char value[], unsigned short length);//Auto计数器清空
void coa_get_cb(const unsigned char value[], unsigned short length);//Auto计数器获取
void cod_set_cb(const unsigned char value[], unsigned short length);//Delta计数器清空
void cod_get_cb(const unsigned char value[], unsigned short length);//Delta计数器获取
void coe_set_cb(const unsigned char value[], unsigned short length);//Error计数器清空
void coe_get_cb(const unsigned char value[], unsigned short length);//Error计数器获取
void wst_set_cb(const unsigned char value[], unsigned short length);//WIFI状态获取
void rcp_set_cb(const unsigned char value[], unsigned short length);//遥测间隔设置
void rcp_get_cb(const unsigned char value[], unsigned short length);//遥测间隔获取
void emr_set_cb(const unsigned char value[], unsigned short length);//遥测超时设置
void emr_get_cb(const unsigned char value[], unsigned short length);//遥测超时获取

#endif
