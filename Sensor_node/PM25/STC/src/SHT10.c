#include "config.h"
#include "SHT10.h"


uchar error;         //全局错误变量
uchar ack;           //全局应答变量
uchar temp_H;        //全局应答变量
uchar temp_L;        //全局应答变量
float temp_disp=0;   //温度显值
float humi_disp=0;   //湿度显值


/********************************************************
函数功能:从总线上读取字节，返回应答信号

参考资料:发出一组测量命令之后控制器要等待测量结束，SHT10
         通过下拉DATA至低电平并进入空闲模式，表示测量的结
		 束,MCU再次触发SCK钱必须等待这个数据备妥信号来读数
		 据。接着传输2字节的测量数据和1字节的CRC奇偶校验。
		 可以使用CRC数据的确认位来终止通讯，如果不使用校验
		 则在测量到LSB之后可以通过保持确认位ack高电平来终
		 止通讯。
***********************************************************/
void read(void) 
{ 
    uchar i,val=0;
    temp_L=0;
    temp_H=0;
  
    DATA1=1;                      //释放数据总线
	////////////////读取MSB，高字节的值///////////
    for (i=0x80;i>0;i/=2)        //位移8位
    { 
        SCK1=1;                   //上升沿读入
        if (DATA1) val=(val | i); //确定值  
        SCK1=0;              
    }

    DATA1=0;                      //读应答信号，有应答为1，为应答为0 通过CPU下拉为应答
    SCK1=1;                       //第9个脉冲
    _nop_(); _nop_(); _nop_();   //SCK线高电平持续时间最低0.1us 
    SCK1=0;                 
    DATA1=1;                      //释放数据总线
    temp_H=val;			 		 //读取MSB的值
    val=0;

    ////////////////读取LSB，低字节的值////////////
    DATA1=1;                      //释放数据总线
    for (i=0x80;i>0;i/=2)        //位移8位
    { 
        SCK1=1;                   //上升沿读入
        if (DATA1) val=(val | i); //确定值  
        SCK1=0;              
    }
    DATA1=1;                      //不需要应答 通过CPU下拉为应答
    SCK1=1;                       //第9个脉冲
    _nop_(); _nop_(); _nop_();   //高电平有效宽度0.5us,最低0.1us 
    SCK1=0;                 
    DATA1=1;                      //释放数据总线
    temp_L=val;					 //读取LSB的值
}



/**********************************************************
函数功能:向总线写字节，返回应答信号
入口参数:value - 命令
返回值: ack - 应答信号，1表示无应答

参考资料:启动传输之后开始发送各种命令，包括三个默认为000的
         地址位和5个命令位。在第8个SCK的下降沿之后将DATA下
		 拉为低电平，则表示SHTXX已经接收到正确命令，在第9
		 个SCK下降沿之后释放DATA
***********************************************************/
char write_SHT(uchar value) 
{ 
    uchar i ;
    ack=0;  
    for (i=0x80;i>0;i/=2)             //释放数据总线
    { 
        if (i & value) DATA1=1;        //写入值
        else DATA1=0;                        
        SCK1=1;                        //上升沿写入
        _nop_(); _nop_(); _nop_();       
        SCK1=0;
    }
    DATA1=1;                           //释放数据总线
    SCK1=1;                            //第9个脉冲
    if(DATA1) ack=1;                   //读应答信号                   
    SCK1=0; 
	DATA1=1;                           //释放数据总线       
    return ack;                       //ack=1表示没有应答
}




/**************************************************
函数功能：传输启动

参考资料:当SCK高电平的时候DATA可以翻转，紧接着CK变
         为低电平，随后是SCK为高电平时DATA翻转为高
		 电平
**************************************************/
void trans_start(void) 
{  
    DATA1=1; 
	SCK1=0;                         //数据为1，SCK=0
    _nop_();
    SCK1=1;                         //第一个脉冲
    _nop_();
    DATA1=0;                        //数据跌落
    _nop_ ();
    SCK1=0;                         //完成一个脉冲
    _nop_(); 
	_nop_(); 
	_nop_();
    SCK1=1;                         //再一个脉冲
    _nop_();
    DATA1=1;                        //数据变为1         
    _nop_();
    SCK1=0;                         //完成该脉冲        
}


/*************************************************
函数功能：通讯复位

参考资料:当DATA保持高电平的时候，SCK触发9次或更多
         然后在下一个指令前发送一个传输启动时序能
		 够复位串口
*************************************************/
void connect_reset(void)  
{  
    uchar i; 
    DATA1=1; 
	SCK1=0;                //数据为1 时钟为0
    for(i=0;i<9;i++)      //9 个脉冲为 复位
    { 
        SCK1=1;
        SCK1=0;
    }
    trans_start();        //传输启动          
}


/*****************************************
函数功能：测量温度或者是湿度，返回校验值
******************************************/
void measure(uchar cmd)
{ 
    uint i;
    trans_start();          //启动
    write_SHT(cmd);         //写入测温命令

    if(ack) 		        //应答失败
    {
        connect_reset();    //通讯复位数据串口
        write_SHT(cmd);     //重新写入测温命令
    }
    
	for(i=0;i<55535;i++)
	{ 
		if(!DATA1) break;	//若已经转换完成则DATA线拉为低电平
		else  
		{
		    ;           
		} 
	}
    read();                //读温度
}    


/*********************************
函数功能：计算温度
**********************************/
void get_temp(void)
{
    float a=0,temp_zi;

	

    a=(float)temp_H*256+(float)temp_L; 	
    temp_zi=D2_14bit*a+D1_5v;		            //数字输出转换为温度值 T=d1+d2*SOt

    if(temp_zi<MIN_TEMP)  temp_zi=MIN_TEMP;		//限位，SHT10量程-40°C  ~ 123.8℃，零下温度不测
	if(temp_zi>MAX_TEMP)  temp_zi=MAX_TEMP;

    temp_disp=temp_zi;
	temp_disp=(int)temp_disp;
}



/*********************************
函数功能：计算湿度
**********************************/
void get_humi(void)
{
    float a=0,b=0,humi_zi;
    a=(float)temp_H*256+(float)temp_L; 
	humi_zi=C1+C2_12bit*a+a*a*C3_12bit;	   //修正输出湿度值
    humi_disp=humi_zi;
	humi_disp=(int)humi_disp;
}



/*********************************
函数功能：温湿度处理函数
**********************************/
void deal_whole(void)
{
    error=0;
    ack=0;

	P1M0 &= 0;
	//P2M0 &= 0;

    connect_reset() ;            //复位
    measure(MEASURE_TEMP);       //测量温度

    get_temp(); 				 //获取温度数值
    measure(MEASURE_HUMI);       //测量湿度   

    get_humi();                  //获得湿度数据

	//Uart1_TxByte(temp_disp);
	//Uart1_TxByte(humi_disp); 

	P1M0 |= 0x11;
	//P2M0 |= 0x70;
}
