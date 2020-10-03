#ifndef _GF3658_H
   #define _GF3658_H

void GF3658_init(void);
void GF3658_data(unsigned char data1);

void GF3658_display(unsigned char data2);
void GF3658_map(unsigned char map);
void delay_GF(void);
void GF3658(void);
void delay_GF2(unsigned char n);
#endif