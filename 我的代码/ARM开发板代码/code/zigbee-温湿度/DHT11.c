#include "DHT11.h"
#include "OnBoard.h"

void Delay_us(void) //1 us��ʱ
{
    MicroWait(1);   
}

void Delay_10us(void) //10 us��ʱ
{
   MicroWait(10);
}

void Delay_ms(unsigned int Time)//n ms��ʱ
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
			while(!DHT11);  //�ȴ��Ͱ����ڹ�ȥ
			//�ж���0����1
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
   //����P0_4 ���
   P0SEL &=  ~(0x1 << 4);
   P0DIR |=   (0x1 << 4);

   //CC2530 ����������д�ź� 
   DHT11 = 0;   //����
   Delay_ms(19); //����19ms
   DHT11 = 1;   //����

   //����P0_4 ����
   P0DIR |=   (0x1 << 4);
   
   //�ȴ�DHT11Ӧ��
   while(DHT11);
   
   //DHT11��Ӧ
   if(!DHT11)
   {
        //DHT11 ��Ӧ�ź�
        while(!DHT11); //�ȴ������ڽ���
        while(DHT11); //�ȴ������ڽ���
        
        //��ȡ40bit����
        
        //��ȡʪ�ȵ�����ֵ 
        *pHum = DHT11_ReadByte();
        
        //��ȡʪ�ȵ�С��ֵ���ݲ�ʹ�� 
        DHT11_ReadByte();       
        
        //��ȡ�¶ȵ�����ֵ 
        *pTemp = DHT11_ReadByte();
        
        //��ȡ�¶ȵ�С��ֵ ���ݲ�ʹ��
        DHT11_ReadByte();  
        
        //��ȡУ��ֵ������
        DHT11_ReadByte();   
       
        DHT11 = 1;
   }
}
