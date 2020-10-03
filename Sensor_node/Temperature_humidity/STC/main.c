#include <STC12C5A60S2.h>
#include <intrins.h>
#include <stdio.h>
#include <stdlib.h>
/**********************************************************/
//晶振频率：11.0592MHz
//文件名  ：Main.c
//功能说明：温湿度传感器AM2321读取实验
//制作    ：www.frotech.com
//技术支持：020-22883196 QQ:
//变更记录: 2013.05.02
//变更内容: 新建造
/**********************************************************/

#define	 BUF_LENTH	128		//定义串口接收缓冲长度
unsigned char 	uart1_wr;		//写指针
unsigned char 	uart1_rd;		//读指针
unsigned char 	xdata RX0_Buffer[BUF_LENTH];	//定义串口接收缓冲
unsigned char flag;	//定义串口中断接收标号，即当其为1时表示串口介绍到数据。
unsigned char i;    //定义普通变量。

int Sensor_Data[5]={0x00,0x00,0x00,0x00,0x00};//定义温湿度传感器数据存放区。
unsigned char Sensor_Check;		  //温湿度传感器校验和，判断读取的温湿度数据是否正确。
unsigned char Sensor_AnswerFlag;  //温湿度传感器收到起始标志位
unsigned char Sensor_ErrorFlag;   //读取传感器错误标志
unsigned char Ascii_buffer[10] = {'0','1','2','3','4','5','6','7','8','9'};
unsigned char   xdata mbus_buffer[255];
unsigned char   xdata mbus_Sendbuf[255];
unsigned char   xdata Crc_buf[2];  //声明存储CRC校验值的高8位及低8位的缓存
unsigned int  Crc_return_data;  //声明CRC校验值


unsigned int  Sys_CNT;
unsigned int  Tmp;

bit		B_TI;
sbit Sensor_SDA = P1^1;//定义P1.1为AM2321的数据口

/********************函数声明区**************************************/
void uart1_init(void); //声明串口初始化函数
void Uart1_TxByte(unsigned char dat); //声明串口发送单字节函数
void Uart1_String(unsigned char code *puts);//声明串口发送字符串函数
void delay_ms(unsigned char ms);//声明普通延时函数
void Clear_Data (void);	//声明AM2321的数据清除函数
void delay_3ms(void);	//声明延时3ms函数
void delay_30us(void);	//声明延时30us函数
unsigned char Read_SensorData(void);//声明读取AM2321数据函数
unsigned char Read_Sensor(void);//声明读取AM2321温湿度数据函数
unsigned int cal_crc(unsigned char *snd, unsigned char num);
void newline();

/**********************************************************/

/*****用户定义参数 ，声明串口参数为9600 8 N 1  ***********/
#define MAIN_Fosc		11059200UL	
#define Baudrate0		115200UL											
/**********************************************************/


/****************** 编译器自动生成，用户请勿修改 ***********************/

#define BRT_Reload			(256 - MAIN_Fosc / 16 / Baudrate0)

/**********************************************************/
//int get_sensor_data[3]={0xff,0xff,0xff};
   /*
typedef struct {
	char DataType;	//数据类型标志
	int temp;		//温度
	int humi;		//湿度
}AM2321_Data;
	 */
//AM2321_Data get_sensor_data;


//**********************************************************************
//函数名：main(void)
//输入  ：无
//输出  ：无
//功能描述：实现STC12C5A16S2单片机的P1.1口读取AM2321的温湿度数据，并按照
//          ASCII格式输出到串口界面中
//          按照"H:xx.x%RH,T:xx.x”格式每隔约2秒显示一次。
//*************************************************************************
int main(void)
{
	int temp, humi;
	char tem[4],hum[4];
	//get_sensor_data.DataType = 1;//1:温湿度
	
	Sensor_SDA = 1;//SDA数据线由上拉电阻拉高一直保持高电平，初始化数据总线。
	uart1_init();  //初始化串口，参数为9600 8 N 1.
	 
	while(1) //死循环开始
	{
		//Uart1_String(b); //串口打印输出测试字符串。
		delay_ms(300); //延时
	    if(Read_Sensor())		  // 读取传感器数据
		{		
			if(Sensor_Data[0]+Sensor_Data[1]+Sensor_Data[2]+Sensor_Data[3]==Sensor_Data[4])
			{	
				
				for(i=0; i<4; i++)
				{
				 	Uart1_TxByte(Sensor_Data[i]);
				} 		
				delay_ms(1000);
				/*
				//串口显示字符数据
				// 湿度 
				humi |= Sensor_Data[0];
				humi <<= 8;
				humi |= Sensor_Data[1];
				get_sensor_data.humi = humi;
				//Uart1_TxByte(humi);
				//delay_ms(1000);
				/*
				hum[0] = (humi/100)+'0';
				hum[1] = ((humi%100)/10)+'0';
				hum[2] = (humi%10)+'0';
				humi &= 0;
				Uart1_String("Humi:");
				Uart1_TxByte(hum[0]);
				Uart1_TxByte(hum[1]);
				Uart1_TxByte('.');
				Uart1_TxByte(hum[2]);
				Uart1_TxByte('%');
				Uart1_TxByte(' ');
				*/
				 /*
				// 温度  
				temp |= Sensor_Data[2];
				temp <<= 8;
				temp |= Sensor_Data[3];
				//get_sensor_data.temp = temp;

				//Uart1_String((char *)&get_sensor_data);
			
				delay_ms(1000);
				  */
				 /*
				tem[0] = (temp/100)+'0';
				tem[1] = ((temp%100)/10)+'0';
				tem[2] = (temp%10)+'0';
				temp &= 0;
				Uart1_String("Temp:");
				Uart1_TxByte(tem[0]);
				Uart1_TxByte(tem[1]);
				Uart1_TxByte('.');
				Uart1_TxByte(tem[2]);
				Uart1_TxByte('c');
				Uart1_TxByte(' ');
				//newline();	
			 	*/  
			}			
		}
	
	}
	return 0;
}



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

