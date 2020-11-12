/************************************版权申明********************************************
**                             广州大彩光电科技有限公司
**                             http://www.gz-dc.com
**-----------------------------------文件信息--------------------------------------------
** 文件名称:   hmi_driver.c
** 修改时间:   2018-05-18
** 文件说明:   用户MCU串口驱动函数库
** 技术支持：  Tel: 020-82186683  Email: hmi@gz-dc.com Web:www.gz-dc.com
--------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------*/
#include "hmi_driver.h"

#define TX_8(P1) SEND_DATA((P1)&0xFF)                    //发送单个字节
#define TX_8N(P,N) SendNU8((u8 *)P,N)                 //发送N个字节
#define TX_16(P1) TX_8((P1)>>8);TX_8(P1)                 //发送16位整数
#define TX_16N(P,N) SendNU16((u16 *)P,N)              //发送N个16位整数
#define TX_32(P1) TX_16((P1)>>16);TX_16((P1)&0xFFFF)     //发送32位整数

#if(CRC16_ENABLE)

static u16 _crc16 = 0xffff;
/*!
*  \brief 添加CRC16校验
*  \param buffer 待校验的数据
*  \param n 数据长度，包含CRC16
*  \param pcrc 校验码
*/
static void AddCRC16(u8 *buffer,u16 n,u16 *pcrc)
{
    u16 i,j,carry_flag,a;

    for (i=0; i<n; i++)
    {
        *pcrc=*pcrc^buffer[i];
        for (j=0; j<8; j++)
        {
            a=*pcrc;
            carry_flag=a&0x0001;
            *pcrc=*pcrc>>1;
            if (carry_flag==1)
                *pcrc=*pcrc^0xa001;
        }
    }
}
/*!
*  \brief  检查数据是否符合CRC16校验
*  \param buffer 待校验的数据，末尾存储CRC16
*  \param n 数据长度，包含CRC16
*  \return 校验通过返回1，否则返回0
*/
u16 CheckCRC16(u8 *buffer,u16 n)
{
    u16 crc0 = 0x0;
    u16 crc1 = 0xffff;

    if(n>=2)
    {
        crc0 = ((buffer[n-2]<<8)|buffer[n-1]);
        AddCRC16(buffer,n-2,&crc1);
    }

    return (crc0==crc1);
}
/*!
*  \brief  发送一个字节
*  \param  c
*/
void SEND_DATA(u8 c)
{
    AddCRC16(&c,1,&_crc16);
    Send232Char(c);
}
/*!
*  \brief  帧头
*/
void BEGIN_CMD()
{
    TX_8(0XEE);
    _crc16 = 0XFFFF;                      //开始计算CRC16
}
/*!
*  \brief  帧尾
*/
void END_CMD()
{
    u16 crc16 = _crc16;
    TX_16(crc16);                         //发送CRC16
    TX_32(0XFFFCFFFF);
}

#else//NO CRC16

#define SEND_DATA(P) Send232Char(P)          //发送一个字节
#define BEGIN_CMD() TX_8(0XEE)            //帧头
#define END_CMD() TX_32(0XFFFCFFFF)       //帧尾

#endif
/*!
*  \brief  延时
*  \param  n 延时时间(毫秒单位)
*/
void DelayMS(unsigned int n)
{
    int i,j;
    for(i = n;i>0;i--)
        for(j=1000;j>0;j--) ;
}
/*!
*  \brief  串口发送送字符串
*  \param  字符串
*/
void SendStrings(u8 *str)
{
	
    while(*str)
    {
        TX_8(*str);
        str++;
    }
}
/*!
*  \brief  串口发送送N个字节
*  \param  个数
*/
void SendNU8(u8 *pData,u16 nDataLen)
{
    u16 i = 0;
    for (;i<nDataLen;++i)
    {
        TX_8(pData[i]);
    }
}
/*!
*  \brief  串口发送送N个16位的数据
*  \param  个数
*/
void SendNU16(u16 *pData,u16 nDataLen)
{
    u16 i = 0;
    for (;i<nDataLen;++i)
    {
        TX_16(pData[i]);
    }
}
/*!
*  \brief  发送握手命令
*/
void SetHandShake()
{
    BEGIN_CMD();
    TX_8(0x04);
    END_CMD();
}

/*!
*  \brief  设置前景色
*  \param  color 前景色
*/
void SetFcolor(u16 color)
{
    BEGIN_CMD();
    TX_8(0x41);
    TX_16(color);
    END_CMD();
}
/*!
*  \brief  设置背景色
*  \param  color 背景色
*/
void SetBcolor(u16 color)
{
    BEGIN_CMD();
    TX_8(0x42);
    TX_16(color);
    END_CMD();
}
/*!
*  \brief 获取
*  \param  color 背景色
*/
void ColorPicker(u8 mode, u16 x,u16 y)
{
    BEGIN_CMD();
    TX_8(0xA3);
    TX_8(mode);
    TX_16(x);
    TX_16(y);
    END_CMD();
}
/*!
*  \brief  清除画面
*/
void GUI_CleanScreen()
{
    BEGIN_CMD();
    TX_8(0x01);
    END_CMD();
}
/*!
*  \brief  设置文字间隔
*  \param  x_w 横向间隔
*  \param  y_w 纵向间隔
*/
void SetTextSpace(u8 x_w, u8 y_w)
{
    BEGIN_CMD();
    TX_8(0x43);
    TX_8(x_w);
    TX_8(y_w);
    END_CMD();
}
/*!
*  \brief  设置文字显示限制
*  \param  enable 是否启用限制
*  \param  width 宽度
*  \param  height 高度
*/
void SetFont_Region(u8 enable,u16 width,u16 height)
{
    BEGIN_CMD();
    TX_8(0x45);
    TX_8(enable);
    TX_16(width);
    TX_16(height);
    END_CMD();
}
/*!
*  \brief  设置过滤色
*  \param  fillcolor_dwon 颜色下界
*  \param  fillcolor_up 颜色上界
*/
void SetFilterColor(u16 fillcolor_dwon, u16 fillcolor_up)
{
    BEGIN_CMD();
    TX_8(0x44);
    TX_16(fillcolor_dwon);
    TX_16(fillcolor_up);
    END_CMD();
}

