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
 * @brief  ��wifi���ڷ���ָ������
 * @param[in] {fr_type} ֡����
 * @param[in] {len} ���ݳ���
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
    //
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
}
/**
 * @brief  wifi�������ݴ������
 * @param  Null
 * @return Null
 * @note   ��MCU������whileѭ���е��øú���
 */
void wifi_uart_service(void)
{
    //#error "��ֱ����main������while(1){}�����wifi_uart_service(),���øú�����Ҫ���κ������ж�,��ɺ�ɾ������"
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

        //���ݽ������
        if(get_check_sum((unsigned char *)wifi_data_process_buf + offset,rx_value_len - 1) != wifi_data_process_buf[offset + rx_value_len - 1]) {
            //У�����
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
 * @brief  ����֡����
 * @param[in] {offset} ������ʼλ
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
        case RST_SET_CMD:rst_set_cb(value);break;//��λ
        case DEF_SET_CMD:def_set_cb(value);break;//��������
        case RAS_SET_CMD:ras_set_cb(value);break;//��ϴ��ʼ
        case RAS_GET_CMD:ras_get_cb();break;//��ϴ��ѯ
        case CND_GET_CMD:cnd_get_cb();break;//TDS��ȡ
        case NET_GET_CMD:net_get_cb();break;//DC��ѹ��ȡ
        case BAT_GET_CMD:bat_get_cb();break;//��ص�ѹ��ȡ
        case ALA_GET_CMD:ala_get_cb();break;//�Զ���ϴʣ��ʱ���ȡ
        case ALA_SET_CMD:ala_set_cb(value);break;//�Զ���ϴʣ��ʱ������
        case ALR_GET_CMD:alr_get_cb();break;//�Զ�����ʣ��ʱ���ȡ
        case ALR_SET_CMD:alr_set_cb(value);break;//�Զ�����ʣ��ʱ���ȡ
        case SUP_GET_CMD:sup_get_cb();break;//��Դ���ӷ�ʽ
        case RSE_SET_CMD:rse_set_cb(value);break;//�Զ�����ʱ������
        case RSE_GET_CMD:rse_get_cb();break;//�Զ�����ʱ���ȡ
        case RSA_SET_CMD:rsa_set_cb(value);break;//�Զ���ϴʱ������
        case RSA_GET_CMD:rsa_get_cb();break;//�Զ���ϴʱ���ȡ
        case RSI_SET_CMD:rsi_set_cb(value);break;//ʱ�䷶Χ����
        case RSI_GET_CMD:rsi_get_cb();break;//ʱ�䷶Χ��ȡ
        case RSD_SET_CMD:rsd_set_cb(value);break;//��ϴʱ������
        case RSD_GET_CMD:rsd_get_cb();break;//��ϴʱ���ȡ
        case CNF_SET_CMD:cnf_set_cb(value);break;//TDSУ׼ֵ����
        case CNF_GET_CMD:cnf_get_cb();break;//TDSУ׼ֵ��ȡ
        case CNL_SET_CMD:cnl_set_cb(value);break;//TDS��ֵ����
        case CNL_GET_CMD:cnl_get_cb();break;//TDS��ֵ��ȡ
        case SSE_SET_CMD:sse_set_cb(value);break;//���ѿ�������
        case SSE_GET_CMD:sse_get_cb();break;//���ѿ��ػ�ȡ
        case SSA_SET_CMD:ssa_set_cb(value);break;//�Զ���������
        case SSA_GET_CMD:ssa_get_cb();break;//�Զ����ػ�ȡ
        case SSD_SET_CMD:ssd_set_cb(value);break;//ѹ�������
        case SSD_GET_CMD:ssd_get_cb();break;//ѹ��ػ�ȡ
        case LNG_SET_CMD:lng_set_cb(value);break;//��������
        case LNG_GET_CMD:lng_get_cb();break;//���Ի�ȡ
        case SRN_GET_CMD:srn_get_cb();break;//���кŻ�ȡ
        case VER_GET_CMD:ver_get_cb();break;//�汾�Ż�ȡ
        case COM_SET_CMD:com_set_cb(value);break;//Manual���������
        case COM_GET_CMD:com_get_cb();break;//Manual��������ȡ
        case COA_SET_CMD:coa_set_cb(value);break;//Auto���������
        case COA_GET_CMD:coa_get_cb();break;//Auto��������ȡ
        case COD_SET_CMD:cod_set_cb(value);break;//Delta���������
        case COD_GET_CMD:cod_get_cb();break;//Delta��������ȡ
        case COE_SET_CMD:coe_set_cb(value);break;//Error���������
        case COE_GET_CMD:coe_get_cb();break;//Error��������ȡ
        case WST_SET_CMD:wst_set_cb(value);break;//WIFI״̬��ȡ
    }
}
