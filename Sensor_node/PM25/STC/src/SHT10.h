#ifndef _SHT10_H
    #define _SHT10_H 



////////////////SHTXX 命令集//////////////////
#define MEASURE_TEMP 0x03      //温度测量命令
#define MEASURE_HUMI 0x05      //湿度测量命令
#define READ_STATUS_REG 0x07   //读状态寄存器命令
#define WRITE_STATUS_REG 0x06  //写状态寄存器命令
#define SOFT_RESET  0x1e	   //软复位命令

/////////////////////////湿度修正时的相关参数///////////////////
///RH_linear = C1 + C2 + C3 * SO * SO,其中SO是传感器输出数值////
#define C1       -4.0				   
#define C2_12bit 0.0405		   
#define C2_8bit  0.648		  
#define C3_12bit -2.8/1000000.0		 
#define C3_8bit  -7.2/1000000.0

////////实测温度和25°相差太大时的湿度传感器的温度修正系数////////
/////RH_true = (T_c - 25)*(T1 + T2*SO)+RH_linear,T_c 环境温度////
#define T1       0.01
#define T2_12bit 0.00008
#define T2_8bit  0.00128

//////////////////将数字输出转换为温度值所需参数///////////////
/////////////Temp = D1 + D2*SO,SO - 传感器数字输出/////////////
#define D1_5v   -40.00
#define D1_4v   -39.75
#define D1_3v   -39.60
#define D2_14bit 0.01
#define D2_12bit 0.04

#define MIN_TEMP 0.00	   //允许测量的最低温度（实际可测到 -40℃）
#define MAX_TEMP 123.8	   //允许测量的最高温度（实际可测到 123.8℃）



//sbit DATA1 =P1^0;     //数据线
//sbit SCK1  =P2^6;     //时钟线
sbit DATA1 =P1^1;     //数据线
sbit SCK1  =P1^4;     //时钟线



void read(void);
char write_SHT(uchar value) ;
void trans_start(void) ;
void connect_reset(void);
void measure(uchar cmd);
void get_temp(void);
void get_humi(void);
void deal_whole(void);




#endif