/*!
*  \brief  设置过滤色
*  \param  x 位置X坐标
*  \param  y 位置Y坐标
*  \param  back 颜色上界
*  \param  font 字体
*  \param  strings 字符串内容
*/
void DisText(u16 x, u16 y,u8 back,u8 font,u8 *strings )
{
    BEGIN_CMD();
    TX_8(0x20);
    TX_16(x);
    TX_16(y);
    TX_8(back);
    TX_8(font);
    SendStrings(strings);
    END_CMD();
}
/*!
*  \brief    显示光标
*  \param  enable 是否显示
*  \param  x 位置X坐标
*  \param  y 位置Y坐标
*  \param  width 宽度
*  \param  height 高度
*/
void DisCursor(u8 enable,u16 x, u16 y,u8 width,u8 height )
{
    BEGIN_CMD();
    TX_8(0x21);
    TX_8(enable);
    TX_16(x);
    TX_16(y);
    TX_8(width);
    TX_8(height);
    END_CMD();
}
/*!
*  \brief      显示全屏图片
*  \param  image_id 图片索引
*  \param  masken 是否启用透明掩码
*/
void DisFull_Image(u16 image_id,u8 masken)
{
    BEGIN_CMD();
    TX_8(0x31);
    TX_16(image_id);
    TX_8(masken);
    END_CMD();
}
/*!
*  \brief      指定位置显示图片
*  \param  x 位置X坐标
*  \param  y 位置Y坐标
*  \param  image_id 图片索引
*  \param  masken 是否启用透明掩码
*/
void DisArea_Image(u16 x,u16 y,u16 image_id,u8 masken)
{
    BEGIN_CMD();
    TX_8(0x32);
    TX_16(x);
    TX_16(y);
    TX_16(image_id);
    TX_8(masken);
    END_CMD();
}
/*!
*  \brief  显示裁剪图片
*  \param  x 位置X坐标
*  \param  y 位置Y坐标
*  \param  image_id 图片索引
*  \param  image_x 图片裁剪位置X坐标
*  \param  image_y 图片裁剪位置Y坐标
*  \param  image_l 图片裁剪长度
*  \param  image_w 图片裁剪高度
*  \param  masken 是否启用透明掩码
*/
void DisCut_Image(u16 x,u16 y,u16 image_id,u16 image_x,u16 image_y,u16 image_l, u16 image_w,u8 masken)
{
    BEGIN_CMD();
    TX_8(0x33);
    TX_16(x);
    TX_16(y);
    TX_16(image_id);
    TX_16(image_x);
    TX_16(image_y);
    TX_16(image_l);
    TX_16(image_w);
    TX_8(masken);
    END_CMD();
}
/*!
*  \brief      显示GIF动画
*  \param  x 位置X坐标
*  \param  y 位置Y坐标
*  \param  flashimage_id 图片索引
*  \param  enable 是否显示
*  \param  playnum 播放次数
*/
void DisFlashImage(u16 x,u16 y,u16 flashimage_id,u8 enable,u8 playnum)
{
    BEGIN_CMD();
    TX_8(0x80);
    TX_16(x);
    TX_16(y);
    TX_16(flashimage_id);
    TX_8(enable);
    TX_8(playnum);
    END_CMD();
}
/*!
*  \brief      画点
*  \param  x 位置X坐标
*  \param  y 位置Y坐标
*/
void GUI_Dot(u16 x,u16 y)
{
    BEGIN_CMD();
    TX_8(0x50);
    TX_16(x);
    TX_16(y);
    END_CMD();
}
/*!
*  \brief      画线
*  \param  x0 起始位置X坐标
*  \param  y0 起始位置Y坐标
*  \param  x1 结束位置X坐标
*  \param  y1 结束位置Y坐标
*/
void GUI_Line(u16 x0, u16 y0, u16 x1, u16 y1)
{
    BEGIN_CMD();
    TX_8(0x51);
    TX_16(x0);
    TX_16(y0);
    TX_16(x1);
    TX_16(y1);
    END_CMD();
}

/*!
*  \brief      画折线
*  \param  mode 模式
*  \param  dot 数据点
*  \param  dot_cnt 点数
*/
void GUI_ConDots(u8 mode,u16 *dot,u16 dot_cnt)
{
    BEGIN_CMD();
    TX_8(0x63);
    TX_8(mode);
    TX_16N(dot,dot_cnt*2);
    END_CMD();
}

/*!
*  \brief   x坐标等距使用前景色连线
*  \param  x 横坐标
*  \param  x_space 距离
*  \param  dot_y  一组纵轴坐标
*  \param  dot_cnt  纵坐标个数
*/
void GUI_ConSpaceDots(u16 x,u16 x_space,u16 *dot_y,u16 dot_cnt)
{
    BEGIN_CMD();
    TX_8(0x59);
    TX_16(x);
    TX_16(x_space);
    TX_16N(dot_y,dot_cnt);
    END_CMD();
}
/*!
*  \brief   按照坐标偏移量用前景色连线
*  \param  x 横坐标
*  \param  y 纵距离
*  \param  dot_offset  偏移量
*  \param  dot_cnt  偏移量个数
*/
void GUI_FcolorConOffsetDots(u16 x,u16 y,u16 *dot_offset,u16 dot_cnt)
{
    BEGIN_CMD();
    TX_8(0x75);
    TX_16(x);
    TX_16(y);
    TX_16N(dot_offset,dot_cnt);
    END_CMD();
}
/*!
*  \brief   按照坐标偏移量用背景色连线
*  \param  x 横坐标
*  \param  y 纵距离
*  \param  dot_offset  偏移量
*  \param  dot_cnt  偏移量个数
*/
void GUI_BcolorConOffsetDots(u16 x,u16 y,u8 *dot_offset,u16 dot_cnt)
{
    BEGIN_CMD();
    TX_8(0x76);
    TX_16(x);
    TX_16(y);
    TX_16N(dot_offset,dot_cnt);
    END_CMD();
}
/*!
*  \brief  自动调节背光亮度
*  \param  enable 使能
*  \param  bl_off_level 待机亮度
*  \param  bl_on_level  激活亮度
*  \param  bl_on_time  偏移量个数
*/
void SetPowerSaving(u8 enable, u8 bl_off_level, u8 bl_on_level, u16  bl_on_time)
{
    BEGIN_CMD();
    TX_8(0x77);
    TX_8(enable);
    TX_8(bl_off_level);
    TX_8(bl_on_level);
    TX_16(bl_on_time);
    END_CMD();
}
/*!
*  \brief  将制定的多个坐标点用前景色连接起来
*  \param  dot  坐标点
*  \param  dot_cnt  偏移量个数
*/
void GUI_FcolorConDots(u16 *dot,u16 dot_cnt)
{
    BEGIN_CMD();
    TX_8(0x68);
    TX_16N(dot,dot_cnt*2);
    END_CMD();
}
/*!
*  \brief  将制定的多个坐标点用背景色连接起来
*  \param  dot  坐标点
*  \param  dot_cnt  偏移量个数
*/
void GUI_BcolorConDots(u16 *dot,u16 dot_cnt)
{
    BEGIN_CMD();
    TX_8(0x69);
    TX_16N(dot,dot_cnt*2);
    END_CMD();
}
/*!
*  \brief     画空心圆
*  \param  x0 圆心位置X坐标
*  \param  y0 圆心位置Y坐标
*  \param  r 半径
*/
void GUI_Circle(u16 x, u16 y, u16 r)
{
    BEGIN_CMD();
    TX_8(0x52);
    TX_16(x);
    TX_16(y);
    TX_16(r);
    END_CMD();
}
/*!
*  \brief      画实心圆
*  \param  x0 圆心位置X坐标
*  \param  y0 圆心位置Y坐标
*  \param  r 半径
*/
void GUI_CircleFill(u16 x, u16 y, u16 r)
{
    BEGIN_CMD();
    TX_8(0x53);
    TX_16(x);
    TX_16(y);
    TX_16(r);
    END_CMD();
}
/*!
*  \brief      画弧线
*  \param  x0 圆心位置X坐标
*  \param  y0 圆心位置Y坐标
*  \param  r 半径
*  \param  sa 起始角度
*  \param  ea 终止角度
*/
void GUI_Arc(u16 x,u16 y, u16 r,u16 sa, u16 ea)
{
    BEGIN_CMD();
    TX_8(0x67);
    TX_16(x);
    TX_16(y);
    TX_16(r);
    TX_16(sa);
    TX_16(ea);
    END_CMD();
}
/*!
*  \brief      画空心矩形
*  \param  x0 起始位置X坐标
*  \param  y0 起始位置Y坐标
*  \param  x1 结束位置X坐标
*  \param  y1 结束位置Y坐标
*/
void GUI_Rectangle(u16 x0, u16 y0, u16 x1,u16 y1 )
{
    BEGIN_CMD();
    TX_8(0x54);
    TX_16(x0);
    TX_16(y0);
    TX_16(x1);
    TX_16(y1);
    END_CMD();
}
/*!
*  \brief      画实心矩形
*  \param  x0 起始位置X坐标
*  \param  y0 起始位置Y坐标
*  \param  x1 结束位置X坐标
*  \param  y1 结束位置Y坐标
*/
void GUI_RectangleFill(u16 x0, u16 y0, u16 x1,u16 y1 )
{
    BEGIN_CMD();
    TX_8(0x55);
    TX_16(x0);
    TX_16(y0);
    TX_16(x1);
    TX_16(y1);
    END_CMD();
}
/*!
*  \brief      画空心椭圆
*  \param  x0 起始位置X坐标
*  \param  y0 起始位置Y坐标
*  \param  x1 结束位置X坐标
*  \param  y1 结束位置Y坐标
*/
void GUI_Ellipse(u16 x0, u16 y0, u16 x1,u16 y1 )
{
    BEGIN_CMD();
    TX_8(0x56);
    TX_16(x0);
    TX_16(y0);
    TX_16(x1);
    TX_16(y1);
    END_CMD();
}
/*!
*  \brief      画实心椭圆
*  \param  x0 起始位置X坐标
*  \param  y0 起始位置Y坐标
*  \param  x1 结束位置X坐标
*  \param  y1 结束位置Y坐标
*/
void GUI_EllipseFill(u16 x0, u16 y0, u16 x1,u16 y1 )
{
    BEGIN_CMD();
    TX_8(0x57);
    TX_16(x0);
    TX_16(y0);
    TX_16(x1);
    TX_16(y1);
    END_CMD();
}
/*!
*  \brief      画线
*  \param  x0 起始位置X坐标
*  \param  y0 起始位置Y坐标
*  \param  x1 结束位置X坐标
*  \param  y1 结束位置Y坐标
*/




