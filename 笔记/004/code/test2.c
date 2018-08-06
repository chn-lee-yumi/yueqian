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
	int x,y;
	while(1)
	{
		r_ret = read(ts_fd,&read_buf,sizeof(read_buf));	
		if(r_ret == -1)
		{
			perror("read ts error"); //填写错误提示信息
			return -1;
		}

		/*
		printf("type = 0x%x\t",read_buf.type);
		printf("code = 0x%x\t",read_buf.code);
		printf("value = %d\n",read_buf.value);*/
		
		if(read_buf.type == EV_ABS) //触摸屏
		{
			if(read_buf.code == ABS_X) //X轴
				x = read_buf.value;
			else if(read_buf.code == ABS_Y) //y轴
				y = read_buf.value;
			else if(read_buf.code == ABS_PRESSURE) //压力值
			{
				if(read_buf.value > 0)				//都只会出现1次
					printf("in\n");
				else if(read_buf.value == 0)		//都只会出现1次
				{
					printf("out\n");
					break;
				}
			}
		}if
		//添加按钮touch松开的判断
	}
	printf("x=%d,y=%d\n",x,y);
	
	close(ts_fd);
	return 0;
}