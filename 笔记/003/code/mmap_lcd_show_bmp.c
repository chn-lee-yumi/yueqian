#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
int lcd_fd;
int *p=NULL;
			//0x00 FF FF FF
			//   A	R  G  B
void lcd_show_point(int x,int y,int color)
{
	*(p+ y*800+x)=color;
}

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
	
	//E.直接将24bit的RGB，转存到内存当中去
	

	int x,y,i;
	int color;
	for(y=0;y<480;y++)
		for(x=0;x<800;x++,i+=3)
		{
			color = bmp_buf[i+0] | bmp_buf[i+1]<<8 | bmp_buf[i+2]<<16;
			lcd_show_point(x,479-y,color);
		}
	
	
	//G.将LCD与BMP都给关闭了
	munmap(p,800*480*4);
	
	close(lcd_fd);
	close(bmp_fd);

	return 0;
}