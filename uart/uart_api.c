#include "rtthread.h"
#include "uart_api.h"
#include "uart_core.h"
#include "cjson.h"

void device_config_put(uint8_t* key,uint32_t value)
{
    if(get_mqtt_valid() == 0)
    {
        return;
    }
    unsigned char length = 0;

    cJSON *root = NULL;
    cJSON *data = NULL;

    data = cJSON_CreateObject();
    cJSON_AddNumberToObject(data, key, value);

    root = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "deviceConfig", data);

    char* out = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);

    if(NULL == out) {
        //可在此添加提示信息，如：printf("xxx");
        return;
    }

    length = set_wifi_uart_buffer(length, out, rt_strlen(out));
    wifi_uart_write_frame(DP_UPLOAD_CMD, MCU_TX_VER, length);
    rt_free(out);
}
void device_info_put(uint8_t* key,uint32_t value)
{
    if(get_mqtt_valid() == 0)
    {
        return;
    }
    unsigned char length = 0;

    cJSON *root = NULL;
    cJSON *data = NULL;

    data = cJSON_CreateObject();
    cJSON_AddNumberToObject(data, key, value);

    root = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "deviceInfo", data);

    char* out = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);

    if(NULL == out) {
        //可在此添加提示信息，如：printf("xxx");
        return;
    }

    length = set_wifi_uart_buffer(length, out, rt_strlen(out));
    wifi_uart_write_frame(DP_UPLOAD_CMD, MCU_TX_VER, length);
    rt_free(out);
}

void device_info_put_buffer(uint8_t* key,uint8_t* value)
{
    if(get_mqtt_valid() == 0)
    {
        return;
    }
    unsigned char length = 0;

    cJSON *root = NULL;
    cJSON *data = NULL;

    data = cJSON_CreateObject();
    cJSON_AddStringToObject(data, key, value);

    root = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "deviceInfo", data);

    char* out = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);

    if(NULL == out) {
        //可在此添加提示信息，如：printf("xxx");
        return;
    }

    length = set_wifi_uart_buffer(length, out, rt_strlen(out));
    wifi_uart_write_frame(DP_UPLOAD_CMD, MCU_TX_VER, length);
    rt_free(out);
}

