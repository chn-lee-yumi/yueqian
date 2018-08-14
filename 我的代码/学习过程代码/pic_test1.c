#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#define ROW 480
#define COLUMN 800
char lcd_buf[ROW][COLUMN][4]={0};//行 列 BGRA灯
char img_buf[ROW][COLUMN][3]={0};//行 列 BGR

int main(){
    //打开图片和LCD设备
    int img_fd=open("huaji.bmp",O_RDONLY);
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
    for(int x=0;x<ROW;x++){
        for(int y=0;y<COLUMN;y++){
            lcd_buf[x][y][0]=img_buf[ROW-1-x][y][0];
            lcd_buf[x][y][1]=img_buf[ROW-1-x][y][1];
            lcd_buf[x][y][2]=img_buf[ROW-1-x][y][2];
            lcd_buf[x][y][3]=0;
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