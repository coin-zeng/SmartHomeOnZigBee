#ifndef _SHT10_H
    #define _SHT10_H 



////////////////SHTXX ���//////////////////
#define MEASURE_TEMP 0x03      //�¶Ȳ�������
#define MEASURE_HUMI 0x05      //ʪ�Ȳ�������
#define READ_STATUS_REG 0x07   //��״̬�Ĵ�������
#define WRITE_STATUS_REG 0x06  //д״̬�Ĵ�������
#define SOFT_RESET  0x1e	   //��λ����

/////////////////////////ʪ������ʱ����ز���///////////////////
///RH_linear = C1 + C2 + C3 * SO * SO,����SO�Ǵ����������ֵ////
#define C1       -4.0				   
#define C2_12bit 0.0405		   
#define C2_8bit  0.648		  
#define C3_12bit -2.8/1000000.0		 
#define C3_8bit  -7.2/1000000.0

////////ʵ���¶Ⱥ�25�����̫��ʱ��ʪ�ȴ��������¶�����ϵ��////////
/////RH_true = (T_c - 25)*(T1 + T2*SO)+RH_linear,T_c �����¶�////
#define T1       0.01
#define T2_12bit 0.00008
#define T2_8bit  0.00128

//////////////////���������ת��Ϊ�¶�ֵ�������///////////////
/////////////Temp = D1 + D2*SO,SO - �������������/////////////
#define D1_5v   -40.00
#define D1_4v   -39.75
#define D1_3v   -39.60
#define D2_14bit 0.01
#define D2_12bit 0.04

#define MIN_TEMP 0.00	   //�������������¶ȣ�ʵ�ʿɲ⵽ -40�棩
#define MAX_TEMP 123.8	   //�������������¶ȣ�ʵ�ʿɲ⵽ 123.8�棩



//sbit DATA1 =P1^0;     //������
//sbit SCK1  =P2^6;     //ʱ����
sbit DATA1 =P1^1;     //������
sbit SCK1  =P1^4;     //ʱ����



void read(void);
char write_SHT(uchar value) ;
void trans_start(void) ;
void connect_reset(void);
void measure(uchar cmd);
void get_temp(void);
void get_humi(void);
void deal_whole(void);




#endif