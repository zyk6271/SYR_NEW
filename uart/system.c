/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-11-01     Rick       the first version
 */
#include "rtthread.h"
#include "uart_core.h"
#include "uart_api.h"
#include "cJSON.h"

#define DBG_TAG "uart_system"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

uint8_t WIFI_AP_Enable;
uint16_t ota_status;
uint8_t MCU_VER[10] = {"0.0.25"};

void product_version_print(void)
{
    LOG_I("System Version is %s\r\n",MCU_VER);
}

/**
 * @brief  产品信息上传
 * @return Null
 */
void product_info_update(void)
{
    unsigned char length = 0;

    cJSON *root = NULL;

    root = cJSON_CreateObject();
    if(NULL == root) {
        //可在此添加提示信息，如：printf("xxx");
        return;
    }

    cJSON_AddStringToObject(root, "ver", MCU_VER);
    cJSON_AddNumberToObject(root, "ap", syr_wifi_ap_enable_get());

    char* out = cJSON_PrintUnformatted(root);

    cJSON_Delete(root);

    if(NULL == out) {
        //可在此添加提示信息，如：printf("xxx");
        return;
    }

    length = set_wifi_uart_buffer(length, out, rt_strlen(out));

    wifi_uart_write_frame(PRODUCT_INFO_CMD, MCU_TX_VER, length);

    rt_free(out);
}

