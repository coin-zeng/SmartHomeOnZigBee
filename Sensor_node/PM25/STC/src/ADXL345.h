#ifndef _ADXL345_H
  #define _ADXL345_H

#define	  SlaveAddress   0xA6	  //定义器件在IIC总线中的从地址,根据ALT  ADDRESS地址引脚不同修改
                                 //ALT  ADDRESS引脚接地时地址为0xA6，接电源时地址为0x3A
sbit	  SCL=P1^4;      //IIC时钟引脚定义
sbit 	  SDA=P1^3;      //IIC数据引脚定义

typedef unsigned char  BYTE;
typedef unsigned short WORD;




void Init_ADXL345(void);             //初始化ADXL345
void  Single_Write_ADXL345(uchar REG_Address,uchar REG_data);   //单个写入数据
uchar Single_Read_ADXL345(uchar REG_Address);                   //单个读取内部寄存器数据
void  Multiple_Read_ADXL345();     
void Delay5us();
void Delay5ms();
void ADXL345_Start();
void ADXL345_Stop();
void ADXL345_SendACK(bit ack);
bit  ADXL345_RecvACK();
void ADXL345_SendByte(BYTE dat);
BYTE ADXL345_RecvByte();


#endif
