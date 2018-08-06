#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{

	int fd;
	//1.打开文件
	fd = open("/dev/fb0",O_RDWR);	
	if(fd == -1)	//错误检测
	{
		perror("open /dev/fb0 error"); //填写错误提示信息
		return -1;
	}
	printf("fd = %d\n",fd);	//检测文件描述符=3

	
	//2.往指定的文件中，写入满屏幕的绿光数据
	char color_buf[800*480*4]={0}; //对应LCD尺寸
	int i=0;
	for(i=0;i<800*480*4;i+=4)
	{
		color_buf[i+0] = 0;		//B
		color_buf[i+1] = 255;		//G
		color_buf[i+2] = 0;		//R
		color_buf[i+3] = 0;		//A
	}
	
	ssize_t w_ret;
	w_ret = write(fd,color_buf,800*480*4);	
	if(w_ret == -1)
	{
		perror("write /dev/fb0 error"); //填写错误提示信息
		return -1;
	}
	printf("write bytes = %d\n",w_ret);
	
	return 0;
}