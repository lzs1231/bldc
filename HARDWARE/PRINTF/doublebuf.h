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
 * @����һ��˫������
 * @db: ˫����ṹ��ָ��
 * @size: ÿ���������Ĵ�С
 * @return SUCC/FAIL
 */
int create_double_buf(double_buf_t *db, int size);
 
/**
 * @����һ��˫������
 */
void destroy_double_buf(double_buf_t *db);
 
/**
 * @��˫������д����
 * @db: ˫����ṹ��ָ��
 * @wdata: ����ָ��
 * @size: ���ݴ�С
 * @return SUCC/FAIL
 */
int write_double_buf(double_buf_t *db, void *wdata, int size);
 
/**
 * @��˫������д����
 * @db: ˫����ṹ��ָ��
 * @wdata: ����ָ��
 * @size: ���ݴ�С
 * @return SUCC
 * @������������������˽����ǵ�һ��������
 */
int write_double_buf_cover(double_buf_t *db, void *wdata, int size);
 
/**
 * @��˫�����ж�����
 * @db: ˫����ṹ��ָ��
 * @rdata: ����ָ��
 * @size: ���ݴ�С
 * @return SUCC/FAIL
 */
int read_double_buf(double_buf_t *db, void *rdata, int size);
 

 

#endif
