#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <termios.h>
#include <string.h>
#include <stdlib.h>


#define DEVICE_NAME "/dev/s3c2410_serial3"

int ser_fd;
int set_serial_uart1(int ser_fd)
{

	struct termios termios_new;
	bzero( &termios_new, sizeof(termios_new));
	/*ԭʼģʽ*/
	cfmakeraw(&termios_new);

	/*������Ϊ115200*/
	termios_new.c_cflag=(B115200);
	termios_new.c_cflag |= CLOCAL | CREAD;

	/*8λ����λ*/
	termios_new.c_cflag &= ~CSIZE;
	termios_new.c_cflag |= CS8;

	/*����żУ��λ*/
	termios_new.c_cflag &= ~PARENB;

	/*1λֹͣλ*/
	termios_new.c_cflag &= ~CSTOPB;
	/*������ڻ�����*/
	tcflush( ser_fd,TCIOFLUSH);
	termios_new.c_cc[VTIME] = 0;
	termios_new.c_cc[VMIN] = 4;
	tcflush ( ser_fd, TCIOFLUSH);
	/*��������ʹ��*/
	tcsetattr( ser_fd ,TCSANOW,&termios_new);
}


int main(void)
{
	char message[6];
	//�򿪴���+��ʼ��
	ser_fd = open(DEVICE_NAME,O_RDWR);
	if(ser_fd == -1)
	{
		printf("serical open failed!\n");
		return -1;
	}
	set_serial_uart1(ser_fd);

	if(fcntl(ser_fd,F_SETFL,0)<0)
	{
		exit(1);
	}
	tcdrain(ser_fd);   //ͨ���ն˿���
	tcflush( ser_fd, TCIOFLUSH); //ˢ�»�����


	char led_ctl[2]={0};
	while(1)
	{
		//���ڵĶ�д
		/*
		//��message�����������
		memset(message,0,6);
		read(ser_fd,message,6);
		printf("message = %s\n",message);
		tcflush( ser_fd, TCIOFLUSH); //ˢ�»�����*/

		led_ctl[0]=0;led_ctl[1]=0;
		write(ser_fd,led_ctl,2);
		sleep(1);
		led_ctl[0]=0;led_ctl[1]=1;
		write(ser_fd,led_ctl,2);
		sleep(1);
		led_ctl[0]=1;led_ctl[1]=0;
		write(ser_fd,led_ctl,2);
		sleep(1);
		led_ctl[0]=1;led_ctl[1]=1;
		write(ser_fd,led_ctl,2);
		sleep(1);
	}

}
