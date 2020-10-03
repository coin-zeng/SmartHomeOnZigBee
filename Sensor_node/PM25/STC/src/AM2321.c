#include "config.h"
#include "AM2321.h"


uchar xdata Sensor_Data[5]={0x00,0x00,0x00,0x00,0x00};
uchar Sensor_Check;		  //校验和
uchar Sensor_AnswerFlag;  //收到起始标志位
uchar Sensor_ErrorFlag;   //读取传感器错误标志
uint  Sys_CNT;


void delay_3ms(void)   
{
    unsigned char a,b;
    for(b=194;b>0;b--)
        for(a=84;a>0;a--);
}
void delay_30us(void)   
{
    unsigned char a;
    for(a=164;a>0;a--);
}

void Clear_Data (void)
  {
	int i;
	for(i=0;i<5;i++)
	{
	   Sensor_Data[i] = 0x00;
	 }//接收数据清零
  }


/********************************************\
|* 功能： 读传感器发送的单个字节	        *|
\********************************************/
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
		delay_30us(); 
		//delay_30us();
		//判断传感器发送数据位
		tmp =0;
		if(Sensor_SDA)	 
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
		buffer <<=1;
		buffer |= tmp;	
	}
	return buffer;
  }

/********************************************\
|* 功能： 读传感器              	        *|
\********************************************/
unsigned char Read_Sensor(void)
  {
	unsigned char i;
	 //Sensor_SDA = 1;
	 //Delay_N1ms(100);  //延时2Ms
	//主机拉低(Min=800US Max=20Ms) 
    Sensor_SDA = 0;
	
	delay_3ms();  //延时2Ms
	
	//释放总线 延时(Min=30us Max=50us)
	Sensor_SDA = 1; 
	
	delay_30us();
	//Sensor_SDA = 1; 
    //Delay_N10us(2);//延时30us	  
	Sensor_AnswerFlag = 0;  // 传感器响应标志	 
	//判断从机是否有低电平响应信号 如不响应则跳出，响应则向下运行
    //Delay_N1ms(1);  //延时2Ms
	
	 //Sensor_SCL = 0;
	if(Sensor_SDA ==0)
	{  
	   
	   Sensor_AnswerFlag = 1;//收到起始信号
	   Sys_CNT = 0;
	   //判断从机是否发出 80us 的低电平响应信号是否结束	 
	   while((!Sensor_SDA))
	   {
	     if(++Sys_CNT>300) //防止进入死循环
		 {
		   Sensor_ErrorFlag = 1;
		   return 0;
		  } 
	    }
	    Sys_CNT = 0;
	    //判断从机是否发出 80us 的高电平，如发出则进入数据接收状态
	    while((Sensor_SDA))
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
	   	Sensor_Check = Sensor_Data[0] + Sensor_Data[1] + Sensor_Data[2] + Sensor_Data[3];
		if(Sensor_Check != Sensor_Data[4])
		{
		    //Clear_Data ();
			;
		}
	  }
	  else
	  {
	    Sensor_AnswerFlag = 0;	  // 未收到传感器响应	
	  }
	  return 1;
  }    
