#include <REGX52.H>
#include "LCD1602.h"
#include "Delay.h"
#include "DHT11.h"
#include "UART.h"
uchar trans[4];

void main()
{
	uchar temp=0, humi=0;
	uchar i=0;

	LCD_Init();						//LCD初始化
	UartInit();
	LCD_ShowString(1,1,"Temp:");
	LCD_ShowString(2,1,"Humi:");
	while(1)
	{
		if (DHT11_ReadData(&temp, &humi)) // 读取成功
		{
			LCD_ShowNum(1,9,temp,2);
			LCD_ShowNum(2,9,humi,2);
			trans[0]='*';
			trans[1]=temp;
			trans[2]=humi;
			trans[3]='#';
			for(i=0;i<4;i++)
			 sendByte(trans[i]);
		}
		
		
		delay_ms(1000);
	}
}
