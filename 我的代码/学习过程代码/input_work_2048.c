//小游戏：2048
//硬件：粤嵌ARM开发板
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <linux/input.h>
#include <sys/mman.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define ROW 480
#define COLUMN 800
//4x4的矩阵，一个框大小200*120
short map[4][4]={0};//2048地图矩阵，坐标系同显示屏，左上角为原点//128,64,0,0,256,32,0,0,512,16,2,0,1024,8,4,0 //测试数组
char lcd_buf[ROW][COLUMN][4]={0};//行 列 BGRA灯
struct input_event input_buf;
int input_fd,lcd_fd;
int *lcd;

//基础函数
int init();
void uninit();
void show_pic_generic(char path[],int x,int y);//将单个图片放到目标位置
//游戏函数
void refresh_screen();//刷新整个屏幕
void join_numbers(char direction);//数字整合
void add_number();//产生随机数字
void show_pic(int number,int x,int y);//将单个图片放到目标位置
#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

int main(){
    //初始化
    int init_status=init();
    if(init_status!=0){
        uninit();
        return -1;
    }
    add_number();add_number();
    //add_number();add_number();add_number();add_number();
    refresh_screen();
    //读取输入，判断滑动方向
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
                //计算四个方向，并调用数字整合函数进行处理
                int dx=touch_x1-touch_x0;int dy=touch_y0-touch_y1;
                double tan0=(double)dy/dx;
                printf("tan0=%.4f ",tan0);
                if(dx>0){
                    if(tan0>-1 && tan0<1){
                        printf("右\n");
                        join_numbers(RIGHT);
                    }else if(tan0>=1){
                        printf("上\n");
                        join_numbers(UP);
                    }else{
                        printf("下\n");
                        join_numbers(DOWN);
                    }
                }else if(dx<0){
                    if(tan0>-1 && tan0<1){
                        printf("左\n");
                        join_numbers(LEFT);
                    }else if(tan0>=1){
                        printf("下\n");
                        join_numbers(DOWN);
                    }else{
                        printf("上\n");
                        join_numbers(UP);
                    }
                }else if(dy>0){
                    printf("上\n");
                    join_numbers(UP);
                }else if(dy<0){
                    printf("下\n");
                    join_numbers(DOWN);
                }else{
                    printf("没动\n");
                }
            }
        }
    }
}

