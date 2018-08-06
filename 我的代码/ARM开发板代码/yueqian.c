#include "yueqian.h"

char lcd_buf[ROW][COLUMN][4]={0};//行 列 BGRA灯
int lcd_fd,input_fd;
int *lcd;//用于mmap
struct input_event input_buf;

int init_uart(void);//初始化串口

void init_board(){
	//打开输入文件
    input_fd=open("/dev/event0",O_RDONLY);
    if(input_fd == -1){
        perror("error opening /dev/event0");
        exit(-1);
    }
    //打开 frame buffer
    lcd_fd=open("/dev/fb0",O_RDWR);
    if(lcd_fd == -1){
        perror("error opening /dev/fb0");
        exit(-1);
    }
    //设置内存映射
    lcd=mmap(NULL,ROW*COLUMN*4,PROT_READ | PROT_WRITE,MAP_SHARED,lcd_fd,0);
	if(lcd == MAP_FAILED)
	{
		perror("mmap error");
		exit(-1);
	}
  //初始化串口
  init_uart();
}

void uninit_board(){
    //结束
    munmap(lcd,ROW*COLUMN*4);
	close(lcd_fd);
	close(input_fd);
}

void refresh_input(){
    //重新打开输入文件
	close(input_fd);
	input_fd=open("/dev/event0",O_RDONLY);
    if(input_fd == -1){
        perror("error opening /dev/event0");
        exit(-1);
    }
}

