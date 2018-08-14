#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#define ROW 480
#define COLUMN 800
#define IMGROW 111
#define IMGCOLUMN 111
#define OFFSET_X 200
#define OFFSET_Y 200
char lcd_buf[ROW][COLUMN][4]={0};//行 列 BGRA灯
char img_buf[IMGROW][IMGCOLUMN+1][3]={0};//行 列 BGR

int main(){
    //打开图片和LCD设备
    int img_fd=open("huaji3.bmp",O_RDONLY);
    if(img_fd == -1){
        perror("error read file(huaji.bmp)");
        return -1;
    }
    printf("img_fd=%d\n",img_fd);
    int fd=open("/dev/fb0",O_RDWR);
    if(fd == -1){
        perror("error read file(/dev/fb0)");
        return -1;
    }
    printf("fd=%d\n",fd);
    
    //读取图片
    off_t offset=54;
    off_t off=lseek(img_fd, offset, SEEK_SET);
    ssize_t read_size=read(img_fd,img_buf,sizeof(img_buf));
    for(int x=0;x<IMGROW;x++){
        for(int y=0;y<IMGCOLUMN;y++){
            for(int z=0;z<3;z++)
            lcd_buf[x+OFFSET_X][y+OFFSET_Y][z]=img_buf[IMGROW-1-x][y][z];
        }
    }
    
    //写入LCD设备
    ssize_t write_size=write(fd,lcd_buf,sizeof(lcd_buf));
    if(write_size==-1){
        perror("error write file(/dev/fb0)");
        return -1;
    }
    printf("%d bytes writed.",write_size);
    
    return 0;
}