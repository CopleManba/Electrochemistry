#include "esp8266.h"

//����ֵ:0,û�еõ��ڴ���Ӧ����
//    ����,�ڴ�Ӧ������λ��(str��λ��)
u8* esp_check_cmd(char *str)
{
	
	//char *strx=0;
	USART2_RX_BUF[USART2_RX_STA]=0;//��ӽ�����
	//strx=strstr((const char*)USART2_RX_BUF,(const char*)str);
	return (u8*)strstr((const char*)USART2_RX_BUF,(const char*)str);
}

u8* esp_check_cmd2(char *str)
{
	
	//char *strx=0;
	USART2_RX_BUF[USART2_RX_STA+1]=0;//��ӽ�����
	//strx=strstr((const char*)USART2_RX_BUF,(const char*)str);
	return (u8*)strstr((const char*)(USART2_RX_BUF+25),(const char*)str);
}

extern u16 USART2_RX_STA;
unsigned char isok = 0;
//����ֵ:0,���ͳɹ�(�õ����ڴ���Ӧ����)
//       1,����ʧ��
u8 esp_send_cmd_IT(char *cmd,u16 waittime)
{
	isok = 0;
	u2_printf("%s\r\n",cmd);	//��������
	while(--waittime)	//�ȴ�����ʱ
	{
		delay_ms(1);
		if(isok)
		{
			break;//�õ���Ч���� ack
		}
	}
	if(waittime==0)return 1; 
	return 0;
} 

int mqtt_connect(void)
{
	while(esp_send_cmd_IT("AT+CWJAP=\"Cople\",\"12345678\"",2000))
	{
//		UsartPrintf(USART1,"connecting wifi\r\n");
		printf("connecting wifi/r/n");
	}
//	UsartPrintf(USART1,".");
	printf(".");
	while(esp_send_cmd_IT("ATE0",200));
	while(esp_send_cmd_IT("AT+MQTTUSERCFG=0,1,\"ESP\",\"esp\",\"1234567890\",0,0,\"\"",500));
	delay_ms(1000);
	while(esp_send_cmd_IT("AT+MQTTCONN=0,\"8.136.41.10\",1883,0",1000));
	while(esp_send_cmd_IT("AT+MQTTSUB=0,\"sub_msg\",0",500));
//	UsartPrintf(USART1,".");
	printf(".");
	delay_ms(500);
	USART2_RX_STA = 0;
	return 1;
}

char pub_flag = 0;

int Publish_MSG(char *topic,char *arr)
{
	char buf[80];
	USART2_RX_STA = 0;
	sprintf(buf,"AT+MQTTPUB=0,%s,\"%s\",0,0",topic,arr);
	while(esp_send_cmd_IT(buf,100))
	{
		pub_flag=1;//����ʧ��һֱ��1��һֱ����
	}
	pub_flag=0;
	USART2_RX_STA = 0;
	return 1;
}

extern unsigned char status;//0 stop  1 start
unsigned int counter_debug = 0;
int DATA_BUF[1024];
u8 dataptr = 0;

static int str4_to_int(unsigned char * str)
{
	int r = 0;
	if(str[0])
	{
		r+= (int)(str[0]-0x30) * 1000;
	}
	
	if(str[1])
	{
		r+= (int)(str[0]-0x30) * 100;
	}
	
	if(str[2])
	{
		r+= (int)(str[0]-0x30) * 10;
	}
	
	if(str[3])
	{
		r+= (int)(str[0]-0x30) * 1;
	}
	return r;
}

void Subscribe_MSG_IT(void)
{
	char * i = 0;
	counter_debug++;
	if(esp_check_cmd2("P"))
	{
//		i = strstr((const char*)USART2_RX_BUF,(const char*)"P:");
//		USART2_RX_BUF[USART2_RX_STA&0X7FFF]=0;
//		counter_debug++;
//		UsartPrintf(USART1,USART2_RX_BUF+29);
//		UsartPrintf(USART1,"\r\n");
//		printf("%s\r\n",USART2_RX_BUF+29);
		printf("%d\r\n",str4_to_int(USART2_RX_BUF+29));
		DATA_BUF[dataptr++] = str4_to_int(USART2_RX_BUF+29);
		if(USART2_RX_STA > 40)
		{
			i = strstr((const char*)(USART2_RX_BUF+30),(const char*)"P:");
			if(i)
			{
//				UsartPrintf(USART1,i+2);
//				UsartPrintf(USART1,"*\r\n");
				counter_debug++;
//				i = strstr((const char*)(i+10),(const char*)"P:");
//				if(i)
//				{
//					UsartPrintf(USART1,i+2);
//					UsartPrintf(USART1,"\r\n");
//					counter_debug++;
//				}
			}
		}
	}
	else if(esp_check_cmd2("N:"))
	{
		//i = strstr((const char*)USART2_RX_BUF,(const char*)"N:");
//		USART2_RX_BUF[USART2_RX_STA&0X7FFF]=0;
//		counter_debug++;
//		UsartPrintf(USART1,USART2_RX_BUF+27);
//		UsartPrintf(USART1,"\r\n");		
	}
	else if(esp_check_cmd2("start"))
	{
		//���յ�start����
		status = 1;
//		UsartPrintf(USART1,"start\r\n");
	}
	else if(esp_check_cmd2("stop"))
	{
		//���յ�stop����
		status = 0;				
//		UsartPrintf(USART1,"stop\r\n");
	}
	else 
		return ;
	USART2_RX_STA = 0;
	
}
