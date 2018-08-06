#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
struct student
{
	char name[20];
	int age;
	int num;
};

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
	struct student zhangsan;
	strcpy(zhangsan.name,"zhangsan");
	zhangsan.age = 10;
	zhangsan.num = 2018;

	ssize_t w_ret;
	w_ret = write(fd,&zhangsan,sizeof(struct student));	
	if(w_ret == -1)
	{
		perror("write 1.txt error"); //填写错误提示信息
		return -1;
	}
	printf("write bytes = %d\n",w_ret);

	return 0;
}