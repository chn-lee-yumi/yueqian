//头文件好多不知道要包含哪些，直接复制上次的……
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

/*
任务1：播放图片的任务（广告）				由thread_autoplay函数完成
任务2：获取触摸屏的坐标。					由主线程完成
	滑动到上方，停止自动播放（手动播放）	pthread_cancel(thread_autoplay);
	滑动到左方，手动播放上一张
	滑动到右方，手动播放下一张
	
	滑动到下方，继续自动播放				pthread_create(&thread_autoplay,NULL,thread_autoplay,NULL);
*/

#define ROW 480
#define COLUMN 800
#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

void * thread_autoplay_func(void *arg);//autoplay函数
void * thread_handle_input_func(void *arg);//处理输入的函数
void create_thread_autoplay();//创建autoplay线程的函数
void create_thread_handle_input();//创建handplay线程的函数
void action(char direction);//响应滑动方向的函数
void init();//初始化函数
int is_type(char type[],char filename[]);//判断一个文件名是不是某个类型的文件
void show_pic_generic(char path[],int x,int y);//将单个图片放到目标位置

char filenames[255][255]={0};//不知道目录有多少个文件，给它255。
int filenum=0;//目录文件总数
int img_num=0;//当前播放的图片编号

pthread_t thread_autoplay,thread_handle_input;
char autoplaying=1;//是否在自动播放

char lcd_buf[ROW][COLUMN][4]={0};//行 列 BGRA灯
struct input_event input_buf;
int input_fd,lcd_fd;
int *lcd;//用于mmap

int main(){
	//初始化
	init();
	
	//创建线程
	create_thread_autoplay();
	create_thread_handle_input();
	
	//死循环
	while(1) sleep(99999);
}

void action(char direction){
	switch(direction){
    case UP://手动模式
		if(autoplaying){
			autoplaying=0;
			pthread_cancel(thread_autoplay);
		}
		break;
	case DOWN://自动模式
		if(!autoplaying){
			autoplaying=1;
			create_thread_autoplay();
		}
		break;
	case LEFT:
		if(!autoplaying){
			img_num--;
			if(img_num==-1) img_num=filenum-1;
			show_pic_generic(filenames[img_num],0,0);
		}
		break;
	case RIGHT:
		if(!autoplaying){
			img_num++;
			if(img_num==filenum) img_num=0;
			show_pic_generic(filenames[img_num],0,0);
		}
		break;
	}
}

void create_thread_autoplay(){
	int ret=pthread_create(&thread_autoplay,NULL,thread_autoplay_func,NULL);
	if(ret !=0)
	{
		perror("pthread_create error");
		exit(-1);
	}
}

void create_thread_handle_input(){
	int ret=pthread_create(&thread_handle_input,NULL,thread_handle_input_func,NULL);
	if(ret !=0)
	{
		perror("pthread_create error");
		exit(-1);
	}
}

void * thread_autoplay_func(void *arg){
	while(1)
	{
		img_num++;
		if(img_num==filenum) img_num=0;
		show_pic_generic(filenames[img_num],0,0);
		sleep(1);
	}
}

void * thread_handle_input_func(void *arg){
	//获取触摸屏输入，参考2048写的。
	int touch_x;int touch_y;
    int touch_x0;int touch_y0;int touch_x1;int touch_y1;
    while(1){
        read(input_fd,&input_buf,sizeof(input_buf));
        if(input_buf.type==3 && input_buf.code==0){
            touch_x=input_buf.value;
        }else if(input_buf.type==3 && input_buf.code==1){
            touch_y=input_buf.value;
        }else if(input_buf.type==1 && input_buf.code==0x14a){
            if(input_buf.value==1){
                touch_x0=touch_x;touch_y0=touch_y;
            }else{
                touch_x1=touch_x;touch_y1=touch_y;
                int dx=touch_x1-touch_x0;int dy=touch_y0-touch_y1;
                double tan0=(double)dy/dx;
                printf("tan0=%.4f ",tan0);
                if(dx>0){
                    if(tan0>-1 && tan0<1){
                        printf("右\n");
                        action(RIGHT);
                    }else if(tan0>=1){
                        printf("上\n");
                        action(UP);
                    }else{
                        printf("下\n");
                        action(DOWN);
                    }
                }else if(dx<0){
                    if(tan0>-1 && tan0<1){
                        printf("左\n");
                        action(LEFT);
                    }else if(tan0>=1){
                        printf("下\n");
                        action(DOWN);
                    }else{
                        printf("上\n");
                        action(UP);
                    }
                }else if(dy>0){
                    printf("上\n");
                    action(UP);
                }else if(dy<0){
                    printf("下\n");
                    action(DOWN);
                }else{
                    printf("没动\n");
                }
            }
        }
    }
}

void init(){
	//打开输入文件
    lcd_fd=open("/dev/fb0",O_RDWR);
    if(lcd_fd == -1){
        perror("error opening /dev/fb0");
        exit(-1);
    }
    printf("lcd_fd=%d\n",lcd_fd);
    input_fd=open("/dev/event0",O_RDONLY);
    if(input_fd == -1){
        perror("error opening /dev/event0");
        exit(-1);
    }
    printf("input_fd=%d\n",input_fd);
    //设置内存映射
    lcd=mmap(NULL,ROW*COLUMN*4,PROT_READ | PROT_WRITE,MAP_SHARED,lcd_fd,0);
	if(lcd == MAP_FAILED)
	{
		perror("mmap error");
		exit(-1);
	}
	//打开目录
    printf("Opening dir...\n");
  	DIR *dir = opendir("./");
  	if(dir == NULL){
  		perror("open dir error");
  		exit(-1);
  	}
  	//获取bmp文件列表（也就是题目要求的广告图片）
    printf("Getting BMP list...\n");
  	struct dirent *dirp;
  	while(1){
  		dirp = readdir(dir);
  		if(dirp == NULL) break;
      if(is_type(".bmp",dirp->d_name)){
    		strcpy(filenames[filenum],dirp->d_name);
        printf("%s\n",dirp->d_name);
        filenum++;
      }
  	}
    printf("Init complete.\n");
}

void show_pic_generic(char path[],int offset_x,int offset_y){ //解决了BMP内存对齐的问题
    int height,width; 
    //读取BMP
    int bmp_fd=open(path,O_RDONLY);
    if(bmp_fd == -1){
        perror("error opening bmp file");
        printf("filename: %s\n",path);
		exit(-1);
    }
    lseek(bmp_fd, 18, SEEK_SET);
    read(bmp_fd,&width,sizeof(int));
    read(bmp_fd,&height,sizeof(int));
    //printf("width=%d,height=%d\n",width,height);
    int empty_bytes=4-width*3%4;
    if(empty_bytes==4) empty_bytes=0;
    //printf("empty_bytes=%d\n",empty_bytes);
    char bmp_buf[height][width][3];
    lseek(bmp_fd, 54, SEEK_SET);
    for(int i=0;i<height;i++){
        read(bmp_fd,bmp_buf[i],width*3);
        lseek(bmp_fd,empty_bytes,SEEK_CUR);
    }
    close(bmp_fd);
    //写入到显存
    for(int y=0;y<height;y++){
        for(int x=0;x<width;x++){
            for(int z=0;z<3;z++)
                lcd_buf[y+offset_y][x+offset_x][z]=bmp_buf[height-1-y][x][z];
        }
    }
    memcpy(lcd,lcd_buf,sizeof(lcd_buf));
}

int is_type(char type[],char filename[]){
    char *p=strstr(filename,type);
    if(NULL != p){
        return 1;
    }else{
        return 0;
    }
}