void join_numbers(char direction){
    char is_moved=0;//如果画面无改变，就不产生新的数字
    switch(direction){
    case UP:
        for(int i=0;i<4;i++){//列，从左到右判断
            //数字排序
            int zero_point=99;//寻找空块，压缩空间
            for(int j=0;j<4;j++){//行
                //printf("行=%d,zero_point=%d\n",j,zero_point);
                if(map[j][i]==0 && zero_point>j){
                    zero_point=j;
                    //printf("!!行=%d,zero_point=%d\n",j,zero_point);
                }else{
                    if(zero_point!=99 && map[j][i]!=0){
                        map[zero_point][i]=map[j][i];
                        map[j][i]=0;
                        zero_point++;
                        is_moved=1;
                    }
                }
            }
            //数字相加
            for(int j=0;j<3;j++){
                if(map[j][i]==map[j+1][i] && map[j][i]!=0){
                    map[j][i]*=2;
                    map[j+1][i]=0;
                    j++;
                    is_moved=1;
                }
            }
            //数字排序(第二次，代码同第一次)
            zero_point=99;
            for(int j=0;j<4;j++){
                if(map[j][i]==0 && zero_point>j){
                    zero_point=j;
                }else{
                    if(zero_point!=99 && map[j][i]!=0){
                        map[zero_point][i]=map[j][i];
                        map[j][i]=0;
                        zero_point++;
                        is_moved=1;
                    }
                }
            }
        }
        break;
    case DOWN:
        for(int i=0;i<4;i++){//列，从左到右判断
            //数字排序
            int zero_point=-99;
            for(int j=3;j>=0;j--){
                if(map[j][i]==0 && zero_point<j){
                    zero_point=j;
                }else{
                    if(zero_point!=-99 && map[j][i]!=0){
                        map[zero_point][i]=map[j][i];
                        map[j][i]=0;
                        zero_point--;
                        is_moved=1;
                    }
                }
            }
            //数字相加
            for(int j=3;j>0;j--){
                if(map[j][i]==map[j-1][i] && map[j][i]!=0){
                    map[j][i]*=2;
                    map[j-1][i]=0;
                    j++;
                    is_moved=1;
                }
            }
            //数字排序(第二次，代码同第一次)
            zero_point=-99;
            for(int j=3;j>=0;j--){
                if(map[j][i]==0 && zero_point<j){
                    zero_point=j;
                }else{
                    if(zero_point!=-99 && map[j][i]!=0){
                        map[zero_point][i]=map[j][i];
                        map[j][i]=0;
                        zero_point--;
                        is_moved=1;
                    }
                }
            }
        }
        break;
    case LEFT:
        for(int j=0;j<4;j++){//行，从上到下判断
            //数字排序
            int zero_point=99;
            for(int i=0;i<4;i++){
                if(map[j][i]==0 && zero_point>i){
                    zero_point=i;
                }else{
                    if(zero_point!=99 && map[j][i]!=0){
                        map[j][zero_point]=map[j][i];
                        map[j][i]=0;
                        zero_point++;
                        is_moved=1;
                    }
                }
            }
            //数字相加
            for(int i=0;i<3;i++){
                if(map[j][i]==map[j][i+1] && map[j][i]!=0){
                    map[j][i]*=2;
                    map[j][i+1]=0;
                    i++;
                    is_moved=1;
                }
            }
            //数字排序(第二次，代码同第一次)
            zero_point=99;
            for(int i=0;i<4;i++){
                if(map[j][i]==0 && zero_point>i){
                    zero_point=i;
                }else{
                    if(zero_point!=99 && map[j][i]!=0){
                        map[j][zero_point]=map[j][i];
                        map[j][i]=0;
                        zero_point++;
                        is_moved=1;
                    }
                }
            }
        }
        break;
    case RIGHT:
        for(int j=0;j<4;j++){//行，从上到下判断
            //数字排序
            int zero_point=-99;
            for(int i=3;i>=0;i--){
                if(map[j][i]==0 && zero_point<i){
                    zero_point=i;
                }else{
                    if(zero_point!=-99 && map[j][i]!=0){
                        map[j][zero_point]=map[j][i];
                        map[j][i]=0;
                        zero_point--;
                        is_moved=1;
                    }
                }
            }
            //数字相加
            for(int i=3;i>0;i--){
                if(map[j][i]==map[j][i-1] && map[j][i]!=0){
                    map[j][i]*=2;
                    map[j][i-1]=0;
                    i++;
                    is_moved=1;
                }
            }
            //数字排序(第二次，代码同第一次)
            zero_point=-99;
            for(int i=3;i>=0;i--){
                if(map[j][i]==0 && zero_point<i){
                    zero_point=i;
                }else{
                    if(zero_point!=-99 && map[j][i]!=0){
                        map[j][zero_point]=map[j][i];
                        map[j][i]=0;
                        zero_point--;
                        is_moved=1;
                    }
                }
            }
        }
        break;
    }
    //判断有没有2048出现，如果有，赢了
    for(int i=0;i<4;i++){
        for(int j=0;j<4;j++){
            if(map[i][j]==2048){
                printf("赢了赢了赢了赢了赢了赢了赢了赢了\n");
                //放图，结束游戏
                refresh_screen();
                show_pic_generic("win.bmp",160,0);
                uninit();//反初始化
                exit(0);
            }
        }
    }
    //如果画面有变化
    if(is_moved==1){
        //产生随机数字
        add_number();
        //刷新屏幕
        refresh_screen();
    }else{//判断是不是输了
        char is_lost=1;
        for(int i=0;i<4;i++){//判断行能否相加
            for(int j=0;j<3;j++){
                if(map[i][j]==map[i][j+1]){
                    is_lost=0;
                    break;
                }
            }
            if(is_lost==0) break;
        }
        if(is_lost==1){
            for(int j=0;j<4;j++){//判断列能否相加
                for(int i=0;i<3;i++){
                    if(map[i][j]==map[i+1][j]){
                        is_lost=0;
                        break;
                    }
                }
                if(is_lost==0) break;
            }
        }
        if(is_lost==1){//输了
            printf("输了输了输了输了输了输了输了输了\n");
            //放图，结束游戏
            refresh_screen();
            show_pic_generic("lost.bmp",213,0);
            uninit();//反初始化
            exit(0);
        }
        
    }
}