/*******
设置屏幕亮度
*******/
void SetBackLight(u8 light_level)
{
    BEGIN_CMD();
    TX_8(0x60);
    TX_8(light_level);
    END_CMD();
}

/*!
*  \brief   蜂鸣器设置
*  \time  time 持续时间(毫秒单位)
*/
void SetBuzzer(u8 time)
{
    BEGIN_CMD();
    TX_8(0x61);
    TX_8(time);
    END_CMD();
}

void GUI_AreaInycolor(u16 x0, u16 y0, u16 x1,u16 y1 )
{
    BEGIN_CMD();
    TX_8(0x65);
    TX_16(x0);
    TX_16(y0);
    TX_16(x1);
    TX_16(y1);
    END_CMD();
}
/*!
*  \brief   触摸屏设置
*  \param enable 触摸使能
*  \param beep_on 触摸蜂鸣器
*  \param work_mode 触摸工作模式：0按下就上传，1松开才上传，2不断上传坐标值，3按下和松开均上传数据
*  \param press_calibration 连续点击触摸屏20下校准触摸屏：0禁用，1启用
*/
void SetTouchPaneOption(u8 enbale,u8 beep_on,u8 work_mode,u8 press_calibration)
{
    u8 options = 0;

    if(enbale)
        options |= 0x01;
    if(beep_on)
        options |= 0x02;
    if(work_mode)
        options |= (work_mode<<2);
    if(press_calibration)
        options |= (press_calibration<<5);

    BEGIN_CMD();
    TX_8(0x70);
    TX_8(options);
    END_CMD();
}
/*!
*  \brief   校准触摸屏
*/
void CalibrateTouchPane()
{
    BEGIN_CMD();
    TX_8(0x72);
    END_CMD();
}
/*!
*  \brief  触摸屏测试
*/
void TestTouchPane()
{
    BEGIN_CMD();
    TX_8(0x73);
    END_CMD();
}

/*!
*  \brief  锁定设备配置，锁定之后需要解锁，才能修改波特率、触摸屏、蜂鸣器工作方式
*/
void UnlockDeviceConfig(void)
{
    BEGIN_CMD();
    TX_8(0x09);
    TX_8(0xDE);
    TX_8(0xED);
    TX_8(0x13);
    TX_8(0x31);
    END_CMD();
}

/*!
*  \brief  解锁设备配置
*/
void LockDeviceConfig(void)
{
    BEGIN_CMD();
    TX_8(0x08);
    TX_8(0xA5);
    TX_8(0x5A);
    TX_8(0x5F);
    TX_8(0xF5);
    END_CMD();
}
/*!
*  \brief    修改串口屏的波特率
*  \details  波特率选项范围[0~14]，对应实际波特率
{1200,2400,4800,9600,19200,38400,57600,115200,1000000,2000000,218750,437500,875000,921800,2500000}
*  \param  option 波特率选项
*/
void SetCommBps(u8 option)
{
    BEGIN_CMD();
    TX_8(0xA0);
    TX_8(option);
    END_CMD();
}
/*!
*  \brief      设置当前写入图层
*  \details  一般用于实现双缓存效果(绘图时避免闪烁)：
*  \details  u8 layer = 0;
*  \details  WriteLayer(layer);   设置写入层
*  \details  ClearLayer(layer);   使图层变透明
*  \details  添加一系列绘图指令
*  \details  DisText(100,100,0,4,"hello hmi!!!");
*  \details  DisplyLayer(layer);  切换显示层
*  \details  layer = (layer+1)%2; 双缓存切换
*  \see DisplyLayer
*  \see ClearLayer
*  \param  layer 图层编号
*/
void WriteLayer(u8 layer)
{
    BEGIN_CMD();
    TX_8(0xA1);
    TX_8(layer);
    END_CMD();
}
/*!
*  \brief      设置当前显示图层
*  \param  layer 图层编号
*/
void DisplyLayer(u8 layer)
{
    BEGIN_CMD();
    TX_8(0xA2);
    TX_8(layer);
    END_CMD();
}
/*!
*  \brief      拷贝图层
*  \param  src_layer 原始图层
*  \param  dest_layer 目标图层
*/
void CopyLayer(u8 src_layer,u8 dest_layer)
{
    BEGIN_CMD();
    TX_8(0xA4);
    TX_8(src_layer);
    TX_8(dest_layer);
    END_CMD();
}
/*!
*  \brief      清除图层，使图层变成透明
*  \param  layer 图层编号
*/
void ClearLayer(u8 layer)
{
    BEGIN_CMD();
    TX_8(0x05);
    TX_8(layer);
    END_CMD();
}

