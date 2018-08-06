#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>		//字符串处理
#include <strings.h>
#include <assert.h>		//assert函数
#include <getopt.h>           
#include <fcntl.h>      //IO
#include <unistd.h>
#include <sys/stat.h>	
#include <sys/types.h>
#include <errno.h>		//出错码
#include <malloc.h>		//动态内存
#include <sys/time.h>	
#include <sys/mman.h>	//mmap
#include <sys/ioctl.h>	//driver的ioctl
#include <pthread.h>	//线程
#include <asm/types.h>        //camread
#include <linux/videodev2.h>
#include <linux/input.h>	//输入子系统
typedef unsigned char   uint8_t;     //无符号8位数
typedef unsigned short    uint16_t;    //无符号16位数
typedef unsigned int   	uint32_t;    //无符号32位数
typedef unsigned long long   uint64_t; //无符号64位数
//（int、long在32位下表示4个字节，而long在64位下表示8个字节,long long 不管在32/64都是8个字节。需要实际考察）
typedef signed   char   int8_t;      //有符号8位数
typedef signed   short    int16_t;     //有符号16位数
typedef signed   int   int32_t;     //有符号32位数
typedef signed   long long   int64_t;     //有符号32位数

#endif