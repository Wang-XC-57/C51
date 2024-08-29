#ifndef _DHT11_H_
#define _DHT11_H_
#include <REGX52.h>
#include "Delay.h"

#define uchar unsigned char
#define uint unsigned int
sbit DHT11_IO=P1^0;


uchar DHT11_ReadByte();       // 读取一个字节
bit DHT11_Check();            // 检测DHT11响应信号
void DHT11_Start();           // 发送开始信号
bit DHT11_ReadData(uchar *temp, uchar *humi); // 读取温湿度数据
#endif