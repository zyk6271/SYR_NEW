#include "rtthread.h"
#include "uart_core.h"
#include "uart_api.h"

#define DBG_TAG "uart_core"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

static volatile unsigned char wifi_data_process_buf[PROTOCOL_HEAD + WIFI_DATA_PROCESS_LMT];      //串口数据处理缓存
static volatile unsigned char wifi_uart_rx_buf[PROTOCOL_HEAD + WIFI_UART_RECV_BUF_LMT];          //串口接收缓存
static volatile unsigned char wifi_uart_tx_buf[PROTOCOL_HEAD + WIFIR_UART_SEND_BUF_LMT];         //串口发送缓存

static volatile unsigned char *queue_in = NULL;
static volatile unsigned char *queue_out = NULL;

unsigned short firm_size;                                  //升级包一包的大小

volatile unsigned char stop_update_flag;                                                 //停止一切数据上报

const DOWNLOAD_CMD_S download_cmd[] =
{
    {RST_SET_CMD, DP_TYPE_VALUE},
    {DEF_SET_CMD, DP_TYPE_VALUE},
    {RAS_SET_CMD, DP_TYPE_VALUE},
    {RAS_GET_CMD, DP_TYPE_VALUE},
    {RAS_PUT_CMD, DP_TYPE_VALUE},
    {CND_GET_CMD, DP_TYPE_VALUE},
    {NET_GET_CMD, DP_TYPE_VALUE},
    {BAT_GET_CMD, DP_TYPE_VALUE},
    {ALA_GET_CMD, DP_TYPE_VALUE},
    {ALA_SET_CMD, DP_TYPE_VALUE},
    {ALR_GET_CMD, DP_TYPE_VALUE},
    {ALR_SET_CMD, DP_TYPE_VALUE},
    {RSE_SET_CMD, DP_TYPE_VALUE},
    {RSE_GET_CMD, DP_TYPE_VALUE},
    {RSE_PUT_CMD, DP_TYPE_VALUE},
    {RSA_SET_CMD, DP_TYPE_VALUE},
    {RSA_GET_CMD, DP_TYPE_VALUE},
    {RSA_PUT_CMD, DP_TYPE_VALUE},
    {RSI_SET_CMD, DP_TYPE_VALUE},
    {RSI_GET_CMD, DP_TYPE_VALUE},
    {RSI_PUT_CMD, DP_TYPE_VALUE},
    {RSD_SET_CMD, DP_TYPE_VALUE},
    {RSD_GET_CMD, DP_TYPE_VALUE},
    {RSD_PUT_CMD, DP_TYPE_VALUE},
    {CNF_SET_CMD, DP_TYPE_VALUE},
    {CNF_GET_CMD, DP_TYPE_VALUE},
    {CNF_PUT_CMD, DP_TYPE_VALUE},
    {CNL_SET_CMD, DP_TYPE_VALUE},
    {CNL_GET_CMD, DP_TYPE_VALUE},
    {CNL_PUT_CMD, DP_TYPE_VALUE},
    {SSE_SET_CMD, DP_TYPE_VALUE},
    {SSE_GET_CMD, DP_TYPE_VALUE},
    {SSE_PUT_CMD, DP_TYPE_VALUE},
    {SSA_SET_CMD, DP_TYPE_VALUE},
    {SSA_GET_CMD, DP_TYPE_VALUE},
    {SSA_PUT_CMD, DP_TYPE_VALUE},
    {SSD_SET_CMD, DP_TYPE_VALUE},
    {SSD_GET_CMD, DP_TYPE_VALUE},
    {SSD_PUT_CMD, DP_TYPE_VALUE},
    {LNG_SET_CMD, DP_TYPE_VALUE},
    {LNG_GET_CMD, DP_TYPE_VALUE},
    {LNG_PUT_CMD, DP_TYPE_VALUE},
    {SRN_GET_CMD, DP_TYPE_VALUE},
    {SUP_GET_CMD, DP_TYPE_VALUE},
    {SUP_PUT_CMD, DP_TYPE_VALUE},
    {VER_GET_CMD, DP_TYPE_VALUE},
    {COM_SET_CMD, DP_TYPE_VALUE},
    {COM_GET_CMD, DP_TYPE_VALUE},
    {COM_PUT_CMD, DP_TYPE_VALUE},
    {COA_SET_CMD, DP_TYPE_VALUE},
    {COA_GET_CMD, DP_TYPE_VALUE},
    {COA_PUT_CMD, DP_TYPE_VALUE},
    {COD_SET_CMD, DP_TYPE_VALUE},
    {COD_GET_CMD, DP_TYPE_VALUE},
    {COD_PUT_CMD, DP_TYPE_VALUE},
    {COE_SET_CMD, DP_TYPE_VALUE},
    {COE_GET_CMD, DP_TYPE_VALUE},
    {COE_PUT_CMD, DP_TYPE_VALUE},
    {CND_PUT_CMD, DP_TYPE_VALUE},
    {WST_GET_CMD, DP_TYPE_VALUE},
    {WST_SET_CMD, DP_TYPE_VALUE},
    {EMR_SET_CMD, DP_TYPE_VALUE},
    {EMR_GET_CMD, DP_TYPE_VALUE},
    {RCP_SET_CMD, DP_TYPE_VALUE},
    {RCP_GET_CMD, DP_TYPE_VALUE},
};

