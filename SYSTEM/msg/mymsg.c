#include "mymsg.h"


int msgArray(int msgid,int msgbody,uint8_t *sendBuffer)   //������Ϣ֡����
{
	sendBuffer[0]=msgid+0x30;
	sendBuffer[2]=msgbody/10+0x30;
	sendBuffer[3]=msgbody%10+0x30;
	sendBuffer[1]=sizeof(sendBuffer)/sizeof(sendBuffer[0])+0x30;  //�������ڳ���
	printf("The Len:%d\r\n",sizeof(sendBuffer)/sizeof(sendBuffer[0]));
	return sizeof(sendBuffer)/sizeof(sendBuffer[0]);
}


///**
//*		����֡��ʽ�� msg_id msg_len msg_body
//*/
//u16 DecodeMSG(u8 *usartRecBuffer)
//{
//	u16 Voltage;
//	u16 msg_len; //,msg_body
//	msg_len=usartRecBuffer[1];  //��ȡ��Ϣ�ĳ���
//	switch((usartRecBuffer[0]&0x0f))
//	{
//		case SetDAMsagID: printf("%d,%d\r\n",usartRecBuffer[1],usartRecBuffer[2]);
//								Voltage = (usartRecBuffer[2]&0x0f)*10+(usartRecBuffer[1]&0x0f);break;   //33 34 31 32 
//		case SetDAAckMsgID:;
//		case GetADValueID:;
//		case GetADAckMsgID:;
//	}
//	return Voltage;
//}

/**
*	@brief �����������
*/
void ClearArrayData(unsigned char array[],unsigned char length)  
{
	int i;
	for(i=0;i<=length;i++)
	{
		array[i] = 0;
	}
	length = 0;
}
