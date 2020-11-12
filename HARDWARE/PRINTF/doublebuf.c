#include "doublebuf.h"

 
double_buf_t   double_buf;
double_buf_t   * p_double_buf = &double_buf;


/**
 * @找到一个可写缓冲区
 * @return 缓冲区编号/FAIL
 */
static int find_writable_buf(double_buf_t *db, int size)
{
	int i;
	
	for(i=0; i<2; i++)
	{
		if(db->buf[i].status==WRITABLE && (db->buf[i].index+size)<=db->buf[i].size)
		   return i;
	}
	
	return FAIL;
}
 
/**
 * @找到一个可读缓冲区
 * @return 缓冲区编号/FAIL
 */
static int find_readable_buf(double_buf_t *db, int size)
{
	int i;
	
	for(i=0; i<2; i++)
	{
		if(db->buf[i].status==READABLE && db->buf[i].index>=size)
		   return i;
	}
	
	return FAIL;
}
 
static void __write_double_buf(double_buf_t *db, int n, void *wdata, int size)
{
	int index = db->buf[n].index;
	
	memcpy((u8 *)db->buf[n].pdata+index, (u8 *)wdata, size);
	
	db->buf[n].index += size;
	if(db->buf[n].index >= db->buf[n].size)
		db->buf[n].status = READABLE;
}
 
static int __read_double_buf(double_buf_t *db, int n, void *rdata, int size)
{
	static int rindex = 0;
	
	if(rindex + size > db->buf[n].size)
		return FAIL;
	
	memcpy((u8 *)rdata, (u8 *)db->buf[n].pdata+rindex, size);
	rindex += size;
	if(rindex >= db->buf[n].size)
	{
		rindex = 0;
		db->buf[n].index = 0;
		db->buf[n].status = WRITABLE;
	}
	
	return SUCC;
}
 
/**
 * @创建一个双缓冲区
 * @db: 双缓冲结构体指针
 * @size: 每个缓冲区的大小
 * @return SUCC/FAIL
 */
int create_double_buf(double_buf_t *db, int size)
{
	int i;
	
	db->buf[0].pdata = (void *)malloc(size);
	if(!db->buf[0].pdata)
		return FAIL;
	
	db->buf[1].pdata = (void *)malloc(size);
	if(!db->buf[1].pdata)
	{
		free(db->buf[0].pdata);
		return FAIL;
	}
	
	for(i=0; i<2; i++)	
	{
		db->buf[i].index = 0;
		db->buf[i].status = WRITABLE;
		db->buf[i].size = size;
	}
	
	return SUCC;
}
 
/**
 * @销毁一个双缓冲区
 */
void destroy_double_buf(double_buf_t *db)
{
	int i;
	
	for(i=0; i<2; i++)
	{
		free(db->buf[i].pdata);
		db->buf[i].status = DEAD;
		db->buf[i].index = 0;
	}
}
 
/**
 * @往双缓冲中写数据
 * @db: 双缓冲结构体指针
 * @wdata: 数据指针
 * @size: 数据大小
 * @return SUCC/FAIL
 */
int write_double_buf(double_buf_t *db, void *wdata, int size)
{
	int i;

	i = find_writable_buf(db, size);
	if(i < 0)
	{
		return FAIL;
	}
	else if(i == 0)
	{
		;
	}
	else if(i == 1)
	{
		;
	}
	__write_double_buf(db, i, wdata, size);
	return SUCC;
}
 
/**
 * @往双缓冲中写数据
 * @db: 双缓冲结构体指针
 * @wdata: 数据指针
 * @size: 数据大小
 * @return SUCC
 * @如果两个缓冲区都满了将覆盖第一个缓冲区
 */
int write_double_buf_cover(double_buf_t *db, void *wdata, int size)
{
	int i;
	
	i = find_writable_buf(db, size);
	if(i < 0)
	{
		i = 0;
		db->buf[i].status = WRITABLE;
		db->buf[i].index = 0;	
	}
	
	__write_double_buf(db, i, wdata, size);
	return SUCC;
}
 
/**
 * @从双缓冲中读数据
 * @db: 双缓冲结构体指针
 * @rdata: 数据指针
 * @size: 数据大小
 * @return SUCC/FAIL
 */
int read_double_buf(double_buf_t *db, void *rdata, int size)
{
	int i;

	
	i = find_readable_buf(db, size);
	if(i < 0)
	{
		return FAIL;
	}
	
	i = __read_double_buf(db, i, rdata, size);
	if(i < 0)
	{
		return FAIL;
	}
	
	return SUCC;
}



