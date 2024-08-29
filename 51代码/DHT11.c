#include <DHT11.h>
#include "LCD1602.h"
uchar Temp_H,Temp_L,Humi_H,Humi_L;
uchar retry = 0;
// 发送开始信号
void DHT11_Start()
{
    DHT11_IO = 0;  // 拉低总线
    delay_ms(20);  // 延时18ms
    DHT11_IO = 1;  // 拉高总线
		delay_us(60);
}

// 检测DHT11响应信号
bit DHT11_Check()  
{
	if(!DHT11_IO){
    retry = 0;
    while (!DHT11_IO && (retry++ < 100)); // DHT11拉低后会释放总线
    if (retry >= 100) return 0; // 未响应
    retry = 0;
    while (DHT11_IO && retry++ < 100); // DHT11释放后会拉高总线
    if (retry >= 100) return 0; // 响应失败
    return 1;  // 响应成功
	}
	else return 0;
}

// 读取一个字节
uchar DHT11_ReadByte()    
{
    uchar i, dat = 0; 
    for (i = 0; i < 8; i++)
    {
			retry=2;
			while (!DHT11_IO && retry++); // 等待50us的低电平
			delay_us(30);           // 延时30us后读取数据位
			dat <<= 1;
			if (DHT11_IO) dat |= 1; // 如果高电平持续时间超过28us，数据位为1
			retry=2;       
			while (DHT11_IO&&retry++);  // 等待高电平结束
    }
    return dat;
}

// 读取温湿度数据
bit DHT11_ReadData(uchar *temp, uchar *humi)   
{
    DHT11_Start();
    if (!DHT11_Check()) return 0;  // 检查DHT11响应信号，失败返回0

    Humi_H = DHT11_ReadByte();      // 读取湿度整数部分
    Humi_L = DHT11_ReadByte();      // 湿度小数部分，DHT11为0
    Temp_H = DHT11_ReadByte();      // 读取温度整数部分
    Temp_L = DHT11_ReadByte();      // 温度小数部分，DHT11为0
                  
		if((Temp_H+Humi_L+Temp_L+Humi_H)!=DHT11_ReadByte())// 校验和（忽略）
			return 0;
		
		DHT11_IO=1;
		*temp=Temp_H;*humi=Humi_H;
    return 1; // 读取成功
	
}

