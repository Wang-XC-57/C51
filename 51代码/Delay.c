#include "Delay.h"

// ��ʱ1΢��Ļ�������
void delay_us(uchar us) {
	us=us/2;
	while(--us);
}

// ��ʱ1����ĺ���
void delay_ms(uint ms) {
	unsigned char j; 
	while(ms--)
	{
		for(j=0;j<125;j++);
	}
}

