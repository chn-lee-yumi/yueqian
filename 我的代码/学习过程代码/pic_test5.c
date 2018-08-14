//小游戏：2048
//硬件：粤嵌ARM开发板
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <linux/input.h>
#include <sys/mman.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define ROW 480
#define COLUMN 800
//4x4的矩阵，一个框大小200*120
short map[4][4]={0};//2048地图矩阵，坐标系同显示屏，左上角为原点//128,64,0,0,256,32,0,0,512,16,2,0,1024,8,4,0 //测试数组
char lcd_buf[ROW][COLUMN][4]={0};//行 列 BGRA灯
struct input_event input_buf;
int input_fd,lcd_fd;
int *lcd;

//基础函数
int init();
void uninit();
void show_pic_generic(char path[],int x,int y);//将单个图片放到目标位置

int main(){
    //初始化
    int init_status=init();
    if(init_status!=0){
        uninit();
        return -1;
    }
    show_pic_generic("3.bmp",176,0);
}

void show_pic_generic(char path[],int offset_x,int offset_y){ //解决了BMP内存对齐的问题
    int height,width; 
    //读取BMP
    int bmp_fd=open(path,O_RDONLY);
    if(bmp_fd == -1){
        perror("error opening bmp file");
        printf("filename: %s\n",path);
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

int init(){
    //打开输入文件
    lcd_fd=open("/dev/fb0",O_RDWR);
    if(lcd_fd == -1){
        perror("error opening /dev/fb0");
        return -1;
    }
    printf("lcd_fd=%d\n",lcd_fd);
    //设置内存映射
    lcd=mmap(NULL,ROW*COLUMN*4,PROT_READ | PROT_WRITE,MAP_SHARED,lcd_fd,0);
	if(lcd == MAP_FAILED)
	{
		perror("mmap error");
		return -1;
	}
	return 0;
}

void uninit(){
    //结束
    munmap(lcd,ROW*COLUMN*4);
	close(lcd_fd);
}
