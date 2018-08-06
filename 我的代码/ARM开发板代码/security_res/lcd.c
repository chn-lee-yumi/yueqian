#include "lcd.h"
#include "common.h"

#if EN_LCD_SHOW_JPG
#include "jpeglib.h"
#endif


static int8_t g_color_buf[FB_SIZE]={0};

static int32_t  g_fb_fd;
static int32_t *g_pfb_memory;


/****************************************************
 *函数名称:file_size_get
 *输入参数:pfile_path	-文件路径
 *返 回 值:-1		-失败
		   其他值	-文件大小
 *说	明:获取文件大小
 ****************************************************/
uint32_t file_size_get(const int8_t *pfile_path)
{
	uint32_t filesize = -1;	
	struct stat statbuff;
	
	if(stat(pfile_path, &statbuff) < 0)
	{
		return filesize;
	}
	else
	{
		filesize = statbuff.st_size;
	}
	
	return filesize;
}

/******************LCD硬件相关***********************************/
//初始化LCD
int32_t lcd_open(void)
{
	g_fb_fd = open("/dev/fb0", O_RDWR);
	
	if(g_fb_fd<0)
	{
			printf("open lcd error\n");
			return -1;
	}

	g_pfb_memory  = (int32_t *)mmap(	NULL, 					//映射区的开始地址，设置为NULL时表示由系统决定映射区的起始地址
									FB_SIZE, 				//映射区的长度
									PROT_READ|PROT_WRITE, 	//内容可以被读取和写入
									MAP_SHARED,				//共享内存
									g_fb_fd, 				//有效的文件描述词
									0						//被映射对象内容的起点
								);

	return g_fb_fd;

}

void Lcd_Show_Point(uint32_t start_x,uint32_t start_y, uint32_t color)
{
	*(g_pfb_memory+start_y*800+start_x)=color;
}


/*
*/
int32_t lcd_draw_bmp_for_buf(uint32_t x,uint32_t y,uint8_t *pbmp_data,uint32_t w,uint32_t h)
{
	uint32_t blue, green, red;
	uint32_t color;
	uint32_t bmp_type;
	uint32_t bmp_size;
	uint32_t x_s = x;
	uint32_t x_e ;	
	uint32_t y_e ;
	uint8_t buf[54]={0};
	uint8_t *pbmp_buf=pbmp_data;
	
	
	/* 设置显示x、y坐标结束位置 */
	x_e = x + w;
	y_e = y + h;
	
	
	for(;y < y_e; y++) 
	{
		for (;x < x_e; x++)
		{
				/* 获取红绿蓝颜色数据 */
				blue  = *pbmp_buf++;
				green = *pbmp_buf++;
				red   = *pbmp_buf++;
				
				/* 组成24bit颜色 */
				color = red << 16 | green << 8 | blue << 0;
				//Lcd_Show_Point(x, y_e-1-y, color);		//逆序显示		
				Lcd_Show_Point(x, y, color);
		}
		
		x = x_s;
	}
	
	return 0;
} 


//LCD关闭
void lcd_close(void)
{
	
	/* 取消内存映射 */
	munmap(g_pfb_memory, FB_SIZE);
	
	/* 关闭LCD设备 */
	close(g_fb_fd);
}


/*
	功能描述：yuyv转RGB24算法（查表+公式），能够正常转换。
		使用时，需要先调用下面的表格初始化Init_Convert_Table函数，再进行转换即可。
	yuyv_in：YUYV画面数据的输入。
	w：yuyv画面的宽度
	h：yuyv画面的高度
	return：成功转换的RGB24数据。（尺寸跟原来的画面相同）
*/

//数组需要用int来定义，不然溢出时无法察觉
int32_t R[256][256];
int32_t G[256][256][256];
int32_t B[256][256];

void Init_Convert_Table(void)
{
	int32_t i=0,j=0,k=0;
	for(i=0; i<256; i++)
	{
		for(j=0; j<256; j++)
		{
			R[i][j] = i + 1.042*(j-128);
			R[i][j] = R[i][j]>255 ? 255 : R[i][j];
			R[i][j] = R[i][j]<0   ? 0   : R[i][j];

			B[i][j] = i + 1.772*(j-128);
			B[i][j] = B[i][j]>255 ? 255 : B[i][j];
			B[i][j] = B[i][j]<0   ? 0   : B[i][j];

			for(k=0; k<256; k++)
			{
				G[i][j][k] = i + 0.344*(j-128)-0.714*(k-128);
				G[i][j][k] = G[i][j][k]>255 ? 255 : G[i][j][k];
				G[i][j][k] = G[i][j][k]<0   ? 0   : G[i][j][k];
			}
		}
	}
}

/*
	注意：YUYV的尺寸，应该是w*h*2，而RGB24的是w*h*3，YUV420的则是w*h*1.5
*/
uint8_t *YUYV_To_RGB24(uint8_t *yuyv_in,uint32_t w,uint32_t h)
{
	uint8_t Y0, U;
	uint8_t Y1, V;

	uint8_t *RGB24 = malloc(w*h*3);

	int32_t yuv_offset,rgb24_offset;
	int32_t x=0,y=0;
	for(y=0; y<h; y++)
	{
		for(x=0; x<w; x+=2)
		{
			yuv_offset = ( w*y + x ) * 2; //yuyv一个像素点的偏移量
			rgb24_offset = ( w*y + x ) * 3; //lcd像素点偏移量
			
			//计算yuv分量
			Y0 = *(yuyv_in + yuv_offset + 0);
			U  = *(yuyv_in + yuv_offset + 1);
			Y1 = *(yuyv_in + yuv_offset + 2);
			V  = *(yuyv_in + yuv_offset + 3);

			//查表得出yuv分量中的RGB值（每次操作是两个像素点）
			*(RGB24 + rgb24_offset + 2) = R[Y0][V];
			*(RGB24 + rgb24_offset + 1) = G[Y0][U][V];
			*(RGB24 + rgb24_offset + 0) = B[Y0][U];

			*(RGB24 + rgb24_offset + 5) = R[Y1][V];
			*(RGB24 + rgb24_offset + 4) = G[Y1][U][V];
			*(RGB24 + rgb24_offset + 3) = B[Y1][U];
		}
	}
	return RGB24;
}
