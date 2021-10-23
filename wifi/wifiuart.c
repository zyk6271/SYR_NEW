#include "rtthread.h"
#include "rtdevice.h"
#include "stm32l4xx.h"
#include "pin_config.h"
#include "wifiuart.h"
#include "adcwork.h"
#include "string.h"
#include "wifiwork.h"

#define DBG_TAG "WIFI_UART"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

#define WIFI_UART_NAME                   "uart1"
#define DATA_CMD_BEGIN                   0x55       /* 结束位设置为 \r，即回车符 */
#define DATA_CMD_END                     0xAA       /* 结束位设置为 \r，即回车符 */
#define ONE_DATA_MAXLEN                  4         /* 不定长数据的最大长度 */

/* 用于接收消息的信号量 */
static struct rt_semaphore rx_sem;
static rt_device_t serial;
rt_thread_t wifi_uart_t = RT_NULL;
static struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;  /* 初始化配置参数 */

/* 接收数据回调函数 */
static rt_err_t uart_rx_ind(rt_device_t dev, rt_size_t size)
{
    /* 串口接收到数据后产生中断，调用此回调函数，然后发送接收信号量 */
    if (size > 0)
    {
        rt_sem_release(&rx_sem);
    }
    return RT_EOK;
}
static char uart_sample_get_char(void)
{
    char ch;

    while (rt_device_read(serial, 0, &ch, 1) == 0)
    {
        rt_sem_control(&rx_sem, RT_IPC_CMD_RESET, RT_NULL);
        rt_sem_take(&rx_sem, RT_WAITING_FOREVER);
    }
    return ch;
}
uint8_t wifi_data_parsing_id = 0;
void wifi_data_parsing(void *parameter)
{
    static char ch;
    static char data[ONE_DATA_MAXLEN];
    while (1)
    {
        ch = uart_sample_get_char();
        data[wifi_data_parsing_id++] = ch;
        if(wifi_data_parsing_id == ONE_DATA_MAXLEN)
        {
            if(data[0] == DATA_CMD_BEGIN && data[3] == DATA_CMD_END)
            {
                wifi_uart_parse(data[1],data[2]);
            }
            else
            {
                LOG_D("wifi_data_parsing Error\r\n");
            }
            wifi_data_parsing_id = 0;
        }
    }
}
void WIFI_Uart_Init(void)
{
    char uart_name[RT_NAME_MAX];

    rt_strncpy(uart_name, WIFI_UART_NAME, RT_NAME_MAX);

    /* 查找系统中的串口设备 */
    serial = rt_device_find(uart_name);
    if (!serial)
    {
        LOG_D("find %s failed!\n", uart_name);
    }

    /* 初始化信号量 */
    rt_sem_init(&rx_sem, "rx_sem", 0, RT_IPC_FLAG_FIFO);
    /* 串口参数配置 */
    config.baud_rate = BAUD_RATE_115200;        //修改波特率为 9600
    config.data_bits = DATA_BITS_8;           //数据位 8
    config.stop_bits = STOP_BITS_1;           //停止位 1
    config.bufsz     = 256;                   //修改缓冲区 buff size 为 128
    config.parity    = PARITY_NONE;           //无奇偶校验位
    rt_device_control(serial, RT_DEVICE_CTRL_CONFIG, &config);
    /* 以中断接收及轮询发送模式打开串口设备 */
    rt_device_open(serial, RT_DEVICE_FLAG_INT_RX);
    /* 设置接收回调函数 */
    rt_device_set_rx_indicate(serial, uart_rx_ind);

    /* 创建 serial 线程 */
    wifi_uart_t = rt_thread_create("wifi_uart", wifi_data_parsing, RT_NULL, 1024, 25, 10);

    /* 创建成功则启动线程 */
    if (wifi_uart_t != RT_NULL)
    {
        rt_thread_startup(wifi_uart_t);
    }
    LOG_D("wifi_uart_t Init Success\r\n");
}
MSH_CMD_EXPORT(WIFI_Uart_Init,WIFI_Uart_Init);
void wifi_uart_send(uint8_t command,uint32_t value)
{
    uint8_t sendbuf[5];
    sendbuf[0]=DATA_CMD_BEGIN;
    sendbuf[1]=command;
    sendbuf[2]=(value&0xFF00)>>8;
    sendbuf[3]=value&0x00FF;
    sendbuf[4]=DATA_CMD_END;
    rt_device_write(serial, 0, sendbuf, 5);
}
