#include "usart.h"	  
////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos ʹ��	  
#endif
 
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

/*ʹ��microLib�ķ���*/
 /* 
int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (uint8_t) ch);

	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}	
   
    return ch;
}
int GetKey (void)  { 

    while (!(USART1->SR & USART_FLAG_RXNE));

    return ((int)(USART1->DR & 0x1FF));
}
*/
 
#if EN_USART1_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
//u8 usartRecBuffer[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
////����״̬
////bit15��	������ɱ�־
////bit14��	���յ�0x0d
////bit13~0��	���յ�����Ч�ֽ���Ŀ
//u16 USART_RX_STA=0;       //����״̬���	  
u8 CtrCode;   //������
u16 CtrNumber; //������
u8 RecData[RecLen];    //��������
u16 ORENum=0;
uint8_t connect_flag=0;
uint8_t wifi_connect_flag=0;
uint8_t wifi_disconnect_flag=0;
extern float   ads1220_ch1_voltage_value;                          //ͨ��1�洢�ĵ�ѹֵ
extern float   ads1220_ch2_voltage_value;   
char sdata[40];
extern u16 dataptr,dataptr2;
extern u16 CtrNumberForUart;

float DutyCycle,Frequency;
  
void uart_init( u32 bound){
 GPIO_InitTypeDef GPIO_InitStruct;
	USART_InitTypeDef USART1_InitStruct;
	NVIC_InitTypeDef  NVIC_InitStruct;
	//1????GPIOA????
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE); 
// <=> RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_USART1,ENABLE);
	
	//2?GPIOA????
	GPIO_InitStruct.GPIO_Pin = USART1_GPIO_PIN_TX;	 //PA9
  	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;      
  	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz; 
  	GPIO_Init(GPIOA, &GPIO_InitStruct);		  

	GPIO_InitStruct.GPIO_Pin = USART1_GPIO_PIN_RX;	
  	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;       
  	//GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz; 
  	GPIO_Init(GPIOA, &GPIO_InitStruct);	
	
	USART1_InitStruct.USART_BaudRate = bound;
	USART1_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART1_InitStruct.USART_Mode = USART_Mode_Rx|USART_Mode_Tx; //??????mode
	USART1_InitStruct.USART_Parity = USART_Parity_No;
	USART1_InitStruct.USART_StopBits = USART_StopBits_1;
	USART1_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1,&USART1_InitStruct);


	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStruct);
	
	USART_Cmd(USART1,ENABLE);
	
}

/**
* @brief ���ͺ���
*/
void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch)
{
	/* ����һ���ֽ����ݵ�USART */
	USART_SendData(pUSARTx,ch);
		
	/* �ȴ��������ݼĴ���Ϊ�� */
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}

void USART_SendByte(USART_TypeDef* USARTx, uint16_t Data)
{
  /* Check the parameters */
  assert_param(IS_USART_ALL_PERIPH(USARTx));
  assert_param(IS_USART_DATA(Data)); 
    
  /* Transmit Data */
  USARTx->DR = (Data & (uint16_t)0x01FF);
	while(USART_GetFlagStatus(USARTx,USART_FLAG_TXE) == RESET);
}

/**
*	@brief ����N�ֽ�
*/
void Usart_SendNByte(USART_TypeDef * pUSARTx,int msglen,uint8_t *sendBuffer)
{
	int i=0;
	for(i=0;i<msglen;i++)
	{
		Usart_SendByte(USART1,sendBuffer[i]);
	}
}


void USART1_IRQHandler()
{
	u8 i; u16 j;
//	u16 temp;
	//u8 Res;
//#if SYSTEM_SUPPORT_OS 		//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
//	OSIntEnter();    
//#endif
	
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)					//����1�����ж�
	{	
		//����ȫ�����ݣ�6λ��������/����λ������
		for (i=0;i<RecLen;i++)
		{
			j=0;
			while(USART_GetFlagStatus(USART1,USART_IT_RXNE)==RESET)  	//δ���յ����������ȴ�ѭ��
			{
				j++;
				if (j>1000) 
				{
					break;               	//  �ȴ���ʱ���˳��ȴ�ѭ��
				}	
			}			
			if (j>1000) 
			{
				CtrCode='E';	//��ʾError
				CtrNumber=i;
				break;//��������forѭ��
			}
			else
			{
				RecData[i]=USART_ReceiveData(USART1);
			}
		}
		
		if (i<RecLen)									//��6λ���ݴ���
		{
			//�յ� c ����mqtt
			if(RecData[0] == 'c')
			{
				wifi_connect_flag=1;
				wifi_disconnect_flag=0;
				CtrCode = ' ';
				dataptr2=dataptr=0;
//				RecData[0] = '\0';
			}
			else if(RecData[0] == 's')
			{
				wifi_disconnect_flag=1;
				wifi_connect_flag=0;
				connect_flag=0;
				CtrCode = ' ';
				dataptr2=dataptr=0;
//				RecData[0] = '\0';
			}
			return ;
		}
		
		if(connect_flag==1)						//*��������
		{
			//��������
			CtrCode='P';
			if(dataptr)
			{
				CtrNumber=(u16)((DATA_BUF[2])*1000+(DATA_BUF[3])*100+(DATA_BUF[4])*10+(DATA_BUF[5]));	
			}
		}
		else													//��δ����
		{
			if (i==RecLen)
			{
				
				if ((((RecData[0]>64)&&(RecData[0]<91))||((RecData[0]>96)&&(RecData[0]<123)))&& //�жϱ����Ǵ��Сд��ĸ�� (RecData[1]<58)&&(RecData[1]>47)&&
						(RecData[2]<58)&&(RecData[2]>47)&&
						(RecData[3]<58)&&(RecData[3]>47)&&(RecData[4]<58)&&(RecData[4]>47))    //�жϱ���������!
				{
					CtrCode=RecData[0];
					CtrNumber=(u16)((RecData[2]-0x30)*1000+(RecData[3]-0x30)*100+(RecData[4]-0x30)*10+(RecData[5]-0x30));	
					CtrNumberForUart = CtrNumber;
				}
				else 
				{
					CtrCode='A';	//��ʾError
					CtrNumber=1000+i;
//					printf("AAAA");
				}
			}
		}
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);   //����жϱ�־λ
		

	}
	//���ش���,�ǳ���Ҫ��20201103�������������ֽ�������ʱ�����û�и��жϣ������ж��г�������
	else if (USART_GetFlagStatus(USART1, USART_FLAG_ORE) != RESET)  
   {  
		j=USART1->SR;      //�ȶ�USART_SR���ٶ�USART_DR������жϱ�־λ���ȶ����˳�����Ӱ�죡20201113��
		j=USART1->DR; 
		ORENum++;         //�������
		CtrCode='E';	//��ʾError
		CtrNumber=2000+ORENum;
		if (ORENum==999)   ORENum=0;	
	}
	 
	#if SYSTEM_SUPPORT_OS 	//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntExit();  											 
#endif
}
#endif	