void wifi_status_change(uint8_t state)
{
    extern uint8_t wifi_ota_update_flag;
    wifi_led(state);
    switch(state)
    {
    case 0://熄灭全部
        break;
    case 1://AP模式
        break;
    case 2://连接路由器
        break;
    case 3://连接路由器成功
        wifi_connect_success();
        break;
    case 4://连接微软云成功
        if(wifi_ota_update_flag)
        {
            wifi_ota_request(2);
            wifi_ota_update_flag = 0;
        }
        else
        {
            telemetry_flush();
        }
        break;
    default:
        break;
    }
}
uint8_t wifi_valid_check(void)
{
    extern uint8_t wifi_status;
    if(wifi_status == 4)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void update_control_parse(uint16_t value)
{
    ota_status = value;
}
/**
 * @brief  dp下发处理函数
 * @param[in] {dpid} dpid 序号
 * @param[in] {value} dp数据缓冲区地址
 * @param[in] {length} dp数据长度
 * @return dp处理结果
 * -           0(ERROR): 失败
 * -           1(SUCCESS): 成功
 * @note   该函数用户不能修改
 */
unsigned char dp_download_handle(unsigned char dpid,const unsigned char value[], unsigned short length)
{
    /****************************************************************
    当前函数处理可下发/可上报数据调用,具体函数内需要实现下发数据处理
    完成用需要将处理结果反馈至APP端,否则APP会认为下发失败
    请在该函数根据子设备的id自行实现子设备的dpid处理
    ****************************************************************/
    unsigned char ret = 0;
    switch(dpid) {
        case RAS_SET_CMD:ras_set_callback(value,length);break;//冲洗开始
        case RAS_GET_CMD:ras_get_callback(value,length);break;//冲洗查询
        case CND_GET_CMD:cnd_get_callback(value,length);break;//TDS获取
        case NET_GET_CMD:net_get_callback(value,length);break;//DC电压获取
        case BAT_GET_CMD:bat_get_callback(value,length);break;//电池电压获取
        case SUP_GET_CMD:sup_get_callback(value,length);break;//电源连接方式
        case RSE_SET_CMD:rse_set_callback(value,length);break;//自动提醒时间设置
        case RSE_GET_CMD:rse_get_callback(value,length);break;//自动提醒时间获取
        case RSA_SET_CMD:rsa_set_callback(value,length);break;//自动冲洗时间设置
        case RSA_GET_CMD:rsa_get_callback(value,length);break;//自动冲洗时间获取
        case RSI_SET_CMD:rsi_set_callback(value,length);break;//时间范围设置
        case RSI_GET_CMD:rsi_get_callback(value,length);break;//时间范围获取
        case RSD_SET_CMD:rsd_set_callback(value,length);break;//冲洗时间设置
        case RSD_GET_CMD:rsd_get_callback(value,length);break;//冲洗时间获取
        case CNF_SET_CMD:cnf_set_callback(value,length);break;//TDS校准值设置
        case CNF_GET_CMD:cnf_get_callback(value,length);break;//TDS校准值获取
        case CNL_SET_CMD:cnl_set_callback(value,length);break;//TDS阈值设置
        case CNL_GET_CMD:cnl_get_callback(value,length);break;//TDS阈值获取
        case SSE_SET_CMD:sse_set_callback(value,length);break;//提醒开关设置
        case SSE_GET_CMD:sse_get_callback(value,length);break;//提醒开关获取
        case SSA_SET_CMD:ssa_set_callback(value,length);break;//自动开关设置
        case SSA_GET_CMD:ssa_get_callback(value,length);break;//自动开关获取
        case SSD_SET_CMD:ssd_set_callback(value,length);break;//压差开关设置
        case SSD_GET_CMD:ssd_get_callback(value,length);break;//压差开关获取
        case LNG_SET_CMD:lng_set_callback(value,length);break;//语言设置
        case LNG_GET_CMD:lng_get_callback(value,length);break;//语言获取
        case COM_SET_CMD:com_set_callback(value,length);break;//Manual计数器清空
        case COM_GET_CMD:com_get_callback(value,length);break;//Manual计数器获取
        case COA_SET_CMD:coa_set_callback(value,length);break;//Auto计数器清空
        case COA_GET_CMD:coa_get_callback(value,length);break;//Auto计数器获取
        case COD_SET_CMD:cod_set_callback(value,length);break;//Delta计数器清空
        case COD_GET_CMD:cod_get_callback(value,length);break;//Delta计数器获取
        case COE_SET_CMD:coe_set_callback(value,length);break;//Error计数器清空
        case COE_GET_CMD:coe_get_callback(value,length);break;//Error计数器获取
        case EMR_SET_CMD:emr_set_callback(value,length);break;//遥测超时获取
        case EMR_GET_CMD:emr_get_callback(value,length);break;//遥测超时设置
        case RCP_SET_CMD:rcp_set_callback(value,length);break;//唤醒周期获取
        case RCP_GET_CMD:rcp_get_callback(value,length);break;//唤醒周期设置
        case WTI_SET_CMD:wti_set_callback(value,length);break;//遥测超时获取
        case WTI_GET_CMD:wti_get_callback(value,length);break;//遥测超时设置
        case APT_SET_CMD:apt_set_callback(value,length);break;//热点超时获取
        case APT_GET_CMD:apt_get_callback(value,length);break;//热点超时设置
        case VLV_GET_CMD:vlv_get_callback(value,length);break;//阀门状态获取
        case ALM_GET_CMD:alm_get_callback(value,length);break;//ERROR历史状态
        case ALW_GET_CMD:alw_get_callback(value,length);break;//ERROR历史状态
        case ALN_GET_CMD:aln_get_callback(value,length);break;//ERROR历史状态
        case WAD_SET_CMD:wad_set_callback(value,length);break;//AP开关获取
        case WAD_GET_CMD:wad_get_callback(value,length);break;//AP开关设置
        default:
            break;
    }
    return ret;
}
void wifi_progress_update(uint8_t value)
{
    extern uint8_t wifi_progress;
    wifi_progress = value;
}
void telemetry_recv_handle(uint8_t command)
{
    extern rt_sem_t telemetry_recv_sem;
    switch(command)
    {
    case 1:
        telemetry_upload();
        break;
    case 2:
        rt_sem_release(telemetry_recv_sem);
        break;
    }
}

void device_reboot(void)
{
    LOG_I("device_reboot");
    rt_hw_cpu_reset();
}
void device_factory_set(void)
{
    LOG_I("device_factory_set");
    wifi_uart_write_frame(FACTORY_SET_CMD,MCU_TX_VER,0);
    Flash_Clear();
}
