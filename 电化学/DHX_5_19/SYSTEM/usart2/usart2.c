#include "delay.h"
#include "usart2.h"
#include "stdarg.h"	 	 
#include "sys.h"

#define Subscribe_IT

//���ڷ��ͻ����� 	
__align(8) u8 USART2_TX_BUF[USART2_MAX_SEND_LEN];
//���ڽ��ջ����� 	
u8 USART2_RX_BUF[USART2_MAX_RECV_LEN]; 				//���ջ���,���USART2_MAX_RECV_LEN���ֽ�.
u8 USART2_RX_BUF_1[USART2_MAX_RECV_LEN];
u8 USART2_RX_BUF_2[USART2_MAX_RECV_LEN];
u8 rec_by = 0;
extern unsigned char status, isok;

u16 USART2_RX_STA=0;   	 
void USART2_IRQHandler(void)
{
	if(USART_GetFlagStatus(USART2, USART_FLAG_IDLE) != RESET)//�������
	{
		USART_ClearFlag(USART2,USART_IT_IDLE);
		USART2->SR;
    USART2->DR; 
		DMA_Cmd(DMA1_Channel6,DISABLE);
		USART2_RX_STA = USART2_MAX_RECV_LEN - DMA_GetCurrDataCounter(DMA1_Channel6);
		
		buf_chn2();//˫�����л�
		MYDMA_Enable_esp(DMA1_Channel6);
		
		if(esp_check_cmd("OK"))
		{
			isok = 1; 
			USART2_RX_STA = 0;
//			MYDMA_Enable_esp(DMA1_Channel6);
			return;
		}
		
		
//		UsartPrintf(USART1,"\r\n@");
//		UsartPrintf(USART1,(char*)USART2_RX_BUF);
//		UsartPrintf(USART1,"@\r\n");
		#ifdef Subscribe_IT
		Subscribe_MSG_IT();
		#endif
		memset(USART2_RX_BUF,0,USART2_RX_STA);
//		MYDMA_Enable_esp(DMA1_Channel6);
	}
}   

void USART2_Init(u32 bound)
{  

	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	// GPIOAʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);

 	USART_DeInit(USART2);  //��λ����2
		 //USART2_TX   PA.2
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOA, &GPIO_InitStructure); //��ʼ��PA2
   
    //USART2_RX	  PA.3
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOA, &GPIO_InitStructure);  //��ʼ��PA3
	
	USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  
	USART_Init(USART2, &USART_InitStructure); //��ʼ������2
  
	
	USART_DMACmd(USART2,USART_DMAReq_Tx,ENABLE);  	//ʹ�ܴ���2��DMA����
	UART_DMA_Config_Tx(DMA1_Channel7,(u32)&USART2->DR,(u32)USART2_TX_BUF);//DMA1ͨ��7,����Ϊ����2,�洢��ΪUSART2_TX_BUF 
	
	USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE); //ʹ�ܴ���2��DMA����
	UART_DMA_Config_Rx(DMA1_Channel6,(u32)&USART2->DR,(u32)USART2_RX_BUF_1, USART2_MAX_RECV_LEN);
	
	USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ��� 	
	
	//ʹ�ܽ����ж�
//  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�����ж�   USART_IT_IDLE
	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
//	TIM4_Init(99,7199);		//1ms�ж�
	USART2_RX_STA=0;		//����
//	TIM4_Set(0);			//�رն�ʱ��4


}
//����2,printf ����
//ȷ��һ�η������ݲ�����USART2_MAX_SEND_LEN�ֽ�
void u2_printf(char* fmt,...)  
{  
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char*)USART2_TX_BUF,fmt,ap);
	va_end(ap);
	while(DMA_GetCurrDataCounter(DMA1_Channel7)!=0);	//�ȴ�ͨ��7�������   
	UART_DMA_Enable(DMA1_Channel7,strlen((const char*)USART2_TX_BUF)); 	//ͨ��dma���ͳ�ȥ
}

//������DMA
void UART_DMA_Config_Tx(DMA_Channel_TypeDef*DMA_CHx,u32 cpar,u32 cmar)
{
	DMA_InitTypeDef DMA_InitStructure_esp;
 	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//ʹ��DMA����
  DMA_DeInit(DMA_CHx);   //��DMA��ͨ��1�Ĵ�������Ϊȱʡֵ
	DMA_InitStructure_esp.DMA_PeripheralBaseAddr = cpar;  //DMA�������ַ
	DMA_InitStructure_esp.DMA_MemoryBaseAddr = cmar;  //DMA�ڴ����ַ
	DMA_InitStructure_esp.DMA_DIR = DMA_DIR_PeripheralDST;  //���ݴ��䷽�򣬴��ڴ��ȡ���͵�����
	DMA_InitStructure_esp.DMA_BufferSize = 0;  //DMAͨ����DMA����Ĵ�С
	DMA_InitStructure_esp.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //�����ַ�Ĵ�������
	DMA_InitStructure_esp.DMA_MemoryInc = DMA_MemoryInc_Enable;  //�ڴ��ַ�Ĵ�������
	DMA_InitStructure_esp.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  //���ݿ��Ϊ8λ
	DMA_InitStructure_esp.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //���ݿ��Ϊ8λ
	DMA_InitStructure_esp.DMA_Mode = DMA_Mode_Normal;  //��������������ģʽ
	DMA_InitStructure_esp.DMA_Priority = DMA_Priority_Medium; //DMAͨ�� xӵ�������ȼ� 
	DMA_InitStructure_esp.DMA_M2M = DMA_M2M_Disable;  //DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��
	DMA_Init(DMA_CHx, &DMA_InitStructure_esp);  //����DMA_InitStruct��ָ���Ĳ�����ʼ��DMA��ͨ��USART1_Tx_DMA_Channel����ʶ�ļĴ���	
} 