void GUI_DispRTC(u8 enable,u8 mode,u8 font,u16 color,u16 x,u16 y)
{
    BEGIN_CMD();
    TX_8(0x85);
    TX_8(enable);
    TX_8(mode);
    TX_8(font);
    TX_16(color);
    TX_16(x);
    TX_16(y);
    END_CMD();
}
/*!
*  \brief  写数据到串口屏用户存储区
*  \param  startAddress 起始地址
*  \param  length 字节数
*  \param  _data 待写入的数据
*/
void WriteUserFlash(u32 startAddress,u16 length,u8 *_data)
{
    BEGIN_CMD();
    TX_8(0x87);
    TX_32(startAddress);
    TX_8N(_data,length);
    END_CMD();
}
/*!
*  \brief  从串口屏用户存储区读取数据
*  \param  startAddress 起始地址
*  \param  length 字节数
*/
void ReadUserFlash(u32 startAddress,u16 length)
{
    BEGIN_CMD();
    TX_8(0x88);
    TX_32(startAddress);
    TX_16(length);
    END_CMD();
}
/*!
*  \brief      获取当前画面
*/
void GetScreen(void )
{
    BEGIN_CMD();
    TX_8(0xB1);
    TX_8(0x01);
    END_CMD();
}
/*!
*  \brief      设置当前画面
*  \param  screen_id 画面ID
*/
void SetScreen(u16 screen_id)
{
    BEGIN_CMD();
    TX_8(0xB1);
    TX_8(0x00);
    TX_16(screen_id);
    END_CMD();
}
/*!
*  \brief     禁用\启用画面更新
*  \details 禁用\启用一般成对使用，用于避免闪烁、提高刷新速度
*  \details 用法：
*	\details SetScreenUpdateEnable(0);//禁止更新
*	\details 一系列更新画面的指令
*	\details SetScreenUpdateEnable(1);//立即更新
*  \param  enable 0禁用，1启用
*/
void SetScreenUpdateEnable(u8 enable)
{
    BEGIN_CMD();
    TX_8(0xB3);
    TX_8(enable);
    END_CMD();
}
/*!
*  \brief     设置控件输入焦点
*  \param  screen_id 画面ID
*  \param  control_id 控件ID
*  \param  focus 是否具有输入焦点
*/
void SetControlFocus(u16 screen_id,u16 control_id,u8 focus)
{
    BEGIN_CMD();
    TX_8(0xB1);
    TX_8(0x02);
    TX_16(screen_id);
    TX_16(control_id);
    TX_8(focus);
    END_CMD();
}
/*!
*  \brief     显示\隐藏控件
*  \param  screen_id 画面ID
*  \param  control_id 控件ID
*  \param  visible 是否显示
*/
void SetControlVisiable(u16 screen_id,u16 control_id,u8 visible)
{
    BEGIN_CMD();
    TX_8(0xB1);
    TX_8(0x03);
    TX_16(screen_id);
    TX_16(control_id);
    TX_8(visible);
    END_CMD();
}
/*!
*  \brief     设置触摸控件使能
*  \param  screen_id 画面ID
*  \param  control_id 控件ID
*  \param  enable 控件是否使能
*/
void SetControlEnable(u16 screen_id,u16 control_id,u8 enable)
{
    BEGIN_CMD();
    TX_8(0xB1);
    TX_8(0x04);
    TX_16(screen_id);
    TX_16(control_id);
    TX_8(enable);
    END_CMD();
}
/*!
*  \brief     设置按钮状态
*  \param  screen_id 画面ID
*  \param  control_id 控件ID
*  \param  value 按钮状态     0弹起  1按下
*/
void SetButtonValue(u16 screen_id,u16 control_id,u8 state)
{
    BEGIN_CMD();
    TX_8(0xB1);
    TX_8(0x10);
    TX_16(screen_id);
    TX_16(control_id);
    TX_8(state);
    END_CMD();
}

/*!
*  \brief     读取按钮状态
*  \param  screen_id 画面ID
*  \param  control_id 控件ID
*  \param  value 按钮状态     0弹起  1按下
*/
void ReadButtonValue(u16 screen_id,u16 control_id)
{
    BEGIN_CMD();
    TX_8(0xB1);
    TX_8(0x11);
    TX_16(screen_id);
    TX_16(control_id);
    END_CMD();
}

/*
*       文本闪烁
*/
void TextTwinkle(u16 screen_id,u16 control_id,u16 time)
{
	BEGIN_CMD();
    TX_8(0xB1);
    TX_8(0x15);
    TX_16(screen_id);
    TX_16(control_id);
    TX_16(time);
    END_CMD();
}

/*
*       文本滚动
*/
void TextScrolling(u16 screen_id,u16 control_id,u16 speed)
{
	BEGIN_CMD();
    TX_8(0xB1);
    TX_8(0x16);
    TX_16(screen_id);
    TX_16(control_id);
    TX_16(speed);
    END_CMD();
}

/*!
*  \brief  设置文本值
*  \param  screen_id 画面ID
*  \param  control_id 控件ID
*  \param  str 文本值
*/
void SetTextValue(u16 screen_id,u16 control_id,u8 *str)
{
    BEGIN_CMD();
    TX_8(0xB1);
    TX_8(0x10);
    TX_16(screen_id);
    TX_16(control_id);
    SendStrings(str);
    END_CMD();
}

