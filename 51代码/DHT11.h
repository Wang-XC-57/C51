#ifndef _DHT11_H_
#define _DHT11_H_
#include <REGX52.h>
#include "Delay.h"

#define uchar unsigned char
#define uint unsigned int
sbit DHT11_IO=P1^0;


uchar DHT11_ReadByte();       // ��ȡһ���ֽ�
bit DHT11_Check();            // ���DHT11��Ӧ�ź�
void DHT11_Start();           // ���Ϳ�ʼ�ź�
bit DHT11_ReadData(uchar *temp, uchar *humi); // ��ȡ��ʪ������
#endif