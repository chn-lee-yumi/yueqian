#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <math.h>
#define ROW 480
#define COLUMN 800
#define FILE_SIZE ROW*COLUMN*4
#define WIDE 8

unsigned int color[8]={0xFF0000,0xFF7F00,0xFFFF00,0x00ff00,0x00FFFF,0x0000FF,0x8B00FF,0xffffff}; //BGR
int main(){
    int fd=open("/dev/fb0",O_RDWR);
    char buf[ROW][COLUMN][4]={0};//行 列 BGRA灯
    int center_x=400;
    int center_y=480;
    if(fd == -1){
        perror("error read file");
    }else{
        printf("fd=%d\n",fd);
        for(int x=0;x<COLUMN;x++){
            for(int y=0;y<ROW;y++){
                int dx=x-center_x;
                int dy=y-center_y;
                double r=sqrt(dx*dx+dy*dy);
                int color_num=((int)r/WIDE)%8;
                buf[y][x][0]=color[color_num%8]>>16;
                buf[y][x][1]=color[color_num%8]>>8&0xFF;
                buf[y][x][2]=color[color_num%8]&0xFF;
                //printf("y=%d,r=%f\n",y,r);
            }
            //printf("x=%d\n",x);
        }
        printf("writing...");
        ssize_t write_size=write(fd,buf,FILE_SIZE);
        if(write_size==-1){
            perror("error write file");
        }else{
            printf("%d bytes writed.",write_size);
        }
    }
}