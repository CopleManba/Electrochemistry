#include "spi2.h"

/**
*	@brief ����SPI2
*	@param SPI_TypeDef* SPIx --- ѡ��SPI
*/
void SPIx_Config(SPI_TypeDef* SPIx)
{
	 SPI_InitTypeDef  SPI_InitStructure;
   SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
   SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
   SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
   SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
   SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
   SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
   SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
   SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
   SPI_InitStructure.SPI_CRCPolynomial = 7;
   SPI_Init(SPIx, &SPI_InitStructure);
   SPI_Cmd(SPIx, ENABLE);
}

/**
*	@breif SPI2_Send_Byte SPI2����һ�ֽ�
*	@param u8 byte --- һ�ֽ�
**/
u8 SPI2_Send_Byte(u8 byte)
{
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);   /* Loop while DR register in not emplty */
  SPI_I2S_SendData(SPI2, byte);                                     /* Send byte through the SPI1 peripheral */
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);  /* Wait to receive a byte */
  return SPI_I2S_ReceiveData(SPI2);                                 /* Return the byte read from the SPI bus */
}


/**
*	@brief SPI2����һ�ֽ�
*	@param none
*/
u8 SPI2_Receive_Byte(void)
{
    return SPI2_Send_Byte(0XFF);
}