void ras_set_callback(const unsigned char value[], unsigned short length)
{
    wifi_uart_write_command_value(RAS_SET_CMD,syr_moto_set());
}
void ras_get_callback(const unsigned char value[], unsigned short length)
{
    wifi_uart_write_command_value(RAS_GET_CMD, syr_moto_get());
}
void cnd_get_callback(const unsigned char value[], unsigned short length)
{
    wifi_uart_write_command_value(CND_GET_CMD, TDS_Get());
}
void net_get_callback(const unsigned char value[], unsigned short length)
{
    wifi_uart_write_command_value(NET_GET_CMD, Get_DC_Fix());
}
void bat_get_callback(const unsigned char value[], unsigned short length)
{
    wifi_uart_write_command_value(BAT_GET_CMD, Get_Bat_Fix());
}
void rse_set_callback(const unsigned char value[], unsigned short length)
{
    unsigned int update_value;
    update_value = mcu_get_dp_download_value(value,length);
    wifi_uart_write_command_value(RSE_SET_CMD,update_value);
    syr_remind_time_set(update_value);
}
void rse_get_callback(const unsigned char value[], unsigned short length)
{
    wifi_uart_write_command_value(RSE_GET_CMD, syr_remind_time_get());
}
void rsa_set_callback(const unsigned char value[], unsigned short length)
{
    unsigned int update_value;
    update_value = mcu_get_dp_download_value(value,length);
    wifi_uart_write_command_value(RSA_SET_CMD,update_value);
    syr_auto_time_set(update_value);
}
void rsa_get_callback(const unsigned char value[], unsigned short length)
{
    wifi_uart_write_command_value(RSA_GET_CMD, syr_remain_auto_time_get());
}
void rsi_set_callback(const unsigned char value[], unsigned short length)
{
    unsigned int update_value;
    update_value = mcu_get_dp_download_value(value,length);
    wifi_uart_write_command_value(RSI_SET_CMD,update_value);
    syr_range_set(update_value);
}
void rsi_get_callback(const unsigned char value[], unsigned short length)
{
    wifi_uart_write_command_value(RSI_GET_CMD,syr_range_get());
}
void rsd_set_callback(const unsigned char value[], unsigned short length)
{
    unsigned int update_value;
    update_value = mcu_get_dp_download_value(value,length);
    wifi_uart_write_command_value(RSD_SET_CMD,update_value);
    syr_backwashtime_set(update_value);
}
void rsd_get_callback(const unsigned char value[], unsigned short length)
{
    wifi_uart_write_command_value(RSD_GET_CMD,syr_backwashtime_get());
}
void cnf_set_callback(const unsigned char value[], unsigned short length)
{
    unsigned int update_value;
    update_value = mcu_get_dp_download_value(value,length);
    wifi_uart_write_command_value(CNF_SET_CMD,update_value);
    syr_tdscalib_set(update_value);
}
void cnf_get_callback(const unsigned char value[], unsigned short length)
{
    wifi_uart_write_command_value(CNF_GET_CMD,syr_tdscalib_get());
}
void cnl_set_callback(const unsigned char value[], unsigned short length)
{
    unsigned int update_value;
    update_value = mcu_get_dp_download_value(value,length);
    wifi_uart_write_command_value(CNL_SET_CMD,update_value);
    syr_tdslimit_set(update_value);
}
void cnl_get_callback(const unsigned char value[], unsigned short length)
{
    wifi_uart_write_command_value(CNL_GET_CMD,syr_tdslimit_get());
}
void sse_set_callback(const unsigned char value[], unsigned short length)
{
    unsigned int update_value;
    update_value = mcu_get_dp_download_value(value,length);
    wifi_uart_write_command_value(SSE_SET_CMD,update_value);
    syr_switch_remind_set(update_value);
}
void sse_get_callback(const unsigned char value[], unsigned short length)
{
    wifi_uart_write_command_value(SSE_GET_CMD,syr_switch_remind_get());
}
void reminder_put_callback(uint32_t weeks,uint32_t days,uint8_t control)
{
    unsigned char length = 0;

    cJSON *root = NULL;
    cJSON *data = NULL;

    data = cJSON_CreateObject();
    cJSON_AddNumberToObject(data, "rse", (weeks*7) + days);
    cJSON_AddNumberToObject(data, "sse", control);

    root = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "deviceConfig", data);

    char* out = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);

    if(NULL == out) {
        //可在此添加提示信息，如：printf("xxx");
        return;
    }

    length = set_wifi_uart_buffer(length, out, rt_strlen(out));
    wifi_uart_write_frame(DP_UPLOAD_CMD, MCU_TX_VER, length);
    rt_free(out);
}
void ssa_set_callback(const unsigned char value[], unsigned short length)
{
    unsigned int update_value;
    update_value = mcu_get_dp_download_value(value,length);
    wifi_uart_write_command_value(SSA_SET_CMD,update_value);
    syr_switch_auto_set(update_value);
}
void ssa_get_callback(const unsigned char value[], unsigned short length)
{
    wifi_uart_write_command_value(SSA_GET_CMD,syr_switch_auto_get());
}
void automatic_put_callback(uint32_t weeks,uint32_t days,uint8_t control)
{
    unsigned char length = 0;

    cJSON *root = NULL;
    cJSON *data = NULL;

    data = cJSON_CreateObject();
    cJSON_AddNumberToObject(data, "rsa", (weeks*7) + days);
    cJSON_AddNumberToObject(data, "ssa", control);

    root = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "deviceConfig", data);

    char* out = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);

    if(NULL == out) {
        //可在此添加提示信息，如：printf("xxx");
        return;
    }

    length = set_wifi_uart_buffer(length, out, rt_strlen(out));
    wifi_uart_write_frame(DP_UPLOAD_CMD, MCU_TX_VER, length);
    rt_free(out);
}
void ssd_set_callback(const unsigned char value[], unsigned short length)
{
    unsigned int update_value;
    update_value = mcu_get_dp_download_value(value,length);
    wifi_uart_write_command_value(SSD_SET_CMD,update_value);
    syr_switch_delta_set(update_value);
}
void ssd_get_callback(const unsigned char value[], unsigned short length)
{
    wifi_uart_write_command_value(SSD_GET_CMD,syr_switch_delta_get());
}
void ssd_put_callback(uint32_t value)
{
    unsigned char length = 0;

    cJSON *root = NULL;
    cJSON *data = NULL;

    data = cJSON_CreateObject();
    cJSON_AddNumberToObject(data, "ssd", value);

    root = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "deviceConfig", data);

    char* out = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);

    if(NULL == out) {
        //可在此添加提示信息，如：printf("xxx");
        return;
    }

    length = set_wifi_uart_buffer(length, out, rt_strlen(out));
    wifi_uart_write_frame(DP_UPLOAD_CMD, MCU_TX_VER, length);
    rt_free(out);
}
void lng_set_callback(const unsigned char value[], unsigned short length)
{
    unsigned int update_value;
    update_value = mcu_get_dp_download_value(value,length);
    wifi_uart_write_command_value(LNG_SET_CMD,update_value);
    syr_language_set(update_value);
}
void lng_get_callback(const unsigned char value[], unsigned short length)
{
    wifi_uart_write_command_value(LNG_GET_CMD,syr_language_get());
}
void sup_get_callback(const unsigned char value[], unsigned short length)
{
    wifi_uart_write_command_value(SUP_GET_CMD, Get_DC_Level());
}
void com_set_callback(const unsigned char value[], unsigned short length)
{
    unsigned int update_value;
    update_value = mcu_get_dp_download_value(value,length);
    wifi_uart_write_command_value(COM_SET_CMD,update_value);
    syr_count_manual_set(update_value);
}
void com_get_callback(const unsigned char value[], unsigned short length)
{
    wifi_uart_write_command_value(COM_GET_CMD,syr_count_manual_get());
}
void coa_set_callback(const unsigned char value[], unsigned short length)
{
    unsigned int update_value;
    update_value = mcu_get_dp_download_value(value,length);
    wifi_uart_write_command_value(COA_SET_CMD,update_value);
    syr_count_auto_set(update_value);
}
void coa_get_callback(const unsigned char value[], unsigned short length)
{
    wifi_uart_write_command_value(COA_GET_CMD,syr_count_auto_get());
}
void cod_set_callback(const unsigned char value[], unsigned short length)
{
    unsigned int update_value;
    update_value = mcu_get_dp_download_value(value,length);
    wifi_uart_write_command_value(COD_SET_CMD,update_value);
    syr_count_delta_set(update_value);
}
void cod_get_callback(const unsigned char value[], unsigned short length)
{
    wifi_uart_write_command_value(COD_GET_CMD,syr_count_delta_get());
}
void coe_set_callback(const unsigned char value[], unsigned short length)
{
    unsigned int update_value;
    update_value = mcu_get_dp_download_value(value,length);
    wifi_uart_write_command_value(COE_SET_CMD,update_value);
    syr_count_error_set(update_value);
}
void coe_get_callback(const unsigned char value[], unsigned short length)
{
    wifi_uart_write_command_value(COE_GET_CMD,syr_count_error_get());
}
void rcp_set_callback(const unsigned char value[], unsigned short length)
{
    unsigned int update_value;
    update_value = mcu_get_dp_download_value(value,length);
    wifi_uart_write_command_value(RCP_SET_CMD,update_value);
    syr_report_interval_set(update_value);
}
void rcp_get_callback(const unsigned char value[], unsigned short length)
{
    wifi_uart_write_command_value(RCP_GET_CMD,syr_report_interval_get());
}
void wti_set_callback(const unsigned char value[], unsigned short length)
{
    unsigned int update_value;
    update_value = mcu_get_dp_download_value(value,length);
    wifi_uart_write_command_value(WTI_SET_CMD,update_value);
    syr_telemetry_interval_set(update_value);
}
void wti_get_callback(const unsigned char value[], unsigned short length)
{
    wifi_uart_write_command_value(WTI_GET_CMD,syr_telemetry_interval_get());
}
void apt_set_callback(const unsigned char value[], unsigned short length)
{
    unsigned int update_value;
    update_value = mcu_get_dp_download_value(value,length);
    wifi_uart_write_command_value(APT_SET_CMD,update_value);
    syr_ap_timeout_set(update_value);
}
void apt_get_callback(const unsigned char value[], unsigned short length)
{
    wifi_uart_write_command_value(APT_GET_CMD,syr_ap_timeout_get());
}
void wad_set_callback(const unsigned char value[], unsigned short length)
{
    unsigned int update_value;
    update_value = mcu_get_dp_download_value(value,length);
    wifi_uart_write_command_value(WAD_SET_CMD,update_value);;
    ap_timeout_control(update_value);
}
void wad_get_callback(const unsigned char value[], unsigned short length)
{
    wifi_uart_write_command_value(WAD_GET_CMD,syr_wifi_ap_enable_get());
}
void emr_set_callback(const unsigned char value[], unsigned short length)
{
    unsigned int update_value;
    update_value = mcu_get_dp_download_value(value,length);
    wifi_uart_write_command_value(EMR_SET_CMD,update_value);
    syr_telemetry_timeout_set(update_value);
}
void emr_get_callback(const unsigned char value[], unsigned short length)
{
    wifi_uart_write_command_value(EMR_GET_CMD,syr_telemetry_timeout_get());
}
void vlv_get_callback(const unsigned char value[], unsigned short length)
{
    wifi_uart_write_command_value(VLV_GET_CMD,Get_MotoStatus());
}
void alm_get_callback(const unsigned char value[], unsigned short length)
{
    uint8_t* array_ptr = RT_NULL;
    get_error_array(&array_ptr);
    uint8_t* alm_buffer = rt_malloc(64);
    rt_sprintf(alm_buffer, "%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x",       \
            array_ptr[0],array_ptr[1],array_ptr[2],array_ptr[3],\
            array_ptr[4],array_ptr[5],array_ptr[6],array_ptr[7]);
    mcu_dp_string_update(ALM_GET_CMD,alm_buffer,rt_strlen(alm_buffer));
    rt_free(alm_buffer);
}

void alw_get_callback(const unsigned char value[], unsigned short length)
{
    uint8_t* array_ptr = RT_NULL;
    get_warning_array(&array_ptr);
    uint8_t* alw_buffer = rt_malloc(64);
    rt_sprintf(alw_buffer, "%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x",       \
            array_ptr[0],array_ptr[1],array_ptr[2],array_ptr[3],\
            array_ptr[4],array_ptr[5],array_ptr[6],array_ptr[7]);
    mcu_dp_string_update(ALW_GET_CMD,alw_buffer,rt_strlen(alw_buffer));
    rt_free(alw_buffer);
}

void aln_get_callback(const unsigned char value[], unsigned short length)
{
    uint8_t* array_ptr = RT_NULL;
    get_notify_array(&array_ptr);
    uint8_t* aln_buffer = rt_malloc(64);
    rt_sprintf(aln_buffer, "%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x",       \
            array_ptr[0],array_ptr[1],array_ptr[2],array_ptr[3],\
            array_ptr[4],array_ptr[5],array_ptr[6],array_ptr[7]);
    mcu_dp_string_update(ALN_GET_CMD,aln_buffer,rt_strlen(aln_buffer));
    rt_free(aln_buffer);
}
