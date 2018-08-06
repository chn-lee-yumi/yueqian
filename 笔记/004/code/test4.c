#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
int main()
{
	int start_x,start_y,end_x,end_y;

	int ts_fd;
	//1.打开输入子系统的文件
	ts_fd = open("/dev/event0",O_RDWR);	
	if(ts_fd == -1)	//错误检测
	{
		perror("open ts error"); //填写错误提示信息
		return -1;
	}
	printf("ts_fd = %d\n",ts_fd);	//检测文件描述符=3

	//2.获取起点、终点坐标
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
				if(read_buf.value > 0)				//起点
				{
					printf("in\n");
					start_x=x;
					start_y=y;
				}	
				else if(read_buf.value == 0)		//终点
				{
					printf("out\n");
					end_x=x;
					end_y=y;
					break;
				}
			}
		}
	}
	
	//3.计算出x，y的偏移量
	int x_off,y_off;
	x_off = end_x - start_x;
	y_off = end_y - start_y;
	
	//4.进行方位判断
	//（1）先根据正负判断象限位置
	//（2）再根据大小，判断象限内的方位
	
	if(y_off > x_off) //45°区分。正（右下）
	{
		return DOWN;
	}
	
	
	close(ts_fd);
	return 0;
}