/**
 * @brief  将int类型拆分四个字节
 * @param[in] {number} 4字节原数据
 * @param[out] {value} 处理完成后4字节数据
 * @return Null
 */
void int_to_byte(unsigned long number,unsigned char value[4])
{
    value[0] = number >> 24;
    value[1] = number >> 16;
    value[2] = number >> 8;
    value[3] = number & 0xff;
}
/**
 * @brief  将4字节合并为1个32bit变量
 * @param[in] {value} 4字节数组
 * @return 合并完成后的32bit变量
 */
unsigned long byte_to_int(const unsigned char value[4])
{
    unsigned long nubmer = 0;

    nubmer = (unsigned long)value[0];
    nubmer <<= 8;
    nubmer |= (unsigned long)value[1];
    nubmer <<= 8;
    nubmer |= (unsigned long)value[2];
    nubmer <<= 8;
    nubmer |= (unsigned long)value[3];

    return nubmer;
}
/**
 * @brief  mcu获取bool型下发dp值
 * @param[in] {value} dp数据缓冲区地址
 * @param[in] {len} dp数据长度
 * @return 当前dp值
 * @note   Null
 */
unsigned char mcu_get_dp_download_bool(const unsigned char value[],unsigned short len)
{
    return(value[0]);
}
/**
 * @brief  mcu获取value型下发dp值
 * @param[in] {value} dp数据缓冲区地址
 * @param[in] {len} dp数据长度
 * @return 当前dp值
 * @note   Null
 */
unsigned long mcu_get_dp_download_value(const unsigned char value[],unsigned short len)
{
    return(byte_to_int(value));
}
/**
 * @brief  写wifi_uart字节
 * @param[in] {dest} 缓存区地址偏移
 * @param[in] {byte} 写入字节值
 * @return 写入完成后的总长度
 */
unsigned short set_wifi_uart_byte(unsigned short dest, unsigned char byte)
{
    unsigned char *obj = (unsigned char *)wifi_uart_tx_buf + DATA_START + dest;

    *obj = byte;
    dest += 1;

    return dest;
}
/**
 * @brief  写wifi_uart_buffer
 * @param[in] {dest} 缓存区地址偏移
 * @param[in] {src} 源地址（需要发送的数据）
 * @param[in] {len} 需要发送的数据长度
 * @return 写入完成后的总长度
 */
unsigned short set_wifi_uart_buffer(unsigned short dest, const unsigned char *src, unsigned short len)
{
    if(NULL == src) {
        return dest;
    }
    unsigned char *obj = (unsigned char *)wifi_uart_tx_buf + DATA_START + dest;

    my_memcpy(obj,src,len);

    dest += len;
    return dest;
}
/**
 * @brief  判断串口接收缓存中是否有数据
 * @param  Null
 * @return 是否有数据  0:无/1:有
 */
