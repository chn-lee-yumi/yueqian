#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{

	int fd;
	//1.打开文件
	fd = open("1.txt",O_RDWR);	
	if(fd == -1)	//错误检测
	{
		perror("open 1.txt error"); //填写错误提示信息
		return -1;
	}
	printf("fd = %d\n",fd);	//检测文件描述符=3

	//2.往指定的文件中，写入hello world！
	char buf[20]="hello world!\n"; //有指定大小的存储空间
	//char *buf="hello world!";  //只有4个字节的存储空间（地址）
	ssize_t w_ret;
	w_ret = write(fd,buf,strlen(buf));	
	if(w_ret == -1)
	{
		perror("write 1.txt error"); //填写错误提示信息
		return -1;
	}
	printf("write bytes = %d\n",w_ret);
	
	return 0;
}