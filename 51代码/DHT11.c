#include <DHT11.h>
#include "LCD1602.h"
uchar Temp_H,Temp_L,Humi_H,Humi_L;
uchar retry = 0;
// ���Ϳ�ʼ�ź�
void DHT11_Start()
{
    DHT11_IO = 0;  // ��������
    delay_ms(20);  // ��ʱ18ms
    DHT11_IO = 1;  // ��������
		delay_us(60);
}

// ���DHT11��Ӧ�ź�
bit DHT11_Check()  
{
	if(!DHT11_IO){
    retry = 0;
    while (!DHT11_IO && (retry++ < 100)); // DHT11���ͺ���ͷ�����
    if (retry >= 100) return 0; // δ��Ӧ
    retry = 0;
    while (DHT11_IO && retry++ < 100); // DHT11�ͷź����������
    if (retry >= 100) return 0; // ��Ӧʧ��
    return 1;  // ��Ӧ�ɹ�
	}
	else return 0;
}

// ��ȡһ���ֽ�
uchar DHT11_ReadByte()    
{
    uchar i, dat = 0; 
    for (i = 0; i < 8; i++)
    {
			retry=2;
			while (!DHT11_IO && retry++); // �ȴ�50us�ĵ͵�ƽ
			delay_us(30);           // ��ʱ30us���ȡ����λ
			dat <<= 1;
			if (DHT11_IO) dat |= 1; // ����ߵ�ƽ����ʱ�䳬��28us������λΪ1
			retry=2;       
			while (DHT11_IO&&retry++);  // �ȴ��ߵ�ƽ����
    }
    return dat;
}

// ��ȡ��ʪ������
bit DHT11_ReadData(uchar *temp, uchar *humi)   
{
    DHT11_Start();
    if (!DHT11_Check()) return 0;  // ���DHT11��Ӧ�źţ�ʧ�ܷ���0

    Humi_H = DHT11_ReadByte();      // ��ȡʪ����������
    Humi_L = DHT11_ReadByte();      // ʪ��С�����֣�DHT11Ϊ0
    Temp_H = DHT11_ReadByte();      // ��ȡ�¶���������
    Temp_L = DHT11_ReadByte();      // �¶�С�����֣�DHT11Ϊ0
                  
		if((Temp_H+Humi_L+Temp_L+Humi_H)!=DHT11_ReadByte())// У��ͣ����ԣ�
			return 0;
		
		DHT11_IO=1;
		*temp=Temp_H;*humi=Humi_H;
    return 1; // ��ȡ�ɹ�
	
}

