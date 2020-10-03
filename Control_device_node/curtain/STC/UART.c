
/**************************************************************

*  ����Ƶ��    ��ѡȡ��RC 11.0592MHz
*  �ļ���      ��STC12C5A16S2_UART.c
*  ����˵��    ��STC12C5A16S2��Ƭ���Ĵ���ͨ����ʾʵ��
*  ����        ��www.frotech.com
*  �����¼    ��2013.04.27
*  �������    ���½���
**************************************************************/

#include <STC12C5A60S2.h>
#include <string.h>

#define		BUF_LENTH	128		                //���崮�ڽ��ջ��峤��
unsigned char 	uart1_wr;		                //д������
unsigned char 	uart1_rd;		                //��������
unsigned char 	xdata RX0_Buffer[BUF_LENTH];	//���ջ���
unsigned char flag;							    //���崮���Ƿ��н��յ����ݵı�־λ
unsigned char i,j;							    //��ͨ��ѭ������
bit		B_TI;								    //����B_TIΪbit����
unsigned char length;
unsigned char    xdata Crc_buf[2];  //�����洢CRCУ��ֵ�ĸ�8λ����8λ�Ļ���
unsigned char code F_Rotation[4]={0x02,0x04,0x08,0x10};//��ת���
unsigned char code B_Rotation[4]={0x10,0x08,0x04,0x02};//��ת���
unsigned int   Crc_return_data;  //����CRCУ��ֵ
unsigned char temp1,temp2,temp3 ;
void	uart1_init(void);					    //���ڳ�ʼ����������
void Uart1_TxByte(unsigned char dat);			//���ڷ��͵����ַ���������
void Uart1_String(unsigned char code *puts);	//���ڷ����ַ���������������
void delay_ms(unsigned char ms);				//��ʱ��������
unsigned int cal_crc(unsigned char *snd, unsigned char num);
void Delay(unsigned int i);
#define MAIN_Fosc		11059200UL			    //�����ⲿ�����Ƶ��
#define Baudrate0		115200UL				    //���崮�ڲ�����Ƶ��									
#define BRT_Reload	   (256 - MAIN_Fosc / 16 / Baudrate0)	//װ�붨ʱ��1������1��ģʽ�µ������
sbit shuchu	=P4^0;

/*************************************************************
*  ��������main( void )
*  ���룺��
*  �������
*  �������������ڳ�ʼ������Ϊ9600 8 N 1.
             ����һ��ʼִ�����"www.frotech.com"��Ȼ��
			 ���ڽ��յ�������ԭ������
**************************************************************/
#define HAVELIGHT	'2' 	 //�й�
#define NOLIGHT		'1' 	 //�޹�
void main(void)
{
	//char flag1 = 0;	//������ʼ״̬��־(1:�� 0:��) ��ʼ״̬Ϊ��
	uart1_init();									//���ô��ڳ�ʼ������
	while(1)
	{	
		if(flag == 1)
		{
			flag = 0;			
			if(RX0_Buffer[0]==NOLIGHT) //û�⿪����
			{	
				uart1_wr=0;
			//	if(flag1 == 0)//�������˲��ܿ�
			//	{
				//	flag1 = 1;
					for(j=0; j<200; j++)
					{
						for(i=0;i<4;i++)      //4��
		                {							      
			                P1=F_Rotation[i];  //�����Ӧ���� �������л��ɷ�ת���
			                Delay(3000);
		                }					
					}
					//Uart1_String("F ");	
				 //}											
			}			
			if(RX0_Buffer[0]==HAVELIGHT)//�й�ش���
			{
				uart1_wr=0;
				//if(flag1 == 1)//�������˲��ܹ�
				//{
					//flag1 = 0;
					for(j=0; j<200; j++)
					{
						for(i=0;i<4;i++)      //4��
		                {													      
			                P1=B_Rotation[i];  //�����Ӧ���� �������л��ɷ�ת���
			                Delay(3000);
		                }					
					}
					//Uart1_String("B ");	
				//}			
				
			} 
		
		
				
		}
	
	}
}


