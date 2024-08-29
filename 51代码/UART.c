#include <UART.h>
unsigned char recv_buf[MAX_REV_NUM];
unsigned char recv_cnt;
unsigned char recv_flag;
unsigned char machine_step;
void UartInit(void)     //9600bps@11.0592MHz
{
    PCON &= 0x7F;       //�����ʲ�����
    SCON = 0x50;        //8λ����,�ɱ䲨����
    TMOD &= 0x0F;       //���ö�ʱ��ģʽ
    TMOD |= 0x20;       //���ö�ʱ��ģʽ
    TL1 = 0xFD;     //���ö�ʱ��ʼֵ
    TH1 = 0xFD;     //���ö�ʱ����ֵ
    ET1 = 0;        //��ֹ��ʱ��1�ж�
    ES = 1;
    //EA = 1;
    TR1 = 1;        //��ʱ��1��ʼ��ʱ
}

//����һ֡����
void sendByte(unsigned char dat)
{
    SBUF = dat;         //10λ�첽����ͨ��
    while(!TI);             //TIΪ1���˳�
    TI = 0;
}

//����һ������
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

//���ݻ�����ֻ����������ݣ�������֡ͷandУ���֣�ֻ����������ʡRAM�ռ�
void uart_ISR() interrupt 4 //һ��һ������
{
    unsigned char recv_data;    //���յ��м����
    static unsigned char sum_check;
    static unsigned char xor_check;
    if(RI)
    {
        RI = 0;
        recv_data = SBUF;
        switch(machine_step)
        {
            case 0:
                if(recv_data == 0x55)   //ֻ����֡ͷ�����浽������
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
                if(recv_data == 0xAA)   //ֻ����֡ͷ�����浽������
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
            case 2: //��ʼ�������ݣ��������ݽ���У�飨��У�顢���У�飩
                sum_check += recv_data;     //sum_check = sum_check + recv_data
                xor_check ^= recv_data;     //xor_check = xor_check���recv_data
                recv_buf[recv_cnt] = recv_data;
                recv_cnt++;
                sendByte('A');
                if(recv_cnt > 2)    //���ݹ̶�����
                {
                    machine_step = 3;
                }
                else
                {
                    machine_step = 2;
                }
                break;
            case 3: //��У��
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
            case 4: //���У��
                if(xor_check == recv_data)  
                {
                    recv_flag = 1;  //��־��ȷ����һ������
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