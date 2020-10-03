#include <STC12C5A60S2.h>
#include <intrins.h>
#include <stdio.h>
#include <stdlib.h>
/**********************************************************/
//����Ƶ�ʣ�11.0592MHz
//�ļ���  ��Main.c
//����˵������ʪ�ȴ�����AM2321��ȡʵ��
//����    ��www.frotech.com
//����֧�֣�020-22883196 QQ:
//�����¼: 2013.05.02
//�������: �½���
/**********************************************************/

#define	 BUF_LENTH	128		//���崮�ڽ��ջ��峤��
unsigned char 	uart1_wr;		//дָ��
unsigned char 	uart1_rd;		//��ָ��
unsigned char 	xdata RX0_Buffer[BUF_LENTH];	//���崮�ڽ��ջ���
unsigned char flag;	//���崮���жϽ��ձ�ţ�������Ϊ1ʱ��ʾ���ڽ��ܵ����ݡ�
unsigned char i;    //������ͨ������

int Sensor_Data[5]={0x00,0x00,0x00,0x00,0x00};//������ʪ�ȴ��������ݴ������
unsigned char Sensor_Check;		  //��ʪ�ȴ�����У��ͣ��ж϶�ȡ����ʪ�������Ƿ���ȷ��
unsigned char Sensor_AnswerFlag;  //��ʪ�ȴ������յ���ʼ��־λ
unsigned char Sensor_ErrorFlag;   //��ȡ�����������־
unsigned char Ascii_buffer[10] = {'0','1','2','3','4','5','6','7','8','9'};
unsigned char   xdata mbus_buffer[255];
unsigned char   xdata mbus_Sendbuf[255];
unsigned char   xdata Crc_buf[2];  //�����洢CRCУ��ֵ�ĸ�8λ����8λ�Ļ���
unsigned int  Crc_return_data;  //����CRCУ��ֵ


unsigned int  Sys_CNT;
unsigned int  Tmp;

bit		B_TI;
sbit Sensor_SDA = P1^1;//����P1.1ΪAM2321�����ݿ�

/********************����������**************************************/
void uart1_init(void); //�������ڳ�ʼ������
void Uart1_TxByte(unsigned char dat); //�������ڷ��͵��ֽں���
void Uart1_String(unsigned char code *puts);//�������ڷ����ַ�������
void delay_ms(unsigned char ms);//������ͨ��ʱ����
void Clear_Data (void);	//����AM2321�������������
void delay_3ms(void);	//������ʱ3ms����
void delay_30us(void);	//������ʱ30us����
unsigned char Read_SensorData(void);//������ȡAM2321���ݺ���
unsigned char Read_Sensor(void);//������ȡAM2321��ʪ�����ݺ���
unsigned int cal_crc(unsigned char *snd, unsigned char num);
void newline();

/**********************************************************/

/*****�û�������� ���������ڲ���Ϊ9600 8 N 1  ***********/
#define MAIN_Fosc		11059200UL	
#define Baudrate0		115200UL											
/**********************************************************/


/****************** �������Զ����ɣ��û������޸� ***********************/

#define BRT_Reload			(256 - MAIN_Fosc / 16 / Baudrate0)

/**********************************************************/
//int get_sensor_data[3]={0xff,0xff,0xff};
   /*
typedef struct {
	char DataType;	//�������ͱ�־
	int temp;		//�¶�
	int humi;		//ʪ��
}AM2321_Data;
	 */
//AM2321_Data get_sensor_data;


