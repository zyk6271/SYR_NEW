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
 * @brief  向wifi串口发送指定数据
 * @param[in] {fr_type} 帧类型
 * @param[in] {len} 数据长度
 * @return Null
 */
void wifi_uart_write_command_value(unsigned char command, unsigned int data)
{
    unsigned char check_sum = 0;

    wifi_uart_tx_buf[HEAD_FIRST] = 0x55;
    wifi_uart_tx_buf[HEAD_SECOND] = 0xaa;
    wifi_uart_tx_buf[PROTOCOL_VERSION] = 0;
    wifi_uart_tx_buf[FRAME_TYPE] = command;
    wifi_uart_tx_buf[LENGTH_HIGH] = 0x00;
    wifi_uart_tx_buf[LENGTH_LOW] = 0x04;
    wifi_uart_tx_buf[DATA_START] = (data >> 24) & 0xFF;
    wifi_uart_tx_buf[DATA_START+1] = (data >> 16) & 0xFF;
    wifi_uart_tx_buf[DATA_START+2] = (data >> 8) & 0xFF;
    wifi_uart_tx_buf[DATA_START+3] = data & 0xff;

    uint8_t len = PROTOCOL_HEAD + 4;
    check_sum = get_check_sum((unsigned char *)wifi_uart_tx_buf, len - 1);
    wifi_uart_tx_buf[len - 1] = check_sum;
    //
    wifi_uart_write_data((unsigned char *)wifi_uart_tx_buf, len);
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
    //
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
}
/**
 * @brief  wifi串口数据处理服务
 * @param  Null
 * @return Null
 * @note   在MCU主函数while循环中调用该函数
 */
void wifi_uart_service(void)
{
    //#error "请直接在main函数的while(1){}中添加wifi_uart_service(),调用该函数不要加任何条件判断,完成后删除该行"
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

        if(wifi_data_process_buf[offset + PROTOCOL_VERSION] != MCU_RX_VER) {
            offset += 2;
            continue;
        }

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
            LOG_E("crc error (crc:0x%X  but data:0x%X)\r\n",get_check_sum((unsigned char *)wifi_data_process_buf + offset,rx_value_len - 1),wifi_data_process_buf[offset + rx_value_len - 1]);
            offset += 3;
            continue;
        }
        //LOG_I("Command is %02X",wifi_data_process_buf[offset + FRAME_TYPE]);
        data_handle(offset,rx_value_len - PROTOCOL_HEAD);
        offset += rx_value_len;
    }
    rx_in -= offset;
    if(rx_in > 0) {
        my_memcpy((char *)wifi_data_process_buf,(const char *)wifi_data_process_buf + offset,rx_in);
    }
}

/**
 * @brief  数据帧处理
 * @param[in] {offset} 数据起始位
 * @return Null
 */
void data_handle(unsigned short offset,unsigned char value_len)
{
    uint32_t value = 0;
    unsigned char cmd_type = wifi_data_process_buf[offset + FRAME_TYPE];
    if(value_len)
    {
        value = wifi_data_process_buf[offset + DATA_HIGH] * 0x1000000;
        value += wifi_data_process_buf[offset + DATA_HIGH + 1] * 0x10000;
        value += wifi_data_process_buf[offset + DATA_HIGH + 2] * 0x100;
        value += wifi_data_process_buf[offset + DATA_HIGH + 3];
    }
    switch(cmd_type) {
        case RST_SET_CMD:rst_set_cb(value);break;//复位
        case DEF_SET_CMD:def_set_cb(value);break;//出厂设置
        case RAS_SET_CMD:ras_set_cb(value);break;//冲洗开始
        case RAS_GET_CMD:ras_get_cb();break;//冲洗查询
        case CND_GET_CMD:cnd_get_cb();break;//TDS获取
        case NET_GET_CMD:net_get_cb();break;//DC电压获取
        case BAT_GET_CMD:bat_get_cb();break;//电池电压获取
        case ALA_GET_CMD:ala_get_cb();break;//自动冲洗剩余时间获取
        case ALA_SET_CMD:ala_set_cb(value);break;//自动冲洗剩余时间设置
        case ALR_GET_CMD:alr_get_cb();break;//自动提醒剩余时间获取
        case ALR_SET_CMD:alr_set_cb(value);break;//自动提醒剩余时间获取
        case SUP_GET_CMD:sup_get_cb();break;//电源连接方式
        case RSE_SET_CMD:rse_set_cb(value);break;//自动提醒时间设置
        case RSE_GET_CMD:rse_get_cb();break;//自动提醒时间获取
        case RSA_SET_CMD:rsa_set_cb(value);break;//自动冲洗时间设置
        case RSA_GET_CMD:rsa_get_cb();break;//自动冲洗时间获取
        case RSI_SET_CMD:rsi_set_cb(value);break;//时间范围设置
        case RSI_GET_CMD:rsi_get_cb();break;//时间范围获取
        case RSD_SET_CMD:rsd_set_cb(value);break;//冲洗时间设置
        case RSD_GET_CMD:rsd_get_cb();break;//冲洗时间获取
        case CNF_SET_CMD:cnf_set_cb(value);break;//TDS校准值设置
        case CNF_GET_CMD:cnf_get_cb();break;//TDS校准值获取
        case CNL_SET_CMD:cnl_set_cb(value);break;//TDS阈值设置
        case CNL_GET_CMD:cnl_get_cb();break;//TDS阈值获取
        case SSE_SET_CMD:sse_set_cb(value);break;//提醒开关设置
        case SSE_GET_CMD:sse_get_cb();break;//提醒开关获取
        case SSA_SET_CMD:ssa_set_cb(value);break;//自动开关设置
        case SSA_GET_CMD:ssa_get_cb();break;//自动开关获取
        case SSD_SET_CMD:ssd_set_cb(value);break;//压差开关设置
        case SSD_GET_CMD:ssd_get_cb();break;//压差开关获取
        case LNG_SET_CMD:lng_set_cb(value);break;//语言设置
        case LNG_GET_CMD:lng_get_cb();break;//语言获取
        case SRN_GET_CMD:srn_get_cb();break;//序列号获取
        case VER_GET_CMD:ver_get_cb();break;//版本号获取
        case COM_SET_CMD:com_set_cb(value);break;//Manual计数器清空
        case COM_GET_CMD:com_get_cb();break;//Manual计数器获取
        case COA_SET_CMD:coa_set_cb(value);break;//Auto计数器清空
        case COA_GET_CMD:coa_get_cb();break;//Auto计数器获取
        case COD_SET_CMD:cod_set_cb(value);break;//Delta计数器清空
        case COD_GET_CMD:cod_get_cb();break;//Delta计数器获取
        case COE_SET_CMD:coe_set_cb(value);break;//Error计数器清空
        case COE_GET_CMD:coe_get_cb();break;//Error计数器获取
        case WST_SET_CMD:wst_set_cb(value);break;//WIFI状态获取
    }
}
