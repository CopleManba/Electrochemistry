#include "sys.h"

u8 status = 0;

uint8_t sendBuffer[8]={0}, order=0,i=0;

extern uint8_t wifi_flag;
extern uint8_t connect_flag;
extern u8 dataptr;



int main(void)
{	 
//	char sdata[20];
//	u16 len;
//	u16 test=0;
	int test_x=1000;
	delay_init();	    	 //��ʱ������ʼ��	  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�

	uart_init(115200);	 	//���ڳ�ʼ��Ϊ115200	
	
	ADS1220_SPI2_Init();           //ADS1220��ʼ��
	ADS1220_Setting(1);            //ADS1220���üĴ���channel2
	
	DAC8563_Init();
	Adc_Init();
	USART2_Init(115200);//����2��ʼ��Ϊ115200	
	
//				mqtt_connect();
//	printf("mqtt connected");
//	Publish_MSG("\"test_data\"","hello");
	
	while(1)
  {
		DAC8563_SetVoltage(Channel_DAC_AB,(u16)(32768-test_x*3.277));
		ADS1220_Channel_Read_Data();
//		printf("num:%d,V:%d\r\n" , test_x,Get_Adc(ADC_Channel_5));
		delay_ms(1000);
		if(wifi_flag)
		{ 
			wifi_flag=0;
				mqtt_connect();
				printf("mqtt connected");
				Publish_MSG("\"test_data\"","hello");
				connect_flag=1;
		}
	}
		
}		
 
 
