#ifndef __UART_CORE_H
#define __UART_CORE_H

#define Control_UART 1

#define WIFI_DATA_PROCESS_LMT           512             //单包1024byte
#define WIFI_UART_RECV_BUF_LMT          512              //串口数据接收缓存区大小,如MCU的RAM不够,可缩小
#define WIFIR_UART_SEND_BUF_LMT         512              //根据用户DP数据大小量定，用户可根据实际情况修改

//=============================================================================
//帧的字节命令
//=============================================================================
#define MCU_RX_VER              0x00                                            //接收的协议版本号
#define MCU_TX_VER              0x00                                            //发送的协议版本号
#define PROTOCOL_HEAD           0x07                                            //固定协议头长度
#define FRAME_FIRST             0x55
#define FRAME_SECOND            0xaa

//=============================================================================
//帧的字节顺序
//=============================================================================
#define         HEAD_FIRST                      0
#define         HEAD_SECOND                     1
#define         PROTOCOL_VERSION                2
#define         FRAME_TYPE                      3
#define         LENGTH_HIGH                     4
#define         LENGTH_LOW                      5
#define         DATA_START                      6
#define         DATA_HIGH                       6
#define         DATA_LOW                        7
//=============================================================================
//数据帧类型
//=============================================================================
#define         RST_SET_CMD                  0x01
#define         DEF_SET_CMD                  0x02
#define         RAS_SET_CMD                  0x03
#define         RAS_GET_CMD                  0x04
#define         RAS_PUT_CMD                  0x05
#define         CND_GET_CMD                  0x06
#define         NET_GET_CMD                  0x07
#define         BAT_GET_CMD                  0x08
#define         ALA_GET_CMD                  0x09
#define         ALA_SET_CMD                  0x0A
#define         ALR_GET_CMD                  0x0B
#define         ALR_SET_CMD                  0x0C
#define         RSE_SET_CMD                  0x0D
#define         RSE_GET_CMD                  0x0E
#define         RSE_PUT_CMD                  0x0F
#define         RSA_SET_CMD                  0x10
#define         RSA_GET_CMD                  0x11
#define         RSA_PUT_CMD                  0x12
#define         RSI_SET_CMD                  0x13
#define         RSI_GET_CMD                  0x14
#define         RSI_PUT_CMD                  0x15
#define         RSD_SET_CMD                  0x16
#define         RSD_GET_CMD                  0x17
#define         RSD_PUT_CMD                  0x18
#define         CNF_SET_CMD                  0x19
#define         CNF_GET_CMD                  0x1A
#define         CNF_PUT_CMD                  0x1B
#define         CNL_SET_CMD                  0x1C
#define         CNL_GET_CMD                  0x1D
#define         CNL_PUT_CMD                  0x1E
#define         SSE_SET_CMD                  0x1F
#define         SSE_GET_CMD                  0x20
#define         SSE_PUT_CMD                  0x21
#define         SSA_SET_CMD                  0x22
#define         SSA_GET_CMD                  0x23
#define         SSA_PUT_CMD                  0x24
#define         SSD_SET_CMD                  0x25
#define         SSD_GET_CMD                  0x26
#define         SSD_PUT_CMD                  0x27
#define         LNG_SET_CMD                  0x28
#define         LNG_GET_CMD                  0x29
#define         LNG_PUT_CMD                  0x2A
#define         SRN_GET_CMD                  0x2B
#define         SUP_GET_CMD                  0x2C
#define         SUP_PUT_CMD                  0x2D
#define         VER_GET_CMD                  0x2E
#define         COM_SET_CMD                  0x2F
#define         COM_GET_CMD                  0x30
#define         COM_PUT_CMD                  0x31
#define         COA_SET_CMD                  0x32
#define         COA_GET_CMD                  0x33
#define         COA_PUT_CMD                  0x34
#define         COD_SET_CMD                  0x35
#define         COD_GET_CMD                  0x36
#define         COD_PUT_CMD                  0x37
#define         COE_SET_CMD                  0x38
#define         COE_GET_CMD                  0x39
#define         COE_PUT_CMD                  0x3A
#define         CND_PUT_CMD                  0x3B
#define         WST_GET_CMD                  0x3C
#define         WST_SET_CMD                  0x3D
#define         WFA_SET_CMD                  0x3E

unsigned short set_wifi_uart_byte(unsigned short dest, unsigned char byte);
unsigned short set_wifi_uart_buffer(unsigned short dest, const unsigned char *src, unsigned short len);
unsigned char get_check_sum(unsigned char *pack, unsigned short pack_len);
void wifi_uart_write_data(unsigned char *in, unsigned short len);
void wifi_uart_write_frame(unsigned char fr_type, unsigned char fr_ver, unsigned short len);
unsigned char Queue_Read_Byte(void);
void uart_receive_input(unsigned char value);
void wifi_protocol_init(void);
void wifi_uart_service(void);

#endif
