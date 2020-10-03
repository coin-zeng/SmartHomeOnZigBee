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
	
/**********发送一个字节************/
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
	 sendbyte1620(0x00); //设置显示模式（00H）
	 _nop_();
	 CS = 1;
	 _nop_();									 
	 CS = 0;
	 sendbyte1620(0x40); // 设置写显存的数据命令，采用地址自动加1模式（40H）
	 _nop_();
	 CS = 1;
	 _nop_();
	 CS = 0;
	 sendbyte1620(0x0C0); //设置起始地址（0C0H）
	 _nop_();
	 CS= 1;
	 _nop_();
	 CS = 0;
	 sendbyte1620(0x00); //显示地址00写0x00
	 sendbyte1620(dat1); //显示地址01写num1
	 sendbyte1620(0x00); //显示地址02写0x00
	 sendbyte1620(dat2); //显示地址03写num2
	 sendbyte1620(0x00); //显示地址04写0x00
	 sendbyte1620(dat3); //显示地址05写num3
	 sendbyte1620(0x00); //显示地址06写0x00
	 sendbyte1620(dat4); //显示地址07写num4
	 sendbyte1620(0x00); //显示地址08写0x00
	 sendbyte1620(0x00); //显示地址09写0x00
	 sendbyte1620(0x00); //显示地址0A写0x00
	 sendbyte1620(0x00); //显示地址0B写0x00
	 sendbyte1620(0x00); //显示地址0C写0x00
	 sendbyte1620(0x00); //显示地址0D写0x00
	 CS = 1;
	 _nop_();
	 CS = 0;
	 _nop_();
	 sendbyte1620(0x8F); //开显示（8FH）
	 CLK = 0;
	 CS = 1;
	 delay_ms(3);
}
