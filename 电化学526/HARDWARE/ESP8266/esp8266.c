#include "esp8266.h"

//����ֵ:0,û�еõ��ڴ���Ӧ����
//    ����,�ڴ�Ӧ������λ��(str��λ��)
u8* esp_check_cmd(char *str)
{
	USART2_RX_BUF[USART2_RX_STA]=0;      //��ӽ�����
	return (u8*)strstr((const char*)USART2_RX_BUF,(const char*)str);
}
u8* esp_check_cmd2(char *str)
{
	USART2_RX_BUF[USART2_RX_STA+1]=0;	   //��ӽ�����
	return (u8*)strstr((const char*)(USART2_RX_BUF+25),(const char*)str);
}

//����ֵ:0,���ͳɹ�(�õ����ڴ���Ӧ����)
//       1,����ʧ��
extern u16 USART2_RX_STA;
unsigned char isok = 0;
u8 esp_send_cmd_IT(char *cmd,u16 waittime)
{
	isok = 0;
	u2_printf("%s\r\n",cmd);					//��������
	printf("%s\r\n",cmd);
	while(--waittime)									//�ȴ�����ʱ
	{
		delay_ms(1);
		if(isok)
		{
			break;												//�õ���Ч���� ack
		}
	}
	if(waittime==0)return 1; 
	return 0;
} 

int mqtt_connect(void)
{
	while(esp_send_cmd_IT("AT+CWJAP=\"iPhone\",\"19990929\"",2000))
	{
		printf("connecting wifi/r/n");
	}
	printf(".");
	while(esp_send_cmd_IT("ATE0",200));
	while(esp_send_cmd_IT("AT+MQTTUSERCFG=0,1,\"ESP\",\"esp\",\"1234567890\",0,0,\"\"",500));
	delay_ms(1000);
	while(esp_send_cmd_IT("AT+MQTTCONN=0,\"124.71.235.8\",1883,0",1000));
	while(esp_send_cmd_IT("AT+MQTTSUB=0,\"sub_msg\",0",500));
	printf(".");
	delay_ms(500);
	USART2_RX_STA = 0;
	return 1;
}

char pub_flag = 0;
u8 cnt = 0;

int Publish_MSG(char *topic,char *arr)
{
	char buf[80];
	cnt = 20;
	USART2_RX_STA = 0;
	sprintf(buf,"AT+MQTTPUB=0,%s,\"%s\",0,0",topic,arr);
	while(esp_send_cmd_IT(buf,200))
	{
		cnt--;
		pub_flag=1;//����ʧ��һֱ��1��һֱ����
		if(cnt<=0)break;
	}
	pub_flag=0;
	USART2_RX_STA = 0;
	return 1;
}

//4λchar����ת��int����
static int str4_to_int(unsigned char * str)
{
	int r = 0;
	if(str[0])r += (int)(str[0] - 0x30) * 1000;
	if(str[1])r += (int)(str[1] - 0x30) * 100 ;
	if(str[2])r += (int)(str[2] - 0x30) * 10  ;
	if(str[3])r += (int)(str[3] - 0x30) * 1   ;
	return r;
}

//extern unsigned char status;	//0 stop  1 start
int DATA_BUF[2048];							  //���������
u16 dataptr = 0;									//��������������
extern u8 CtrCode;

void Subscribe_MSG_IT(void)
{
	if(esp_check_cmd2("P:"))
	{
		//P����
//		CtrCode = 'P';
		DATA_BUF[dataptr++] = str4_to_int(USART2_RX_BUF+29) * 1;
	}
	else if(esp_check_cmd2("N:"))
	{
		//N����
//		CtrCode = 'N';
		DATA_BUF[dataptr++] = str4_to_int(USART2_RX_BUF+29) * (-1);
	}
//	else if(esp_check_cmd2("start"))
//	{
//		//���յ�start����
//		status = 1;
//	}
//	else if(esp_check_cmd2("stop"))
//	{
//		//���յ�stop����
//		status = 0;	
//	}
	else 
		return ;
	USART2_RX_STA = 0;
}
