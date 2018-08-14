#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

struct student{
    char name[20];
    char year;
    unsigned int number;
};

int main(){
    int fd=open("1.txt",O_RDWR);
    if(fd == -1){
        perror("error read file");
        return -1;
    }
    printf("fd=%d\n",fd);
    
    struct student buf;
    strcpy(buf.name,"zhangsan");
    buf.year=20;
    buf.number=3115001234U;
    
    ssize_t write_size=write(fd,&buf,sizeof(buf));
    if(write_size==-1){
        perror("error write file");
        return -1;
    }
    printf("%d bytes writed.\n",write_size);
    
    off_t offset=0;
    off_t off = lseek(fd, offset, SEEK_SET);
    struct student read_buf;
    ssize_t read_size=read(fd,&read_buf,200);
    if(read_size==-1){
        perror("error read file");
        return -1;
    }
    printf("%d bytes readed.\n",read_size);
    printf("name:%s\nyear:%u\nnumber:%u\n",read_buf.name,read_buf.year,read_buf.number);
    
    return 0;
}