void show_pic_generic(char path[],int offset_x,int offset_y){ //解决了BMP内存对齐的问题
    int height,width;
    //读取BMP
    int bmp_fd=open(path,O_RDONLY);
    if(bmp_fd == -1){
        perror("error opening bmp file");
        printf("filename: %s\n",path);
		exit(-1);
    }
    lseek(bmp_fd, 18, SEEK_SET);
    read(bmp_fd,&width,sizeof(int));
    read(bmp_fd,&height,sizeof(int));
    int empty_bytes=4-width*3%4;
    if(empty_bytes==4) empty_bytes=0;
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

int is_type(char type[],char filename[]){
    char *p=strstr(filename,type);
    if(NULL != p){
        return 1;
    }else{
        return 0;
    }
}

void get_coordinates(int *touch_x,int *touch_y){
	//读取输入
    while(1){
        read(input_fd,&input_buf,sizeof(input_buf));
        //type=0,code=0,value=0 同步
        //type=3,code=0,value=x坐标 触摸屏
        //type=3,code=1,value=y坐标 触摸屏
        //type=1,code=14a,value=是否按下 1表示按下，0表示离开 触摸屏
        if(input_buf.type==3 && input_buf.code==0){
            *touch_x=input_buf.value;
        }else if(input_buf.type==3 && input_buf.code==1){
            *touch_y=input_buf.value;
        }else if(input_buf.type==1 && input_buf.code==0x14a){
            if(input_buf.value==0){
                return;
            }
        }
    }
}

int get_direction(){
	//读取输入，判断滑动方向
    int touch_x;int touch_y;
    int touch_x0;int touch_y0;int touch_x1;int touch_y1;
    while(1){
        read(input_fd,&input_buf,sizeof(input_buf));
        if(input_buf.type==3 && input_buf.code==0){
            touch_x=input_buf.value;
        }else if(input_buf.type==3 && input_buf.code==1){
            touch_y=input_buf.value;
        }else if(input_buf.type==1 && input_buf.code==0x14a){
            if(input_buf.value==1){
                touch_x0=touch_x;touch_y0=touch_y;
            }else{
                touch_x1=touch_x;touch_y1=touch_y;
                //计算四个方向，并调用数字整合函数进行处理
                int dx=touch_x1-touch_x0;int dy=touch_y0-touch_y1;
                double tan0=(double)dy/dx;
                printf("tan0=%.4f ",tan0);
                if(dx>0){
                    if(tan0>-1 && tan0<1){
                        printf("右\n");
                        return(RIGHT);
                    }else if(tan0>=1){
                        printf("上\n");
                        return(UP);
                    }else{
                        printf("下\n");
                        return(DOWN);
                    }
                }else if(dx<0){
                    if(tan0>-1 && tan0<1){
                        printf("左\n");
                        return(LEFT);
                    }else if(tan0>=1){
                        printf("下\n");
                        return(DOWN);
                    }else{
                        printf("上\n");
                        return(UP);
                    }
                }else if(dy>0){
                    printf("上\n");
                    return(UP);
                }else if(dy<0){
                    printf("下\n");
                    return(DOWN);
                }else{
                    printf("没动\n");
                }
            }
        }
    }
}

int get_coordinates_and_direction(int *touch_x,int *touch_y){//同时获取点击和方向
	int x;int y;
    int touch_x0;int touch_y0;int touch_x1;int touch_y1;
    while(1){
        read(input_fd,&input_buf,sizeof(input_buf));
        if(input_buf.type==3 && input_buf.code==0){
            *touch_x=x=input_buf.value;
        }else if(input_buf.type==3 && input_buf.code==1){
            *touch_y=y=input_buf.value;
        }else if(input_buf.type==1 && input_buf.code==0x14a){
            if(input_buf.value==1){
                touch_x0=x;touch_y0=y;
            }else{
                touch_x1=x;touch_y1=y;
                //计算四个方向，并调用数字整合函数进行处理
                int dx=touch_x1-touch_x0;int dy=touch_y0-touch_y1;
                double tan0=(double)dy/dx;
                printf("tan0=%.4f ",tan0);
                if(dx>0){
                    if(tan0>-1 && tan0<1){
                        printf("右\n");
                        return(RIGHT);
                    }else if(tan0>=1){
                        printf("上\n");
                        return(UP);
                    }else{
                        printf("下\n");
                        return(DOWN);
                    }
                }else if(dx<0){
                    if(tan0>-1 && tan0<1){
                        printf("左\n");
                        return(LEFT);
                    }else if(tan0>=1){
                        printf("下\n");
                        return(DOWN);
                    }else{
                        printf("上\n");
                        return(UP);
                    }
                }else if(dy>0){
                    printf("上\n");
                    return(UP);
                }else if(dy<0){
                    printf("下\n");
                    return(DOWN);
                }else{
                    printf("没动\n");
					return(NOT_MOVED);
                }
            }
        }
    }
}


#define DEVICE_NAME "/dev/s3c2410_serial3"
int ser_fd;
int set_serial_uart1(int ser_fd)
{

	struct termios termios_new;
	bzero( &termios_new, sizeof(termios_new));
	/*原始模式*/
	cfmakeraw(&termios_new);

	/*波特率为115200*/
	termios_new.c_cflag=(B115200);
	termios_new.c_cflag |= CLOCAL | CREAD;

	/*8位数据位*/
	termios_new.c_cflag &= ~CSIZE;
	termios_new.c_cflag |= CS8;

	/*无奇偶校验位*/
	termios_new.c_cflag &= ~PARENB;

	/*1位停止位*/
	termios_new.c_cflag &= ~CSTOPB;
	/*清除串口缓冲区*/
	tcflush( ser_fd,TCIOFLUSH);
	termios_new.c_cc[VTIME] = 0;
	termios_new.c_cc[VMIN] = 4;
	tcflush ( ser_fd, TCIOFLUSH);
	/*串口设置使能*/
	tcsetattr( ser_fd ,TCSANOW,&termios_new);
}

int init_uart(void)
{
	char message[6];
	//打开串口+初始化
	ser_fd = open(DEVICE_NAME,O_RDWR);
	if(ser_fd == -1)
	{
		printf("serical open failed!\n");
		return -1;
	}
	set_serial_uart1(ser_fd);

	if(fcntl(ser_fd,F_SETFL,0)<0)
	{
		exit(1);
	}
	tcdrain(ser_fd);   //通用终端控制
	tcflush(ser_fd, TCIOFLUSH); //刷新缓冲区

}

void uart_send_data(const void * data){
    write(ser_fd,data,sizeof(data));
}

void uart_read_data(void * data,int size){
    read(ser_fd,data,size);
}




/*
int lsdir(char **filenames,char path[]){
	int filenum=0;
	//打开目录
  	DIR *dir = opendir(path);
  	if(dir == NULL){
  		perror("open dir error");
  		exit(-1);
  	}
  	//获取文件列表
  	struct dirent *dirp;
  	while(1){
  		dirp = readdir(dir);
  		if(dirp == NULL) break;
    	strcpy(filenames[filenum],dirp->d_name);
        filenum++;
    }
	return filenum;
}*/
