#include "Delay.h"

// 延时1微秒的基础函数
void delay_us(uchar us) {
	us=us/2;
	while(--us);
}

// 延时1毫秒的函数
void delay_ms(uint ms) {
	unsigned char j; 
	while(ms--)
	{
		for(j=0;j<125;j++);
	}
}

