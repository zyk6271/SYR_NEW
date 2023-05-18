#include "rtthread.h"
#include "rtdevice.h"
#include "pin_config.h"
#include "uart.h"
#include "uart_core.h"

#define DBG_TAG "uart"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

static struct rt_semaphore rx_sem;
static rt_device_t serial;
static struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;

#define WiFi_UART_NAME                   "uart1"

rt_thread_t wifi_recv_t = RT_NULL;
rt_thread_t wifi_serv_t = RT_NULL;

static rt_err_t uart_rx_ind(rt_device_t dev, rt_size_t size)
{
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
void data_parsing(void)
{
    char ch;
    LOG_D("WiFi-Uart Init Success\r\n");

    while (1)
    {
        ch = uart_sample_get_char();
        wifi_uart_receive_input(ch);
    }
}
void WiFi_Byte_Send(uint8_t data)
{
    rt_device_write(serial,0,&data,1);
}
void wifi_uart_init(void)
{
    char uart_name[RT_NAME_MAX];

    rt_strncpy(uart_name, WiFi_UART_NAME, RT_NAME_MAX);

    serial = rt_device_find(uart_name);
    if (!serial)
    {
        LOG_D("find %s failed!\n", uart_name);
    }

    rt_sem_init(&rx_sem, "rx_sem", 0, RT_IPC_FLAG_FIFO);
    config.baud_rate = BAUD_RATE_115200;
    config.data_bits = DATA_BITS_8;
    config.stop_bits = STOP_BITS_1;
    config.parity    = PARITY_NONE;
    config.bufsz     = 1024;
    rt_device_control(serial, RT_DEVICE_CTRL_CONFIG, &config);
    rt_device_open(serial, RT_DEVICE_FLAG_INT_RX);
    rt_device_set_rx_indicate(serial, uart_rx_ind);

    wifi_recv_t = rt_thread_create("wifi_recv_t", (void (*)(void *parameter))data_parsing, RT_NULL, 512, 7, 10);
    if (wifi_recv_t != RT_NULL)
    {
        rt_thread_startup(wifi_recv_t);
    }
}
void wifi_service_callback(void *parameter)
{
    while(1)
    {
        wifi_uart_service();
        rt_thread_mdelay(5);
    }
}
void wifi_service_init(void)
{
    wifi_serv_t = rt_thread_create("wifi-service", wifi_service_callback, RT_NULL, 2048, 8, 10);
    if(wifi_serv_t != RT_NULL)
    {
        rt_thread_startup(wifi_serv_t);
    }
}
void WiFi_Init(void)
{
    WiFi_Pin_Init();
    wifi_protocol_init();
    wifi_uart_init();
    wifi_service_init();
    wifi_connect_init();
    wifi_ota_init();
}
