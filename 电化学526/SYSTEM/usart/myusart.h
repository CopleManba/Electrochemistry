#ifndef __MYUSART_H__
#define __MYUSART_H__

#include "sys.h"

#define USART1_GPIO_PIN_TX GPIO_Pin_9  //PA9
#define USART1_GPIO_PIN_RX GPIO_Pin_10

enum MessageID{    
	SetDAMsagID = 3,  //����DA
	SetDAAckMsgID = 4, //����DAӦ��
	GetADValueID = 5, //��ȡADֵ
	GetADAckMsgID = 6, //��ȡADֵӦ��
};



void MY_USART1_Init(u32 bound);
void USART_SendByte(USART_TypeDef* USARTx, uint16_t Data);
void USART_SendString(USART_TypeDef *USARTx, char *str);
uint8_t USART_ReceiveByte(USART_TypeDef* USARTx);
#endif

