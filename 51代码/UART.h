#ifndef __UART_H__
#define __UART_H__
#include <REGX52.h>
#include <stdio.h>
#define uchar unsigned char
#define uint unsigned int

#define MAX_REV_NUM 20
#define MAX_REV_TIME 5   //3到5 * 1.042ms

//其他文件要使用的话一定要声明外部extern（全局变量）
extern unsigned char recv_buf[MAX_REV_NUM];
extern unsigned char recv_cnt;
extern unsigned char recv_flag;

void UartInit(void);
void sendByte(unsigned char dat);
void sendString(unsigned char *dat);
char putchar(char c);
void clr_recvbuffer(unsigned char *buf);


#endif