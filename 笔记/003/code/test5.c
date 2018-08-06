#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

			//0x00 FF FF FF
			//   A	R  G  B
void lcd_show_point(int x,int y,int color)
{
	*(p+ y*800+x)=color;
}
int lcd_fd;
int *p=NULL;
int main()
{
	//A.open LCD
	lcd_fd=open("/dev/fb0",O_RDWR);
	{
	
	}
	
	//内存映射
	p=mmap(NULL,800*480*4,PROT_READ | PROT_WRITE,MAP_SHARED,lcd_fd,0);
	if(p == MAP_FAILED)
	{
		perror("mmap error");
		return -1;
	}
	
	//B.open BMP_file （图片文件要下载到板子上）
	int bmp_fd;
	//1.打开文件
	bmp_fd = open("1.bmp",O_RDWR);	
	if(bmp_fd == -1)	//错误检测
	{
		perror("open bmp error"); //填写错误提示信息
		return -1;
	}
	
	//C.lseek （跳过54个字节的头信息） 
	lseek(bmp_fd,54,SEEK_SET);
	
	//D.read （宽*高*3的RGB数据）	
	char bmp_buf[800*480*3]={0};
	read(bmp_fd,bmp_buf,sizeof(bmp_buf));
	
	//E.准备好color_buf（24bit的RGB 转 32bit的ARGB）
	char lcd_buf[800*480*4]={0};
	int x,y,i;
	for(y=0;y<480;y++)
		for(x=0;x<800*4;x+=4,i+=3)
		{
			lcd_buf[(479-y)*800 +x+0] = bmp_buf[i+0];
			lcd_buf[(479-y)*800 +x+1] = bmp_buf[i+1];
			lcd_buf[(479-y)*800 +x+2] = bmp_buf[i+2]
			lcd_buf[(479-y)*800 +x+3] = 0;
		}
	/*
	for(x=0;x<800*480*4;x+=4,y+=3)
	{
		lcd_buf[x+0] = bmp_buf[y+0];//B;
		lcd_buf[x+1] = bmp_buf[y+1];//G;
		lcd_buf[x+2] = bmp_buf[y+2];//R;
		lcd_buf[x+3] = 0;//A;
	}*/
	
	//F.将color_buf write LCD对应的文件
	write(lcd_fd,lcd_buf,sizeof(lcd_buf));
	
	//G.将LCD与BMP都给关闭了
	munmap(p,800*480*4);
	
	close(lcd_fd);
	close(bmp_fd);

	return 0;
}