#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <linux/input.h>
/*
struct input_event {
	struct timeval time;
	__u16 type;
	__u16 code;
	__s32 value;
};
*/
/*
type=3,code=0,value=195
type=3,code=1,value=319
type=1,code=14a,value=1
type=0,code=0,value=0
type=1,code=14a,value=0
type=0,code=0,value=0

type=3,code=0,value=133
type=3,code=1,value=322
type=1,code=14a,value=1
type=0,code=0,value=0
type=3,code=0,value=136
type=3,code=1,value=321
type=0,code=0,value=0
type=3,code=0,value=137
type=3,code=1,value=318
type=0,code=0,value=0
type=1,code=14a,value=0
type=0,code=0,value=0

type=3,code=0,value=174
type=3,code=1,value=232
type=1,code=14a,value=1
type=0,code=0,value=0
type=3,code=0,value=177
type=3,code=1,value=231
type=0,code=0,value=0
type=3,code=0,value=178
type=3,code=1,value=228
type=0,code=0,value=0
type=3,code=0,value=180
type=3,code=1,value=226
type=0,code=0,value=0
type=1,code=14a,value=0
type=0,code=0,value=0
*/

#define ROW 480
#define COLUMN 800
char lcd_buf[ROW][COLUMN][4]={0};//行 列 BGRA灯
struct input_event input_buf;
int input_fd;

void debug();

int main(){
    //打开输入文件
    input_fd=open("/dev/event0",O_RDONLY);
    if(input_fd == -1){
        perror("error read file(huaji.bmp)");
        return -1;
    }
    printf("input_fd=%d\n",input_fd);
    
    //debug();
    
    //读取输入
    while(1){
        read(input_fd,&input_buf,sizeof(input_buf));
        //type=0,code=0,value=0 同步
        //type=3,code=0,value=x坐标 触摸屏
        //type=3,code=1,value=y坐标 触摸屏
        //type=1,code=14a,value=是否按下 1表示按下，0表示离开 触摸屏
        if(input_buf.type==3 && input_buf.code==0)  printf("X=%d ",input_buf.value);
        if(input_buf.type==3 && input_buf.code==1)  printf("Y=%d\n",input_buf.value);
    }
    
    return 0;
}


void debug(){
    while(1){
        read(input_fd,&input_buf,sizeof(input_buf));
        printf("type=%x,code=%x,value=%d\n",input_buf.type,input_buf.code,input_buf.value);
    }
}