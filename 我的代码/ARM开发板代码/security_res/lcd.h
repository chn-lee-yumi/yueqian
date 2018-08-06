#ifndef __LCD_H__
#define __LCD_H__
#include "common.h"

#define LCD_WIDTH  			800
#define LCD_HEIGHT 			480
#define FB_SIZE				(LCD_WIDTH * LCD_HEIGHT * 4)

#define EN_LCD_SHOW_JPG		0

typedef struct 
{
	uint32_t *buf;
	uint32_t w;
	uint32_t h;	
}RGB_attr;

//bmp存储的文件头格式
typedef int BOOL;
typedef int LONG;
typedef unsigned char BYTE;
typedef unsigned int DWORD;
typedef unsigned short WORD;

typedef struct {
        WORD    bfType;//2
        DWORD   bfSize;//4
        WORD    bfReserved1;//2
        WORD    bfReserved2;//2
        DWORD   bfOffBits;//4
}__attribute__((packed))FileHead;

typedef struct{
        DWORD      biSize;//4
        LONG       biWidth;//4
        LONG       biHeight;//4
        WORD       biPlanes;//2
        WORD       biBitCount;//2
        DWORD      biCompress;//4
        DWORD      biSizeImage;//4
        LONG       biXPelsPerMeter;//4
        LONG       biYPelsPerMeter;//4
        DWORD      biClrUsed;//4
        DWORD      biClrImportant;//4
}__attribute__((packed))Infohead;

typedef struct
{
    BYTE b;
    BYTE g;
    BYTE r;
}RGB_data;//RGB TYPE

int32_t lcd_open(void);
void lcd_draw_point(uint32_t x,uint32_t y, uint32_t color);

int32_t lcd_draw_bmp_for_buf(uint32_t x,uint32_t y,uint8_t *pbmp_data,uint32_t w,uint32_t h);
int32_t lcd_draw_bmp(uint32_t x,uint32_t y,const int8_t *pbmp_path);
void lcd_close(void);

/*
	功能：用于将YUV（yuv422）格式的数据，转换为yuv420p，可以方便用于h264的编码
	YUYV_in：yuv422的数据输入。
	YUV420P_out：用于保存yuv420p的数组。
	（注意点：yuv420的数组大小是：宽*高*1.5）
	width：宽度
	height：高度
*/
int32_t YUYV_to_YUV420P(uint8_t *YUYV_in,uint8_t* YUV420P_out,int32_t width,int32_t height);

/*
	功能：用于将yuv420p格式的数据，转换为RGB24，可以方便用于LCD的显示。
	pYUV：yuv420p的数据输入。数组大小：宽*高*1.5
	pBGR24：用于保存RGB的数组。注意点：RGB的数组大小是：宽*高*3
	width：宽度
	height：高度
*/
int32_t YUV420P_to_RGB24(uint8_t* pYUV,uint8_t* pBGR24,int32_t width,int32_t height);

/*
	功能：用于判断是否溢出上下限。配套下面的YUYV_to_RGB24函数
*/
int32_t minmax(int32_t min, int32_t v, int32_t max);

/*
	功能：用于将yuyv(YUV422)(摄像头输出)格式的数据，转换为RGB24，可以方便用于LCD的显示。
	YUYV_in：yuyv的数据输入。
	width：宽度
	height：高度
	return：转换之后RGB堆空间数据，需要free释放
*/
uint8_t* YUYV_to_RGB24(uint8_t* YUYV_in, uint32_t width, uint32_t height);

/*	编写完，待测试
	功能：将一张bmp图片，进行提取RGB32数据
	bmp_path：要进行提取的bmp图片文件路径
	return：提取成功后的RGB信息结构体
*/
RGB_attr * BmpFile_to_RGB32(const uint8_t *bmp_path);

/*	编写完，待测试
	功能：将一张jpg图片，进行提取RGB32数据
	jpg_path：要进行提取的jpg图片文件路径
	return：提取成功后的RGB信息结构体
*/
RGB_attr * JpgFile_to_RGB32(const uint8_t *jpg_path);

/*	编写完，待测试
	功能：将一段存储着jpg数据的缓冲区，进行提取RGB32数据。
	（适用于直接生成jpg数据的场合：摄像头）
	jpg_buf：要进行提取的jpg数据缓冲区
	jpg_buf_size：jpg数据缓冲区的大小
	return：提取成功后的RGB信息结构体
*/
RGB_attr * JpgBuf_to_RGB32(uint8_t *jpg_buf,uint32_t jpg_buf_size);

/*	写完，待测试
	功能：根据得到的RGB24数据，进行图像任意位置显示
	（主要是迎合上面的各种转换函数的显示测试）
	x:显示的x轴起点
	y:显示的y轴起点
	w:显示图像宽度。
	h:显示图像高度。
	pbmp_data：要进行显示的RGB24_buf
*/
int32_t Lcd_Show_RGB24(uint32_t x,uint32_t y,uint32_t w,uint32_t h,uint8_t *pbmp_data);

/*	写完，待测试
	功能：根据转换后的RGB32数据，进行图像任意位置的缩放显示
	start_x:显示的x轴起点
	start_y:显示的y轴起点
	show_w:显示图像宽度。（缩小：<实际宽度）（放大：>实际宽度）
	show_h:显示图像高度。（缩小：<实际高度）（放大：>实际高度）
	RGB_buf:存储转换后的RGB32的参数
*/
int32_t Lcd_Show_RGB32(uint32_t start_x,uint32_t start_y,uint32_t show_w,uint32_t show_h,RGB_attr * RGB_buf);

/*	完成，待测试
	功能：实现LCD任意位置出现原图大小的横向百叶窗
	start_x:显示的x轴起点
	start_y:显示的y轴起点
	RGB_buf:存储转换后的RGB32的参数
*/
int32_t Lcd_ShowBroadwiseLeafs_RGB32(uint32_t start_x,uint32_t start_y,RGB_attr * RGB_buf);

/*	写完，待测试
	功能：实现LCD任意位置出现原图大小的纵向百叶窗
	start_x:显示的x轴起点
	start_y:显示的y轴起点
	RGB_buf:存储转换后的RGB32的参数
*/
int32_t Lcd_ShowDirectionLeafs_RGB32(uint32_t start_x,uint32_t start_y,RGB_attr * RGB_buf);

/*	写完(没有拆解)，待测试
	功能：实现LCD从左往右，飞入显示原图大小的画面
	start_x:显示的x轴起点
	start_y:显示的y轴起点
	RGB_buf:存储转换后的RGB32的参数
*/
int32_t Lcd_ShowFly_RGB32(uint32_t start_x,uint32_t start_y,RGB_attr * RGB_buf);

#endif