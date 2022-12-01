#include "rtthread.h"
#include "rtdevice.h"
#include "stm32l4xx.h"
#include "pin_config.h"
#include "tds_uart.h"
#include "tds_service.h"
#include "adcwork.h"
#include "string.h"

#define DBG_TAG "TDS_UART"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

uint32_t TDS_Value = 0;
uint8_t TDS_Warn = 0;

#define TDS_UART_NAME                   "uart2"

static struct rt_semaphore rx_sem;
static rt_device_t serial;
static struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;  /* 初始化配置参数 */

rt_thread_t TDS_t = RT_NULL;

char TdsRead[]={0x55,0x07,0x05,0x01,0x00,0x00,0x00,0x62};

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
void tds_data_parsing(void *parameter)
{
    char ch;
    while (1)
    {
        ch = uart_sample_get_char();
        tds_uart_receive_input(ch);
    }
}
void TDS_Init(void)
{
    TDS_Uart_Init();
    TDS_Service_Init();
}
void TDS_Uart_Init(void)
{
    char uart_name[RT_NAME_MAX];

    rt_strncpy(uart_name, TDS_UART_NAME, RT_NAME_MAX);

    /* 查找系统中的串口设备 */
    serial = rt_device_find(uart_name);
    if (!serial)
    {
        LOG_D("find %s failed!\n", uart_name);
    }

    /* 初始化信号量 */
    rt_sem_init(&rx_sem, "rx_sem", 0, RT_IPC_FLAG_FIFO);
    /* 串口参数配置 */
    config.baud_rate = BAUD_RATE_9600;
    config.data_bits = DATA_BITS_8;
    config.stop_bits = STOP_BITS_1;
    config.parity    = PARITY_NONE;
    config.bufsz     = 128;

    rt_device_control(serial, RT_DEVICE_CTRL_CONFIG, &config);
    /* 以中断接收及轮询发送模式打开串口设备 */
    rt_device_open(serial, RT_DEVICE_FLAG_INT_RX);
    /* 设置接收回调函数 */
    rt_device_set_rx_indicate(serial, uart_rx_ind);

    /* 创建 serial 线程 */
    TDS_t = rt_thread_create("tds_recv_t", tds_data_parsing, RT_NULL, 512, 10, 10);

    /* 创建成功则启动线程 */
    if (TDS_t != RT_NULL)
    {
        rt_thread_startup(TDS_t);
    }
    LOG_D("TDS Init Success\r\n");
}
void TDS_GpioInit(void)
{
    __HAL_RCC_USART2_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStruct1 = {0};
    GPIO_InitStruct1.Alternate = 7;
    GPIO_InitStruct1.Pin = GPIO_PIN_2;
    GPIO_InitStruct1.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct1.Pull = GPIO_PULLUP;
    GPIO_InitStruct1.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct1);
    GPIO_InitStruct1.Pin = GPIO_PIN_3;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct1);
}
void TDS_GpioDeInit(void)
{
    __HAL_RCC_USART2_CLK_DISABLE();
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2|GPIO_PIN_3);
    __HAL_RCC_USART3_CLK_DISABLE();
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_3|GPIO_PIN_4);
}
uint32_t TDS_Get(void)
{
    rt_device_write(serial, 0, TdsRead, 8);
    rt_thread_mdelay(50);
    LOG_I("TDS_Get %d\r\n",TDS_Value);
    return TDS_Value;
}
uint32_t TDS_Work(void)
{
    uint32_t Data=0;
    extern uint32_t Setting_Hardness;
    if(Setting_Hardness!=0)
    {
        LOG_D("Read TDS Now\r\n");
        Data = TDS_Value;
        TDS_Value = 0;
        rt_device_write(serial, 0, TdsRead, 8);
        return Data;
    }
    else {
        LOG_D("TDS is OFF\r\n");
        return 0;
    }
}
MSH_CMD_EXPORT(TDS_Work,TDS_Work);
uint32_t GetTDS(void)
{
    return TDS_Value;
}
void TDS_WarnSet(uint8_t value)
{
    TDS_Warn = value;
}
uint8_t TDS_WarnGet(void)
{
    return TDS_Warn;
}