void show_pic_generic(char path[],int offset_x,int offset_y){
    int width_new,height,width; 
    //读取BMP
    int bmp_fd=open(path,O_RDONLY);
    if(bmp_fd == -1){
        perror("error opening bmp file");
        printf("filename: %s\n",path);
    }
    lseek(bmp_fd, 18, SEEK_SET);
    read(bmp_fd,&width,sizeof(int));
    read(bmp_fd,&height,sizeof(int));
    width_new=width;
    //TODO：解决四字节对齐问题 https://blog.csdn.net/jkhere/article/details/8155017
    char bmp_buf[height][width_new][3];
    lseek(bmp_fd, 54, SEEK_SET);
    read(bmp_fd,bmp_buf,sizeof(bmp_buf));
    close(bmp_fd);
    printf("width=%d,width_new=%d,height=%d\n",width,width_new,height);
    //写入到显存
    for(int y=0;y<height;y++){
        for(int x=0;x<width;x++){
            for(int z=0;z<3;z++)
                lcd_buf[y+offset_y][x+offset_x][z]=bmp_buf[height-1-y][x][z];
        }
    }
    memcpy(lcd,lcd_buf,sizeof(lcd_buf));
}

void add_number(){
    /*//这段不需要，因为画面变化必然会有空格出现。
    int k=0;
    for(int i=0;i<4;i++){
        for(int j=0;j<4;j++){
            if(map[i][j]!=0) k++;
        }
    }
    if(k==16) return;//如果满了就不再新增数字
    */
    char not_ok=1;
    while(not_ok){
        int x=rand()%4;
        int y=rand()%4;
        if(map[y][x]==0){//此方法当棋盘很大时效率会很低，但4x4效果还行
            map[y][x]=(rand()%2+1)*2;
            not_ok=0;
        }
    }
}

void refresh_screen(){
    for(int i=0;i<4;i++){//行
        for(int j=0;j<4;j++){//列
            show_pic(map[j][i],i*COLUMN/4,j*ROW/4);//显示出来的行列与打印出来的相反，故这里把数组里的i与j对调
            printf("%d\t",map[i][j]);
        }
        printf("\n");
    }
}

void show_pic(int number,int offset_x,int offset_y){
    //printf("x=%d,y=%d,number=%d\n",offset_x,offset_y,number);
    char bmp_buf[ROW/4][COLUMN/4][3];//行 列 BGR
    char img_path[20];
    sprintf(img_path, "%d.bmp", number);//数字转字符串
    //读取BMP
    int bmp_fd=open(img_path,O_RDONLY);
    if(bmp_fd == -1){
        perror("error opening bmp file");
        printf("filename: %s\n",img_path);
    }
    lseek(bmp_fd, 54, SEEK_SET);
    read(bmp_fd,bmp_buf,sizeof(bmp_buf));
    close(bmp_fd);
    //写入到显存
    for(int y=0;y<ROW/4;y++){
        for(int x=0;x<COLUMN/4;x++){
            for(int z=0;z<3;z++)
                lcd_buf[y+offset_y][x+offset_x][z]=bmp_buf[ROW/4-1-y][x][z];
        }
    }
    
    memcpy(lcd,lcd_buf,sizeof(lcd_buf));
}

int init(){
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
	//初始化随机数种子
	srand(time(NULL));
	return 0;
}

void uninit(){
    //结束
    munmap(lcd,ROW*COLUMN*4);
	close(input_fd);
	close(lcd_fd);
}
