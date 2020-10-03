
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
unsigned char 	uart1_wr=0;		                //д������
unsigned char 	uart1_rd;		                //��������
unsigned char 	xdata RX0_Buffer[BUF_LENTH];	//���ջ���
unsigned char flag;							    //���崮���Ƿ��н��յ����ݵı�־λ
unsigned char i;							    //��ͨ��ѭ������
bit		B_TI;								    //����B_TIΪbit����
unsigned char length;
unsigned char    xdata Crc_buf[2];  //�����洢CRCУ��ֵ�ĸ�8λ����8λ�Ļ���

unsigned int   Crc_return_data;  //����CRCУ��ֵ
void	uart1_init(void);					    //���ڳ�ʼ����������
void Uart1_TxByte(unsigned char dat);			//���ڷ��͵����ַ���������
void Uart1_String(unsigned char code *puts);	//���ڷ����ַ���������������
void delay_ms(unsigned char ms);				//��ʱ��������
unsigned int cal_crc(unsigned char *snd, unsigned char num);

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
#define HAVEFIRE 		'd' 	 //�л���
#define NOFIRE	    	'e' 	 //�޻���
#define HAVEKEY 		'b' 	 //�а���
#define NOKEY	   	    'c'	 	//�ް���
#define OPENALARM       '9'     //������
#define CLOSEALARM      'a'     //�ؾ���
char fire_flag=1;	//����⵽����ʱ������Ϊ��⵽û��Ӧ����ť���¶��ص�����
void main(void)
{	
	uart1_init();	   	//���ô��ڳ�ʼ������
	P4M1&=0xfe;
	P4M0 |= 0x01;
	shuchu = 0;	
								
	while(1)
	{
		if(flag == 1)									//�Ƿ��д����ж�
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
			uart1_wr = 0;								//��д������0 
		}
	
	}
}


/*************************************************************
*  ��������uart1_init(void)
*  ���룺��
*  �������
*  �������������ڳ�ʼ������Ϊ9600 8 N 1.
             
**************************************************************/
void uart1_init(void)							   
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
		flag = 1;										  //����־λ��1�����������ж�
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