#if FIRMWARE_VER>=908
/*!
*  \brief     设置文本为整数值，要求FIRMWARE_VER>=908
*  \param  screen_id 画面ID
*  \param  control_id 控件ID
*  \param  value 文本数值
*  \param  sign 0-无符号，1-有符号
*  \param  fill_zero 数字位数，不足时左侧补零
*/
void SetTextInt32(u16 screen_id,u16 control_id,u32 value,u8 sign,u8 fill_zero)
{
    BEGIN_CMD();
    TX_8(0xB1);
    TX_8(0x07);
    TX_16(screen_id);
    TX_16(control_id);
    TX_8(sign?0X01:0X00);
    TX_8((fill_zero&0x0f)|0x80);
    TX_32(value);
    END_CMD();
}
/*!
*  \brief     设置文本单精度浮点值，要求FIRMWARE_VER>=908
*  \param  screen_id 画面ID
*  \param  control_id 控件ID
*  \param  value 文本数值
*  \param  precision 小数位数
*  \param  show_zeros 为1时，显示末尾0
*/
void SetTextFloat(u16 screen_id,u16 control_id,float value,u8 precision,u8 show_zeros)
{
	u8 i = 0;

	BEGIN_CMD();
	TX_8(0xB1);
	TX_8(0x07);
	TX_16(screen_id);
	TX_16(control_id);
	TX_8(0x02);
	TX_8((precision&0x0f)|(show_zeros?0x80:0x00));

	for (i=0;i<4;++i)
	{
	 //需要区分大小端
#if(0)
		TX_8(((u8 *)&value)[i]);
#else
		TX_8(((u8 *)&value)[3-i]);
#endif
	}
	END_CMD();
}
#endif
/*!
*  \brief      设置进度值
*  \param  screen_id 画面ID
*  \param  control_id 控件ID
*  \param  value 需要显示的数值  
*/
void SetProgressValue(u16 screen_id,u16 control_id,u32 value)
{
    BEGIN_CMD();
    TX_8(0xB1);
    TX_8(0x10);
    TX_16(screen_id);
    TX_16(control_id);
    TX_32(value);       
    END_CMD();
}
/*!
*  \brief     设置仪表值
*  \param  screen_id 画面ID
*  \param  control_id 控件ID
*  \param  value 数值
*/
void SetMeterValue(u16 screen_id,u16 control_id,u32 value)
{
    BEGIN_CMD();
    TX_8(0xB1);
    TX_8(0x10);
    TX_16(screen_id);
    TX_16(control_id);
    TX_32(value);
    END_CMD();
}
/*!
*  \brief     设置仪表值
*  \param  screen_id 画面ID
*  \param  control_id 图片控件ID
*  \param  value 数值
*/
void Set_picMeterValue(u16 screen_id,u16 control_id,u16 value)
{
    BEGIN_CMD();
    TX_8(0xB1);
    TX_8(0x10);
    TX_16(screen_id);
    TX_16(control_id);
    TX_16(value);
    END_CMD();
}
/*!
*  \brief      设置滑动条
*  \param  screen_id 画面ID
*  \param  control_id 控件ID
*  \param  value 数值
*/

void SetSliderValue(u16 screen_id,u16 control_id,u32 value)
{
    BEGIN_CMD();
    TX_8(0xB1);
    TX_8(0x10);
    TX_16(screen_id);
    TX_16(control_id);
    TX_32(value);
    END_CMD();
}
/*!
*  \brief      设置选择控件
*  \param  screen_id 画面ID
*  \param  control_id 控件ID
*  \param  item 当前选项
*/
void SetSelectorValue(u16 screen_id,u16 control_id,u8 item)
{
    BEGIN_CMD();
    TX_8(0xB1);
    TX_8(0x10);
    TX_16(screen_id);
    TX_16(control_id);
    TX_8(item);
    END_CMD();
}
/*!
*  \brief     获取控件值
*  \param  screen_id 画面ID
*  \param  control_id 控件ID
*/
void GetControlValue(u16 screen_id,u16 control_id)
{
    BEGIN_CMD();
    TX_8(0xB1);
    TX_8(0x11);
    TX_16(screen_id);
    TX_16(control_id);
    END_CMD();
}

/*!
*  \brief      开始播放动画
*  \param  screen_id 画面ID
*  \param  control_id 控件ID
*/
void AnimationStart(u16 screen_id,u16 control_id)
{
    BEGIN_CMD();
    TX_8(0xB1);
    TX_8(0x20);
    TX_16(screen_id);
    TX_16(control_id);
    END_CMD();
}

/*!
*  \brief      停止播放动画
*  \param  screen_id 画面ID
*  \param  control_id 控件ID
*/
void AnimationStop(u16 screen_id,u16 control_id)
{
    BEGIN_CMD();
    TX_8(0xB1);
    TX_8(0x21);
    TX_16(screen_id);
    TX_16(control_id);
    END_CMD();
}
/*!
*  \brief      暂停播放动画
*  \param  screen_id 画面ID
*  \param  control_id 控件ID
*/
void AnimationPause(u16 screen_id,u16 control_id)
{
    BEGIN_CMD();
    TX_8(0xB1);
    TX_8(0x22);
    TX_16(screen_id);
    TX_16(control_id);
    END_CMD();
}
/*!
*  \brief     播放制定帧
*  \param  screen_id 画面ID
*  \param  control_id 控件ID
*  \param  frame_id 帧ID
*/
void AnimationPlayFrame(u16 screen_id,u16 control_id,u8 frame_id)
{
    BEGIN_CMD();
    TX_8(0xB1);
    TX_8(0x23);
    TX_16(screen_id);
    TX_16(control_id);
    TX_8(frame_id);
    END_CMD();
}
/*!
*  \brief     播放上一帧
*  \param  screen_id 画面ID
*  \param  control_id 控件ID
*/
void AnimationPlayPrev(u16 screen_id,u16 control_id)
{
    BEGIN_CMD();
    TX_8(0xB1);
    TX_8(0x24);
    TX_16(screen_id);
    TX_16(control_id);
    END_CMD();
}
/*!
*  \brief     播放下一帧
*  \param  screen_id 画面ID
*  \param  control_id 控件ID
*/
void AnimationPlayNext(u16 screen_id,u16 control_id)
{
    BEGIN_CMD();
    TX_8(0xB1);
    TX_8(0x25);
    TX_16(screen_id);
    TX_16(control_id);
    END_CMD();
}
/*!
*  \brief     曲线控件-添加通道
*  \param  screen_id 画面ID
*  \param  control_id 控件ID
*  \param  channel 通道号
*  \param  color 颜色
*/
void GraphChannelAdd(u16 screen_id,u16 control_id,u8 channel,u16 color)
{
    BEGIN_CMD();
    TX_8(0xB1);
    TX_8(0x30);
    TX_16(screen_id);
    TX_16(control_id);
    TX_8(channel);
    TX_16(color);
    END_CMD();
}
/*!
*  \brief     曲线控件-删除通道
*  \param  screen_id 画面ID
*  \param  control_id 控件ID
*  \param  channel 通道号
*/
void GraphChannelDel(u16 screen_id,u16 control_id,u8 channel)
{
    BEGIN_CMD();
    TX_8(0xB1);
    TX_8(0x31);
    TX_16(screen_id);
    TX_16(control_id);
    TX_8(channel);
    END_CMD();
}
/*!
*  \brief     曲线控件-添加数据
*  \param  screen_id 画面ID
*  \param  control_id 控件ID
*  \param  channel 通道号
*  \param  pData 曲线数据
*  \param  nDataLen 数据个数
*/
void GraphChannelDataAdd(u16 screen_id,u16 control_id,u8 channel,u8 *pData,u16 nDataLen)
{
    BEGIN_CMD();
    TX_8(0xB1);
    TX_8(0x32);
    TX_16(screen_id);
    TX_16(control_id);
    TX_8(channel);
    TX_16(nDataLen);
    TX_8N(pData,nDataLen);
    END_CMD();
}
/*!
*  \brief     曲线控件-清除数据
*  \param  screen_id 画面ID
*  \param  control_id 控件ID
*  \param  channel 通道号
*/
void GraphChannelDataClear(u16 screen_id,u16 control_id,u8 channel)
{
    BEGIN_CMD();
    TX_8(0xB1);
    TX_8(0x33);
    TX_16(screen_id);
    TX_16(control_id);
    TX_8(channel);
    END_CMD();
}
/*!
*  \brief     曲线控件-设置视图窗口
*  \param  screen_id 画面ID
*  \param  control_id 控件ID
*  \param  x_offset 水平偏移
*  \param  x_mul 水平缩放系数
*  \param  y_offset 垂直偏移
*  \param  y_mul 垂直缩放系数
*/
void GraphSetViewport(u16 screen_id,u16 control_id,s16 x_offset,u16 x_mul,s16 y_offset,u16 y_mul)
{
    BEGIN_CMD();
    TX_8(0xB1);
    TX_8(0x34);
    TX_16(screen_id);
    TX_16(control_id);
    TX_16(x_offset);
    TX_16(x_mul);
    TX_16(y_offset);
    TX_16(y_mul);
    END_CMD();
}
/*!
*  \brief     开始批量更新
*  \param  screen_id 画面ID
*/
void BatchBegin(u16 screen_id)
{
    BEGIN_CMD();
    TX_8(0xB1);
    TX_8(0x12);
    TX_16(screen_id);
}
/*!
*  \brief     批量更新按钮控件
*  \param  control_id 控件ID
*  \param  value 数值
*/
void BatchSetButtonValue(u16 control_id,u8 state)
{
    TX_16(control_id);
    TX_16(1);
    TX_8(state);
}
/*!
*  \brief     批量更新进度条控件
*  \param  control_id 控件ID
*  \param  value 数值
*/
void BatchSetProgressValue(u16 control_id,u32 value)
{
    TX_16(control_id);
    TX_16(4);
    TX_32(value);
}

