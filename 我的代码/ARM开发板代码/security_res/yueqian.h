//Version: 20180717
//粤嵌嵌入式实习万能头文件 封装by：昊天金阙无上至尊自然妙有弥罗至真玉米上帝
/*使用方法：
在你的.c代码文件第一行输入：#include "yueqian.h"
把yueqian.h和yueqian.c复制到你的.c代码目录下
编译需要加参数 -std=gnu99
*/
//TODO:增加列目录文件等功能。
#include <pthread.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/input.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>

#define ROW 480
#define COLUMN 800

#define NOT_MOVED 0
#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4

void init_board();//初始化开发板函数
/*
功能：
	打开/dev/fb0
	打开/dev/event0
	设置mmap
一般在main函数第一句执行。
*/

void uninit_board();//反初始化开发板函数
/*
功能：
	释放mmap的内存
	关闭/dev/fb0
	关闭/dev/event0
一般在main函数return前调用。不调用也行。进程退出后是会自动释放共享内存区域的数据的。
*/

void refresh_input();//重新打开输入文件
/*
功能：
	关闭/dev/event0
	打开/dev/event0
清空未读取的输入，某些情况要用。
*/

void show_pic_generic(char path[],int offset_x,int offset_y);//放置图片的函数
/*
功能：
	将一张24位的bmp图片放置在屏幕上的任何位置。（图片边缘不能超出屏幕）
用法：
	char path[]：文件路径，如"1.bmp"
	int offset_x,int offset_y：图片左上角在屏幕里的坐标。
*/

int is_type(char type[],char filename[]);//判断一个文件名是不是某个类型的文件
/*
功能：
	判断一个文件名是不是某个类型的文件
用法：
	char type[]：文件类型，如".bmp"或".mp3"
	char filename[]：要判断的文件名，如"1.bmp"
	返回值：1表示是，0表示不是。
*/

void get_coordinates(int *touch_x,int *touch_y);//获取触摸点
/*
功能：
	获取触摸屏输入点（按下之后，松开时所在的点）
用法：
	int touch_x,touch_y;
	get_coordinates(&touch_x,&touch_y);
	返回值：直接修改传入的touch_x和touch_y。
*/

int get_direction();//获取滑动方向
/*
功能：
	获取触摸屏滑动方向
用法：
	返回值：NOT_MOVED、UP、DOWN、LEFT、RIGHT。
*/

int get_coordinates_and_direction(int *touch_x,int *touch_y);//获取触摸点和滑动方向
/*
功能：
	实现get_coordinates和get_direction的功能
用法：
	int touch_x,touch_y,direction;
	direction=get_coordinates_and_direction(&touch_x,&touch_y);
	返回值：UP、DOWN、LEFT、RIGHT。直接修改传入的touch_x和touch_y。
*/









//int lsdir(char *filenames,char path[]);
