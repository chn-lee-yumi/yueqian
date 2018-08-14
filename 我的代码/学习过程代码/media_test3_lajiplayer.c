//实现触摸屏点击屏幕，能够播放、暂停、继续、上下一首音乐
//已知BUG：刚运行程序时直接点击上一首会有段错误。
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
#define ROW 480
#define COLUMN 800
void init();
int is_type(char type[],char filename[]);
char filenames[255][255]={0};
int filenum=0;
int main()
{
    init();
    system("killall iot");
    //MP3列表为filenames[0]~filenames[filenum-1]
    //判断按钮
    int playing_num=-1;//正在播放第几首，范围0~filenum-1，当playing_num=-1或playing_num=filenum时变为filenum-1或0
    char is_playing=0;//是否正在播放
    char cmd[255];//播放MP3的命令
    //读取输入
    int touch_x;int touch_y;
    int input_fd;
    struct input_event input_buf;
    input_fd=open("/dev/event0",O_RDONLY);
    while(1){
        read(input_fd,&input_buf,sizeof(input_buf));
        if(input_buf.type==3 && input_buf.code==0){
            touch_x=input_buf.value;
        }else if(input_buf.type==3 && input_buf.code==1){
            touch_y=input_buf.value;
        }else if(input_buf.type==1 && input_buf.code==0x14a){
            if(input_buf.value==0  && touch_y>=260 && touch_y <=400){
                //判断按钮范围
                if(touch_x>180 && touch_x <=305){//上一首
                    playing_num--;
                    if(playing_num==-1) playing_num=filenum-1;
                    system("killall madplay");
                    sprintf(cmd,"madplay \"%s\" &", filenames[playing_num]);
                    printf("madplay \"%s\" &\n", filenames[playing_num]);
                    system(cmd);
                    is_playing=1;
                }else if(touch_x>305 && touch_x <=500){//播放、暂停
                    if(playing_num==-1 && is_playing==0){
                      playing_num=0;
                      is_playing=1;
                      sprintf(cmd,"madplay \"%s\" &", filenames[0]);
                      printf("madplay \"%s\" &\n", filenames[playing_num]);
                      system(cmd);
                    }else if(is_playing==1){
                        system("killall -STOP madplay");
                        is_playing=0;
                    }else{
                        system("killall -CONT madplay");
                        is_playing=1;
                    }
                }else if(touch_x>305 && touch_x <=630){//下一首
                    playing_num++;
                    if(playing_num==filenum) playing_num=0;
                    system("killall madplay");
                    sprintf(cmd,"madplay \"%s\" &", filenames[playing_num]);
                    printf("madplay \"%s\" &\n", filenames[playing_num]);
                    system(cmd);
                    is_playing=1;
                }
            }
        }
    }
}

int is_type(char type[],char filename[]){
    char *p=strstr(filename,type);
    if(NULL != p){
        return 1;
    }else{
        return 0;
    }
}

void init(){
    //打开输入文件
    int lcd_fd=open("/dev/fb0",O_RDWR);
    //打开背景
    int img_fd=open("lajiplayer.bmp",O_RDONLY);
    if(img_fd == -1){
        perror("error read file(lajiplayer.bmp)");
        exit(-1);
    }
    //读取图片和写入背景
    printf("Reading bg pic...\n");
    char lcd_buf[ROW][COLUMN][4];
    char img_buf[ROW][COLUMN][3];
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
    printf("Writing LCD...\n");
    ssize_t write_size=write(lcd_fd,lcd_buf,sizeof(lcd_buf));
    close(img_fd);
    close(lcd_fd);

    //打开目录
    printf("Opening dir...\n");
  	DIR *dir = opendir("./");
  	if(dir == NULL){
  		perror("open dir error");
  		exit(-1);
  	}
  	//获取mp3文件列表
    printf("Getting MP3 list...\n");
  	struct dirent *dirp;
  	while(1){
  		dirp = readdir(dir);
  		if(dirp == NULL) break;
      if(is_type(".mp3",dirp->d_name)){
    		strcpy(filenames[filenum],dirp->d_name);
        printf("%s\n",dirp->d_name);
        filenum++;
      }
  	}
    printf("Init complete.\n");
}