/*!
*  \brief     批量更新滑动条控件
*  \param  control_id 控件ID
*  \param  value 数值
*/
void BatchSetSliderValue(u16 control_id,u32 value)
{
    TX_16(control_id);
    TX_16(4);
    TX_32(value);
}
/*!
*  \brief     批量更新仪表控件
*  \param  control_id 控件ID
*  \param  value 数值
*/
void BatchSetMeterValue(u16 control_id,u32 value)
{
    TX_16(control_id);
    TX_16(4);
    TX_32(value);
}
/*!
*  \brief      计算字符串长度
*/
u32 GetStringLen(u8 *str)
{
    u8 *p = str;
    while(*str)
    {
        str++;
    }

    return (str-p);
}
/*!
*  \brief     批量更新文本控件
*  \param  control_id 控件ID
*  \param  strings 字符串
*/
void BatchSetText(u16 control_id,u8 *strings)
{
    TX_16(control_id);
    TX_16(GetStringLen(strings));
    SendStrings(strings);
}


/*!
*  \brief     批量更新动画\图标控件
*  \param  control_id 控件ID
*  \param  frame_id 帧ID
*/
void BatchSetFrame(u16 control_id,u16 frame_id)
{
    TX_16(control_id);
    TX_16(2);
    TX_16(frame_id);
}

#if FIRMWARE_VER>=908

/*!
*  \brief     批量设置控件可见
*  \param  control_id 控件ID
*  \param  visible 帧ID
*/
void BatchSetVisible(u16 control_id,u8 visible)
{
    TX_16(control_id);
    TX_8(1);
    TX_8(visible);
}
/*!
*  \brief     批量设置控件使能
*  \param  control_id 控件ID
*  \param  enable 帧ID
*/
void BatchSetEnable(u16 control_id,u8 enable)
{
    TX_16(control_id);
    TX_8(2);
    TX_8(enable);
}

#endif
/*!
*  \brief    结束批量更新
*/
void BatchEnd()
{
    END_CMD();
}
/*!
*  \brief     设置倒计时控件
*  \param  screen_id 画面ID
*  \param  control_id 控件ID
*  \param  timeout 倒计时(秒)
*/
void SeTimer(u16 screen_id,u16 control_id,u32 timeout)
{
    BEGIN_CMD();
    TX_8(0xB1);
    TX_8(0x40);
    TX_16(screen_id);
    TX_16(control_id);
    TX_32(timeout);
    END_CMD();
}
/*!
*  \brief     开启倒计时控件
*  \param  screen_id 画面ID
*  \param  control_id 控件ID
*/
void StartTimer(u16 screen_id,u16 control_id)
{
    BEGIN_CMD();
    TX_8(0xB1);
    TX_8(0x41);
    TX_16(screen_id);
    TX_16(control_id);
    END_CMD();
}
/*!
*  \brief     停止倒计时控件
*  \param  screen_id 画面ID
*  \param  control_id 控件ID
*/
void StopTimer(u16 screen_id,u16 control_id)
{
    BEGIN_CMD();
    TX_8(0xB1);
    TX_8(0x42);
    TX_16(screen_id);
    TX_16(control_id);
    END_CMD();
}
/*!
*  \brief     暂停倒计时控件
*  \param  screen_id 画面ID
*  \param  control_id 控件ID
*/
void PauseTimer(u16 screen_id,u16 control_id)
{
    BEGIN_CMD();
    TX_8(0xB1);
    TX_8(0x44);
    TX_16(screen_id);
    TX_16(control_id);
    END_CMD();
}
/*!
*  \brief     设置控件背景色
*  \details  支持控件：进度条、文本
*  \param  screen_id 画面ID
*  \param  control_id 控件ID
*  \param  color 背景色
*/
void SetControlBackColor(u16 screen_id,u16 control_id,u16 color)
{
    BEGIN_CMD();
    TX_8(0xB1);
    TX_8(0x18);
    TX_16(screen_id);
    TX_16(control_id);
    TX_16(color);
    END_CMD();
}
/*!
*  \brief     设置控件前景色
* \details  支持控件：进度条
*  \param  screen_id 画面ID
*  \param  control_id 控件ID
*  \param  color 前景色
*/
void SetControlForeColor(u16 screen_id,u16 control_id,u16 color)
{
    BEGIN_CMD();
    TX_8(0xB1);
    TX_8(0x19);
    TX_16(screen_id);
    TX_16(control_id);
    TX_16(color);
    END_CMD();
}