//����������DMA
static u16 DMA1_MEM_LEN;
DMA_InitTypeDef DMA_InitStructure_esp;

void UART_DMA_Config_Rx(DMA_Channel_TypeDef*DMA_CHx,u32 cpar,u32 cmar, u16 cndtr)
{
 	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//ʹ��DMA����
  DMA_DeInit(DMA_CHx);   //��DMA��ͨ��1�Ĵ�������Ϊȱʡֵ
	DMA1_MEM_LEN=cndtr;
	DMA_InitStructure_esp.DMA_PeripheralBaseAddr = cpar;  //DMA�������ַ
	DMA_InitStructure_esp.DMA_MemoryBaseAddr = cmar;  //DMA�ڴ����ַ
	DMA_InitStructure_esp.DMA_DIR = DMA_DIR_PeripheralSRC;  //���ݴ��䷽�򣬴��ڴ��ȡ���͵�����
	DMA_InitStructure_esp.DMA_BufferSize = cndtr;  //DMAͨ����DMA����Ĵ�С
	DMA_InitStructure_esp.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //�����ַ�Ĵ�������
	DMA_InitStructure_esp.DMA_MemoryInc = DMA_MemoryInc_Enable;  //�ڴ��ַ�Ĵ�������
	DMA_InitStructure_esp.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  //���ݿ��Ϊ8λ
	DMA_InitStructure_esp.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //���ݿ��Ϊ8λ
	DMA_InitStructure_esp.DMA_Mode = DMA_Mode_Normal;  //��������������ģʽ
	DMA_InitStructure_esp.DMA_Priority = DMA_Priority_High; //DMAͨ�� xӵ�������ȼ� 
	DMA_InitStructure_esp.DMA_M2M = DMA_M2M_Disable;  //DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��
	DMA_Init(DMA_CHx, &DMA_InitStructure_esp);  //����DMA_InitStruct��ָ���Ĳ�����ʼ��DMA��ͨ��USART1_Tx_DMA_Channel����ʶ�ļĴ���	
	//DMA_Cmd(DMA_CHx, ENABLE); //ʹ��USART1 TX DMA1 ��ָʾ��ͨ�� 
	MYDMA_Enable_esp(DMA1_Channel5);//��ʼһ��DMA���䣡
} 

void DMA2_Channel5_IRQHandler(void)
{
    if(DMA_GetITStatus(DMA2_IT_TC2))
    {
        DMA_ClearITPendingBit(DMA2_IT_GL2); //���ȫ���жϱ�־
    }
}

//����һ��DMA����
void UART_DMA_Enable(DMA_Channel_TypeDef*DMA_CHx,u8 len)
{
	DMA_Cmd(DMA_CHx, DISABLE );  //�ر� ָʾ��ͨ��        
	DMA_SetCurrDataCounter(DMA_CHx,len);//DMAͨ����DMA����Ĵ�С	
	DMA_Cmd(DMA_CHx, ENABLE);           //����DMA����
}	   

void MYDMA_Enable_esp(DMA_Channel_TypeDef*DMA_CHx)
{ 
DMA_Cmd(DMA_CHx, DISABLE ); //�ر�USART1 TX DMA1 ��ָʾ��ͨ�� 
DMA_SetCurrDataCounter(DMA_CHx,DMA1_MEM_LEN);//�������û����С,ָ������0
DMA_Cmd(DMA_CHx, ENABLE); //ʹ��USART1 TX DMA1 ��ָʾ��ͨ�� 
}   

static void buf_chn2(void)
{
	DMA_InitStructure_esp.DMA_BufferSize = USART2_MAX_SEND_LEN;
	if(rec_by==0)
	{
		rec_by=1;
		memcpy(USART2_RX_BUF,USART2_RX_BUF_1,USART2_RX_STA);    
    //memset(USART2_RX_BUF_1,0,USART2_MAX_SEND_LEN);
		DMA_InitStructure_esp.DMA_MemoryBaseAddr = (uint32_t)USART2_RX_BUF_2;         // ���ý��ջ������׵�ַ
		DMA_Init(DMA1_Channel6, &DMA_InitStructure_esp);               // д������
	}
	else
	{
		rec_by=0;
		memcpy(USART2_RX_BUF,USART2_RX_BUF_2,USART2_RX_STA);
    //memset(USART2_RX_BUF_2,0,USART2_MAX_SEND_LEN);
		DMA_InitStructure_esp.DMA_MemoryBaseAddr = (uint32_t)USART2_RX_BUF_1;         // ���ý��ջ������׵�ַ
		DMA_Init(DMA1_Channel6, &DMA_InitStructure_esp);               // д������    
	}
	DMA_Cmd(DMA1_Channel6, ENABLE);  
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 									 





















