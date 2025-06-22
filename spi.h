#ifndef _SPI_H
#define _SPI_H

enum {
  SPI_1,
  SPI_2,
};

#define SSPI_Send(c)    SSPI_Data(c)
#define SSPI_Recv()     SSPI_Data(0)

#define HSPI_Send(p, c) HSPI_Data(p, c)
#define HSPI_Recv(p)    HSPI_Data(p, 0)

void SSPI_Init(int mode);
unsigned char SSPI_Data(unsigned char val);

void HSPI_Init(int idx, int mode);
unsigned char HSPI_Data(int idx, unsigned char val);

#endif  /* _SPI_H */