/*!
*  \brief     显示\隐藏弹出菜单控件
*  \param  screen_id 画面ID
*  \param  control_id 控件ID
*  \param  show 是否显示，为0时focus_control_id无效
*  \param  focus_control_id 关联的文本控件(菜单控件的内容输出到文本控件)
*/
void ShowPopupMenu(u16 screen_id,u16 control_id,u8 show,u16 focus_control_id)
{
    BEGIN_CMD();
    TX_8(0xB1);
    TX_8(0x13);
    TX_16(screen_id);
    TX_16(control_id);
    TX_8(show);
    TX_16(focus_control_id);
    END_CMD();
}
/*!
*  \brief     显示\隐藏系统键盘
*  \param  show 0隐藏，1显示
*  \param  x 键盘显示位置X坐标
*  \param  y 键盘显示位置Y坐标
*  \param  type 0小键盘，1全键盘
*  \param  option 0正常字符，1密码，2时间设置
*  \param  max_len 键盘录入字符长度限制
*/
void ShowKeyboard(u8 show,u16 x,u16 y,u8 type,u8 option,u8 max_len)
{
    BEGIN_CMD();
    TX_8(0x86);
    TX_8(show);
    TX_16(x);
    TX_16(y);
    TX_8(type);
    TX_8(option);
    TX_8(max_len);
    END_CMD();
}

#if FIRMWARE_VER>=921
/*!
*  \brief     多语言设置
*  \param  ui_lang 用户界面语言0~9
*  \param  sys_lang 系统键盘语言-0中文，1英文
*/
void SetLanguage(u8 ui_lang,u8 sys_lang)
{
    u8 lang = ui_lang;
    if(sys_lang)   lang |= 0x80;

    BEGIN_CMD();
    TX_8(0xC1);
    TX_8(lang);
    TX_8(0xC1+lang);//校验，防止意外修改语言
    END_CMD();
}
#endif


#if FIRMWARE_VER>=921
/*!
*  \brief     开始保存控件数值到FLASH
*  \param  version 数据版本号，可任意指定，高16位为主版本号，低16位为次版本号
*  \param  address 数据在用户存储区的存放地址，注意防止地址重叠、冲突
*/
void FlashBeginSaveControl(u32 version,u32 address)
{
    BEGIN_CMD();
    TX_8(0xB1);
    TX_8(0xAA);
    TX_32(version);
    TX_32(address);
}

/*!
*  \brief     保存某个控件的数值到FLASH
*  \param  screen_id 画面ID
*  \param  control_id 控件ID
*/
void FlashSaveControl(u16 screen_id,u16 control_id)
{
    TX_16(screen_id);
    TX_16(control_id);
}
/*!
*  \brief     保存某个控件的数值到FLASH
*  \param  screen_id 画面ID
*  \param  control_id 控件ID
*/
void FlashEndSaveControl()
{
    END_CMD();
}
/*!
*  \brief     从FLASH中恢复控件数据
*  \param  version 数据版本号，主版本号必须与存储时一致，否则会加载失败
*  \param  address 数据在用户存储区的存放地址
*/
void FlashRestoreControl(u32 version,u32 address)
{
    BEGIN_CMD();
    TX_8(0xB1);
    TX_8(0xAB);
    TX_32(version);
    TX_32(address);
    END_CMD();
}

#endif

#if FIRMWARE_VER>=921
/*!
*  \brief     设置历史曲线采样数据值(单字节，u8或int8)
*  \param  screen_id 画面ID
*  \param  control_id 控件ID
*  \param  value 采样点数据
*  \param  channel 通道数
*/
void HistoryGraph_SetValueInt8(u16 screen_id,u16 control_id,u8 *value,u8 channel)
{
    BEGIN_CMD();
    TX_8(0xB1);
    TX_8(0x60);
    TX_16(screen_id);
    TX_16(control_id);
    TX_8N(value,channel);
    END_CMD();
}
/*!
*  \brief     设置历史曲线采样数据值(双字节，u16或s16)
*  \param  screen_id 画面ID
*  \param  control_id 控件ID
*  \param  value 采样点数据
*  \param  channel 通道数
*/
void HistoryGraph_SetValues16(u16 screen_id,u16 control_id,u16 *value,u8 channel)
{
    BEGIN_CMD();
    TX_8(0xB1);
    TX_8(0x60);
    TX_16(screen_id);
    TX_16(control_id);
    TX_16N(value,channel);
    END_CMD();
}
/*!
*  \brief     设置历史曲线采样数据值(四字节，u32或int32)
*  \param  screen_id 画面ID
*  \param  control_id 控件ID
*  \param  value 采样点数据
*  \param  channel 通道数
*/
void HistoryGraph_SetValueInt32(u16 screen_id,u16 control_id,u32 *value,u8 channel)
{
    u8 i = 0;

    BEGIN_CMD();
    TX_8(0xB1);
    TX_8(0x60);
    TX_16(screen_id);
    TX_16(control_id);

    for (;i<channel;++i)
    {
        TX_32(value[i]);
    }

    END_CMD();
}
/*!
*  \brief     设置历史曲线采样数据值(单精度浮点数)
*  \param  screen_id 画面ID
*  \param  control_id 控件ID
*  \param  value 采样点数据
*  \param  channel 通道数
*/
void HistoryGraph_SetValueFloat(u16 screen_id,u16 control_id,float *value,u8 channel)
{
    u8 i = 0;
    u32 tmp = 0;

    BEGIN_CMD();
    TX_8(0xB1);
    TX_8(0x60);
    TX_16(screen_id);
    TX_16(control_id);

    for (;i<channel;++i)
    {
        tmp = *(u32 *)(value+i);
        TX_32(tmp);
    }

    END_CMD();
}
/*!
*  \brief     允许或禁止历史曲线采样
*  \param  screen_id 画面ID
*  \param  control_id 控件ID
*  \param  enable 0-禁止，1-允许
*/
void HistoryGraph_EnableSampling(u16 screen_id,u16 control_id,u8 enable)
{
    BEGIN_CMD();
    TX_8(0xB1);
    TX_8(0x61);
    TX_16(screen_id);
    TX_16(control_id);
    TX_8(enable);
    END_CMD();
}
/*!
*  \brief     显示或隐藏历史曲线通道
*  \param  screen_id 画面ID
*  \param  control_id 控件ID
*  \param  channel 通道编号
*  \param  show 0-隐藏，1-显示
*/
void HistoryGraph_ShowChannel(u16 screen_id,u16 control_id,u8 channel,u8 show)
{
    BEGIN_CMD();
    TX_8(0xB1);
    TX_8(0x62);
    TX_16(screen_id);
    TX_16(control_id);
    TX_8(channel);
    TX_8(show);
    END_CMD();
}
/*!
*  \brief     设置历史曲线时间长度(即采样点数)
*  \param  screen_id 画面ID
*  \param  control_id 控件ID
*  \param  sample_count 一屏显示的采样点数
*/
void HistoryGraph_SetTimeLength(u16 screen_id,u16 control_id,u16 sample_count)
{
    BEGIN_CMD();
    TX_8(0xB1);
    TX_8(0x63);
    TX_16(screen_id);
    TX_16(control_id);
    TX_8(0x00);
    TX_16(sample_count);
    END_CMD();
}