static unsigned char get_queue_total_data(void)
{
  if(queue_in != queue_out)
    return 1;
  else
    return 0;
}
/**
 * @brief  串口发送一段数据
 * @param[in] {in} 发送缓存指针
 * @param[in] {len} 数据发送长度
 * @return Null
 */
void wifi_uart_write_data(unsigned char *in, unsigned short len)
{
    if((NULL == in) || (0 == len)) {
        return;
    }

    while(len --) {
        uart_transmit_output(*in);
        in ++;
    }
}
/**
 * @brief  value型dp数据上传
 * @param[in] {dpid} dpid号
 * @param[in] {value} 当前dp值
 * @param[in] {sub_id_buf} 存放子设备id的首地址
 * @param[in] {sub_id_len} 子设备id长度
 * @return Null
 * @note   Null
 */
unsigned char mcu_dp_value_update(unsigned char dpid,unsigned long value)
{
    unsigned short send_len = 0;

    if(stop_update_flag == ENABLE)
        return SUCCESS;

    send_len = set_wifi_uart_byte(send_len,dpid);
    send_len = set_wifi_uart_byte(send_len,DP_TYPE_VALUE);
    //
    send_len = set_wifi_uart_byte(send_len,0);
    send_len = set_wifi_uart_byte(send_len,4);
    //
    send_len = set_wifi_uart_byte(send_len,value >> 24);
    send_len = set_wifi_uart_byte(send_len,value >> 16);
    send_len = set_wifi_uart_byte(send_len,value >> 8);
    send_len = set_wifi_uart_byte(send_len,value & 0xff);

    wifi_uart_write_frame(DATA_ISSUED_CMD, MCU_TX_VER, send_len);

    return SUCCESS;
}
/**
 * @brief  string型dp数据上传
 * @param[in] {dpid} dpid号
 * @param[in] {value} 当前dp值指针
 * @param[in] {len} 数据长度
 * @param[in] {sub_id_buf} 存放子设备id的首地址
 * @param[in] {sub_id_len} 子设备id长度
 * @return Null
 * @note   Null
 */
unsigned char mcu_dp_string_update(unsigned char dpid,const unsigned char value[],unsigned short len)
{
    unsigned short send_len = 0;

    if(stop_update_flag == ENABLE)
        return SUCCESS;

    //
    send_len = set_wifi_uart_byte(send_len,dpid);
    send_len = set_wifi_uart_byte(send_len,DP_TYPE_STRING);
    //
    send_len = set_wifi_uart_byte(send_len,len / 0x100);
    send_len = set_wifi_uart_byte(send_len,len % 0x100);
    //
    send_len = set_wifi_uart_buffer(send_len,(unsigned char *)value,len);

    wifi_uart_write_frame(STATE_UPLOAD_CMD, MCU_TX_VER, send_len);

    return SUCCESS;
}
/**
 * @brief  向wifi串口发送指定数据
 * @param[in] {fr_type} 帧类型
 * @param[in] {len} 数据长度
 * @return Null
 */
void wifi_uart_write_command_value(unsigned char command, unsigned int data)
{
    mcu_dp_value_update(command,data);
}
/**
 * @brief  向wifi串口发送一帧数据
 * @param[in] {fr_type} 帧类型
 * @param[in] {fr_ver} 帧版本
 * @param[in] {len} 数据长度
 * @return Null
 */
void wifi_uart_write_frame(unsigned char fr_type, unsigned char fr_ver, unsigned short len)
{
    unsigned char check_sum = 0;

    wifi_uart_tx_buf[HEAD_FIRST] = 0x55;
    wifi_uart_tx_buf[HEAD_SECOND] = 0xaa;
    wifi_uart_tx_buf[PROTOCOL_VERSION] = fr_ver;
    wifi_uart_tx_buf[FRAME_TYPE] = fr_type;
    wifi_uart_tx_buf[LENGTH_HIGH] = len >> 8;
    wifi_uart_tx_buf[LENGTH_LOW] = len & 0xff;

    len += PROTOCOL_HEAD;
    check_sum = get_check_sum((unsigned char *)wifi_uart_tx_buf, len - 1);
    wifi_uart_tx_buf[len - 1] = check_sum;
    wifi_uart_write_data((unsigned char *)wifi_uart_tx_buf, len);
}

