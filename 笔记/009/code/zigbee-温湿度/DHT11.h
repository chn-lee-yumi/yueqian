#ifndef __DHT11_H__
#define __DHT11_H__

#define DHT11 P0_4

extern void Delay_us(void); //1 us—” ±
extern void Delay_10us(void); //10 us—” ±
extern void Delay_ms(unsigned int Time);//n ms—” ±

extern unsigned char DHT11_ReadByte(void);
extern void DHT11_Read(unsigned char *pTemp,unsigned char *pHum);

#endif