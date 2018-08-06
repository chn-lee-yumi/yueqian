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

	//2.从指定的文件中，取得数据出来
	char buf[100]={0};
	

	ssize_t r_ret;
	r_ret = read(fd,buf,sizeof(buf)); //希望能装满整个buf
	if(r_ret == -1)
	{
		perror("read 1.txt error"); //填写错误提示信息
		return -1;
	}
	printf("read bytes = %d\n",r_ret);
	printf("read data is %s\n",buf);
	
	return 0;
}