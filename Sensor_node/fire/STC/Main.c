/**********************************************************/
//����Ƶ�ʣ�11.0592MHz
//�ļ���  ��Main.c
//����˵�������洫������ȡʵ��
//����    ��www.frotech.com
//����֧�֣�020-22883196 QQ:
//�����¼: 2013.05.02
//�������: �½���
/**********************************************************/

#include <STC12C5A60S2.h>


#define		BUF_LENTH	128		//���崮�ڽ��ջ��峤��
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
#define HAVEFIRE 	'd' 	 //�л���
#define NOFIRE	    'e' 	 //�޻���

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
//�������������л�����ߵ�ֱ����洫����ʱ��D1��,���Ͳ�ѯָ��᷵���л�������
//          P1.0����׼˫��ڹ���ģʽ
//**********************************************************************
void main(void)
{
	uart1_init();//��ʼ������
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
//��������uart1_init(void)
//����  ����
//���  ����
//�������������ڳ�ʼ��������ͨ�Ų���Ϊ9600 8 N 1
//**********************************************************************
void	uart1_init(void)
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