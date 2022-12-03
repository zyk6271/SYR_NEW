#include "rtthread.h"
#include "uart_core.h"
#include "uart_api.h"

#define DBG_TAG "uart_core"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

static volatile unsigned char wifi_data_process_buf[PROTOCOL_HEAD + WIFI_DATA_PROCESS_LMT];      //�������ݴ�����
static volatile unsigned char wifi_uart_rx_buf[PROTOCOL_HEAD + WIFI_UART_RECV_BUF_LMT];          //���ڽ��ջ���
static volatile unsigned char wifi_uart_tx_buf[PROTOCOL_HEAD + WIFIR_UART_SEND_BUF_LMT];         //���ڷ��ͻ���

static volatile unsigned char *queue_in = NULL;
static volatile unsigned char *queue_out = NULL;

unsigned short firm_size;                                  //������һ���Ĵ�С

volatile unsigned char stop_update_flag;                                                 //ֹͣһ�������ϱ�

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
 * @brief  ��int���Ͳ���ĸ��ֽ�
 * @param[in] {number} 4�ֽ�ԭ����
 * @param[out] {value} ������ɺ�4�ֽ�����
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
 * @brief  ��4�ֽںϲ�Ϊ1��32bit����
 * @param[in] {value} 4�ֽ�����
 * @return �ϲ���ɺ��32bit����
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
 * @brief  mcu��ȡbool���·�dpֵ
 * @param[in] {value} dp���ݻ�������ַ
 * @param[in] {len} dp���ݳ���
 * @return ��ǰdpֵ
 * @note   Null
 */
unsigned char mcu_get_dp_download_bool(const unsigned char value[],unsigned short len)
{
    return(value[0]);
}
/**
 * @brief  mcu��ȡvalue���·�dpֵ
 * @param[in] {value} dp���ݻ�������ַ
 * @param[in] {len} dp���ݳ���
 * @return ��ǰdpֵ
 * @note   Null
 */
unsigned long mcu_get_dp_download_value(const unsigned char value[],unsigned short len)
{
    return(byte_to_int(value));
}
/**
 * @brief  дwifi_uart�ֽ�
 * @param[in] {dest} ��������ַƫ��
 * @param[in] {byte} д���ֽ�ֵ
 * @return д����ɺ���ܳ���
 */
unsigned short set_wifi_uart_byte(unsigned short dest, unsigned char byte)
{
    unsigned char *obj = (unsigned char *)wifi_uart_tx_buf + DATA_START + dest;

    *obj = byte;
    dest += 1;

    return dest;
}
/**
 * @brief  дwifi_uart_buffer
 * @param[in] {dest} ��������ַƫ��
 * @param[in] {src} Դ��ַ����Ҫ���͵����ݣ�
 * @param[in] {len} ��Ҫ���͵����ݳ���
 * @return д����ɺ���ܳ���
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
 * @brief  �жϴ��ڽ��ջ������Ƿ�������
 * @param  Null
 * @return �Ƿ�������  0:��/1:��
 */
static unsigned char get_queue_total_data(void)
{
  if(queue_in != queue_out)
    return 1;
  else
    return 0;
}
/**
 * @brief  ���ڷ���һ������
 * @param[in] {in} ���ͻ���ָ��
 * @param[in] {len} ���ݷ��ͳ���
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
 * @brief  value��dp�����ϴ�
 * @param[in] {dpid} dpid��
 * @param[in] {value} ��ǰdpֵ
 * @param[in] {sub_id_buf} ������豸id���׵�ַ
 * @param[in] {sub_id_len} ���豸id����
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
 * @brief  string��dp�����ϴ�
 * @param[in] {dpid} dpid��
 * @param[in] {value} ��ǰdpֵָ��
 * @param[in] {len} ���ݳ���
 * @param[in] {sub_id_buf} ������豸id���׵�ַ
 * @param[in] {sub_id_len} ���豸id����
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
 * @brief  ��wifi���ڷ���ָ������
 * @param[in] {fr_type} ֡����
 * @param[in] {len} ���ݳ���
 * @return Null
 */
void wifi_uart_write_command_value(unsigned char command, unsigned int data)
{
    mcu_dp_value_update(command,data);
}
/**
 * @brief  ��wifi���ڷ���һ֡����
 * @param[in] {fr_type} ֡����
 * @param[in] {fr_ver} ֡�汾
 * @param[in] {len} ���ݳ���
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
 * @brief  ��ȡ����1�ֽ�����
 * @param  Null
 * @return Read the data
 */
