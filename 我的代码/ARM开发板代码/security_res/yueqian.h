//Version: 20180717
//��ǶǶ��ʽʵϰ����ͷ�ļ� ��װby������������������Ȼ�����������������ϵ�
/*ʹ�÷�����
�����.c�����ļ���һ�����룺#include "yueqian.h"
��yueqian.h��yueqian.c���Ƶ����.c����Ŀ¼��
������Ҫ�Ӳ��� -std=gnu99
*/
//TODO:������Ŀ¼�ļ��ȹ��ܡ�
#include <pthread.h>
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

#define NOT_MOVED 0
#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4

void init_board();//��ʼ�������庯��
/*
���ܣ�
	��/dev/fb0
	��/dev/event0
	����mmap
һ����main������һ��ִ�С�
*/

void uninit_board();//����ʼ�������庯��
/*
���ܣ�
	�ͷ�mmap���ڴ�
	�ر�/dev/fb0
	�ر�/dev/event0
һ����main����returnǰ���á�������Ҳ�С������˳����ǻ��Զ��ͷŹ����ڴ���������ݵġ�
*/

void refresh_input();//���´������ļ�
/*
���ܣ�
	�ر�/dev/event0
	��/dev/event0
���δ��ȡ�����룬ĳЩ���Ҫ�á�
*/

void show_pic_generic(char path[],int offset_x,int offset_y);//����ͼƬ�ĺ���
/*
���ܣ�
	��һ��24λ��bmpͼƬ��������Ļ�ϵ��κ�λ�á���ͼƬ��Ե���ܳ�����Ļ��
�÷���
	char path[]���ļ�·������"1.bmp"
	int offset_x,int offset_y��ͼƬ���Ͻ�����Ļ������ꡣ
*/

int is_type(char type[],char filename[]);//�ж�һ���ļ����ǲ���ĳ�����͵��ļ�
/*
���ܣ�
	�ж�һ���ļ����ǲ���ĳ�����͵��ļ�
�÷���
	char type[]���ļ����ͣ���".bmp"��".mp3"
	char filename[]��Ҫ�жϵ��ļ�������"1.bmp"
	����ֵ��1��ʾ�ǣ�0��ʾ���ǡ�
*/

void get_coordinates(int *touch_x,int *touch_y);//��ȡ������
/*
���ܣ�
	��ȡ����������㣨����֮���ɿ�ʱ���ڵĵ㣩
�÷���
	int touch_x,touch_y;
	get_coordinates(&touch_x,&touch_y);
	����ֵ��ֱ���޸Ĵ����touch_x��touch_y��
*/

int get_direction();//��ȡ��������
/*
���ܣ�
	��ȡ��������������
�÷���
	����ֵ��NOT_MOVED��UP��DOWN��LEFT��RIGHT��
*/

int get_coordinates_and_direction(int *touch_x,int *touch_y);//��ȡ������ͻ�������
/*
���ܣ�
	ʵ��get_coordinates��get_direction�Ĺ���
�÷���
	int touch_x,touch_y,direction;
	direction=get_coordinates_and_direction(&touch_x,&touch_y);
	����ֵ��UP��DOWN��LEFT��RIGHT��ֱ���޸Ĵ����touch_x��touch_y��
*/









//int lsdir(char *filenames,char path[]);
