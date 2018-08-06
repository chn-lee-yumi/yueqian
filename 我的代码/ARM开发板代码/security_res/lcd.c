#include "lcd.h"
#include "common.h"

#if EN_LCD_SHOW_JPG
#include "jpeglib.h"
#endif


static int8_t g_color_buf[FB_SIZE]={0};

static int32_t  g_fb_fd;
static int32_t *g_pfb_memory;


/****************************************************
 *��������:file_size_get
 *�������:pfile_path	-�ļ�·��
 *�� �� ֵ:-1		-ʧ��
		   ����ֵ	-�ļ���С
 *˵	��:��ȡ�ļ���С
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

/******************LCDӲ�����***********************************/
//��ʼ��LCD
int32_t lcd_open(void)
{
	g_fb_fd = open("/dev/fb0", O_RDWR);
	
	if(g_fb_fd<0)
	{
			printf("open lcd error\n");
			return -1;
	}

	g_pfb_memory  = (int32_t *)mmap(	NULL, 					//ӳ�����Ŀ�ʼ��ַ������ΪNULLʱ��ʾ��ϵͳ����ӳ��������ʼ��ַ
									FB_SIZE, 				//ӳ�����ĳ���
									PROT_READ|PROT_WRITE, 	//���ݿ��Ա���ȡ��д��
									MAP_SHARED,				//�����ڴ�
									g_fb_fd, 				//��Ч���ļ�������
									0						//��ӳ��������ݵ����
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
	
	
	/* ������ʾx��y�������λ�� */
	x_e = x + w;
	y_e = y + h;
	
	
	for(;y < y_e; y++) 
	{
		for (;x < x_e; x++)
		{
				/* ��ȡ��������ɫ���� */
				blue  = *pbmp_buf++;
				green = *pbmp_buf++;
				red   = *pbmp_buf++;
				
				/* ���24bit��ɫ */
				color = red << 16 | green << 8 | blue << 0;
				//Lcd_Show_Point(x, y_e-1-y, color);		//������ʾ		
				Lcd_Show_Point(x, y, color);
		}
		
		x = x_s;
	}
	
	return 0;
} 


//LCD�ر�
void lcd_close(void)
{
	
	/* ȡ���ڴ�ӳ�� */
	munmap(g_pfb_memory, FB_SIZE);
	
	/* �ر�LCD�豸 */
	close(g_fb_fd);
}


/*
	����������yuyvתRGB24�㷨�����+��ʽ�����ܹ�����ת����
		ʹ��ʱ����Ҫ�ȵ�������ı���ʼ��Init_Convert_Table�������ٽ���ת�����ɡ�
	yuyv_in��YUYV�������ݵ����롣
	w��yuyv����Ŀ��
	h��yuyv����ĸ߶�
	return���ɹ�ת����RGB24���ݡ����ߴ��ԭ���Ļ�����ͬ��
*/

//������Ҫ��int�����壬��Ȼ���ʱ�޷����
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
	ע�⣺YUYV�ĳߴ磬Ӧ����w*h*2����RGB24����w*h*3��YUV420������w*h*1.5
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
			yuv_offset = ( w*y + x ) * 2; //yuyvһ�����ص��ƫ����
			rgb24_offset = ( w*y + x ) * 3; //lcd���ص�ƫ����
			
			//����yuv����
			Y0 = *(yuyv_in + yuv_offset + 0);
			U  = *(yuyv_in + yuv_offset + 1);
			Y1 = *(yuyv_in + yuv_offset + 2);
			V  = *(yuyv_in + yuv_offset + 3);

			//���ó�yuv�����е�RGBֵ��ÿ�β������������ص㣩
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
