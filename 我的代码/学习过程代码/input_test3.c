#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <linux/input.h>
#include <sys/mman.h>
#include <string.h>

struct input_event input_buf;
int input_fd;

int init();
void uninit();

int main(){
    //初始化
    int init_status=init();
    if(init_status!=0){
        uninit();
        return -1;
    }
    
    //读取输入
    int touch_x;int touch_y;
    int touch_x0;int touch_y0;int touch_x1;int touch_y1;
    while(1){
        read(input_fd,&input_buf,sizeof(input_buf));
        //type=0,code=0,value=0 同步
        //type=3,code=0,value=x坐标 触摸屏
        //type=3,code=1,value=y坐标 触摸屏
        //type=1,code=14a,value=是否按下 1表示按下，0表示离开 触摸屏
        if(input_buf.type==3 && input_buf.code==0){
            touch_x=input_buf.value;
        }else if(input_buf.type==3 && input_buf.code==1){
            touch_y=input_buf.value;
        }else if(input_buf.type==1 && input_buf.code==0x14a){
            if(input_buf.value==1){
                touch_x0=touch_x;touch_y0=touch_y;
            }else{
                touch_x1=touch_x;touch_y1=touch_y;
                //计算12个方向
                int dx=touch_x1-touch_x0;int dy=touch_y0-touch_y1;
                double tan0=(double)dy/dx;
                printf("tan0=%.4f ",tan0);
                if(dx>0){
                    if(tan0>-0.2679 && tan0<0.2679){
                        printf("右\n");
                    }else if(tan0>=0.2679 && tan0<1){
                        printf("右上偏右\n");
                    }else if(tan0>=1 && tan0<3.732){
                        printf("右上偏上\n");
                    }else if(tan0>=3.732){
                        printf("上\n");
                    }else if(tan0>=-1 && tan0<-0.2679){
                        printf("右下偏右\n");
                    }else if(tan0>=-3.732 && tan0<-1){
                        printf("右下偏下\n");
                    }else{
                        printf("下\n");
                    }
                }else if(dx<0){
                    if(tan0>-0.2679 && tan0<0.2679){
                        printf("左\n");
                    }else if(tan0>=0.2679 && tan0<1){
                        printf("左下偏左\n");
                    }else if(tan0>=1 && tan0<3.732){
                        printf("左下偏下\n");
                    }else if(tan0>=3.732){
                        printf("下\n");
                    }else if(tan0>=-1 && tan0<-0.2679){
                        printf("左上偏左\n");
                    }else if(tan0>=-3.732 && tan0<-1){
                        printf("左上偏上\n");
                    }else{
                        printf("上\n");
                    }
                }else if(dy>0){
                    printf("上\n");
                }else if(dy<0){
                    printf("下\n");
                }else{
                    printf("没动\n");
                }
            }
        }
    }
    
    //反初始化
    uninit();
    return 0;
}


int init(){
    //打开输入文件
    input_fd=open("/dev/event0",O_RDONLY);
    if(input_fd == -1){
        perror("error opening /dev/event0");
        return -1;
    }
    printf("input_fd=%d\n",input_fd);
	return 0;
}

void uninit(){
    //结束
	close(input_fd);
}