/**********************************************************/
//晶振频率：11.0592MHz
//文件名  ：Main.c
//功能说明：光敏传感器读取实验
//制作    ：www.frotech.com
//技术支持：020-22883196 QQ:
//变更记录: 2013.05.02
//变更内容: 新建造
/**********************************************************/

#include <STC12C5A60S2.h>


#define		BUF_LENTH	128		//定义串口接收缓冲长度
#define     uint unsigned int
#define     uchar unsigned char
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
//								7       6      5       4         3      2    1    0   Reset Value
//sfr ADC_CONTR = 0xBC;		ADC_POWER SPEED1 SPEED0 ADC_FLAG ADC_START CHS2 CHS1 CHS0 0000,0000	//AD 转换控制寄存器 
#define ADC_OFF()	ADC_CONTR = 0
#define ADC_ON		(1 << 7)
#define ADC_90T		(3 << 5)
#define ADC_180T	(2 << 5)
#define ADC_360T	(1 << 5)
#define ADC_540T	0
#define ADC_FLAG	(1 << 4)	//软件清0
#define ADC_START	(1 << 3)	//自动清0
#define ADC_CH0		0
#define ADC_CH1		1
#define ADC_CH2		2
#define ADC_CH3		3
#define ADC_CH4		4
#define ADC_CH5		5
#define ADC_CH6		6
#define ADC_CH7		7


#define HAVELIGHT	'2' 	 //有光
#define NOLIGHT		'1' 	 //无光

uint adc10_start(uchar channel);
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
//功能描述：当有物体挡住光敏传感器上的光敏电阻时，ADC1(P1.1)的电压变小
//          当无遮挡时，ADC1的值变大
            
//**********************************************************************
void main(void)
{
	uint	j;
	uart1_init();//初始化串口
	P1ASF = (1 << ADC_CH1);	//STC12C5A16S2系列模拟输入(AD)选择ADC1(P1.1)
	ADC_CONTR = ADC_360T | ADC_ON;	
	while(1)
	{
		delay_ms(50);
		j = adc10_start(1);	//(P1.1)ADC1转换
		
		if(j > 0xB0)	//有光关窗帘
		{
			Uart1_TxByte(HAVELIGHT);	
		}
		if(j < 0xB0)   //无光开窗帘
		{
			Uart1_TxByte(NOLIGHT);	
		} 	
		//Uart1_TxByte(j);
		
	}
}
//**********************************************************************
//函数名：adc10_start(uchar channel)
//输入  ：ADC转换的通道
//输出  ：ADC值
//功能描述：ADC转换
//**********************************************************************
uint	adc10_start(uchar channel)	//channel = 0~7
{
	uint	adc;
	uchar	i;

	ADC_RES = 0;
	ADC_RESL = 0;

	ADC_CONTR = (ADC_CONTR & 0xe0) | ADC_START | channel; 
	i = 250;
	do{
		if(ADC_CONTR & ADC_FLAG)
		{
			ADC_CONTR &= ~ADC_FLAG;
			adc = (uint)ADC_RES;
			adc = (adc << 2) | (ADC_RESL & 3);
			return	adc;
		}
	}while(--i);
	return	1024;
}

//**********************************************************************
//函数名：uart1_init(void)
//输入  ：无
//输出  ：无
//功能描述：串口初始化函数，通信参数为9600 8 N 1
//**********************************************************************
void uart1_init(void)
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