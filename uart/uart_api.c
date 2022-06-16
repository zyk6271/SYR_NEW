#include "rtthread.h"
#include "uart_api.h"
#include "uart_core.h"

void rst_set_cb(uint32_t value)
{
    wifi_uart_write_frame(RST_SET_CMD,MCU_TX_VER,0);
    rt_hw_cpu_reset();
}
void def_set_cb(uint32_t value)
{
    wifi_uart_write_frame(DEF_SET_CMD,MCU_TX_VER,0);
    Flash_Clear();
}
void ras_set_cb(uint32_t value)
{
    wifi_uart_write_command_value(RAS_SET_CMD,syr_moto_set());
}
void ras_get_cb(void)
{
    wifi_uart_write_command_value(RAS_GET_CMD, syr_moto_get());
}
void wifi_ras_update(void)
{
    wifi_uart_write_command_value(RAS_PUT_CMD, syr_moto_get());
}
void cnd_get_cb(void)
{
    wifi_uart_write_command_value(CND_GET_CMD, TDS_Get());
}
void wifi_cnd_update(uint32_t value)
{
    wifi_uart_write_command_value(CND_PUT_CMD, value);
}
void net_get_cb(void)
{
    wifi_uart_write_command_value(NET_GET_CMD, Get_DC_Value());
}
void bat_get_cb(void)
{
    wifi_uart_write_command_value(BAT_GET_CMD, Get_Bat_Value());
}
void ala_get_cb(void)
{
    wifi_uart_write_command_value(ALA_GET_CMD, syr_remain_auto_time_get());
}
void ala_set_cb(uint32_t value)
{
    uint8_t ret = syr_remain_auto_time_set(value);
    if(ret==RT_EOK)
    {
        wifi_uart_write_command_value(ALA_SET_CMD,value);
    }
    else
    {
        wifi_uart_write_command_value(ALA_SET_CMD,0);
    }
}
void alr_get_cb(void)
{
    wifi_uart_write_command_value(ALR_GET_CMD, syr_remain_remind_time_get());
}
void alr_set_cb(uint32_t value)
{
    uint8_t ret = syr_remain_remind_time_set(value);
    if(ret==RT_EOK)
    {
        wifi_uart_write_command_value(ALR_SET_CMD,value);
    }
    else
    {
        wifi_uart_write_command_value(ALR_SET_CMD,0);
    }
}
void rse_set_cb(uint32_t value)
{
    wifi_uart_write_command_value(RSE_SET_CMD,value);
    syr_remind_time_set(value);
}
void rse_get_cb(void)
{
    wifi_uart_write_command_value(RSE_GET_CMD, syr_remind_time_get());
}
void wifi_rse_update(void)
{
    wifi_uart_write_command_value(RSE_PUT_CMD, syr_remind_time_get());
}
void rsa_set_cb(uint32_t value)
{
    wifi_uart_write_command_value(RSA_SET_CMD,value);
    syr_auto_time_set(value);
}
void rsa_get_cb(void)
{
    wifi_uart_write_command_value(RSA_GET_CMD, syr_auto_time_get());
}
void wifi_rsa_update(void)
{
    wifi_uart_write_command_value(RSA_PUT_CMD, syr_auto_time_get());
}
void rsi_set_cb(uint32_t value)
{
    wifi_uart_write_command_value(RSI_SET_CMD,value);
    syr_range_set(value);
}
void rsi_get_cb(void)
{
    wifi_uart_write_command_value(RSI_GET_CMD,syr_range_get());
}
void wifi_rsi_update(void)
{
    wifi_uart_write_command_value(RSI_PUT_CMD, syr_range_get());
}
void rsd_set_cb(uint32_t value)
{
    wifi_uart_write_command_value(RSD_SET_CMD,value);
    syr_backwashtime_set(value);
}
void rsd_get_cb(void)
{
    wifi_uart_write_command_value(RSD_GET_CMD,syr_backwashtime_get());
}
void wifi_rsd_update(void)
{
    wifi_uart_write_command_value(RSD_PUT_CMD, syr_backwashtime_get());
}
void cnf_set_cb(uint32_t value)
{
    wifi_uart_write_command_value(CNF_SET_CMD,value);
    syr_tdscalib_set(value);
}
void cnf_get_cb(void)
{
    wifi_uart_write_command_value(CNF_GET_CMD,syr_tdscalib_get());
}
void wifi_cnf_update(void)
{
    wifi_uart_write_command_value(CNF_PUT_CMD, syr_tdscalib_get());
}
void cnl_set_cb(uint32_t value)
{
    wifi_uart_write_command_value(CNL_SET_CMD,value);
    syr_tdslimit_set(value);
}
void cnl_get_cb(void)
{
    wifi_uart_write_command_value(CNL_GET_CMD,syr_tdslimit_get());
}
void wifi_cnl_update(void)
{
    wifi_uart_write_command_value(CNL_PUT_CMD, syr_tdslimit_get());
}
void sse_set_cb(uint32_t value)
{
    wifi_uart_write_command_value(SSE_SET_CMD,value);
    syr_switch_remind_set(value);
}
void sse_get_cb(void)
{
    wifi_uart_write_command_value(SSE_GET_CMD,syr_switch_remind_get());
}
void wifi_sse_update(void)
{
    wifi_uart_write_command_value(SSE_PUT_CMD, syr_switch_remind_get());
}
void ssa_set_cb(uint32_t value)
{
    wifi_uart_write_command_value(SSA_SET_CMD,value);
    syr_switch_auto_set(value);
}
void ssa_get_cb(void)
{
    wifi_uart_write_command_value(SSA_GET_CMD,syr_switch_auto_get());
}
void wifi_ssa_update(void)
{
    wifi_uart_write_command_value(SSA_PUT_CMD, syr_switch_auto_get());
}
void ssd_set_cb(uint32_t value)
{
    wifi_uart_write_command_value(SSD_SET_CMD,value);
    syr_switch_delta_set(value);
}
void ssd_get_cb(void)
{
    wifi_uart_write_command_value(SSD_GET_CMD,syr_switch_delta_get());
}
void wifi_ssd_update(void)
{
    wifi_uart_write_command_value(SSD_PUT_CMD, syr_switch_delta_get());
}
void lng_set_cb(uint32_t value)
{
    wifi_uart_write_command_value(LNG_SET_CMD,value);
    syr_language_set(value);
}
void lng_get_cb(void)
{
    wifi_uart_write_command_value(LNG_GET_CMD,syr_language_get());
}
void wifi_lng_update(void)
{
    wifi_uart_write_command_value(LNG_PUT_CMD, syr_language_get());
}
void sup_get_cb(void)
{
    wifi_uart_write_command_value(SUP_GET_CMD, Get_DC_Level());
}
void wifi_sup_update(void)
{
    wifi_uart_write_command_value(SUP_PUT_CMD, Get_DC_Level());
}
void ver_get_cb(void)
{
    wifi_uart_write_command_value(VER_GET_CMD,syr_ver_get());
}
void srn_get_cb(void)
{
    wifi_uart_write_command_value(SRN_GET_CMD,syr_srn_get());
}
void com_set_cb(uint32_t value)
{
    wifi_uart_write_command_value(COM_SET_CMD,value);
    syr_count_manual_set(value);
}
void com_get_cb(void)
{
    wifi_uart_write_command_value(COM_GET_CMD,syr_count_manual_get());
}
void wifi_com_update(void)
{
    wifi_uart_write_command_value(COM_PUT_CMD, syr_count_manual_get());
}
void coa_set_cb(uint32_t value)
{
    wifi_uart_write_command_value(COA_SET_CMD,value);
    syr_count_auto_set(value);
}
void coa_get_cb(void)
{
    wifi_uart_write_command_value(COA_GET_CMD,syr_count_auto_get());
}
void wifi_coa_update(void)
{
    wifi_uart_write_command_value(COA_PUT_CMD, syr_count_auto_get());
}
void cod_set_cb(uint32_t value)
{
    wifi_uart_write_command_value(COD_SET_CMD,value);
    syr_count_delta_set(value);
}
void cod_get_cb(void)
{
    wifi_uart_write_command_value(COD_GET_CMD,syr_count_delta_get());
}
void wifi_cod_update(void)
{
    wifi_uart_write_command_value(COD_PUT_CMD, syr_count_delta_get());
}
void coe_set_cb(uint32_t value)
{
    wifi_uart_write_command_value(COE_SET_CMD,value);
    syr_count_error_set(value);
}
void coe_get_cb(void)
{
    wifi_uart_write_command_value(COE_GET_CMD,syr_count_error_get());
}
void wifi_coe_update(void)
{
    wifi_uart_write_command_value(COE_PUT_CMD, syr_count_error_get());
}
void wfs_set_cb(uint32_t value)
{
    wifi_uart_write_command_value(WFS_SET_CMD,value);
    wifi_led(value);
}
void wifi_factory(void)
{
    wifi_uart_write_command_value(WFT_SET_CMD,0);
}
