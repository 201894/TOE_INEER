
/** @file bsp_uart.c
 *  @version 4.0
 *  @date  June 2019
 *
 *  @brief receive uart message and send it to usart2 ,deal with the message in STMGood
 */
#include "bsp_uart.h"
#include "STMGood.h"
//#include "detect_task.h"
#include "usart.h"
#include "stdio.h"

/* dma double buffer */
uint8_t uart2_buff[50],uart3_buff[50];
/**
  * @brief   initialize uart device 
  */
void USART_InitArgument(void)
{
//	  USER_DMA_INIT(&mx_usart,&hdma_usart2_rx,uart2_buff,MX_BUFFERLEN);
		USER_DMA_INIT(&bt_usart,&hdma_usart3_rx,uart3_buff,BT_BUFFERLEN);		
}

void USER_DMA_INIT(UART_HandleTypeDef *huart, DMA_HandleTypeDef *hdma, uint8_t *bufferAdress, uint8_t bufferLen)
{
	HAL_DMA_Start_IT(hdma,(uint32_t)huart->Instance->DR,(uint32_t)bufferAdress,bufferLen);
	huart->Instance->CR3 |= USART_CR3_DMAR;
	__HAL_UART_ENABLE_IT(huart,UART_IT_IDLE);
	HAL_UART_Receive_DMA(huart,bufferAdress,bufferLen);
	__HAL_UART_ENABLE_IT(huart,UART_IT_ERR);
}

/**
 * @brief Error Callback function
 * @param None
 * @return None
 * @attention None
 */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
	if(huart->ErrorCode == HAL_UART_ERROR_ORE)
	{
		__HAL_UART_CLEAR_OREFLAG(huart); //清除错误标志位，清空SR、DR寄存器
	}
}
int fputc(int ch, FILE *f)
{ 	
	while((BT_USART->SR&0X40)==0); 
	BT_USART->DR = (uint8_t) ch;      
	return ch;
}
/**
 * @brief uart Interrupt function
 * @param None
 * @return None
 * @attention Replace huart1 interrupt in stm32f4xx_it.c
 */
void UART_RX_IDLE_IRQ(UART_HandleTypeDef *huart){
	if(huart->Instance == BT_USART)
	{
			if(__HAL_UART_GET_FLAG(&bt_usart,UART_FLAG_IDLE) != RESET){
				__HAL_UART_CLEAR_IDLEFLAG(&bt_usart);						
				HAL_UART_Receive_DMA(&bt_usart,uart3_buff,BT_BUFFERLEN);
	    }
	}
//	if(huart->Instance == MX_USART)
//	{
//			if(__HAL_UART_GET_FLAG(&mx_usart,UART_FLAG_IDLE) != RESET){
//				__HAL_UART_CLEAR_IDLEFLAG(&mx_usart);						
//				HAL_UART_Receive_DMA(&mx_usart,uart2_buff,MX_BUFFERLEN);
//	    }
//	}
}
	