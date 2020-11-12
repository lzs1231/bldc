#ifndef _DOUBLEBUF_H
#define _DOUBLEBUF_H


#include "sys.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
#ifndef NULL
	#define NULL	((void *)0)
#endif
 
#ifndef FAIL
	#define FAIL	(-1)
#endif
 
#ifndef SUCC
	#define SUCC	(0)
#endif
 
#ifndef TRUE
	#define TRUE	(1)
#endif
 
#ifndef FALSE
	#define FALSE	(0)
#endif
 

 
typedef struct buf {
	void *pdata;
	int index;
	int status;
	int size;
}buf_t;
 
typedef struct double_buf {
	buf_t buf[2];
}double_buf_t;
 
enum double_buf_status {
	WRITABLE = 0,
	READABLE,
	DEAD,
};
 
extern double_buf_t   * p_double_buf;

/**
 * @创建一个双缓冲区
 * @db: 双缓冲结构体指针
 * @size: 每个缓冲区的大小
 * @return SUCC/FAIL
 */
int create_double_buf(double_buf_t *db, int size);
 
/**
 * @销毁一个双缓冲区
 */
void destroy_double_buf(double_buf_t *db);
 
/**
 * @往双缓冲中写数据
 * @db: 双缓冲结构体指针
 * @wdata: 数据指针
 * @size: 数据大小
 * @return SUCC/FAIL
 */
int write_double_buf(double_buf_t *db, void *wdata, int size);
 
/**
 * @往双缓冲中写数据
 * @db: 双缓冲结构体指针
 * @wdata: 数据指针
 * @size: 数据大小
 * @return SUCC
 * @如果两个缓冲区都满了将覆盖第一个缓冲区
 */
int write_double_buf_cover(double_buf_t *db, void *wdata, int size);
 
/**
 * @从双缓冲中读数据
 * @db: 双缓冲结构体指针
 * @rdata: 数据指针
 * @size: 数据大小
 * @return SUCC/FAIL
 */
int read_double_buf(double_buf_t *db, void *rdata, int size);
 

 

#endif
