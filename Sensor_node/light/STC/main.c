/**********************************************************/
//����Ƶ�ʣ�11.0592MHz
//�ļ���  ��Main.c
//����˵����������������ȡʵ��
//����    ��www.frotech.com
//����֧�֣�020-22883196 QQ:
//�����¼: 2013.05.02
//�������: �½���
/**********************************************************/

#include <STC12C5A60S2.h>


#define		BUF_LENTH	128		//���崮�ڽ��ջ��峤��
#define     uint unsigned int
#define     uchar unsigned char
unsigned char 	uart1_wr;		//дָ��
unsigned char 	uart1_rd;		//��ָ��
unsigned char 	xdata RX0_Buffer[BUF_LENTH];	//���ջ���
unsigned char flag;
unsigned char i;
unsigned char   xdata mbus_buffer[255];
unsigned char   xdata mbus_Sendbuf[255];
unsigned char   xdata Crc_buf[2];  //�����洢CRCУ��ֵ�ĸ�8λ����8λ�Ļ���
unsigned int  Crc_return_data;  //����CRCУ��ֵ
bit		B_TI; //������ɱ�־
sbit  P1_0 = P1^0;//����P1.0�˿�
//								7       6      5       4         3      2    1    0   Reset Value
//sfr ADC_CONTR = 0xBC;		ADC_POWER SPEED1 SPEED0 ADC_FLAG ADC_START CHS2 CHS1 CHS0 0000,0000	//AD ת�����ƼĴ��� 
#define ADC_OFF()	ADC_CONTR = 0
#define ADC_ON		(1 << 7)
#define ADC_90T		(3 << 5)
#define ADC_180T	(2 << 5)
#define ADC_360T	(1 << 5)
#define ADC_540T	0
#define ADC_FLAG	(1 << 4)	//�����0
#define ADC_START	(1 << 3)	//�Զ���0
#define ADC_CH0		0
#define ADC_CH1		1
#define ADC_CH2		2
#define ADC_CH3		3
#define ADC_CH4		4
#define ADC_CH5		5
#define ADC_CH6		6
#define ADC_CH7		7


#define HAVELIGHT	'2' 	 //�й�
#define NOLIGHT		'1' 	 //�޹�

uint adc10_start(uchar channel);
void	uart1_init(void);
void Uart1_TxByte(unsigned char dat);
void Uart1_String(unsigned char code *puts);
void delay_ms(unsigned char ms);
 unsigned int cal_crc(unsigned char *snd, unsigned char num);
/*************** �û�������� *****************************/

#define MAIN_Fosc		11059200UL	
#define Baudrate0		115200UL		
									
/**********************************************************/


/****************** �������Զ����ɣ��û������޸� ************************************/

#define BRT_Reload			(256 - MAIN_Fosc / 16 / Baudrate0)		//Calculate the timer1 reload value ar 1T mode


/**********************************************************/

//**********************************************************************
//��������main(void)
//����  ����
//���  ����
//�����������������嵲ס�����������ϵĹ�������ʱ��ADC1(P1.1)�ĵ�ѹ��С
//          �����ڵ�ʱ��ADC1��ֵ���
            
//**********************************************************************
void main(void)
{
	uint	j;
	uart1_init();//��ʼ������
	P1ASF = (1 << ADC_CH1);	//STC12C5A16S2ϵ��ģ������(AD)ѡ��ADC1(P1.1)
	ADC_CONTR = ADC_360T | ADC_ON;	
	while(1)
	{
		delay_ms(50);
		j = adc10_start(1);	//(P1.1)ADC1ת��
		
		if(j > 0xB0)	//�й�ش���
		{
			Uart1_TxByte(HAVELIGHT);	
		}
		if(j < 0xB0)   //�޹⿪����
		{
			Uart1_TxByte(NOLIGHT);	
		} 	
		//Uart1_TxByte(j);
		
	}
}
//**********************************************************************
//��������adc10_start(uchar channel)
//����  ��ADCת����ͨ��
//���  ��ADCֵ
//����������ADCת��
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
//��������uart1_init(void)
//����  ����
//���  ����
//�������������ڳ�ʼ��������ͨ�Ų���Ϊ9600 8 N 1
//**********************************************************************
void uart1_init(void)
{
	PCON |= 0x80;		//UART0 Double Rate Enable
	SCON = 0x50;		//UART0 set as 10bit , UART0 RX enable
	AUXR |=  0x01;		//UART0 ʹ��BRT
	AUXR |=  0x04;		//BRT set as 1T mode
	BRT = BRT_Reload;
	AUXR |=  0x10;		//start BRT	

	ES  = 1;
	EA = 1;
}
//**********************************************************************
//��������Uart1_TxByte(unsigned char dat)
//����  ����Ҫ���͵��ֽ�����
//���  ����
//�����������Ӵ��ڷ��͵��ֽ�����
//**********************************************************************
void Uart1_TxByte(unsigned char dat)
{
    B_TI = 0;
	SBUF = dat;
	while(!B_TI);
	B_TI = 0;
}
/*****************************CRCУ����㺯��********************************/
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
//��������Uart1_String(unsigned char code *puts)
//����  ���ַ����׵�ַ
//���  ����
//�����������Ӵ��ڷ����ַ���
//**********************************************************************
void Uart1_String(unsigned char code *puts)
{
    for(; *puts != 0; puts++)
	{
        Uart1_TxByte(*puts);

	}
}

//**********************************************************************
//��������UART1_RCV (void)
//����  ����
//���  ����
//���������������жϽ��պ���
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