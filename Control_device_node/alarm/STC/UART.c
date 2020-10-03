
/**************************************************************

*  晶振频率    ：选取用RC 11.0592MHz
*  文件名      ：STC12C5A16S2_UART.c
*  功能说明    ：STC12C5A16S2单片机的串口通信演示实验
*  制作        ：www.frotech.com
*  变更记录    ：2013.04.27
*  变更内容    ：新建造
**************************************************************/

#include <STC12C5A60S2.h>
#include <string.h>

#define		BUF_LENTH	128		                //定义串口接收缓冲长度
unsigned char 	uart1_wr=0;		                //写计数器
unsigned char 	uart1_rd;		                //读计数器
unsigned char 	xdata RX0_Buffer[BUF_LENTH];	//接收缓冲
unsigned char flag;							    //定义串口是否有接收到数据的标志位
unsigned char i;							    //普通的循环变量
bit		B_TI;								    //定义B_TI为bit类型
unsigned char length;
unsigned char    xdata Crc_buf[2];  //声明存储CRC校验值的高8位及低8位的缓存

unsigned int   Crc_return_data;  //声明CRC校验值
void	uart1_init(void);					    //串口初始化函数声明
void Uart1_TxByte(unsigned char dat);			//串口发送单个字符函数声明
void Uart1_String(unsigned char code *puts);	//串口发送字符串函数函数声明
void delay_ms(unsigned char ms);				//延时函数声明
unsigned int cal_crc(unsigned char *snd, unsigned char num);

#define MAIN_Fosc		11059200UL			    //定义外部晶振的频率
#define Baudrate0		115200UL				    //定义串口波特率频率									
#define BRT_Reload	   (256 - MAIN_Fosc / 16 / Baudrate0)	//装入定时器1工作在1倍模式下的益出数
sbit shuchu	=P4^0;
/*************************************************************
*  函数名：main( void )
*  输入：无
*  输出：无
*  功能描述：串口初始化参数为9600 8 N 1.
             程序一开始执行输出"www.frotech.com"，然后将
			 串口接收到的数据原样返回
**************************************************************/
#define HAVEFIRE 		'd' 	 //有火焰
#define NOFIRE	    	'e' 	 //无火焰
#define HAVEKEY 		'b' 	 //有按键
#define NOKEY	   	    'c'	 	//无按键
#define OPENALARM       '9'     //开警报
#define CLOSEALARM      'a'     //关警报
char fire_flag=1;	//当检测到火焰时不能因为检测到没有应急按钮按下而关掉警报
void main(void)
{	
	uart1_init();	   	//调用串口初始化函数
	P4M1&=0xfe;
	P4M0 |= 0x01;
	shuchu = 0;	
								
	while(1)
	{
		if(flag == 1)									//是否有串口中断
		{	 
			flag = 0;
					
			if(RX0_Buffer[0]==NOFIRE )
			{
				fire_flag = 1;
				shuchu = 0;
			}
			
			if(RX0_Buffer[0]==HAVEFIRE)
			{
				fire_flag = 0;
				shuchu = 1;
			}
			 
			if(RX0_Buffer[0]==HAVEKEY)
			{					
				shuchu = 1;				
			}
			if(RX0_Buffer[0]==NOKEY)
			{
				if(fire_flag == 1)	
					shuchu = 0;
			}
			
			if(RX0_Buffer[0]==OPENALARM)
			{			
				shuchu = 1;				
			}
			if(RX0_Buffer[0]==CLOSEALARM)
			{			
				shuchu = 0;		
			}		  
			uart1_wr = 0;								//将写计数器0 
		}
	
	}
}


/*************************************************************
*  函数名：uart1_init(void)
*  输入：无
*  输出：无
*  功能描述：串口初始化参数为9600 8 N 1.
             
**************************************************************/
void uart1_init(void)							   
{
	PCON |= 0x80;									   //SMD=1    串行通讯的波特率加倍  P265
	SCON = 0x50;									   //当SM0/FE=0 SM1=1时组成工作方式1（8位UART,波特率可变）    P264
	AUXR |=  0x01;									   //S1BRS=1  使用独立波特率发生器作为串行口的波特率发生器    P266
	AUXR |=  0x04;									   //BRTx12=1 波特率发生器每一个时钟计数一次 
	BRT = BRT_Reload;							       //把根据波特率计算的 定时器的值，装入波特率发生器          P267 
	//BRT =0xB8;
	AUXR |=  0x10;		                               //BRTR=1   开启独立波特率发生器运行						  P266																											      
	ES  = 1;										   //IE寄存器的ES位为1 串口1中断开							  P192
	EA = 1;											   //IE寄存器的EA位为1 CPU开中断
}
//************************************************************
//函数名：cal_crc(unsigned char *snd, unsigned char num)
//输入：需要生成CRC校验码的字符串首地址，字符串的字节个数
//输出：16位的CRC校验码
//功能描述：生成CRC校验码        
//************************************************************ 
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
/*************************************************************
*  函数名：Uart1_TxByte(unsigned char dat)
*  输入：发送字符
*  输出：无
*  功能描述：将单个字符通过串口发送.
             
**************************************************************/
void Uart1_TxByte(unsigned char dat)				   //串口1发送单个字符函数
{
    B_TI = 0;										   //将B_TI置0
	SBUF = dat;										   //将发送的数据写入SBUF缓冲器中
	while(!B_TI);									   //等待发送 缓冲器发送完数据
	B_TI = 0;										   //将B_TI置位0
}
/*************************************************************
*  函数名：Uart1_String(unsigned char code *puts)
*  输入：发送字符串
*  输出：无
*  功能描述：将字符串通过调用单个字符函数发送.
             
**************************************************************/
void Uart1_String(unsigned char code *puts)			   
{
    for(; *puts != 0; puts++)
	{
        Uart1_TxByte(*puts);						   //以指针的形式将字符串分解为单个字符，调用上面的单个字符发送函数发送
	}
}


/*************************************************************
*  函数名：UART1_RCV (void)
*  输入：无
*  输出：无
*  功能描述：定义串口的中断处理函数，串口中断号为4.
             当串口接受完或者发送完一帧数据
			 时产生中断，告知CPU进行中断处理
**************************************************************/
void UART1_RCV (void) interrupt 4					  //中断接收函数 中断号为4    P188
{
	if(RI)											  //判断接收的数据是否接收完，当接收到第8位时 接收结束  RI会置1	  P265
	{
		RI = 0;										  //接收中断使能位清0
		RX0_Buffer[uart1_wr++] = SBUF;				  //将串口缓冲器SBUF的数据放到数据接收缓冲器RX0_Buffer[]中	
		flag = 1;										  //将标志位置1方便主函数判断
	}

	if(TI)											  //判断发送的数据是否发送完，当发送到第8位时 发送结束  TI会置1	   P265
	{
		TI = 0;										  //将发送结束标志位 TI清0
		B_TI = 1;									  //将B_TI 置1 方便上面的Uart1_TxByte 函数判断
	}
}
/*************************************************************
*  函数名：delay_ms(unsigned char ms)
*  输入：延时时间
*  输出：无
*  功能描述：产生毫秒级别延时.
             
**************************************************************/
void delay_ms(unsigned char ms)						 //毫秒级延时函数
{
    unsigned int i;
	do{
	    i = MAIN_Fosc /1400;
		while(--i);
	}while(--ms);
}