/*!
*  \brief     历史曲线缩放到全屏
*  \param  screen_id 画面ID
*  \param  control_id 控件ID
*/
void HistoryGraph_SetTimeFullScreen(u16 screen_id,u16 control_id)
{
    BEGIN_CMD();
    TX_8(0xB1);
    TX_8(0x63);
    TX_16(screen_id);
    TX_16(control_id);
    TX_8(0x01);
    END_CMD();
}
/*!
*  \brief     设置历史曲线缩放比例系数
*  \param  screen_id 画面ID
*  \param  control_id 控件ID
*  \param  zoom 缩放百分比(zoom>100%时水平方向缩小，反正放大)
*  \param  max_zoom 缩放限制，一屏最多显示采样点数
*  \param  min_zoom 缩放限制，一屏最少显示采样点数
*/
void HistoryGraph_SetTimeZoom(u16 screen_id,u16 control_id,u16 zoom,u16 max_zoom,u16 min_zoom)
{
    BEGIN_CMD();
    TX_8(0xB1);
    TX_8(0x63);
    TX_16(screen_id);
    TX_16(control_id);
    TX_8(0x02);
    TX_16(zoom);
    TX_16(max_zoom);
    TX_16(min_zoom);
    END_CMD();
}

#endif

#if SD_FILE_EN
/*!
*  \brief     检测SD卡是否插入
*/
void SD_IsInsert(void)
{
    BEGIN_CMD();
    TX_8(0x36);
    TX_8(0x01);
    END_CMD();
}
/*!
*  \brief     打开或创建文件
*  \param  filename 文件名称(仅ASCII编码)
*  \param  mode 模式，可选组合模式如上FA_XXXX
*/
void SD_CreateFile(u8 *filename,u8 mode)
{
    BEGIN_CMD();
    TX_8(0x36);
    TX_8(0x05);
    TX_8(mode);
    SendStrings(filename);
    END_CMD();
}
/*!
*  \brief     以当前时间创建文件，例如:20161015083000.txt
*  \param  ext 文件后缀，例如 txt
*/
void SD_CreateFileByTime(u8 *ext)
{
    BEGIN_CMD();
    TX_8(0x36);
    TX_8(0x02);
    SendStrings(ext);
    END_CMD();
}
/*!
*  \brief     在当前文件末尾写入数据
*  \param  buffer 数据
*  \param  dlc 数据长度
*/
void SD_WriteFile(u8 *buffer,u16 dlc)
{
    BEGIN_CMD();
    TX_8(0x36);
    TX_8(0x03);
    TX_16(dlc);
    TX_8N(buffer,dlc);
    END_CMD();
}
/*!
*  \brief     读取当前文件
*  \param  offset 文件位置偏移
*  \param  dlc 数据长度
*/
void SD_ReadFile(u32 offset,u16 dlc)
{
    BEGIN_CMD();
    TX_8(0x36);
    TX_8(0x07);
    TX_32(offset);
    TX_16(dlc);
    END_CMD();
}

/*!
*  \brief     获取当前文件长度
*/
void SD_GetFileSize()
{
    BEGIN_CMD();
    TX_8(0x36);
    TX_8(0x06);
    END_CMD();
}
/*!
*  \brief     关闭当前文件
*/
void SD_CloseFile()
{
    BEGIN_CMD();
    TX_8(0x36);
    TX_8(0x04);
    END_CMD();
}

#endif//SD_FILE_EN
/*!
*  \brief     记录控件-触发警告
*  \param  screen_id 画面ID
*  \param  control_id 控件ID
*  \param  value 告警值
*  \param  time 告警产生的时间，为0时使用屏幕内部时间
*/
void Record_SetEvent(u16 screen_id,u16 control_id,u16 value,u8 *time)
{
    u8 i  = 0;

    BEGIN_CMD();
    TX_8(0xB1);
    TX_8(0x50);
    TX_16(screen_id);
    TX_16(control_id);
    TX_16(value);

    if(time)
    {
        for(i=0;i<7;++i)
            TX_8(time[i]);
    }

    END_CMD();
}
/*!
*  \brief     记录控件-解除警告
*  \param  screen_id 画面ID
*  \param  control_id 控件ID
*  \param  value 告警值
*  \param  time 告警解除的时间，为0时使用屏幕内部时间
*/
void Record_ResetEvent(u16 screen_id,u16 control_id,u16 value,u8 *time)
{
    u8 i  = 0;

    BEGIN_CMD();
    TX_8(0xB1);
    TX_8(0x51);
    TX_16(screen_id);
    TX_16(control_id);
    TX_16(value);

    if(time)
    {
        for(i=0;i<7;++i)
            TX_8(time[i]);
    }

    END_CMD();
}
/*!
*  \brief    记录控件- 添加常规记录
*  \param  screen_id 画面ID
*  \param  control_id 控件ID
*  \param  record 一条记录(字符串)，子项通过分号隔开，例如：第一项;第二项;第三项;
*/
void Record_Add(u16 screen_id,u16 control_id,u8 *record)
{
    BEGIN_CMD();
    TX_8(0xB1);
    TX_8(0x52);
    TX_16(screen_id);
    TX_16(control_id);

    SendStrings(record);

    END_CMD();
}
/*!
*  \brief     记录控件-清除记录数据
*  \param  screen_id 画面ID
*  \param  control_id 控件ID
*/
void Record_Clear(u16 screen_id,u16 control_id)
{
    BEGIN_CMD();
    TX_8(0xB1);
    TX_8(0x53);
    TX_16(screen_id);
    TX_16(control_id);
    END_CMD();
}
/*!
*  \brief     记录控件-设置记录显示偏移
*  \param  screen_id 画面ID
*  \param  control_id 控件ID
*  \param  offset 显示偏移，滚动条位置
*/
void Record_SetOffset(u16 screen_id,u16 control_id,u16 offset)
{
    BEGIN_CMD();
    TX_8(0xB1);
    TX_8(0x54);
    TX_16(screen_id);
    TX_16(control_id);
    TX_16(offset);
    END_CMD();
}
/*!
*  \brief     记录控件-获取当前记录数目
*  \param  screen_id 画面ID
*  \param  control_id 控件ID
*/
void Record_GetCount(u16 screen_id,u16 control_id)
{
    BEGIN_CMD();
    TX_8(0xB1);
    TX_8(0x55);
    TX_16(screen_id);
    TX_16(control_id);
    END_CMD();
}
/*!
*  \brief     读取屏幕RTC时间
*/
void ReadRTC(void)
{
    BEGIN_CMD();
    TX_8(0x82);
    END_CMD();
}

/*!
*  \brief   播放音乐
*  \param   buffer 十六进制的音乐路径及名字
*/
void PlayMusic(u8 *buffer)
{
    u8 i  = 0;

    BEGIN_CMD();
    if(buffer)
    {
        for(i=0;i<19;++i)
            TX_8(buffer[i]);
    }
    END_CMD();
}