void Uart1_TxByte(unsigned char dat)
{
    B_TI = 0;
	SBUF = dat;
	while(!B_TI);
	B_TI = 0;
}

void Uart1_String(unsigned char code *puts)
{
    for(; *puts != 0; puts++)
	{
        Uart1_TxByte(*puts);

	}
}


/**********************************************/
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
//函数名：delay_3ms(void)
//输入  ：无
//输出  ：无
//功能描述：延时3ms函数
//*************************************************************************
void delay_3ms(void)   
{
    unsigned char a,b;
    for(b=194;b>0;b--)
        for(a=84;a>0;a--);
}

//**********************************************************************
//函数名：delay_30us(void)
//输入  ：无
//输出  ：无
//功能描述：延时3ms函数
//*************************************************************************
void delay_30us(void)   
{
    unsigned char a;
    for(a=164;a>0;a--);
}

void delay_ms(unsigned char ms)
{
    unsigned int i;
	do{
	    i = MAIN_Fosc /1400;
		while(--i);
	}while(--ms);
}

//**********************************************************************
//函数名：Read_SensorData(void)
//输入  ：无
//输出  ：单字节的温湿度数据或者校验值
//功能描述：读取单字节的AM2321的数据
//*************************************************************************
unsigned char Read_SensorData(void)
  {
	unsigned char i,cnt;
	unsigned char buffer,tmp;
	buffer = 0;
	for(i=0;i<8;i++)
	{
		cnt=0;
		while(!Sensor_SDA)	//检测上次低电平是否结束
		{
		  if(++cnt >= 290)
		   {
			  break;
		   }
		}
		//延时Min=26us Max50us 跳过数据"0" 的高电平
		delay_30us();//延时30us 
		tmp =0;
		if(Sensor_SDA)//延时30us后如果数据口还是高，则该位为1，否则为0，见P19	 
		{
		  tmp = 1;
		}  
		cnt =0;
		while(Sensor_SDA)		//等待高电平 结束
		{
		   	if(++cnt >= 200)
			{
			  break;
			}
		}
		buffer <<=1; //移位，使得数据的最低位准备接收下一位
		buffer |= tmp;//把本次接收到的位加入到数据中	
	}
	return buffer;//返回单字节数据
  }

//**********************************************************************
//函数名：Read_Sensor(void)
//输入  ：无
//输出  ：无
//功能描述：读取AM2321的温湿度及校验值放在Sensor_Data[]中。
//*************************************************************************
unsigned char Read_Sensor(void)
{
	unsigned char i;

    Sensor_SDA = 0;//起始信号拉低，见AM2321数据手册P18
	delay_3ms();  //延时3Ms，当然一般1ms就可以了。
	Sensor_SDA = 1; //拉高，释放总线
	delay_30us(); //延时30us
	Sensor_AnswerFlag = 0;  // 传感器响应标志	 
	if(Sensor_SDA ==0)//从高电平到低电平经过30us(大于20us)是否为低
	{  
	   //如果为低，那么传感器发出响应信号
	   Sensor_AnswerFlag = 1;//收到起始信号
	   Sys_CNT = 0;//判断从机是否发出 80us 的低电平响应信号是否结束	 
	   while((!Sensor_SDA))	//等待传感器响应信号80us的低电平结束
	   {
	     if(++Sys_CNT>300) //防止进入死循环
		 {
		   Sensor_ErrorFlag = 1;
		   return 0;
		 } 
	   }
	    Sys_CNT = 0;
	    //判断从机是否发出 80us 的高电平，如发出则进入数据接收状态
	    while((Sensor_SDA))//等待传感器响应信号80us的高电平结束
	    {
	       if(++Sys_CNT>300) //防止进入死循环
		   {
		     Sensor_ErrorFlag = 1;
		     return 0;
		   } 
	    } 		 
	    // 数据接收	传感器共发送40位数据 
	    // 即5个字节 高位先送  5个字节分别为湿度高位 湿度低位 温度高位 温度低位 校验和
	    // 校验和为：湿度高位+湿度低位+温度高位+温度低位
	    for(i=0;i<5;i++)
	    {
	      Sensor_Data[i] = Read_SensorData();
	    }
	   
	  }
	  else
	  {
	    Sensor_AnswerFlag = 0;	  // 未收到传感器响应	
	  }
	  return 1;
  }    


