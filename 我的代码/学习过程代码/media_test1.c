#include <stdlib.h>
#include <unistd.h>
int main()
{
    /*
    在代码里面实现对音乐、视频的控制。
	音乐播放10s，暂停2s，继续20s，结束
	视频播放10s，暂停2s，继续20s，结束
	*/
	system("madplay wokou.mp3 &");
	sleep(10);
	system("killall -STOP madplay");
	sleep(2);
	system("killall -CONT madplay");
	sleep(20);
	system("killall madplay");
	system("mplayer mv.mp4 &");
	sleep(10);
	system("killall -STOP mplayer");
	sleep(2);
	system("killall -CONT mplayer");
	sleep(20);
	system("killall mplayer");
	return 0;
}
