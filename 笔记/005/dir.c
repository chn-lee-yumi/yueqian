#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
int main()
{
	//1.打开目录
	DIR * op=NULL;
	op = opendir("/");
	if(op == NULL)
	{
		perror("open dir error");
		return -1;
	}
	
	//2.返回目录
	struct dirent * dp=NULL;
	while(1)
	{
		dp = readdir(op);
		if(dp == NULL)
		{
			perror("read error");
			return -1;
		}
		
		printf("d_name = %s\n",dp->d_name);
	}
	
	
	return 0;
}
    