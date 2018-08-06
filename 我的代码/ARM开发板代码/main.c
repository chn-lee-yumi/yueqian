//arm-linux-gcc main.c yueqian.c -std=gnu99 -o main
//注：2048.c单独编译成一个可执行文件。
#include "yueqian.h"

#define BOOT 0
#define HOMEPAGE 1
#define PHOTO 2
#define MEDIA 3
#define MUSIC 31
#define VIDEO 32
#define ENV 4
#define HA 5
#define SECURITY 6
#define GAME 7

void router(int page);//路由，转到某个页面，并执行该页面的处理函数
void ShowDHT11();//刷新DHT11数据
char img_path[255];//背景图片路径
int touch_x,touch_y;

int main()
{
	//初始化
	init_board();
	//开机画面
	router(BOOT);
	//显示操作系统主页
	router(HOMEPAGE);
}

void router(int page){//路由，转到某个页面，并执行该页面的处理函数
	switch(page){//根据页面执行具体的参数

	case BOOT://启动页面
	    sprintf(img_path,"./bg/%d.bmp",page);//路径如 ./bg/1.bmp
	    show_pic_generic(img_path,0,0);//设置背景
		sleep(1);//启动页面停留三秒就返回
		break;

	case HOMEPAGE://主页
		while(1){//根据用户点击决定去到哪个页面
		    sprintf(img_path,"./bg/%d.bmp",page);
	        show_pic_generic(img_path,0,0);
			get_coordinates(&touch_x,&touch_y);//获取输入
			if(touch_y<ROW/3){//*****点击第一行的图标*****
				if(touch_x<COLUMN/3){//图库
				    router(PHOTO);
				}else if(touch_x>=COLUMN/3 && touch_x<COLUMN/3*2){//音视频
				    router(MEDIA);
				}else{//环境监测
				    router(ENV);
				}
			}else if(touch_y>=ROW/3 && touch_y<ROW/3*2){//*****点击第二行的图标*****
			    if(touch_x<COLUMN/3){//家电
				    router(HA);
				}else if(touch_x>=COLUMN/3 && touch_x<COLUMN/3*2){//安防
				    router(SECURITY);
				}else{//游戏
				    router(GAME);
				}
			}else{//*****点击第三行的图标*****//此处无图标
			    if(touch_x<COLUMN/3){
				    ;
				}else if(touch_x>=COLUMN/3 && touch_x<COLUMN/3*2){
				    ;
				}else{
				    ;
				}
			}
		}
		break;

	case PHOTO:{//图库    这段语句加了花括号，原因见break那行的注释
	    char filenames[255][255]={0};//不知道目录有多少个文件，给它255。
        int filenum=0;//目录文件总数
        //打开目录
        printf("Opening dir...\n");
      	DIR *dir = opendir("./photos/");
      	if(dir == NULL){
      		perror("open dir error");
      		exit(-1);
      	}
      	//获取bmp文件列表（也就是题目要求的广告图片）
        printf("Getting BMP list...\n");
      	struct dirent *dirp;
      	while(1){
      		dirp = readdir(dir);
      		if(dirp == NULL) break;
			if(is_type(".bmp",dirp->d_name)){
			    char img_path[255];//背景图片路径
			    sprintf(img_path,"./photos/%s",dirp->d_name);
        		strcpy(filenames[filenum],img_path);
            printf("%s\n",dirp->d_name);
            filenum++;
          }
      	}
		int img_num=0;//当前播放的图片编号
		show_pic_generic(filenames[img_num],0,0);
      	while(1){
      	    char is_exit=0;
        	switch(get_direction()){
            case UP://退出
            case DOWN:
        		is_exit=1;
        		break;
        	case LEFT:
    			img_num--;
    			if(img_num==-1) img_num=filenum-1;
    			show_pic_generic(filenames[img_num],0,0);
        		break;
        	case RIGHT:
    			img_num++;
    			if(img_num==filenum) img_num=0;
    			show_pic_generic(filenames[img_num],0,0);
        		break;
        	}
        	if(is_exit==1) break;//返回到首页
      	}
		break;}//因为定义了变量，所以需要加花括号，参考 https://www.cnblogs.com/3me-linux/p/4527358.html

	case MEDIA://音视频
		while(1){
			int is_exit=0;
			sprintf(img_path,"./bg/%d.bmp",page);
			show_pic_generic(img_path,0,0);
			get_coordinates(&touch_x,&touch_y);
			if(touch_y<ROW/2){
				if(touch_x<COLUMN/3){//音乐播放器
					router(MUSIC);
				}else if(touch_x>=COLUMN/3 && touch_x<COLUMN/3*2){//视频播放器
					router(VIDEO);
				}else{//返回
					is_exit=1;
				}
			}
			if(is_exit==1) break;
		}
	    break;

	case ENV:{//环境监测
		char is_exit=0;
		sprintf(img_path,"./bg/%d.bmp",page);
		show_pic_generic(img_path,0,0);
		//先刷新一遍
		ShowDHT11();
		while(1){
			get_coordinates(&touch_x,&touch_y);
			if(touch_y>=ROW/3*2){
				if(touch_x<=COLUMN/3){//刷新
					ShowDHT11();
				}else if(touch_x>COLUMN/3*2){//返回
					is_exit=1;
				}
			}
			if(is_exit==1) break;
		}
	    break;}

	case HA:{//家电
		char is_exit=0;
		char ctr[3]={0};
		sprintf(img_path,"./bg/%d.bmp",page);
		show_pic_generic(img_path,0,0);
		while(1){
			get_coordinates(&touch_x,&touch_y);
			if(touch_y>=ROW/3 && touch_y <=ROW/3*2){
				if(touch_x<=COLUMN/3){//LED1
					ctr[1]=!ctr[1];
					uart_send_data(ctr);
				}else if(touch_x>COLUMN/3 && touch_x<=COLUMN/3*2){//LED2
					ctr[2]=!ctr[2];
					uart_send_data(ctr);
				}else if(touch_x>COLUMN/3*2){//返回
					is_exit=1;
				}
			}
			if(is_exit==1) break;
        }
	    break;}

	case SECURITY://安防
		sprintf(img_path,"./bg/%d.bmp",page);
		show_pic_generic(img_path,0,0);
		system("./security");
		refresh_input();//清空历史输入，不然会被读到
		break;

	case GAME://游戏
	    system("./2048");
		refresh_input();//清空历史输入，不然会被读到
	    break;

	case MUSIC:{//音乐播放器
		sprintf(img_path,"./bg/%d.bmp",page);
		show_pic_generic(img_path,0,0);
		char filenames[255][255]={0};//不知道目录有多少个文件，给它255。
        int filenum=0;//目录文件总数
        //打开目录
        printf("Opening dir...\n");
      	DIR *dir = opendir("./music/");
      	if(dir == NULL){
      		perror("open dir error");
      		exit(-1);
      	}
        printf("Getting MP3 list...\n");
      	struct dirent *dirp;
      	while(1){
      		dirp = readdir(dir);
      		if(dirp == NULL) break;
			if(is_type(".mp3",dirp->d_name)){
			    char path[255];
			    sprintf(path,"./music/%s",dirp->d_name);
        		strcpy(filenames[filenum],path);
            printf("%s\n",dirp->d_name);
            filenum++;
          }
      	}
		char is_playing=0;
		int playing_num=-1;
		while(1){
			int is_exit=0;
			char cmd[255];
			get_coordinates(&touch_x,&touch_y);
			if(touch_y>=260 && touch_y <=400){
				if(touch_x>180 && touch_x <=305){//上一首
					playing_num--;
					if(playing_num<=-1) playing_num=filenum-1;
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
			}else if(touch_y>400 && touch_x>680){
				system("killall madplay");
				is_exit=1;
			}
			if(is_exit==1) break;
        }
		break;}

	case VIDEO://视频播放器
		//可加功能：暂停、播放、上一个、下一个
		system("mplayer ./video/mv.mp4 -framedrop &");
	    while(1){
      	    char is_exit=0;
        	switch(get_direction()){
            case UP://退出
            case DOWN:
				system("killall mplayer");
        		is_exit=1;
        		break;
        	case LEFT:
        		break;
        	case RIGHT:
        		break;
        	}
        	if(is_exit==1) break;//返回到首页
      	}
	    break;

	}
}

void ShowDHT11(){
	char ctr[3]={1,1,1};
	unsigned char DHT11[2];
	printf("刷新\n");
	uart_send_data(ctr);
	sleep(2);
	uart_read_data(DHT11,2);
	printf("Temp=%d,Hum=%d\n",DHT11[0],DHT11[1]);
	sprintf(img_path,"./number/%d.bmp",DHT11[0]/10);
	show_pic_generic(img_path,395,75);
	sprintf(img_path,"./number/%d.bmp",DHT11[0]%10);
	show_pic_generic(img_path,455,75);
	sprintf(img_path,"./number/%d.bmp",DHT11[1]/10);
	show_pic_generic(img_path,395,160);
	sprintf(img_path,"./number/%d.bmp",DHT11[1]%10);
	show_pic_generic(img_path,455,160);
}