/**
 * @brief  读取队列1字节数据
 * @param  Null
 * @return Read the data
 */
unsigned char Queue_Read_Byte(void)
{
  unsigned char value = 0;

  if(queue_out != queue_in)
  {
    //有数据
    if(queue_out >= (unsigned char *)(wifi_uart_rx_buf + sizeof(wifi_uart_rx_buf)))
    {
      //数据已经到末尾
      queue_out = (unsigned char *)(wifi_uart_rx_buf);
    }

    value = *queue_out ++;
  }

  return value;
}
/**
 * @brief  串口发送数据
 * @param[in] {value} 串口要发送的1字节数据
 * @return Null
 */
void uart_transmit_output(unsigned char value)
{
    extern void WiFi_Byte_Send(uint8_t data);
    WiFi_Byte_Send(value);
}
/**
 * @brief  串口接收数据暂存处理
 * @param[in] {value} 串口收到的1字节数据
 * @return Null
 * @note   在MCU串口处理函数中调用该函数,并将接收到的数据作为参数传入
 */
void wifi_uart_receive_buf_input(unsigned char *value,uint32_t length)
{
    if(1 == queue_out - queue_in) {
        //数据队列满
    }else if((queue_in > queue_out) && ((queue_in - queue_out) >= length)) {
        //数据队列满
    }else {
        //队列不满
        if(queue_in >= (unsigned char *)(wifi_uart_rx_buf + length)) {
            queue_in = (unsigned char *)(wifi_uart_rx_buf);
        }
        uint32_t i = 0;
        while(i < length)
        {
            *queue_in ++ = value[i++];
        }

    }
}
/**
 * @brief  串口接收数据暂存处理
 * @param[in] {value} 串口收到的1字节数据
 * @return Null
 * @note   在MCU串口处理函数中调用该函数,并将接收到的数据作为参数传入
 */
void wifi_uart_receive_input(unsigned char value)
{
    if(1 == queue_out - queue_in) {
        //数据队列满
    }else if((queue_in > queue_out) && ((queue_in - queue_out) >= sizeof(wifi_data_process_buf))) {
        //数据队列满
    }else {
        //队列不满
        if(queue_in >= (unsigned char *)(wifi_uart_rx_buf + sizeof(wifi_uart_rx_buf))) {
            queue_in = (unsigned char *)(wifi_uart_rx_buf);
        }

        *queue_in ++ = value;
    }
}
/**
 * @brief  协议串口初始化函数
 * @param  Null
 * @return Null
 * @note   在MCU初始化代码中调用该函数
 */
void wifi_protocol_init(void)
{
    //#error " 请在main函数中添加wifi_protocol_init()完成wifi协议初始化,并删除该行"
    queue_in = (unsigned char *)wifi_uart_rx_buf;
    queue_out = (unsigned char *)wifi_uart_rx_buf;

    stop_update_flag = DISABLE;
}
/**
 * @brief  wifi串口数据处理服务
 * @param  Null
 * @return Null
 * @note   在MCU主函数while循环中调用该函数
 */
