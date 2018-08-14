#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
int len(char string[]);
int is_type(char type[],char filename[]);
char filenames[255][255]={0};//不知道有多少个文件，暂且这样，可以在查完文件之后再定义。
int main()
{
	//打开目录
	DIR *dir = opendir("./");
	if(dir == NULL){
		perror("open dir error");
		return -1;
	}
	//列出文件
	struct dirent *dirp;
	int filenum=0;
	while(1){//TODO:判断read error
		dirp = readdir(dir);
		if(dirp == NULL) break;
		//printf("d_name = %s\n",dirp->d_name);
		strcpy(filenames[filenum],dirp->d_name);
		//printf("filename = %s\n",filenames[filenum]);
		filenum++;
	}
	//printf("sizeof(filenames) = %d, sizeof(filenames[0])= %d\n",sizeof(filenames),sizeof(filenames[0]));
	//https://blog.csdn.net/bufanq/article/details/51567454
	for(int i=0;i<filenum;i++){
	    //printf("filename = %s\n",filenames[i]);
	    if(is_type(".mp3",filenames[i])) printf("%s\n",filenames[i]);
	    if(is_type(".mp4",filenames[i])) printf("%s\n",filenames[i]);
	    if(is_type(".bmp",filenames[i])) printf("%s\n",filenames[i]);
	}
	return 0;
}

int len(char string[]){
    int length=sizeof(string);
    int true_length=length;
    for(int i=length-1;i>=0;i--){
        if(string[i]==0) true_length--;
        else break;
    }
    return true_length;
}

int is_type(char type[],char filename[]){//非正常代码，待改进
    //https://blog.csdn.net/qiantanlong/article/details/78505306
    /*
    char *ptr;
    ptr = strtok(filename, ".");
	while (ptr != NULL) {
		printf("ptr=%s\n", ptr);
		ptr = strtok(NULL, ".");
	}*/
	char *p=strstr(filename,type);
    if(NULL != p)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