/*************************************************************
*  ��������uart1_init(void)
*  ���룺��
*  �������
*  �������������ڳ�ʼ������Ϊ9600 8 N 1.
             
**************************************************************/
void	uart1_init(void)							   
{
	PCON |= 0x80;									   //SMD=1    ����ͨѶ�Ĳ����ʼӱ�  P265
	SCON = 0x50;									   //��SM0/FE=0 SM1=1ʱ��ɹ�����ʽ1��8λUART,�����ʿɱ䣩    P264
	AUXR |=  0x01;									   //S1BRS=1  ʹ�ö��������ʷ�������Ϊ���пڵĲ����ʷ�����    P266
	AUXR |=  0x04;									   //BRTx12=1 �����ʷ�����ÿһ��ʱ�Ӽ���һ�� 
	BRT = BRT_Reload;							       //�Ѹ��ݲ����ʼ���� ��ʱ����ֵ��װ�벨���ʷ�����          P267 
	//BRT =0xB8;
	AUXR |=  0x10;		                               //BRTR=1   �������������ʷ���������						  P266																											      
	ES  = 1;										   //IE�Ĵ�����ESλΪ1 ����1�жϿ�							  P192
	EA = 1;											   //IE�Ĵ�����EAλΪ1 CPU���ж�
}
//************************************************************
//��������cal_crc(unsigned char *snd, unsigned char num)
//���룺��Ҫ����CRCУ������ַ����׵�ַ���ַ������ֽڸ���
//�����16λ��CRCУ����
//��������������CRCУ����        
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
*  ��������Uart1_TxByte(unsigned char dat)
*  ���룺�����ַ�
*  �������
*  �����������������ַ�ͨ�����ڷ���.
             
**************************************************************/
void Uart1_TxByte(unsigned char dat)				   //����1���͵����ַ�����
{
    B_TI = 0;										   //��B_TI��0
	SBUF = dat;										   //�����͵�����д��SBUF��������
	while(!B_TI);									   //�ȴ����� ����������������
	B_TI = 0;										   //��B_TI��λ0
}
/*************************************************************
*  ��������Uart1_String(unsigned char code *puts)
*  ���룺�����ַ���
*  �������
*  �������������ַ���ͨ�����õ����ַ���������.
             
**************************************************************/
void Uart1_String(unsigned char code *puts)			   
{
    for(; *puts != 0; puts++)
	{
        Uart1_TxByte(*puts);						   //��ָ�����ʽ���ַ����ֽ�Ϊ�����ַ�����������ĵ����ַ����ͺ�������
	}
}


/*************************************************************
*  ��������UART1_RCV (void)
*  ���룺��
*  �������
*  �������������崮�ڵ��жϴ������������жϺ�Ϊ4.
             �����ڽ�������߷�����һ֡����
			 ʱ�����жϣ���֪CPU�����жϴ���
**************************************************************/
void UART1_RCV (void) interrupt 4					  //�жϽ��պ��� �жϺ�Ϊ4    P188
{
	if(RI)											  //�жϽ��յ������Ƿ�����꣬�����յ���8λʱ ���ս���  RI����1	  P265
	{
		RI = 0;										  //�����ж�ʹ��λ��0
		RX0_Buffer[uart1_wr++] = SBUF;				  //�����ڻ�����SBUF�����ݷŵ����ݽ��ջ�����RX0_Buffer[]��
		flag = 1;									  //����־λ��1�����������ж�
	}

	if(TI)											  //�жϷ��͵������Ƿ����꣬�����͵���8λʱ ���ͽ���  TI����1	   P265
	{
		TI = 0;										  //�����ͽ�����־λ TI��0
		B_TI = 1;									  //��B_TI ��1 ���������Uart1_TxByte �����ж�
	}
}
/*************************************************************
*  ��������delay_ms(unsigned char ms)
*  ���룺��ʱʱ��
*  �������
*  �����������������뼶����ʱ.
             
**************************************************************/
void delay_ms(unsigned char ms)						 //���뼶��ʱ����
{
    unsigned int i;
	do{
	    i = MAIN_Fosc /1400;
		while(--i);
	}while(--ms);
}
void Delay(unsigned int i)//��ʱ
{
 while(--i);              
}