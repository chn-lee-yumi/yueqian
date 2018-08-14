#include <stdio.h>
#include <pthread.h>


void * thread1_func(void *arg)
{
	while(1)
	{
		printf("我是线程1，来杀我啊\n");
		sleep(1);
	}
}

void * thread2_func(void *arg)
{
	int t=0;
	while(1)
	{
		printf("我是线程2，存在感(%d)刷到一百就自杀\n",t);
		t+=10;
		sleep(1);
		if(t>=100) pthread_exit(NULL);
	}
}

void * thread3_func(void *arg)
{
	int t=0;
	while(1)
	{
		printf("我是线程3，存在感(%d)刷到一百就自杀\n",t);
		t+=5;
		sleep(1);
		if(t>100) pthread_exit(NULL);
	}
}

int main()
{
	pthread_t thread1,thread2,thread3;

	int ret=0;
	if(ret !=0)
	{
		perror("pthread_create error");
		return -1;
	}
	
	pthread_create(&thread1,NULL,thread1_func,NULL);
	pthread_create(&thread2,NULL,thread2_func,NULL);
	pthread_create(&thread3,NULL,thread3_func,NULL);
	
	sleep(8);	
	printf("线程1去死吧，biubiubiu~\n");
	pthread_cancel(thread1);
	printf("等线程3刷完存在感\n");
	pthread_join(thread3,NULL);
	
	return 0;
}