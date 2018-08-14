#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <linux/input.h>
#include <sys/mman.h>
#include <string.h>


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
#define BMP_PATH "huaji.bmp"
#define PIECE 3
char lcd_buf[ROW][COLUMN][4]={0};//行 列 BGRA灯
char bmp_buf[ROW][COLUMN+COLUMN%4][3]={0};//行 列 BGR
struct input_event input_buf;
int input_fd,lcd_fd;
int *lcd;

int init();
void uninit();
void show_pic(int x,int y,int w,int h);

int main(){
    //初始化
    int init_status=init();
    if(init_status!=0){
        uninit();
        return -1;
    }
    
    //读取输入
    int touch_x;int touch_y;
    while(1){
        read(input_fd,&input_buf,sizeof(input_buf));
        //type=0,code=0,value=0 同步
        //type=3,code=0,value=x坐标 触摸屏
        //type=3,code=1,value=y坐标 触摸屏
        //type=1,code=14a,value=是否按下 1表示按下，0表示离开 触摸屏
        if(input_buf.type==3 && input_buf.code==0){
            printf("X=%d ",input_buf.value);
            touch_x=input_buf.value;
        }else if(input_buf.type==3 && input_buf.code==1){
            printf("Y=%d ",input_buf.value);
            touch_y=input_buf.value;
        }else if(input_buf.type==1 && input_buf.code==0x14a){
            printf("Press=%d ",input_buf.value);
            if(input_buf.value==0){
                show_pic(
                    touch_x/(COLUMN/PIECE)*(COLUMN/PIECE),
                    touch_y/(ROW/PIECE)*(ROW/PIECE),
                    COLUMN/PIECE,
                    ROW/PIECE
                );
                /*
                void show_pic(int x,int y,int w,int h)
                x图片左上角坐标：touch_x/(COLUMN/PIECE)*(COLUMN/PIECE)
                y图片左上角坐标：touch_y/(ROW/PIECE)*(ROW/PIECE)
                w宽度（长度）：COLUMN/PIECE
                h高度：ROW/PIECE
                在九宫格里，PIECE=3。因此一个格子的宽度和高度就是COLUMN/PIECE和ROW/PIECE
                关于touch_x/(COLUMN/PIECE)*(COLUMN/PIECE)，斜杠/是整除，
                touch_x整除COLUMN/PIECE，可能的值是0、1、2，
                然后再乘以COLUMN/PIECE，就能得到九宫格一行三个图的x坐标。
                y坐标同理。
                */
            }
        }
        if(input_buf.type==0 && input_buf.code==0)  printf("\n");
    }
    
    //反初始化
    uninit();
    return 0;
}

void show_pic(int x,int y,int w,int h){
    for(int j=y;j<y+h;j++){//行
        for(int i=x;i<x+w;i++){//列
            for(int z=0;z<3;z++){
                lcd_buf[j][i][z]=bmp_buf[ROW-1-j][i][z];
            }
        }
    }
    memcpy(lcd,lcd_buf,sizeof(lcd_buf));
}






int init(){
    //读取图片
    printf("reading bmp...\n");
    int bmp_fd=open(BMP_PATH,O_RDONLY);
    if(bmp_fd == -1){
        perror("error opening bmp file");
        return -1;
    }
    off_t offset=54;
    off_t off=lseek(bmp_fd, offset, SEEK_SET);
    ssize_t read_size=read(bmp_fd,bmp_buf,sizeof(bmp_buf));
    printf("off=%d,read_size=%d\n",off,read_size);
    close(bmp_fd);
    //打开输入文件
    lcd_fd=open("/dev/fb0",O_RDWR);
    if(lcd_fd == -1){
        perror("error opening /dev/fb0");
        return -1;
    }
    printf("lcd_fd=%d\n",lcd_fd);
    input_fd=open("/dev/event0",O_RDONLY);
    if(input_fd == -1){
        perror("error opening /dev/event0");
        return -1;
    }
    printf("input_fd=%d\n",input_fd);
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
	close(input_fd);
	close(lcd_fd);
}