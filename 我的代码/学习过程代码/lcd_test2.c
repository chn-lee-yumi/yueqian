#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#define ROW 480
#define COLUMN 800
#define FILE_SIZE ROW*COLUMN*4

unsigned int color[8]={0xFF0000,0xFF7F00,0xFFFF00,0x00ff00,0x00FFFF,0x0000FF,0x8B00FF,0xffffff}; //BGR
int main(){
    int fd=open("/dev/fb0",O_RDWR);
    char buf[FILE_SIZE]={0};
    if(fd == -1){
        perror("error read file");
    }else{
        printf("fd=%d\n",fd);
        int k=0;
        for(int j=0;j<ROW;j++){
            for(int i=0;i<COLUMN*4;i+=4){
                buf[i+j*COLUMN*4]=color[k%6]>>16;
                buf[i+j*COLUMN*4+1]=color[k%6]>>8&0xFF;
                buf[i+j*COLUMN*4+2]=color[k%6]&0xFF;
                buf[i+j*COLUMN*4+3]=0;
            }
            if(j%8==0){
                k++;
            }
        }
        ssize_t write_size=write(fd,buf,FILE_SIZE);
        if(write_size==-1){
            perror("error write file");
        }else{
            printf("%d bytes writed.",write_size);
        }
    }
}