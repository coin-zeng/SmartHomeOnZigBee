#include"config.h"
#include"seg.h"

extern unsigned char i;
sbit DT=P1^4;
sbit CS=P1^2;
sbit CLK=P1^3;

void init1620(void)
{
CS=1;
DT=1;
CLK=1;
}
	
/**********����һ���ֽ�************/
void sendbyte1620(unsigned char dat)
{
    uchar i;
	 for(i = 0; i < 8; i++)
	 {
	      DT = 0;
		  if(dat & 0x01)
		  {
		      DT = 1;
		  }
		  CLK = 0;
		  _nop_();
		  _nop_();			  
		  CLK = 1;
		  dat >>= 1;

	 }
}

void display1620(unsigned char dat1,unsigned char dat2,unsigned char dat3,unsigned char dat4)	  
{
     CS = 1;
	 CLK = 1;
	 _nop_();
	 _nop_();
	 CS = 0;
	 _nop_();
	 CLK = 0;
	 sendbyte1620(0x00); //������ʾģʽ��00H��
	 _nop_();
	 CS = 1;
	 _nop_();									 
	 CS = 0;
	 sendbyte1620(0x40); // ����д�Դ������������õ�ַ�Զ���1ģʽ��40H��
	 _nop_();
	 CS = 1;
	 _nop_();
	 CS = 0;
	 sendbyte1620(0x0C0); //������ʼ��ַ��0C0H��
	 _nop_();
	 CS= 1;
	 _nop_();
	 CS = 0;
	 sendbyte1620(0x00); //��ʾ��ַ00д0x00
	 sendbyte1620(dat1); //��ʾ��ַ01дnum1
	 sendbyte1620(0x00); //��ʾ��ַ02д0x00
	 sendbyte1620(dat2); //��ʾ��ַ03дnum2
	 sendbyte1620(0x00); //��ʾ��ַ04д0x00
	 sendbyte1620(dat3); //��ʾ��ַ05дnum3
	 sendbyte1620(0x00); //��ʾ��ַ06д0x00
	 sendbyte1620(dat4); //��ʾ��ַ07дnum4
	 sendbyte1620(0x00); //��ʾ��ַ08д0x00
	 sendbyte1620(0x00); //��ʾ��ַ09д0x00
	 sendbyte1620(0x00); //��ʾ��ַ0Aд0x00
	 sendbyte1620(0x00); //��ʾ��ַ0Bд0x00
	 sendbyte1620(0x00); //��ʾ��ַ0Cд0x00
	 sendbyte1620(0x00); //��ʾ��ַ0Dд0x00
	 CS = 1;
	 _nop_();
	 CS = 0;
	 _nop_();
	 sendbyte1620(0x8F); //����ʾ��8FH��
	 CLK = 0;
	 CS = 1;
	 delay_ms(3);
}
