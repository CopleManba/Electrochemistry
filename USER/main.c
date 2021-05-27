#include "sys.h"

u8 status = 0;

uint8_t sendBuffer[20]={0}, order=0,i=0;

extern u8 CtrCode;
extern u16 CtrNumber;
u16 CtrNumberForUart;
u16 DACInData;       //DAC����������˸�ֵ
extern char sdata[];

extern uint8_t wifi_connect_flag;
extern uint8_t wifi_disconnect_flag;
extern uint8_t connect_flag;
extern u16 dataptr;
u16 dataptr2 = 0;


int main(void)
{	 

	delay_init();	    	 //��ʱ������ʼ��	  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�

	uart_init(115200);	 	//���ڳ�ʼ��Ϊ115200	
	
	ADS1220_SPI2_Init();           //ADS1220��ʼ��
	ADS1220_Setting(1);            //ADS1220���üĴ���channel2
	
	DAC8563_Init();
	USART2_Init(115200);  //����2��ʼ��Ϊ115200	
	
	esp_send_cmd_IT("AT+MQTTCLEAN=0\r\n", 200);
	esp_send_cmd_IT("AT+CWQAP\r\n", 200);
	printf("Clear WIFI!");
	
	while(1)
  {
		if(wifi_connect_flag)
		{
			wifi_connect_flag=0;
			mqtt_connect();
			printf("mqtt connected");
			//Publish_MSG("\"test_data\"","hello");
			connect_flag=1;
		}
		else if(wifi_disconnect_flag)
		{
			wifi_disconnect_flag=0;
			esp_send_cmd_IT("AT+MQTTCLEAN=0\r\n", 200);
			esp_send_cmd_IT("AT+CWQAP\r\n", 200);
			printf("Clear WIFI!");
		}
		
		

		if(dataptr>0)  // ����cmd
		{
			if(DATA_BUF[dataptr2]>=0)
			{
				CtrCode = 'P';
				CtrNumber = DATA_BUF[dataptr2];
			}
			else
			{
				CtrCode = 'N';
				CtrNumber = -DATA_BUF[dataptr2];
			}
		}
		
		switch (CtrCode)
		{
			case'P'://DAC����  // 0 65535  -32768  0  32768
 				DACInData = (u16)(CtrNumber*3.277+32767);	
				DAC8563_SetVoltage(Channel_DAC_AB,DACInData);
//				printf("%d",DACInData);
				delay_ms(3);
				ADS1220_Channel_Read_Data();
				break;
			case'N'://DAC����
				DACInData = (u16)(32768-CtrNumber*3.277);	
				DAC8563_SetVoltage(Channel_DAC_AB,DACInData);
				delay_ms(3);
				ADS1220_Channel_Read_Data();
				CtrCode = ' ';
				break;
			case'E'://����������
				printf("%c%d\r\n",CtrCode,CtrNumber);
				CtrCode = ' ';
				break;
			case'S'://���Ӵ���Ӧ��
				printf("J");
				CtrCode = ' ';
				break;
		}
		
		}
		
}		
 
 
