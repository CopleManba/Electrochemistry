#ifndef _ADS1220_H
#define _ADS1220_H

#include "sys.h"

//#define DRDY     GPIO_Pin_6      //PC6

#define DRDY     GPIO_Pin_8   //PA8
#define CS_Pin   GPIO_Pin_12     //PB12
#define SCK_Pin  GPIO_Pin_13     //PB13
#define MISO_Pin GPIO_Pin_14     //PB14
#define MOSI_Pin GPIO_Pin_15     //PB15
#define ADC_DRDY GPIO_ReadInputDataBit(GPIOC,DRDY)

//ADS1220�����
#define ADC_CMD_RESET      0X06             //��λ����                      
#define ADC_CMD_START      0X08             //����������ת��                
#define ADC_CMD_POWERDOWN  0X02             //�������ģʽ                  
#define ADC_CMD_RDATA      0X10             //ͨ�������ȡ����             
#define ADC_CMD_RREG       0X20             //��ȡnn �Ĵ�������ʼ��ַ��rr�� 
#define ADC_CMD_WREG       0X40             //д��nn �Ĵ�������ʼ��ַ��rr�� 
//��������rr = ���üĴ�����00 ��11����nn = �ֽ����C 1��00 ��11���Լ�x = �޹�ֵ��

//ADS�Ĵ������ñ�


//���ſ���
#define ADC_CS_0    GPIO_ResetBits(GPIOB, GPIO_Pin_12)    //PB12
#define ADC_CS_1    GPIO_SetBits(GPIOB, GPIO_Pin_12)
#define ADC_SCLK_0  GPIO_ResetBits(GPIOB, GPIO_Pin_13)
#define ADC_SCLK_1  GPIO_SetBits(GPIOB, GPIO_Pin_13)

//ads1220��ȡ���洢����
//void ADS1220_Read_Data(void);
void ADS1220_SPI2_Init(void);
void ADS1220_Setting(u8 ch);//ADC��ʼ������
double ADS1220_Get_Voltage_Conversion_Data(void);
void ADS1220_Channel_Read_Data(void);
#endif

