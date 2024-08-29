#include <UART.h>
unsigned char recv_buf[MAX_REV_NUM];
unsigned char recv_cnt;
unsigned char recv_flag;
unsigned char machine_step;
void UartInit(void)     //9600bps@11.0592MHz
{
    PCON &= 0x7F;       //波特率不倍速
    SCON = 0x50;        //8位数据,可变波特率
    TMOD &= 0x0F;       //设置定时器模式
    TMOD |= 0x20;       //设置定时器模式
    TL1 = 0xFD;     //设置定时初始值
    TH1 = 0xFD;     //设置定时重载值
    ET1 = 0;        //禁止定时器1中断
    ES = 1;
    //EA = 1;
    TR1 = 1;        //定时器1开始计时
}

//发送一帧数据
void sendByte(unsigned char dat)
{
    SBUF = dat;         //10位异步串口通信
    while(!TI);             //TI为1才退出
    TI = 0;
}

//发送一串数据
void sendString(unsigned char *dat) //hellow world!
{
    while(*dat != '\0')
    {
        sendByte(*dat++);
    }
}

//printf
char putchar(char c)
{
    sendByte(c);
    return c;
}

void clr_recvbuffer(unsigned char *buf)
{
    unsigned char i;
    for(i = 0;i < MAX_REV_NUM;i++)
    {
        buf[i] = 0;
    }
}

//数据缓冲区只保存具体数据，不包含帧头and校验字（只解析），节省RAM空间
void uart_ISR() interrupt 4 //一个一个接收
{
    unsigned char recv_data;    //接收的中间变量
    static unsigned char sum_check;
    static unsigned char xor_check;
    if(RI)
    {
        RI = 0;
        recv_data = SBUF;
        switch(machine_step)
        {
            case 0:
                if(recv_data == 0x55)   //只解析帧头不保存到数组中
                {
                    machine_step = 1;
                    sendByte('A');
                }
                else
                {
                    machine_step = 0;
                }
                break;
            case 1:
                if(recv_data == 0xAA)   //只解析帧头不保存到数组中
                {
                    machine_step = 2;
                    recv_cnt = 0;
                    sendByte('A');
                }
                else
                {
                    machine_step = 0;
                }
                break;
            case 2: //开始接收数据，并对数据进行校验（和校验、异或校验）
                sum_check += recv_data;     //sum_check = sum_check + recv_data
                xor_check ^= recv_data;     //xor_check = xor_check异或recv_data
                recv_buf[recv_cnt] = recv_data;
                recv_cnt++;
                sendByte('A');
                if(recv_cnt > 2)    //数据固定个数
                {
                    machine_step = 3;
                }
                else
                {
                    machine_step = 2;
                }
                break;
            case 3: //和校验
                if(sum_check == recv_data)  
                {
                    machine_step = 4;
                    sendByte('A');
                }
                else
                {
                    machine_step = 0;
                    printf("SUM CHECK ERROR!\r\n");
                }
                break;
            case 4: //异或校验
                if(xor_check == recv_data)  
                {
                    recv_flag = 1;  //标志正确接收一串数据
                    sendByte('A');
                }
                else
                {
                    printf("XOR CHECK ERROR!\r\n");
                }
                machine_step = 0;
                recv_cnt = 0;
                sum_check = 0;
                xor_check = 0;
                break;
                /*else
                {
                    machine_step = 0;
                }*/
            default:break;
        }
    }
    if(TI)
    {
        TI = 0;
    }
}