
/** @file bsp_uart.h
 *  @version 1.0
 *  @date Jan 2019
 *
 *  @brief uart receive data from DBus/judge_system/manifold etc.
 *
 *
 */

#ifndef __BSP_UART_H__
#define __BSP_UART_H__

#include "usart.h"
#include <stdio.h>


/* usart relevant */

#define BT_USART                USART3 //for debug
//#define MX_USART                USART2

#define bt_usart                     huart3 //for dji remote controler reciever
//#define mx_usart                    huart2

#define BT_BUFFERLEN        1
#define MX_BUFFERLEN       10

void uart_receive_handler(UART_HandleTypeDef *huart);
void 	UART_RX_IDLE_IRQ(UART_HandleTypeDef *huart);
void USART_InitArgument(void);
void USER_DMA_INIT(UART_HandleTypeDef *huart, DMA_HandleTypeDef *hdma, uint8_t *bufferAdress, uint8_t bufferLen);
extern  uint8_t uart2_buff[50],uart3_buff[50];
#endif
