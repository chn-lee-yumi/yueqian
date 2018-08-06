#include <stdio.h>
#include <stdlib.h>

#include "lcd.h"
#include "api_v4l2.h"

#include "yueqian.h"

//gcc *.c -std=gnu99 -lpthread


/*
使用触摸屏（while） + 线程 +摄像头（while），实现对摄像头采集的控制
			开始，实时显示画面
			暂停
			继续
			返回
*/
//右边160*480的地方为按键区

void * thread_cam_func(void *arg);

int main()
{

	//	yuyv摄像头输出测试代码
	

	lcd_open();
	linux_v4l2_device_init();
	linux_v4l2_start_capturing();
	
	
	Init_Convert_Table(); //提前初始化表格
	printf("init table\n");
	
	show_pic_generic("security.bmp",640,0);
	int touch_x,touch_y;
	pthread_t thread_cam;
	pthread_create(&thread_cam,NULL,thread_cam_func,NULL);
	while(1){//根据用户点击决定去到哪个页面
		get_coordinates(&touch_x,&touch_y);//获取输入
		if(touch_x>=640){
			if(touch_y<ROW/3){
				pthread_create(&thread_cam,NULL,thread_cam_func,NULL);
			}else if(touch_y>=ROW/3 && touch_y<ROW/3*2){
				pthread_cancel(thread_cam);
			}else{
				return 0;
			}
		}
	}
	
	linux_v4l2_stop_capturing();
	linux_v4l2_device_uinit();
	lcd_close();
}

void * thread_cam_func(void *arg)
{
	FreamBuffer freambuf;
	uint8_t *data=NULL;
	while(1)
	{
		linux_v4l2_get_fream(&freambuf);
		data = YUYV_To_RGB24(freambuf.buf,640,480);
		
		lcd_draw_bmp_for_buf(0,0,data,640,480);
		free(data);
		data=NULL;
	}
}
