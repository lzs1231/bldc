#include "doublebuf.h"

 
double_buf_t   double_buf;
double_buf_t   * p_double_buf = &double_buf;


/**
 * @�ҵ�һ����д������
 * @return ���������/FAIL
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
 * @�ҵ�һ���ɶ�������
 * @return ���������/FAIL
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
 * @����һ��˫������
 * @db: ˫����ṹ��ָ��
 * @size: ÿ���������Ĵ�С
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
 * @����һ��˫������
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
 * @��˫������д����
 * @db: ˫����ṹ��ָ��
 * @wdata: ����ָ��
 * @size: ���ݴ�С
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
 * @��˫������д����
 * @db: ˫����ṹ��ָ��
 * @wdata: ����ָ��
 * @size: ���ݴ�С
 * @return SUCC
 * @������������������˽����ǵ�һ��������
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
 * @��˫�����ж�����
 * @db: ˫����ṹ��ָ��
 * @rdata: ����ָ��
 * @size: ���ݴ�С
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



