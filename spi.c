#include <stdio.h>

#include "io.h"
#include "system.h"
#include "spi.h"

//////////////////////////////////////////////////////////////////
// SOFTWARE SPI

static int s_CPOL, s_CPHA;

static void SSPI_Delay(void)
{
  //DelayUs(2);
}

void SSPI_Init(int mode)
{
  mode &= 0x03;
  s_CPOL = mode >> 1;
  s_CPHA = mode & 1;
  
  // SSPI uçlari yapilandirilacak
  IO_Write(IOP_SSPI_SCK, s_CPOL);
  IO_Init(IOP_SSPI_SCK, IO_MODE_OUTPUT);
  
  IO_Init(IOP_SSPI_MISO, IO_MODE_INPUT);
  IO_Init(IOP_SSPI_MOSI, IO_MODE_OUTPUT);
}

// Parametredeki deger gönderilir
// Geri dönüs alinan deger
unsigned char SSPI_Data(unsigned char val)
{
  int i;
  
  for (i = 0; i < 8; ++i) {
    // 1) Data setup (verinin hazirlanmasi)
    IO_Write(IOP_SSPI_MOSI, (val & 0x80) ? 1 : 0);
    val <<= 1;
    SSPI_Delay();
    
    // 2) Clock generation (saat isareti üretimi)
    IO_Write(IOP_SSPI_SCK, !s_CPOL);
    SSPI_Delay();
    
    if (!s_CPHA)
      if (IO_Read(IOP_SSPI_MISO))
        val |= 1;

    IO_Write(IOP_SSPI_SCK, s_CPOL);
    SSPI_Delay();
    
    if (s_CPHA)
      if (IO_Read(IOP_SSPI_MISO))
        val |= 1;
  }
  
  return val;
}

#if 0
unsigned char SSPI_DataLSB(unsigned char val)
{
  int i;
  
  for (i = 0; i < 8; ++i) {
    // 1) Data setup (verinin hazirlanmasi)
    IO_Write(IOP_SSPI_MOSI, (val & 1));
    val >>= 1;
    SSPI_Delay();
    
    // 2) Clock generation (saat isareti üretimi)
    IO_Write(IOP_SSPI_SCK, !s_CPOL);
    SSPI_Delay();
    
    if (!s_CPHA)
      if (IO_Read(IOP_SSPI_MISO))
        val |= 0x80;

    IO_Write(IOP_SSPI_SCK, s_CPOL);
    SSPI_Delay();
    
    if (s_CPHA)
      if (IO_Read(IOP_SSPI_MISO))
        val |= 0x80;
  }
  
  return val;
}
#endif

//////////////////////////////////////////////////////////////////
// HARDWARE SPI

void HSPI_Init(int idx, int mode)
{
    SPI_TypeDef *pSPI = (idx == SPI_1) ? SPI1 : SPI2; 
    SPI_InitTypeDef sInit;
    int CPOL, CPHA;
    
    SPI_StructInit(&sInit);
    
    mode &= 0x03;
    CPOL = (mode >> 1);
    CPHA = (mode & 1);
    
    if (idx == SPI_1) {
      // 1) SPI Clock aktif edilecek
      RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
      
      // 2) I/O uçlari alternatif fonksiyon olarak yapilandirilacak
      IO_Init(IOP_SPI1_SCK, IO_MODE_ALTERNATE);
      IO_Init(IOP_SPI1_MOSI, IO_MODE_ALTERNATE);
      IO_Init(IOP_SPI1_MISO, IO_MODE_ALTERNATE); // bagzi peripherallarda input olarak da yapilandirilabiliyor DIKKAT!!
      
      GPIO_PinAFConfig(g_ios[IOP_SPI1_SCK].port, g_ios[IOP_SPI1_SCK].pin, GPIO_AF_SPI1);
      GPIO_PinAFConfig(g_ios[IOP_SPI1_MOSI].port, g_ios[IOP_SPI1_MOSI].pin, GPIO_AF_SPI1);
      GPIO_PinAFConfig(g_ios[IOP_SPI1_MISO].port, g_ios[IOP_SPI1_MISO].pin, GPIO_AF_SPI1);      
    }
    else if (idx == SPI_2) {
      // 1) SPI Clock aktif edilecek
      RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

      // 2) I/O uçlari alternatif fonksiyon olarak yapilandirilacak
      IO_Init(IOP_SPI2_SCK, IO_MODE_ALTERNATE);
      IO_Init(IOP_SPI2_MOSI, IO_MODE_ALTERNATE);
      IO_Init(IOP_SPI2_MISO, IO_MODE_ALTERNATE);

      GPIO_PinAFConfig(g_ios[IOP_SPI2_SCK].port, g_ios[IOP_SPI2_SCK].pin, GPIO_AF_SPI2);
      GPIO_PinAFConfig(g_ios[IOP_SPI2_MOSI].port, g_ios[IOP_SPI2_MOSI].pin, GPIO_AF_SPI2);
      GPIO_PinAFConfig(g_ios[IOP_SPI2_MISO].port, g_ios[IOP_SPI2_MISO].pin, GPIO_AF_SPI2);
    }
    else
      return;

    // 3) Init struct kullanilarak SPI parametreleri yapilandirilacak
    sInit.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
    sInit.SPI_CPOL = CPOL ? SPI_CPOL_High : SPI_CPOL_Low;
    sInit.SPI_CPHA = CPHA ? SPI_CPHA_2Edge : SPI_CPHA_1Edge;
    sInit.SPI_DataSize = SPI_DataSize_8b;
    sInit.SPI_FirstBit = SPI_FirstBit_MSB;
    sInit.SPI_Mode = SPI_Mode_Master;
    sInit.SPI_NSS = SPI_NSS_Soft;
    
    SPI_Init(pSPI, &sInit);
    
    // 4) SPI çevresel birimi baslatilacak
    SPI_Cmd(pSPI, ENABLE);
}

// Parametredeki deger gönderilir
// Geri dönüs alinan deger
unsigned char HSPI_Data(int idx, unsigned char val)
{
    SPI_TypeDef *pSPI = (idx == SPI_1) ? SPI1 : SPI2; 
  
    while (!SPI_I2S_GetFlagStatus(pSPI, SPI_I2S_FLAG_TXE)) ;
    
    SPI_I2S_SendData(pSPI, val);
    
    // Rx Buffer bos oldugu müddetçe bekle
    while (!SPI_I2S_GetFlagStatus(pSPI, SPI_I2S_FLAG_RXNE)) ;
    
    val = SPI_I2S_ReceiveData(pSPI);
    
    return val;
}


