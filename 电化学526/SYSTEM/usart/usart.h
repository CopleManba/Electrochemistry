#ifndef __USART_H
#define __USART_H

#include "stdio.h"	
#include "sys.h" 

#define USART_REC_LEN  			200  	//�����������ֽ��� 200
#define EN_USART1_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����

#define USART1_GPIO_PIN_TX GPIO_Pin_9  //PA9
#define USART1_GPIO_PIN_RX GPIO_Pin_10

#define RecLen 6
	  	
extern u8  usartRecBuffer[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART_RX_STA;         		//����״̬���	
extern int DATA_BUF[];
//����봮���жϽ��գ��벻Ҫע�����º궨��
void uart_init(u32 bound);
void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch);
void USART_SendString(USART_TypeDef *USARTx, char *str);
void Usart_SendNByte(USART_TypeDef * pUSARTx,int msglen,uint8_t *sendBuffer);

#endif


