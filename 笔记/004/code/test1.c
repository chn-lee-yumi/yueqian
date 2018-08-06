#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
int main()
{

	int ts_fd;
	//1.打开输入子系统的文件
	ts_fd = open("/dev/event0",O_RDWR);	
	if(ts_fd == -1)	//错误检测
	{
		perror("open ts error"); //填写错误提示信息
		return -1;
	}
	printf("ts_fd = %d\n",ts_fd);	//检测文件描述符=3

	//2.从文件中读取数据
	struct input_event  read_buf;
	ssize_t r_ret;
	
	while(1)
	{
		r_ret = read(ts_fd,&read_buf,sizeof(read_buf));	
		if(r_ret == -1)
		{
			perror("read ts error"); //填写错误提示信息
			return -1;
		}
		//printf("reade bytes = %d\n",r_ret);
		
		printf("type = 0x%x\t",read_buf.type);
		printf("code = 0x%x\t",read_buf.code);
		printf("value = %d\n",read_buf.value);
	}
	
	close(ts_fd);
	return 0;
}