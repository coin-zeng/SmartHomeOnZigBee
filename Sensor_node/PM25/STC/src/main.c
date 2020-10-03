#include "config.h"

uchar 	uart1_wr;		//写指针
uchar 	uart1_rd;		//读指针
uchar 	xdata RX1_Buffer[BUF_LENTH];	//接收缓冲
uchar   xdata mbus_buffer[255];
uchar   xdata mbus_Sendbuf[255];
uchar   xdata Crc_buf[2];  //声明存储CRC校验值的高8位及低8位的缓存

unsigned char xdata led_buffer[4];


uint  Crc_return_data;  //声明CRC校验值


uchar devid;
uchar leng =0;
uchar leng_w = 0;
uchar flag;
uchar flag1 = 1;
uchar flag2;
uchar flag3;
unsigned int i, j;
uint T0_flag = 0;
extern unsigned char xdata Sensor_Data[5];
extern unsigned char xdata BUF[8];  //接收数据缓存区

extern float temp_disp;   //温度显值
extern float humi_disp;   //湿度显值
extern uchar temp_H;        //全局应答变量
extern uchar temp_L;        //全局应答变量

unsigned int 	xdata ADH_Buffer[55];
unsigned int 	xdata ADL_Buffer[1000];
unsigned int    xdata AD_data[10];
unsigned int PM25;
unsigned char k = 0;
unsigned char code F_Rotation[4]={0x02,0x04,0x08,0x10};//正转表格
unsigned char code B_Rotation[4]={0x10,0x08,0x04,0x02};//反转表格
unsigned char temp1,temp2,temp3 ;
	
bit		B_TI;


void Read_Sense(void);


void Time0_init(void);
int PM25DATA(void);

void Delay(unsigned int i);
void Delay10ms(); 
void Delay30usA();
void Delay280us();

//#define PMFLAG	0x11   //PM2.5标志

int main(void)
{
	char value[5];
    int PM25_value = 0;
	//Sensor_SDA = 1;
	uart1_init();
	ADC_CONTR = ADC_360T | ADC_ON;
	
	while(1) 
	{
		//delay_ms(20);
		PM25_value = PM25DATA();
		
		Uart1_TxByte(PM25_value);
		//Sensor_SDA = 1;
		//Uart1_TxByte(PMFLAG);
		
		
		//串口字符显示
		//Uart1_String("PM2.5=");
		//value[0] = PM25_value/100+'0';
		//value[1] = (PM25_value%100)/10+'0';
		//value[2] = (PM25_value%10)+'0';
		//Uart1_TxByte(value[0]);
		//Uart1_TxByte('P');	
		//Uart1_TxByte(value[1]);
		//Uart1_TxByte(value[2]);
		
		
	}

	return 0;
}

//**********************************************************************
//函数名：void PM25DATA(void)
//输入  ：无
//输出  ：无
//功能描述：采集PM2.5传感器的平均值
//**********************************************************************
int PM25DATA(void)
{
	 int PM25_value;
     int PM25_avg;
	 for(i = 0; i < 300; i++)
	 {
	     PM25LED = 0;  
	     Delay280us();
	     ADL_Buffer[i] = adc10_start(1);	//(P1.1)ADC1转换
	     Delay30usA();
	     PM25LED = 1;
		 Delay10ms();
	 }
	 PM25 = 0;
	 for(i = 0; i <300; i++)
	 {
	     PM25 = PM25 + ADL_Buffer[i]; 
	 }
	 PM25_avg = (int)(PM25 / 300);
	 PM25_value = PM25_avg - 120;
	 return  PM25_value;

}




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
		RX1_Buffer[uart1_wr++] = SBUF;
		flag = 1;
	}

	if(TI)
	{
		TI = 0;
		B_TI = 1;
	}
}

void Time0_init(void)
{
    AUXR = 0x00;
	TMOD = 0x01;
	TL0  = 0x67;
	TH0  = 0xFC;		
	TR0  = 1;
	//ET0  = 1;
	EA   = 1;
}

void delay_ms(unsigned char ms)
{
    unsigned int i;
	do{
	    i = MAIN_Fosc /1400;
		while(--i);
	}while(--ms);
}

void Delay(unsigned int i)//延时
{
 while(--i);              
}

void Delay280us()		//@11.0592MHz
{
	unsigned char i, j;

	_nop_();
	i = 3;
	j = 0;
	do
	{
		while (--j);
	} while (--i);
}

void Delay30usA()		//@11.0592MHz
{
	unsigned char i;

	i = 80;
	while (--i);
}

void Delay10ms()		//@11.0592MHz
{
	unsigned char i, j;

	_nop_();
	_nop_();
	i = 108;
	j = 143;
	do
	{
		while (--j);
	} while (--i);
}




