/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-01-15     Rick       the first version
 */
#include "rtthread.h"
#include "tds_service.h"

#define DBG_TAG "TDS_SERV"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

rt_thread_t tds_sevice_t = RT_NULL;

static volatile unsigned char tds_data_process_buf[PROTOCOL_HEAD + 64];      //串口数据处理缓存
static volatile unsigned char tds_uart_rx_buf[PROTOCOL_HEAD + 64];          //串口接收缓存

static volatile unsigned char *queue_in = NULL;
static volatile unsigned char *queue_out = NULL;

extern uint32_t TDS_Value;
extern uint8_t TDS_CND_Value;

static unsigned char get_queue_total_data(void)
{
  if(queue_in != queue_out)
    return 1;
  else
    return 0;
}
void *my_memcpy(void *dest, const void *src, unsigned short count)
{
    unsigned char *pdest = (unsigned char *)dest;
    const unsigned char *psrc  = (const unsigned char *)src;
    unsigned short i;

    if(dest == NULL || src == NULL) {
        return NULL;
    }

    if((pdest <= psrc) || (pdest > psrc + count)) {
        for(i = 0; i < count; i ++) {
            pdest[i] = psrc[i];
        }
    }else {
        for(i = count; i > 0; i --) {
            pdest[i - 1] = psrc[i - 1];
        }
    }

    return dest;
}

/**
 * @brief  字符串拷贝
 * @param[in] {dest} 目标地址
 * @param[in] {src} 源地址
 * @return 数据处理完后的源地址
 */
char *my_strcpy(char *dest, const char *src)
{
    char *p = dest;

    if(dest == NULL || src == NULL) {
        return NULL;
    }

    while(*src!='\0') {
        *dest++ = *src++;
    }
    *dest = '\0';
    return p;
}

/**
 * @brief  读取队列1字节数据
 * @param  Null
 * @return Read the data
 */
static unsigned char Queue_Read_Byte(void)
{
  unsigned char value = 0;

  if(queue_out != queue_in)
  {
    //有数据
    if(queue_out >= (unsigned char *)(tds_uart_rx_buf + sizeof(tds_uart_rx_buf)))
    {
      //数据已经到末尾
      queue_out = (unsigned char *)(tds_uart_rx_buf);
    }

    value = *queue_out ++;
  }

  return value;
}
unsigned char get_check_sum(unsigned char *pack, unsigned short pack_len)
{
    unsigned short i;
    unsigned char check_sum = 0;

    for(i = 0; i < pack_len; i ++) {
        check_sum += *pack ++;
    }

    return check_sum;
}
void tds_uart_receive_input(unsigned char value)
{
    if(1 == queue_out - queue_in) {
        LOG_W("Wifi Queue is Full\r\n");
        //数据队列满
    }else if((queue_in > queue_out) && ((queue_in - queue_out) >= sizeof(tds_data_process_buf))) {
        LOG_W("Wifi Queue is Full\r\n");
        //数据队列满
    }else {
        //队列不满
        if(queue_in >= (unsigned char *)(tds_uart_rx_buf + sizeof(tds_uart_rx_buf))) {
            queue_in = (unsigned char *)(tds_uart_rx_buf);
        }
        *queue_in ++ = value;
    }
}
void tds_handle(unsigned short offset)
{
    unsigned char cmd_type = tds_data_process_buf[offset + FRAME_TYPE];
    switch(cmd_type) {
        case 0x85:                                //产品信息
            TDS_Value = ((tds_data_process_buf[offset + DATA_START]<<8) + tds_data_process_buf[offset + DATA_START + 1]);
            TDS_Value = (int)(0.01 * TDS_Value * TDS_CND_Value);
            LOG_D("TDS_High=%x,TDS_Low=%x,TDS_Value is %d\r\n",tds_data_process_buf[offset + DATA_START],tds_data_process_buf[offset + DATA_START + 1],TDS_Value);
        break;
    }
}
void tds_uart_service(void)
{
    static unsigned short rx_in = 0;
    unsigned short offset = 0;
    unsigned short rx_value_len = 0;             //数据帧长度

    while((rx_in < sizeof(tds_data_process_buf)) && get_queue_total_data() > 0) {
        tds_data_process_buf[rx_in ++] = Queue_Read_Byte();
    }

    if(rx_in < PROTOCOL_HEAD)
        return;

    while((rx_in - offset) >= PROTOCOL_HEAD) {
        if(tds_data_process_buf[offset + HEAD_FIRST] != FRAME_FIRST) {
            offset ++;
            continue;
        }

        rx_value_len = tds_data_process_buf[offset + LENGTH];
        if(rx_value_len > sizeof(tds_data_process_buf)) {
            offset ++;
            continue;
        }

        if((rx_in - offset) < rx_value_len) {
            break;
        }

        //数据接收完成
        if(get_check_sum((unsigned char *)tds_data_process_buf + offset,rx_value_len - 1) != tds_data_process_buf[offset + rx_value_len]) {
            //校验出错
            LOG_W("crc error (crc:0x%X  but data:0x%X)\r\n",get_check_sum((unsigned char *)tds_data_process_buf + offset,rx_value_len - 1),tds_data_process_buf[offset + rx_value_len]);
            offset += 9;
            continue;
        }
        LOG_I("crc correct (crc:0x%X data:0x%X)\r\n",get_check_sum((unsigned char *)tds_data_process_buf + offset,rx_value_len - 1),tds_data_process_buf[offset + rx_value_len]);
        tds_handle(offset);
        offset += rx_value_len;
    }

    rx_in -= offset;
    if(rx_in > 0) {
        my_memcpy((char *)tds_data_process_buf,(const char *)tds_data_process_buf + offset,rx_in);
    }
}
void tds_service_callback(void *parameter)
{
    queue_in = (unsigned char *)tds_uart_rx_buf;
    queue_out = (unsigned char *)tds_uart_rx_buf;
    while(1)
    {
        tds_uart_service();
        rt_thread_mdelay(10);
    }
}
void TDS_Service_Init(void)
{
    tds_sevice_t = rt_thread_create("tds-service", tds_service_callback, RT_NULL, 1024, 11, 10);
    if(tds_sevice_t!=RT_NULL)
    {
        rt_thread_startup(tds_sevice_t);
    }
}
