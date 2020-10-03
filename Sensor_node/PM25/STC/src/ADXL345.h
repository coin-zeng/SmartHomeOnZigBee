#ifndef _ADXL345_H
  #define _ADXL345_H

#define	  SlaveAddress   0xA6	  //����������IIC�����еĴӵ�ַ,����ALT  ADDRESS��ַ���Ų�ͬ�޸�
                                 //ALT  ADDRESS���Žӵ�ʱ��ַΪ0xA6���ӵ�Դʱ��ַΪ0x3A
sbit	  SCL=P1^4;      //IICʱ�����Ŷ���
sbit 	  SDA=P1^3;      //IIC�������Ŷ���

typedef unsigned char  BYTE;
typedef unsigned short WORD;




void Init_ADXL345(void);             //��ʼ��ADXL345
void  Single_Write_ADXL345(uchar REG_Address,uchar REG_data);   //����д������
uchar Single_Read_ADXL345(uchar REG_Address);                   //������ȡ�ڲ��Ĵ�������
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
