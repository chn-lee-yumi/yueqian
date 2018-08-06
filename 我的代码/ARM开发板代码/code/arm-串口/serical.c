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
	/*原始模式*/
	cfmakeraw(&termios_new);

	/*波特率为115200*/
	termios_new.c_cflag=(B115200);
	termios_new.c_cflag |= CLOCAL | CREAD;

	/*8位数据位*/
	termios_new.c_cflag &= ~CSIZE;
	termios_new.c_cflag |= CS8;

	/*无奇偶校验位*/
	termios_new.c_cflag &= ~PARENB;

	/*1位停止位*/
	termios_new.c_cflag &= ~CSTOPB;
	/*清除串口缓冲区*/
	tcflush( ser_fd,TCIOFLUSH);
	termios_new.c_cc[VTIME] = 0;
	termios_new.c_cc[VMIN] = 4;
	tcflush ( ser_fd, TCIOFLUSH);
	/*串口设置使能*/
	tcsetattr( ser_fd ,TCSANOW,&termios_new);
}


int main(void)
{
	char message[6];
	//打开串口+初始化
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
	tcdrain(ser_fd);   //通用终端控制
	tcflush( ser_fd, TCIOFLUSH); //刷新缓冲区


	char led_ctl[2]={0};
	while(1)
	{
		//串口的读写
		/*
		//对message数据清零操作
		memset(message,0,6);
		read(ser_fd,message,6);
		printf("message = %s\n",message);
		tcflush( ser_fd, TCIOFLUSH); //刷新缓冲区*/

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
