#include "yueqian.h"
//小游戏：2048
//硬件：粤嵌ARM开发板
//arm-linux-gcc 2048.c yueqian.c -std=gnu99
short map[4][4]={0};
//游戏函数
void refresh_screen();//刷新整个屏幕
void join_numbers(char direction);//数字整合
void add_number();//产生随机数字
void show_pic(int number,int x,int y);//将单个图片放到目标位置
int touch_x;int touch_y;

int main(){
    init_board();
    add_number();add_number();
    refresh_screen();
    while(1){
		char direction=get_coordinates_and_direction(&touch_x,&touch_y);
		if(direction!=NOT_MOVED) join_numbers(direction);
		else if(touch_x>=COLUMN-100 && touch_y>=ROW-50) pthread_exit(NULL);
    }
	uninit_board();
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
                show_pic_generic("./2048_res/win.bmp",160,0);
				sleep(3);
                pthread_exit(NULL);
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
            show_pic_generic("./2048_res/lost.bmp",213,0);
			sleep(3);
            pthread_exit(NULL);
        }
        
    }
}

void add_number(){
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
	show_pic_generic("./2048_res/exit.bmp",COLUMN-100,ROW-50);
}

void show_pic(int number,int offset_x,int offset_y){
    char img_path[20];
    sprintf(img_path, "./2048_res/%d.bmp", number);//数字转字符串
	show_pic_generic(img_path,offset_x,offset_y);
}