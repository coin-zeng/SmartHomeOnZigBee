#include "config.h"
#include "AM2321.h"


uchar xdata Sensor_Data[5]={0x00,0x00,0x00,0x00,0x00};
uchar Sensor_Check;		  //У���
uchar Sensor_AnswerFlag;  //�յ���ʼ��־λ
uchar Sensor_ErrorFlag;   //��ȡ�����������־
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
	 }//������������
  }


/********************************************\
|* ���ܣ� �����������͵ĵ����ֽ�	        *|
\********************************************/
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
		delay_30us(); 
		//delay_30us();
		//�жϴ�������������λ
		tmp =0;
		if(Sensor_SDA)	 
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
		buffer <<=1;
		buffer |= tmp;	
	}
	return buffer;
  }

/********************************************\
|* ���ܣ� ��������              	        *|
\********************************************/
unsigned char Read_Sensor(void)
  {
	unsigned char i;
	 //Sensor_SDA = 1;
	 //Delay_N1ms(100);  //��ʱ2Ms
	//��������(Min=800US Max=20Ms) 
    Sensor_SDA = 0;
	
	delay_3ms();  //��ʱ2Ms
	
	//�ͷ����� ��ʱ(Min=30us Max=50us)
	Sensor_SDA = 1; 
	
	delay_30us();
	//Sensor_SDA = 1; 
    //Delay_N10us(2);//��ʱ30us	  
	Sensor_AnswerFlag = 0;  // ��������Ӧ��־	 
	//�жϴӻ��Ƿ��е͵�ƽ��Ӧ�ź� �粻��Ӧ����������Ӧ����������
    //Delay_N1ms(1);  //��ʱ2Ms
	
	 //Sensor_SCL = 0;
	if(Sensor_SDA ==0)
	{  
	   
	   Sensor_AnswerFlag = 1;//�յ���ʼ�ź�
	   Sys_CNT = 0;
	   //�жϴӻ��Ƿ񷢳� 80us �ĵ͵�ƽ��Ӧ�ź��Ƿ����	 
	   while((!Sensor_SDA))
	   {
	     if(++Sys_CNT>300) //��ֹ������ѭ��
		 {
		   Sensor_ErrorFlag = 1;
		   return 0;
		  } 
	    }
	    Sys_CNT = 0;
	    //�жϴӻ��Ƿ񷢳� 80us �ĸߵ�ƽ���緢����������ݽ���״̬
	    while((Sensor_SDA))
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
	   	Sensor_Check = Sensor_Data[0] + Sensor_Data[1] + Sensor_Data[2] + Sensor_Data[3];
		if(Sensor_Check != Sensor_Data[4])
		{
		    //Clear_Data ();
			;
		}
	  }
	  else
	  {
	    Sensor_AnswerFlag = 0;	  // δ�յ���������Ӧ	
	  }
	  return 1;
  }    