void wifi_uart_service(void)
{
    static unsigned short rx_in = 0;
    unsigned short offset = 0;
    unsigned short rx_value_len = 0;             //数据帧长度

    while((rx_in < sizeof(wifi_data_process_buf)) && get_queue_total_data() > 0) {
        wifi_data_process_buf[rx_in ++] = Queue_Read_Byte();
    }

    if(rx_in < PROTOCOL_HEAD)
        return;

    while((rx_in - offset) >= PROTOCOL_HEAD) {
        if(wifi_data_process_buf[offset + HEAD_FIRST] != FRAME_FIRST) {
            offset ++;
            continue;
        }

        if(wifi_data_process_buf[offset + HEAD_SECOND] != FRAME_SECOND) {
            offset ++;
            continue;
        }

//        if(wifi_data_process_buf[offset + PROTOCOL_VERSION] != MCU_RX_VER) {
//            offset += 2;
//            continue;
//        }

        rx_value_len = wifi_data_process_buf[offset + LENGTH_HIGH] * 0x100;
        rx_value_len += (wifi_data_process_buf[offset + LENGTH_LOW] + PROTOCOL_HEAD);
        if(rx_value_len > sizeof(wifi_data_process_buf)) {
            offset += 3;
            continue;
        }

        if((rx_in - offset) < rx_value_len) {
            break;
        }

        //数据接收完成
        if(get_check_sum((unsigned char *)wifi_data_process_buf + offset,rx_value_len - 1) != wifi_data_process_buf[offset + rx_value_len - 1]) {
            //校验出错
            LOG_E("crc error (rx_value_len:%d  offset %d  crc:0x%X  but data:0x%X)\r\n",rx_value_len,offset,get_check_sum((unsigned char *)wifi_data_process_buf + offset,rx_value_len - 1),wifi_data_process_buf[offset + rx_value_len - 1]);
            offset += 3;
            continue;
        }
        data_handle(offset);
        offset += rx_value_len;
    }
    rx_in -= offset;
    if(rx_in > 0) {
        my_memcpy((char *)wifi_data_process_buf,(const char *)wifi_data_process_buf + offset,rx_in);
    }
}
/**
 * @brief  获取所有dp命令总和
 * @param[in] Null
 * @return 下发命令总和
 * @note   该函数用户不能修改
 */
unsigned char get_download_cmd_total(void)
{
    return(sizeof(download_cmd) / sizeof(download_cmd[0]));
}
/**
 * @brief  获取制定DPID在数组中的序号
 * @param[in] {dpid} dpid
 * @return dp序号
 */
unsigned char get_dowmload_dpid_index(unsigned char dpid)
{
    unsigned char index;
    unsigned char total = get_download_cmd_total();

    for(index = 0; index < total; index ++) {
        if(download_cmd[index].dp_id == dpid) {
            break;
        }
    }

    return index;
}
static unsigned char data_point_handle(const unsigned char value[])
{
    //这边是数据下发函数，请根据实际情况调用
    unsigned char dp_id,index;
    unsigned char dp_type;
    unsigned char ret;
    unsigned short dp_len;

    dp_id = value[0];
    dp_type = value[1];
    dp_len = (value[2] <<8) + value[3];

    index = get_dowmload_dpid_index(dp_id);

    if(dp_type != download_cmd[index].dp_type) {
        //错误提示
        return 0;
    }else {
        //请根据dpid自行做出不同子设备的dp处理
        //请根据sub_id来决定给哪个子设备下发数据
        //请在该函数根据子设备的id自行实现子设备的dpid处理，"0000"代表的是需要进行下发数据处理的是网关本身
        ret = dp_download_handle(dp_id, value + 4, dp_len);
    }

    return ret;
}
/**
 * @brief  网关本身MCU升级包传输
 * @param[in] {value} 固件数据缓冲区
 * @param[in] {position} 当前数据包在于固件位置
 * @param[in] {length} 当前固件包长度(固件包长度为0时,表示固件包发送完成)
 * @return 成功/失败
 * @note   MCU需要自行实现该功能
 */
unsigned char mcu_firm_update_handle(const unsigned char value[],unsigned long position,unsigned short length)
{
    //#error "请自行完成MCU固件升级代码,完成后请删除该行"
    unsigned char ret = 0;
    if(length == 0) {
        wifi_ota_end(value,position);
        //固件数据发送完成
    }else {
        ret = wifi_ota_receive(value,position,length);
        //固件数据处理
    }

    if(ret == RT_EOK){
        return SUCCESS;
    }
    else {
        return ERROR;
    }
}
/**
 * @brief  数据帧处理
 * @param[in] {offset} 数据起始位
 * @return Null
 */
