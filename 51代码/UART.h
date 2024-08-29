#ifndef __UART_H__
#define __UART_H__
#include <REGX52.h>
#include <stdio.h>
#define uchar unsigned char
#define uint unsigned int

#define MAX_REV_NUM 20
#define MAX_REV_TIME 5   //3��5 * 1.042ms

//�����ļ�Ҫʹ�õĻ�һ��Ҫ�����ⲿextern��ȫ�ֱ�����
extern unsigned char recv_buf[MAX_REV_NUM];
extern unsigned char recv_cnt;
extern unsigned char recv_flag;

void UartInit(void);
void sendByte(unsigned char dat);
void sendString(unsigned char *dat);
char putchar(char c);
void clr_recvbuffer(unsigned char *buf);


#endif