unsigned char Queue_Read_Byte(void)
{
  unsigned char value = 0;

  if(queue_out != queue_in)
  {
    //������
    if(queue_out >= (unsigned char *)(wifi_uart_rx_buf + sizeof(wifi_uart_rx_buf)))
    {
      //�����Ѿ���ĩβ
      queue_out = (unsigned char *)(wifi_uart_rx_buf);
    }

    value = *queue_out ++;
  }

  return value;
}
/**
 * @brief  ���ڷ�������
 * @param[in] {value} ����Ҫ���͵�1�ֽ�����
 * @return Null
 */
void uart_transmit_output(unsigned char value)
{
    extern void WiFi_Byte_Send(uint8_t data);
    WiFi_Byte_Send(value);
}
/**
 * @brief  ���ڽ��������ݴ洦��
 * @param[in] {value} �����յ���1�ֽ�����
 * @return Null
 * @note   ��MCU���ڴ������е��øú���,�������յ���������Ϊ��������
 */
void wifi_uart_receive_buf_input(unsigned char *value,uint32_t length)
{
    if(1 == queue_out - queue_in) {
        //���ݶ�����
    }else if((queue_in > queue_out) && ((queue_in - queue_out) >= length)) {
        //���ݶ�����
    }else {
        //���в���
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
 * @brief  ���ڽ��������ݴ洦��
 * @param[in] {value} �����յ���1�ֽ�����
 * @return Null
 * @note   ��MCU���ڴ������е��øú���,�������յ���������Ϊ��������
 */
void wifi_uart_receive_input(unsigned char value)
{
    if(1 == queue_out - queue_in) {
        //���ݶ�����
    }else if((queue_in > queue_out) && ((queue_in - queue_out) >= sizeof(wifi_data_process_buf))) {
        //���ݶ�����
    }else {
        //���в���
        if(queue_in >= (unsigned char *)(wifi_uart_rx_buf + sizeof(wifi_uart_rx_buf))) {
            queue_in = (unsigned char *)(wifi_uart_rx_buf);
        }

        *queue_in ++ = value;
    }
}
/**
 * @brief  Э�鴮�ڳ�ʼ������
 * @param  Null
 * @return Null
 * @note   ��MCU��ʼ�������е��øú���
 */
void wifi_protocol_init(void)
{
    //#error " ����main���������wifi_protocol_init()���wifiЭ���ʼ��,��ɾ������"
    queue_in = (unsigned char *)wifi_uart_rx_buf;
    queue_out = (unsigned char *)wifi_uart_rx_buf;

    stop_update_flag = DISABLE;
}
/**
 * @brief  wifi�������ݴ������
 * @param  Null
 * @return Null
 * @note   ��MCU������whileѭ���е��øú���
 */
void wifi_uart_service(void)
{
    static unsigned short rx_in = 0;
    unsigned short offset = 0;
    unsigned short rx_value_len = 0;             //����֡����

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

        //���ݽ������
        if(get_check_sum((unsigned char *)wifi_data_process_buf + offset,rx_value_len - 1) != wifi_data_process_buf[offset + rx_value_len - 1]) {
            //У�����
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
 * @brief  ��ȡ����dp�����ܺ�
 * @param[in] Null
 * @return �·������ܺ�
 * @note   �ú����û������޸�
 */
unsigned char get_download_cmd_total(void)
{
    return(sizeof(download_cmd) / sizeof(download_cmd[0]));
}
/**
 * @brief  ��ȡ�ƶ�DPID�������е����
 * @param[in] {dpid} dpid
 * @return dp���
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
    //����������·������������ʵ���������
    unsigned char dp_id,index;
    unsigned char dp_type;
    unsigned char ret;
    unsigned short dp_len;

    dp_id = value[0];
    dp_type = value[1];
    dp_len = (value[2] <<8) + value[3];

    index = get_dowmload_dpid_index(dp_id);

    if(dp_type != download_cmd[index].dp_type) {
        //������ʾ
        return 0;
    }else {
        //�����dpid����������ͬ���豸��dp����
        //�����sub_id���������ĸ����豸�·�����
        //���ڸú����������豸��id����ʵ�����豸��dpid����"0000"���������Ҫ�����·����ݴ���������ر���
        ret = dp_download_handle(dp_id, value + 4, dp_len);
    }

    return ret;
}
/**
 * @brief  ���ر���MCU����������
 * @param[in] {value} �̼����ݻ�����
 * @param[in] {position} ��ǰ���ݰ����ڹ̼�λ��
 * @param[in] {length} ��ǰ�̼�������(�̼�������Ϊ0ʱ,��ʾ�̼����������)
 * @return �ɹ�/ʧ��
 * @note   MCU��Ҫ����ʵ�ָù���
 */
unsigned char mcu_firm_update_handle(const unsigned char value[],unsigned long position,unsigned short length)
{
    //#error "���������MCU�̼���������,��ɺ���ɾ������"
    unsigned char ret = 0;
    if(length == 0) {
        wifi_ota_end(value,position);
        //�̼����ݷ������
    }else {
        ret = wifi_ota_receive(value,position,length);
        //�̼����ݴ���
    }

    if(ret == RT_EOK){
        return SUCCESS;
    }
    else {
        return ERROR;
    }
}
/**
 * @brief  ����֡����
 * @param[in] {offset} ������ʼλ
 * @return Null
 */
void data_handle(unsigned short offset)
{
    unsigned char *firmware_addr = NULL;
    static unsigned long firm_length;                          //MCU�����ļ�����
    static unsigned char firm_update_flag;                     //MCU������־
    unsigned long dp_len_up;                                   //mcu������dplen

    unsigned short dp_len;

    unsigned char ret = 0;
    unsigned short i,total_len;
    unsigned char cmd_type = wifi_data_process_buf[offset + FRAME_TYPE];

    switch(cmd_type) {
        case PRODUCT_INFO_CMD:                                //��Ʒ��Ϣ
            product_info_update();
        case WIFI_STATE_CMD:                                  //wifi����״̬
            wifi_uart_write_frame(WIFI_STATE_CMD, MCU_TX_VER, 0);
            wifi_status_change(wifi_data_process_buf[offset + DATA_START]);
        case DATA_ISSUED_CMD:                                 //�����·�
            total_len = (wifi_data_process_buf[offset + LENGTH_HIGH] << 8) | wifi_data_process_buf[offset + LENGTH_LOW];

            for(i = 0 ;i < total_len; ) {
                dp_len = (wifi_data_process_buf[offset + DATA_START + i + 2] << 8) | wifi_data_process_buf[offset + DATA_START + i + 3];

                ret = data_point_handle((unsigned char *)wifi_data_process_buf + offset + DATA_START + i);

                i += (dp_len + 4);
            }
        break;
        case UPDATE_CONTROL_CMD:                                //������ʼ
            update_control_parse(wifi_data_process_buf[offset + DATA_START]<<8 | wifi_data_process_buf[offset + DATA_START + 1]);
        break;
        case TELEMETRY_CONTROL_CMD:                           //ң�����
            total_len = (wifi_data_process_buf[offset + LENGTH_HIGH] << 8) | wifi_data_process_buf[offset + LENGTH_LOW];
            telemetry_recv_handle(wifi_data_process_buf[offset + DATA_START]);
        break;
        case UPDATE_START_CMD:                                //������ʼ
            //������������С
            firm_size = 512;

            //�����ļ���С
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

        case UPDATE_TRANS_CMD:                                //��������
            if(firm_update_flag == UPDATE_START_CMD) {

            //ֹͣһ�������ϱ�
            stop_update_flag = ENABLE;
            //һ�������ܳ���
            total_len = (wifi_data_process_buf[offset + LENGTH_HIGH] << 8) | wifi_data_process_buf[offset + LENGTH_LOW];

            //��ƫ��
            dp_len_up = wifi_data_process_buf[offset + DATA_START];
            dp_len_up <<= 8;
            dp_len_up |= wifi_data_process_buf[offset + DATA_START + 1];
            dp_len_up <<= 8;
            dp_len_up |= wifi_data_process_buf[offset + DATA_START + 2];
            dp_len_up <<= 8;
            dp_len_up |= wifi_data_process_buf[offset + DATA_START + 3];

            //���ݰ�
            firmware_addr = (unsigned char *)wifi_data_process_buf;
            firmware_addr += (offset + DATA_START + 4);  //���ݰ�������ʼ��ַ
            //û���������ݻ��߰�ƫ�Ƶ��������ļ���С�����ݷ�����ɣ�
            if((total_len == 4) && (dp_len_up == firm_length)) {
                //���һ�����������ݷ�����ɣ�
                ret = mcu_firm_update_handle(firmware_addr,dp_len_up,0);

                firm_update_flag = 0;
            }else if((total_len - 4) <= firm_size) { //����pack���滹������
                //����δ������ɣ��������ݴ���
                ret = mcu_firm_update_handle(firmware_addr,dp_len_up,total_len - 4);
            }else {
                firm_update_flag = 0;
                ret = ERROR;
            }

            if(ret == SUCCESS) {
                wifi_uart_write_frame(UPDATE_TRANS_CMD, MCU_TX_VER, 0);
            }
            //�ָ�һ�������ϱ�
            stop_update_flag = DISABLE;
        }
        break;
        case UPDATE_PROGRSS_CMD:                                //������������
            wifi_progress_update(wifi_data_process_buf[offset + DATA_START]);
        break;
    }
}
