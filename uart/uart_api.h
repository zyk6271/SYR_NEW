#ifndef __UART_API_H
#define __UART_API_H

void rst_set_cb(uint32_t value);//复位
void def_set_cb(uint32_t value);//出厂设置
void ras_set_cb(uint32_t value);//冲洗开始
void ras_get_cb(void);//冲洗查询
void cnd_get_cb(void);//TDS获取
void net_get_cb(void);//DC电压获取
void bat_get_cb(void);//电池电压获取
void ala_get_cb(void);//自动冲洗剩余时间获取
void ala_set_cb(uint32_t value);//自动冲洗剩余时间设置
void alr_get_cb(void);//自动提醒剩余时间获取
void alr_set_cb(uint32_t value);//自动提醒剩余时间获取
void sup_get_cb(void);//电源连接方式
void rse_set_cb(uint32_t value);//自动提醒时间设置
void rse_get_cb(void);//自动提醒时间获取
void rsa_set_cb(uint32_t value);//自动冲洗时间设置
void rsa_get_cb(void);//自动冲洗时间获取
void rsi_set_cb(uint32_t value);//时间范围设置
void rsi_get_cb(void);//时间范围获取
void rsd_set_cb(uint32_t value);//冲洗时间设置
void rsd_get_cb(void);//冲洗时间获取
void cnf_set_cb(uint32_t value);//TDS校准值设置
void cnf_get_cb(void);//TDS校准值获取
void cnl_set_cb(uint32_t value);//TDS阈值设置
void cnl_get_cb(void);//TDS阈值获取
void sse_set_cb(uint32_t value);//提醒开关设置
void sse_get_cb(void);//提醒开关获取
void ssa_set_cb(uint32_t value);//自动开关设置
void ssa_get_cb(void);//自动开关获取
void ssd_set_cb(uint32_t value);//压差开关设置
void ssd_get_cb(void);//压差开关获取
void lng_set_cb(uint32_t value);//语言设置
void lng_get_cb(void);//语言获取
void srn_get_cb(void);//序列号获取
void ver_get_cb(void);//版本号获取
void com_set_cb(uint32_t value);//Manual计数器清空
void com_get_cb(void);//Manual计数器获取
void coa_set_cb(uint32_t value);//Auto计数器清空
void coa_get_cb(void);//Auto计数器获取
void cod_set_cb(uint32_t value);//Delta计数器清空
void cod_get_cb(void);//Delta计数器获取
void coe_set_cb(uint32_t value);//Error计数器清空
void coe_get_cb(void);//Error计数器获取

#endif
