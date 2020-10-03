/**********************************************************/
//晶振频率：11.0592MHz
//文件名  ：Main.c
//功能说明：火焰传感器读取实验
//制作    ：www.frotech.com
//技术支持：020-22883196 QQ:
//变更记录: 2013.05.02
//变更内容: 新建造
/**********************************************************/

#include <STC12C5A60S2.h>


#define		BUF_LENTH	128		//定义串口接收缓冲长度
unsigned char 	uart1_wr;		//写指针
unsigned char 	uart1_rd;		//读指针
unsigned char 	xdata RX0_Buffer[BUF_LENTH];	//接收缓冲
unsigned char flag;
unsigned char i;
unsigned char   xdata mbus_buffer[255];
unsigned char   xdata mbus_Sendbuf[255];
unsigned char   xdata Crc_buf[2];  //声明存储CRC校验值的高8位及低8位的缓存
unsigned int  Crc_return_data;  //声明CRC校验值
bit		B_TI; //发送完成标志
sbit  P1_0 = P1^0;//定义P1.0端口
#define HAVEFIRE 	'd' 	 //有火焰
#define NOFIRE	    'e' 	 //无火焰

void	uart1_init(void);
void Uart1_TxByte(unsigned char dat);
void Uart1_String(unsigned char code *puts);
void delay_ms(unsigned char ms);
 unsigned int cal_crc(unsigned char *snd, unsigned char num);

/*************** 用户定义参数 *****************************/

#define MAIN_Fosc		11059200UL	
#define Baudrate0		115200UL		
									
/**********************************************************/


/****************** 编译器自动生成，用户请勿修改 ************************************/

#define BRT_Reload			(256 - MAIN_Fosc / 16 / Baudrate0)		//Calculate the timer1 reload value ar 1T mode


/**********************************************************/

//**********************************************************************
//函数名：main(void)
//输入  ：无
//输出  ：无
//功能描述：当有火焰或者灯直射火焰传感器时候，D1亮,发送查询指令会返回有火焰数据
//          P1.0采用准双向口工作模式
//**********************************************************************
void main(void)
{
	uart1_init();//初始化串口
	while(1)
	{
		delay_ms(50);
		if(P1_0==0)
		{
			Uart1_TxByte(HAVEFIRE);
		}
		else
		{
			Uart1_TxByte(NOFIRE);
		}
	}
}
//**********************************************************************
//函数名：uart1_init(void)
//输入  ：无
//输出  ：无
//功能描述：串口初始化函数，通信参数为9600 8 N 1
//**********************************************************************
void	uart1_init(void)
{
	PCON |= 0x80;		//UART0 Double Rate Enable
	SCON = 0x50;		//UART0 set as 10bit , UART0 RX enable
	AUXR |=  0x01;		//UART0 使用BRT
	AUXR |=  0x04;		//BRT set as 1T mode
	BRT = BRT_Reload;
	AUXR |=  0x10;		//start BRT	

	ES  = 1;
	EA = 1;
}
/*****************************CRC校验计算函数********************************/
 unsigned int cal_crc(unsigned char *snd, unsigned char num)

{

   unsigned char i, j;

   unsigned int c,crc=0xFFFF;

   for(i = 0; i < num; i ++)

   {

       c = snd[i] & 0x00FF;

       crc ^= c;

       for(j = 0;j < 8; j ++)

       {

           if (crc & 0x0001)

           {

               crc>>=1;

               crc^=0xA001;

           }

           else crc>>=1;

       }

   }   

   return(crc);
}
//**********************************************************************
//函数名：Uart1_TxByte(unsigned char dat)
//输入  ：需要发送的字节数据
//输出  ：无
//功能描述：从串口发送单字节数据
//**********************************************************************
void Uart1_TxByte(unsigned char dat)
{
    B_TI = 0;
	SBUF = dat;
	while(!B_TI);
	B_TI = 0;
}
//**********************************************************************
//函数名：Uart1_String(unsigned char code *puts)
//输入  ：字符串首地址
//输出  ：无
//功能描述：从串口发送字符串
//**********************************************************************
void Uart1_String(unsigned char code *puts)
{
    for(; *puts != 0; puts++)
	{
        Uart1_TxByte(*puts);

	}
}

//**********************************************************************
//函数名：UART1_RCV (void)
//输入  ：无
//输出  ：无
//功能描述：串口中断接收函数
//**********************************************************************
void UART1_RCV (void) interrupt 4
{
	if(RI)
	{
		RI = 0;
		RX0_Buffer[uart1_wr++] = SBUF;
		//if(++uart0_wr >= BUF_LENTH)	uart0_wr = 0;
		flag = 1;
	}

	if(TI)
	{
		TI = 0;
		B_TI = 1;
	}
}

void delay_ms(unsigned char ms)
{
    unsigned int i;
	do{
	    i = MAIN_Fosc /1400;
		while(--i);
	}while(--ms);
}