#include "config.h"
#include "GF3658.h"

void GF3658_init(void)
{
    //P1M0 |= 0x11;
	//P2M0 |= 0x70;
	
	/*P0M0 |= 0x21;
	P1M0 |= 0x02;
	P2M0 |= 0x20;
	P3M0 |= 0x04;*/
    
    P0M0 &= 0x00;
	P1M0 &= 0x00;
	P2M0 &= 0x00;
	P3M0 &= 0x00;

	P0M0 |= 0x21;
	P1M0 |= 0x02;
	P2M0 |= 0x20;
	P3M0 |= 0x04;


    COM1 = 0;
	COM2 = 0;
	COM3 = 0;
	COM4 = 0;
	COM5 = 0;

	set1 = 1;
	set2 = 1;
	set3 = 1;
	set4 = 1;
	set5 = 1;
	set6 = 1;
	set7 = 1;
	set8 = 1;

}

void GF3658_data(unsigned char data1)
{
   	 unsigned char shi,ge;
	 data1 = (data1%100);
     shi  = (data1/10);
     COM1 = 1;
	 GF3658_display(shi);
	 delay_GF();
	 delay_GF();
	 COM1 = 0;
	 set1 = 1;
	 set2 = 1;
	 set3 = 1;
	 set4 = 1;
	 set5 = 1;
	 set6 = 1;
	 set7 = 1;
	 delay_GF();

	 ge   = data1%10;
	 COM2 = 1;
	 GF3658_display(ge);
	 delay_GF();
	 delay_GF();
	 COM2 = 0;
	 set1 = 1;
	 set2 = 1;
	 set3 = 1;
	 set4 = 1;
	 set5 = 1;
	 set6 = 1;
	 set7 = 1;
	 delay_GF();
}

void GF3658_display(unsigned char data2)
{
    switch	(data2)
	{
	    case 0x00:
		        set1 = 0;
				set2 = 0;
				set3 = 0;
				set4 = 0;
				set5 = 0;
				set6 = 0;
			    set7 = 1;
				break;
	    case 0x01:
		        set1 = 1;
				set2 = 0;
				set3 = 0;
				set4 = 1;
				set5 = 1;
				set6 = 1;
				set7 = 1;
				break;
	    case 0x02:
		        set1 = 0;
				set2 = 0;
				set3 = 1;
				set4 = 0;
				set5 = 0;
				set6 = 1;
				set7 = 0;
				break;
	    case 0x03:
		        set1 = 0;
				set2 = 0;
				set3 = 0;
				set4 = 0;
				set5 = 1;
				set6 = 1;
			    set7 = 0;
				break;
	    case 0x04:
		        set1 = 1;
				set2 = 0;
				set3 = 0;
				set4 = 1;
				set5 = 1;
				set6 = 0;
				set7 = 0;
				break;
	    case 0x05:
		        set1 = 0;
				set2 = 1;
				set3 = 0;
				set4 = 0;
				set5 = 1;
				set6 = 0;
				set7 = 0;
				break;
	    case 0x06:
		        set1 = 0;
				set2 = 1;
				set3 = 0;
				set4 = 0;
				set5 = 0;
				set6 = 0;
				set7 = 0;
				break;
	    case 0x07:
		        set1 = 0;
				set2 = 0;
				set3 = 0;
				set4 = 1;
				set5 = 1;
				set6 = 1;
				set7 = 1;
				break;
	    case 0x08:
		        set1 = 0;
				set2 = 0;
				set3 = 0;
				set4 = 0;
				set5 = 0;
				set6 = 0;
				set7 = 0;
				break;
	    case 0x09:
		        set1 = 0;
				set2 = 0;
				set3 = 0;
				set4 = 1;
				set5 = 1;
				set6 = 0;
				set7 = 0;
				break;
		 
	}
}

void GF3658_map(unsigned char map)
{   
    COM1 = 0;
	COM2 = 0;
    switch (map)
	{
	    case 0x01:
		       COM3 = 1;
			   set1 = 0;
			   delay_GF2(20);
			   set1 = 1;
			   COM3 = 0;
			   break;
		case 0x02:
		       COM3 = 1;
			   set2 = 0;
			   delay_GF2(20);
			   set2 = 1;
			   COM3 = 0;
			   break;
		case 0x03:
		       COM3 = 1;
			   set3 = 0;
			   delay_GF2(20);
			   set3 = 1;
			   COM3 = 0;
			   break;
		case 0x04:
		       COM3 = 1;
			   set4 = 0;
			   delay_GF2(20);
			   set4 = 1;
			   COM3 = 0;
			   break;
		case 0x05:
		       COM3 = 1;
			   set5 = 0;
			   delay_GF2(20);
			   set5 = 1;
			   COM3 = 0;
			   break;
		case 0x06:
		       COM3 = 1;
			   set6 = 0;
			   delay_GF2(20);
			   set6 = 1;
			   COM3 = 0;
			   break;
		case 0x07:
		       COM3 = 1;
			   set7 = 0;
			   delay_GF2(20);
			   set7 = 1;
			   COM3 = 0;
			   break;
		case 0x08:
		       COM5 = 1;
			   set8 = 0;
			   delay_GF2(20);
			   set8 = 1;
			   COM5 = 0;
			   break;
		case 0x09:
		       COM4 = 1;
			   set1 = 0;
			   delay_GF();
			   set1 = 1;
			   COM4 = 0;
			   break;
		case 0x0A:
		       COM4 = 1;
			   set2 = 0;
			   delay_GF();
			   set2 = 1;
			   COM4 = 0;
			   break;
	    case 0x0B:
		       COM4 = 1;
			   set3 = 0;
			   delay_GF();
			   set3 = 1;
			   COM4 = 0;
			   break;
		case 0x0C:
		       COM4 = 1;
			   set4 = 0;
			   delay_GF();
			   set4 = 1;
			   COM4 = 0;
			   break;
		 case 0x0D:
		       COM4 = 1;
			   set5 = 0;
			   delay_GF();
			   set5 = 1;
			   COM4 = 0;
			   break;
	     case 0x0E:
		       COM4 = 1;
			   set6 = 0;
			   delay_GF();
			   set6 = 1;
			   COM4 = 0;
			   break;
		       

	}
}

void GF3658(void)
{
    COM4 = 1;
	set1 = 0;
	delay_ms(25);
	set2 = 0;
	delay_ms(25);
	set3 = 0;
	delay_ms(25);
	set4 = 0;
	delay_ms(25);
	COM4 = 0;
	set1 = 1;
	set2 = 1;
	set3 = 1;
	set4 = 1;
}


void delay_GF(void)
{
    unsigned char a;
    for(a=164;a>0;a--);
	for(a=164;a>0;a--);
	

}
					  
void delay_GF2(unsigned char n)
{
    unsigned char i;
    for(i = 0; i < n; i++)
	{
        delay_GF();
	}
}

