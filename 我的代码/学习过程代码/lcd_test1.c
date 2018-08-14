#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#define FILE_SIZE 800*480*4

int main(){
    int fd=open("/dev/fb0",O_RDWR);
    char buf[FILE_SIZE]={0};
    if(fd == -1){
        perror("error read file");
    }else{
        printf("fd=%d\n",fd);
        for(int i=0;i<FILE_SIZE;i++){
            if(i%4==1){
                buf[i]=255;
            }else{
                buf[i]=0;
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