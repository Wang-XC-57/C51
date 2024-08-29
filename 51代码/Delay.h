#ifndef __DELAY_H__
#define __DELAY_H__
#include <REGX52.h>
#include <intrins.h>

#define uchar unsigned char
#define uint unsigned int

void delay_us(uchar us); 		//us延时函数
void delay_ms(uint ms);		//ms延时函数

#endif
