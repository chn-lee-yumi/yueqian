#include <stdio.h>
#include <string.h>
#include <pthread.h>

//（2）给孩子制定一个人生目标（函数）
void * child_fun(void *arg)
{
	while(1)
	{
		printf("罪过罪过\n");
		sleep(1);
		
		
		//if(ret ==-1)
		//	pthread_exit("over");
	}
}

int main() //main既是父进程，又叫主线程
{
	//（1）给孩子准备一个名字
	pthread_t child_name;

	//（3）生下孩子
	int ret;
	ret = pthread_create(&child_name,NULL,child_fun,NULL);
	if(ret !=0)
	{
		perror("pthread_create error");
		return -1;
	}

	printf("善哉善哉\n");
	sleep(1);	
	
	
	return 0;
}