void data_handle(unsigned short offset)
{
    unsigned char *firmware_addr = NULL;
    static unsigned long firm_length;                          //MCU升级文件长度
    static unsigned char firm_update_flag;                     //MCU升级标志
    unsigned long dp_len_up;                                   //mcu升级的dplen

    unsigned short dp_len;

    unsigned char ret = 0;
    unsigned short i,total_len;
    unsigned char cmd_type = wifi_data_process_buf[offset + FRAME_TYPE];

    switch(cmd_type) {
        case PRODUCT_INFO_CMD:                                //产品信息
            product_info_update();
        case WIFI_STATE_CMD:                                  //wifi联网状态
            wifi_uart_write_frame(WIFI_STATE_CMD, MCU_TX_VER, 0);
            wifi_status_change(wifi_data_process_buf[offset + DATA_START]);
        case DATA_ISSUED_CMD:                                 //命令下发
            total_len = (wifi_data_process_buf[offset + LENGTH_HIGH] << 8) | wifi_data_process_buf[offset + LENGTH_LOW];

            for(i = 0 ;i < total_len; ) {
                dp_len = (wifi_data_process_buf[offset + DATA_START + i + 2] << 8) | wifi_data_process_buf[offset + DATA_START + i + 3];

                ret = data_point_handle((unsigned char *)wifi_data_process_buf + offset + DATA_START + i);

                i += (dp_len + 4);
            }
        break;
        case UPDATE_CONTROL_CMD:                                //升级开始
            update_control_parse(wifi_data_process_buf[offset + DATA_START]<<8 | wifi_data_process_buf[offset + DATA_START + 1]);
        break;
        case TELEMETRY_CONTROL_CMD:                           //遥测控制
            total_len = (wifi_data_process_buf[offset + LENGTH_HIGH] << 8) | wifi_data_process_buf[offset + LENGTH_LOW];
            telemetry_recv_handle(wifi_data_process_buf[offset + DATA_START]);
        break;
        case UPDATE_START_CMD:                                //升级开始
            //升级包单包大小
            firm_size = 512;

            //升级文件大小
            firm_length = wifi_data_process_buf[offset + DATA_START];
            firm_length <<= 8;
            firm_length |= wifi_data_process_buf[offset + DATA_START + 1];
            firm_length <<= 8;
            firm_length |= wifi_data_process_buf[offset + DATA_START + 2];
            firm_length <<= 8;
            firm_length |= wifi_data_process_buf[offset + DATA_START + 3];

            firm_update_flag = UPDATE_START_CMD;
            wifi_ota_begin(firm_length);
        break;

        case UPDATE_TRANS_CMD:                                //升级传输
            if(firm_update_flag == UPDATE_START_CMD) {

            //停止一切数据上报
            stop_update_flag = ENABLE;
            //一包数据总长度
            total_len = (wifi_data_process_buf[offset + LENGTH_HIGH] << 8) | wifi_data_process_buf[offset + LENGTH_LOW];

            //包偏移
            dp_len_up = wifi_data_process_buf[offset + DATA_START];
            dp_len_up <<= 8;
            dp_len_up |= wifi_data_process_buf[offset + DATA_START + 1];
            dp_len_up <<= 8;
            dp_len_up |= wifi_data_process_buf[offset + DATA_START + 2];
            dp_len_up <<= 8;
            dp_len_up |= wifi_data_process_buf[offset + DATA_START + 3];

            //数据包
            firmware_addr = (unsigned char *)wifi_data_process_buf;
            firmware_addr += (offset + DATA_START + 4);  //数据包内容起始地址
            //没有数据内容或者包偏移等于升级文件大小（数据发送完成）
            if((total_len == 4) && (dp_len_up == firm_length)) {
                //最后一包（代表数据发送完成）
                ret = mcu_firm_update_handle(firmware_addr,dp_len_up,0);

                firm_update_flag = 0;
            }else if((total_len - 4) <= firm_size) { //数据pack里面还有数据
                //数据未发送完成，进行数据处理
                ret = mcu_firm_update_handle(firmware_addr,dp_len_up,total_len - 4);
            }else {
                firm_update_flag = 0;
                ret = ERROR;
            }

            if(ret == SUCCESS) {
                wifi_uart_write_frame(UPDATE_TRANS_CMD, MCU_TX_VER, 0);
            }
            //恢复一切数据上报
            stop_update_flag = DISABLE;
        }
        break;
        case UPDATE_PROGRSS_CMD:                                //网关升级进度
            wifi_progress_update(wifi_data_process_buf[offset + DATA_START]);
        break;
    }
}
