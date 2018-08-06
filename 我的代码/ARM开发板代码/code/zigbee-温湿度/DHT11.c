#include "DHT11.h"
#include "OnBoard.h"

void Delay_us(void) //1 us延时
{
    MicroWait(1);   
}

void Delay_10us(void) //10 us延时
{
   MicroWait(10);
}

void Delay_ms(unsigned int Time)//n ms延时
{
  unsigned char i;
  while(Time--)
  {
    for(i=0;i<100;i++)
     Delay_10us();
  }
}

unsigned char DHT11_ReadByte(void)
{
	unsigned char bit_value;
	unsigned char value=0;
	unsigned char count;
	
	for(count=0;count<8;count++)
	{
		if(!DHT11)
		{
			while(!DHT11);  //等待低半周期过去
			//判断是0还是1
			Delay_10us();
			Delay_10us();
			Delay_10us();
			
			if(DHT11)
			bit_value = 1;
			else
			bit_value = 0;
		}
		value <<= 1;
		value |=  bit_value;
		while(DHT11);
	}              
	return value;
}


void DHT11_Read(unsigned char *pTemp,unsigned char *pHum)
{
   //设置P0_4 输出
   P0SEL &=  ~(0x1 << 4);
   P0DIR |=   (0x1 << 4);

   //CC2530 主机启动读写信号 
   DHT11 = 0;   //拉低
   Delay_ms(19); //保持19ms
   DHT11 = 1;   //拉高

   //设置P0_4 输入
   P0DIR |=   (0x1 << 4);
   
   //等待DHT11应答
   while(DHT11);
   
   //DHT11响应
   if(!DHT11)
   {
        //DHT11 响应信号
        while(!DHT11); //等待低周期结束
        while(DHT11); //等待高周期结束
        
        //读取40bit数据
        
        //读取湿度的整数值 
        *pHum = DHT11_ReadByte();
        
        //读取湿度的小数值，暂不使用 
        DHT11_ReadByte();       
        
        //读取温度的整数值 
        *pTemp = DHT11_ReadByte();
        
        //读取温度的小数值 ，暂不使用
        DHT11_ReadByte();  
        
        //读取校验值，忽略
        DHT11_ReadByte();   
       
        DHT11 = 1;
   }
}