//**********************************************************************
//��������main(void)
//����  ����
//���  ����
//����������ʵ��STC12C5A16S2��Ƭ����P1.1�ڶ�ȡAM2321����ʪ�����ݣ�������
//          ASCII��ʽ��������ڽ�����
//          ����"H:xx.x%RH,T:xx.x����ʽÿ��Լ2����ʾһ�Ρ�
//*************************************************************************
int main(void)
{
	int temp, humi;
	char tem[4],hum[4];
	//get_sensor_data.DataType = 1;//1:��ʪ��
	
	Sensor_SDA = 1;//SDA��������������������һֱ���ָߵ�ƽ����ʼ���������ߡ�
	uart1_init();  //��ʼ�����ڣ�����Ϊ9600 8 N 1.
	 
	while(1) //��ѭ����ʼ
	{
		//Uart1_String(b); //���ڴ�ӡ��������ַ�����
		delay_ms(300); //��ʱ
	    if(Read_Sensor())		  // ��ȡ����������
		{		
			if(Sensor_Data[0]+Sensor_Data[1]+Sensor_Data[2]+Sensor_Data[3]==Sensor_Data[4])
			{	
				
				for(i=0; i<4; i++)
				{
				 	Uart1_TxByte(Sensor_Data[i]);
				} 		
				delay_ms(1000);
				/*
				//������ʾ�ַ�����
				// ʪ�� 
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
				// �¶�  
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
	AUXR |=  0x01;		//UART0 ʹ��BRT
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
//��������delay_3ms(void)
//����  ����
//���  ����
//������������ʱ3ms����
//*************************************************************************
void delay_3ms(void)   
{
    unsigned char a,b;
    for(b=194;b>0;b--)
        for(a=84;a>0;a--);
}

//**********************************************************************
//��������delay_30us(void)
//����  ����
//���  ����
//������������ʱ3ms����
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
//��������Read_SensorData(void)
//����  ����
//���  �����ֽڵ���ʪ�����ݻ���У��ֵ
//������������ȡ���ֽڵ�AM2321������
//*************************************************************************
unsigned char Read_SensorData(void)
  {
	unsigned char i,cnt;
	unsigned char buffer,tmp;
	buffer = 0;
	for(i=0;i<8;i++)
	{
		cnt=0;
		while(!Sensor_SDA)	//����ϴε͵�ƽ�Ƿ����
		{
		  if(++cnt >= 290)
		   {
			  break;
		   }
		}
		//��ʱMin=26us Max50us ��������"0" �ĸߵ�ƽ
		delay_30us();//��ʱ30us 
		tmp =0;
		if(Sensor_SDA)//��ʱ30us��������ݿڻ��Ǹߣ����λΪ1������Ϊ0����P19	 
		{
		  tmp = 1;
		}  
		cnt =0;
		while(Sensor_SDA)		//�ȴ��ߵ�ƽ ����
		{
		   	if(++cnt >= 200)
			{
			  break;
			}
		}
		buffer <<=1; //��λ��ʹ�����ݵ����λ׼��������һλ
		buffer |= tmp;//�ѱ��ν��յ���λ���뵽������	
	}
	return buffer;//���ص��ֽ�����
  }

//**********************************************************************
//��������Read_Sensor(void)
//����  ����
//���  ����
//������������ȡAM2321����ʪ�ȼ�У��ֵ����Sensor_Data[]�С�
//*************************************************************************
unsigned char Read_Sensor(void)
{
	unsigned char i;

    Sensor_SDA = 0;//��ʼ�ź����ͣ���AM2321�����ֲ�P18
	delay_3ms();  //��ʱ3Ms����Ȼһ��1ms�Ϳ����ˡ�
	Sensor_SDA = 1; //���ߣ��ͷ�����
	delay_30us(); //��ʱ30us
	Sensor_AnswerFlag = 0;  // ��������Ӧ��־	 
	if(Sensor_SDA ==0)//�Ӹߵ�ƽ���͵�ƽ����30us(����20us)�Ƿ�Ϊ��
	{  
	   //���Ϊ�ͣ���ô������������Ӧ�ź�
	   Sensor_AnswerFlag = 1;//�յ���ʼ�ź�
	   Sys_CNT = 0;//�жϴӻ��Ƿ񷢳� 80us �ĵ͵�ƽ��Ӧ�ź��Ƿ����	 
	   while((!Sensor_SDA))	//�ȴ���������Ӧ�ź�80us�ĵ͵�ƽ����
	   {
	     if(++Sys_CNT>300) //��ֹ������ѭ��
		 {
		   Sensor_ErrorFlag = 1;
		   return 0;
		 } 
	   }
	    Sys_CNT = 0;
	    //�жϴӻ��Ƿ񷢳� 80us �ĸߵ�ƽ���緢����������ݽ���״̬
	    while((Sensor_SDA))//�ȴ���������Ӧ�ź�80us�ĸߵ�ƽ����
	    {
	       if(++Sys_CNT>300) //��ֹ������ѭ��
		   {
		     Sensor_ErrorFlag = 1;
		     return 0;
		   } 
	    } 		 
	    // ���ݽ���	������������40λ���� 
	    // ��5���ֽ� ��λ����  5���ֽڷֱ�Ϊʪ�ȸ�λ ʪ�ȵ�λ �¶ȸ�λ �¶ȵ�λ У���
	    // У���Ϊ��ʪ�ȸ�λ+ʪ�ȵ�λ+�¶ȸ�λ+�¶ȵ�λ
	    for(i=0;i<5;i++)
	    {
	      Sensor_Data[i] = Read_SensorData();
	    }
	   
	  }
	  else
	  {
	    Sensor_AnswerFlag = 0;	  // δ�յ���������Ӧ	
	  }
	  return 1;
  }    


