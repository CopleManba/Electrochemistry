#include "DAC8552.h"
#include "delay.h"

#define Vref  2.48f

void DAC8552_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
 	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 //ʹ��PA�˿�ʱ��
	
	GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11);				 //LED0-->PA.8 �˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_SetBits(GPIOC,(GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11));						 //PA.8 �����
}
/*******************************************
��������DAC8552_write
���ܣ���8552д����
������commond��ͨ������ Channel_A/Channel_B/Channel_AB
      date������0-65535
������
********************************************/
void DAC8552_write(unsigned char commond, unsigned int date)
{
	unsigned char i;
	SCLK1;
	SYNC0;
	for(i=0; i<8; i++)
	{
		if (commond & 0x80)
		{
		  DIN1;
		}
		else
		{
		  DIN0;
		}
		SCLK0;
		commond <<= 1;
		SCLK1;
	} 
	for (i=0; i<16; i++)
	{
		if (date & 0x8000)
		{
			DIN1;
		}
		else
		{
			DIN0;
		}
		SCLK0;
		date <<= 1;
		SCLK1;
	} 
	SYNC1;
}
void voltage_output(unsigned char channel, unsigned int vl2)
{
	switch(channel)
	{
		case Channel_A: DAC_A(vl2); break;
		case Channel_B: DAC_B(vl2); break;
		case Channel_AB: DAC_AB(vl2,vl2); break;
		default: DAC_AB(0,0);break;
	}
}


void write_8552(unsigned int data) 
{ 
  u8 i;  
	SYNC1;   
	delay_us(1);  
	SCLK0;   
	SYNC0; 
	for(i=0;i<24;i++)   
	{ 
    if(0x800000&data)DIN1;     
		else             DIN0;     
		SCLK1;
		delay_us(1);  
		SCLK0;
		data=data<<1;   
	} 
  delay_us(1);   
	SYNC1; 
} 


void DAC_A(unsigned int data) 
{ 
  u16 ctr=0x10;  
	data=data|(ctr<<16);  
	printf("%d\r\n",data);
	write_8552(data); 
}

void DAC_B(unsigned int data)
{ 
  u16 ctr=0x24;   
	data=data|(ctr<<16);   
	write_8552(data); 
}

void DAC_AB(unsigned int data_A,unsigned int data_B) 
{ 
  u16 ctr=0x00; 
  data_A=data_A|(ctr<<16);
	write_8552(data_A);  

  ctr=0x34; 
  data_B=data_B|(ctr<<16);  
	write_8552(data_B); 

}









/*
void voltage_output(unsigned char channel, double vl2)  //�ⲿ�ο���ѹ
{
	unsigned int vl;
	
	vl = (unsigned int)(vl2 / Vref * 0xffff);
	DAC8552_write(channel, vl);
}
*/
