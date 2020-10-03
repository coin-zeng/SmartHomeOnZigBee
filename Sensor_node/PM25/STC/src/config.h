#ifndef _CONFIG_H
#define _CONFIG_H

#include <STC12C5A60S2.h>
#include <intrins.h>
#include <math.h>

#include "EEPROM.h"
#include "crc.h"
#include "ADC.h"

sbit Sensor_SDA = P1^1;
sbit  P1_0 = P1^0;//����P1.0�˿�
sbit shuchu = P1^1;
sbit shuchu1 = P1^1;
sbit Relay = P4^0;
sbit PM25LED = P1^2;
sbit PM25AD  = P1^1;


/*************** �û�������� *****************************/

#define MAIN_Fosc		11059200UL	
#define Baudrate0		115200UL		
									
/**********************************************************/
/****************** �������Զ����ɣ��û������޸� ************************************/

#define BRT_Reload			(256 - MAIN_Fosc / 16 / Baudrate0)		//Calculate the timer1 reload value ar 1T mode

/**********************************************************/
#define		BUF_LENTH	255		//���崮�ڽ��ջ��峤��
#define	uchar	unsigned char
#define uint	unsigned int
#define ulong	unsigned long




void uart1_init(void);
void Uart1_TxByte(unsigned char dat);
void Uart1_String(unsigned char code *puts);
void delay_ms(unsigned char ms);

#endif