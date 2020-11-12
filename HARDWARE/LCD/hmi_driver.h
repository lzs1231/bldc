/*! 
*  \file hmi_driver.h
*  \brief �����������ļ�
*  \version 1.0
*  \date 2012-2018
*  \copyright ���ݴ�ʹ��Ƽ����޹�˾
*/

#ifndef _HMI_DRIVER_
#define _HMI_DRIVER_

#define FIRMWARE_VER 921          // ���ִ˹̼��汾������ʵ��Ļһ�£�ȷ���������ܿ���
#define CRC16_ENABLE 1            // �����ҪCRC16У�鹦�ܣ��޸Ĵ˺�Ϊ1(��ʱ��Ҫ��VisualTFT��������CRCУ��)
#define CMD_MAX_SIZE 128           // ����ָ���С��������Ҫ�������������ô�һЩ
#define QUEUE_MAX_SIZE 512        // ָ����ջ�������С��������Ҫ�������������ô�һЩ

#define SD_FILE_EN 0



#include "usart.h"

/*! 
*  \brief  ��������Ƿ����CRC16У��
*  \param buffer ��У������ݣ�ĩβ�洢CRC16
*  \param n ���ݳ��ȣ�����CRC16
*  \return У��ͨ������1�����򷵻�0
*/
u16 CheckCRC16(u8 *buffer,u16 n);

/*! 
*  \brief  ��ʱ
*  \param  n ��ʱʱ��(���뵥λ)
*/
void DelayMS(unsigned int n);

/*! 
*  \brief  �����豸���ã�����֮����Ҫ�����������޸Ĳ����ʡ���������������������ʽ
*/
void LockDeviceConfig(void);

/*! 
*  \brief  �����豸����
*/
void UnlockDeviceConfig(void);

/*! 
*  \brief    �޸Ĵ������Ĳ�����
*  \details  ������ѡ�Χ[0~14]����Ӧʵ�ʲ�����
{1200,2400,4800,9600,19200,38400,57600,115200,1000000,2000000,218750,437500,875000,921800,2500000}
*  \param  option ������ѡ��
*/
void SetCommBps(u8 option);

/*! 
*  \brief  ������������
*/
void SetHandShake(void);

/*! 
*  \brief  ����ǰ��ɫ
*  \param  color ǰ��ɫ
*/
void SetFcolor(u16 color);

/*! 
*  \brief  ���ñ���ɫ
*  \param  color ����ɫ
*/
void SetBcolor(u16 color);

/*! 
*  \brief  �������
*/
void GUI_CleanScreen(void);

/*! 
*  \brief  �������ּ��
*  \param  x_w ������
*  \param  y_w ������
*/
void SetTextSpace(u8 x_w, u8 y_w);

/*! 
*  \brief  ����������ʾ����
*  \param  enable �Ƿ���������
*  \param  width ���
*  \param  height �߶�
*/
void SetFont_Region(u8 enable,u16 width,u16 height );

/*! 
*  \brief  ���ù���ɫ
*  \param  fillcolor_dwon ��ɫ�½�
*  \param  fillcolor_up ��ɫ�Ͻ�
*/
void SetFilterColor(u16 fillcolor_dwon, u16 fillcolor_up);

/*! 
*  \brief  ���ù���ɫ
*  \param  x λ��X����
*  \param  y λ��Y����
*  \param  back ��ɫ�Ͻ�
*  \param  font ����
*  \param  strings �ַ�������
*/
void DisText(u16 x, u16 y,u8 back,u8 font,u8 *strings );

/*! 
*  \brief    ��ʾ���
*  \param  enable �Ƿ���ʾ
*  \param  x λ��X����
*  \param  y λ��Y����
*  \param  width ���
*  \param  height �߶�
*/
void DisCursor(u8 enable,u16 x, u16 y,u8 width,u8 height );

/*! 
*  \brief      ��ʾȫ��ͼƬ
*  \param  image_id ͼƬ����
*  \param  masken �Ƿ�����͸������
*/
void DisFull_Image(u16 image_id,u8 masken);

/*! 
*  \brief      ָ��λ����ʾͼƬ
*  \param  x λ��X����
*  \param  y λ��Y����
*  \param  image_id ͼƬ����
*  \param  masken �Ƿ�����͸������
*/
void DisArea_Image(u16 x,u16 y,u16 image_id,u8 masken);

/*! 
*  \brief      ��ʾ�ü�ͼƬ
*  \param  x λ��X����
*  \param  y λ��Y����
*  \param  image_id ͼƬ����
*  \param  image_x ͼƬ�ü�λ��X����
*  \param  image_y ͼƬ�ü�λ��Y����
*  \param  image_l ͼƬ�ü�����
*  \param  image_w ͼƬ�ü��߶�
*  \param  masken �Ƿ�����͸������
*/
void DisCut_Image(u16 x,u16 y,u16 image_id,u16 image_x,u16 image_y,
				  u16 image_l, u16 image_w,u8 masken);

/*! 
*  \brief      ��ʾGIF����
*  \param  x λ��X����
*  \param  y λ��Y����
*  \param  flashimage_id ͼƬ����
*  \param  enable �Ƿ���ʾ
*  \param  playnum ���Ŵ���
*/
void DisFlashImage(u16 x,u16 y,u16 flashimage_id,u8 enable,u8 playnum);

/*! 
*  \brief      ����
*  \param  x λ��X����
*  \param  y λ��Y����
*/
void GUI_Dot(u16 x,u16 y);

/*! 
*  \brief      ����
*  \param  x0 ��ʼλ��X����
*  \param  y0 ��ʼλ��Y����
*  \param  x1 ����λ��X����
*  \param  y1 ����λ��Y����
*/
void GUI_Line(u16 x0, u16 y0, u16 x1, u16 y1);

/*! 
*  \brief      ������
*  \param  mode ģʽ
*  \param  dot ���ݵ�
*  \param  dot_cnt ����
*/
void GUI_ConDots(u8 mode,u16 *dot,u16 dot_cnt);

/*! 
*  \brief      ������Բ
*  \param  x0 Բ��λ��X����
*  \param  y0 Բ��λ��Y����
*  \param  r �뾶
*/
void GUI_Circle(u16 x0, u16 y0, u16 r);

/*! 
*  \brief      ��ʵ��Բ
*  \param  x0 Բ��λ��X����
*  \param  y0 Բ��λ��Y����
*  \param  r �뾶
*/
void GUI_CircleFill(u16 x0, u16 y0, u16 r);


void SetPowerSaving(u8 enable, u8 bl_off_level, u8 bl_on_level, u16  bl_on_time);
/*! 
*  \brief      ������
*  \param  x0 Բ��λ��X����
*  \param  y0 Բ��λ��Y����
*  \param  r �뾶
*  \param  sa ��ʼ�Ƕ�
*  \param  ea ��ֹ�Ƕ�
*/
void GUI_Arc(u16 x,u16 y, u16 r,u16 sa, u16 ea);

/*! 
*  \brief      �����ľ���
*  \param  x0 ��ʼλ��X����
*  \param  y0 ��ʼλ��Y����
*  \param  x1 ����λ��X����
*  \param  y1 ����λ��Y����
*/
void GUI_Rectangle(u16 x0, u16 y0, u16 x1,u16 y1 );

/*! 
*  \brief      ��ʵ�ľ���
*  \param  x0 ��ʼλ��X����
*  \param  y0 ��ʼλ��Y����
*  \param  x1 ����λ��X����
*  \param  y1 ����λ��Y����
*/
void GUI_RectangleFill(u16 x0, u16 y0, u16 x1,u16 y1 );

/*! 
*  \brief      ��������Բ
*  \param  x0 ��ʼλ��X����
*  \param  y0 ��ʼλ��Y����
*  \param  x1 ����λ��X����
*  \param  y1 ����λ��Y����
*/
void GUI_Ellipse (u16 x0, u16 y0, u16 x1,u16 y1 );

/*! 
*  \brief      ��ʵ����Բ
*  \param  x0 ��ʼλ��X����
*  \param  y0 ��ʼλ��Y����
*  \param  x1 ����λ��X����
*  \param  y1 ����λ��Y����
*/
void GUI_EllipseFill (u16 x0, u16 y0, u16 x1,u16 y1 );

/*! 
*  \brief      ����
*  \param  x0 ��ʼλ��X����
*  \param  y0 ��ʼλ��Y����
*  \param  x1 ����λ��X����
*  \param  y1 ����λ��Y����
*/
void SetBackLight(u8 light_level);

/*! 
*  \brief   ����������
*  \time  time ����ʱ��(���뵥λ)
*/
void SetBuzzer(u8 time);

/*! 
*  \brief   ����������
*  \param enable ����ʹ��
*  \param beep_on ����������
*  \param work_mode ��������ģʽ��0���¾��ϴ���1�ɿ����ϴ���2�����ϴ�����ֵ��3���º��ɿ����ϴ�����
*  \param press_calibration �������������20��У׼��������0���ã�1����
*/
void SetTouchPaneOption(u8 enbale,u8 beep_on,u8 work_mode,u8 press_calibration);

/*! 
*  \brief   У׼������
*/
void	CalibrateTouchPane(void);

/*! 
*  \brief  ����������
*/
void TestTouchPane(void);

/*! 
*  \brief      ���õ�ǰд��ͼ��
*  \details  һ������ʵ��˫����Ч��(��ͼʱ������˸)��
*  \details  u8 layer = 0;
*  \details  WriteLayer(layer);   ����д���
*  \details  ClearLayer(layer);   ʹͼ���͸��
*  \details  ���һϵ�л�ͼָ��
*  \details  DisText(100,100,0,4,"hello hmi!!!");
*  \details  DisplyLayer(layer);  �л���ʾ��
*  \details  layer = (layer+1)%2; ˫�����л�
*  \see DisplyLayer
*  \see ClearLayer
*  \param  layer ͼ����
*/
void WriteLayer(u8 layer);

/*! 
*  \brief      ���õ�ǰ��ʾͼ��
*  \param  layer ͼ����
*/
void DisplyLayer(u8 layer);

/*! 
*  \brief      ���ͼ�㣬ʹͼ����͸��
*  \param  layer ͼ����
*/
void ClearLayer(u8 layer);

/*! 
*  \brief  д���ݵ��������û��洢��
*  \param  startAddress ��ʼ��ַ
*  \param  length �ֽ���
*  \param  _data ��д�������
*/
void WriteUserFlash(u32 startAddress,u16 length,u8 *_data);

/*! 
*  \brief  �Ӵ������û��洢����ȡ����
*  \param  startAddress ��ʼ��ַ
*  \param  length �ֽ���
*/
void ReadUserFlash(u32 startAddress,u16 length);

/*! 
*  \brief      ����ͼ��
*  \param  src_layer ԭʼͼ��
*  \param  dest_layer Ŀ��ͼ��
*/
void CopyLayer(u8 src_layer,u8 dest_layer);

/*! 
*  \brief      ���õ�ǰ����
*  \param  screen_id ����ID
*/
void SetScreen(u16 screen_id);

/*! 
*  \brief      ��ȡ��ǰ����
*/
void GetScreen(void);

/*! 
*  \brief     ����\���û������
*  \details ����\����һ��ɶ�ʹ�ã����ڱ�����˸�����ˢ���ٶ�
*  \details �÷���
*	\details SetScreenUpdateEnable(0);//��ֹ����
*	\details һϵ�и��»����ָ��
*	\details SetScreenUpdateEnable(1);//��������
*  \param  enable 0���ã�1����
*/
void SetScreenUpdateEnable(u8 enable);

/*! 
*  \brief     ���ÿؼ����뽹��
*  \param  screen_id ����ID
*  \param  control_id �ؼ�ID
*  \param  focus �Ƿ�������뽹��
*/
void SetControlFocus(u16 screen_id,u16 control_id,u8 focus);

/*! 
*  \brief     ��ʾ\���ؿؼ�
*  \param  screen_id ����ID
*  \param  control_id �ؼ�ID
*  \param  visible �Ƿ���ʾ
*/
void SetControlVisiable(u16 screen_id,u16 control_id,u8 visible);

/*! 
*  \brief     ���ô����ؼ�ʹ��
*  \param  screen_id ����ID
*  \param  control_id �ؼ�ID
*  \param  enable �ؼ��Ƿ�ʹ��
*/
void SetControlEnable(u16 screen_id,u16 control_id,u8 enable);

/*! 
*  \brief     ��ȡ�ؼ�ֵ
*  \param  screen_id ����ID
*  \param  control_id �ؼ�ID
*/
void GetControlValue(u16 screen_id,u16 control_id);

/*! 
*  \brief     ���ð�ť״̬
*  \param  screen_id ����ID
*  \param  control_id �ؼ�ID
*  \param  value ��ť״̬
*/
void SetButtonValue(u16 screen_id,u16 control_id,u8 value);

/*********
��ȡ��ť״̬
*********/
void ReadButtonValue(u16 screen_id,u16 control_id);

/*********
�����ı���˸
*********/
void TextTwinkle(u16 screen_id,u16 control_id,u16 time);

/*
*       �ı�����
*/
void TextScrolling(u16 screen_id,u16 control_id,u16 speed);

/*! 
*  \brief     �����ı�ֵ
*  \param  screen_id ����ID
*  \param  control_id �ؼ�ID
*  \param  str �ı�ֵ
*/
void SetTextValue(u16 screen_id,u16 control_id,u8 *str);

#if FIRMWARE_VER>=908

/*! 
*  \brief     �����ı�Ϊ����ֵ��Ҫ��FIRMWARE_VER>=908
*  \param  screen_id ����ID
*  \param  control_id �ؼ�ID
*  \param  value �ı���ֵ
*  \param  sign 0-�޷��ţ�1-�з���
*  \param  fill_zero ����λ��������ʱ��ಹ��
*/
void SetTextInt32(u16 screen_id,u16 control_id,u32 value,u8 sign,u8 fill_zero);

/*! 
*  \brief     �����ı������ȸ���ֵ��Ҫ��FIRMWARE_VER>=908
*  \param  screen_id ����ID
*  \param  control_id �ؼ�ID
*  \param  value �ı���ֵ
*  \param  precision С��λ��
*  \param  show_zeros Ϊ1ʱ����ʾĩβ0
*/
void SetTextFloat(u16 screen_id,u16 control_id,float value,u8 precision,u8 show_zeros);

#endif

/*! 
*  \brief      ���ý���ֵ
*  \param  screen_id ����ID
*  \param  control_id �ؼ�ID
*  \param  value ��ֵ
*/
void SetProgressValue(u16 screen_id,u16 control_id,u32 value);

/*! 
*  \brief     �����Ǳ�ֵ
*  \param  screen_id ����ID
*  \param  control_id �ؼ�ID
*  \param  value ��ֵ
*/
void SetMeterValue(u16 screen_id,u16 control_id,u32 value);

/*! 
*  \brief     �����Ǳ�ֵ
*  \param  screen_id ����ID
*  \param  control_id ͼƬ�ؼ�ID
*  \param  value ��ֵ
*/
void Set_picMeterValue(u16 screen_id,u16 control_id,u16 value);

/*! 
*  \brief      ���û�����
*  \param  screen_id ����ID
*  \param  control_id �ؼ�ID
*  \param  value ��ֵ
*/

void SetSliderValue(u16 screen_id,u16 control_id,u32 value);

/*! 
*  \brief      ����ѡ��ؼ�
*  \param  screen_id ����ID
*  \param  control_id �ؼ�ID
*  \param  item ��ǰѡ��
*/
void SetSelectorValue(u16 screen_id,u16 control_id,u8 item);

/*! 
*  \brief      ��ʼ���Ŷ���
*  \param  screen_id ����ID
*  \param  control_id �ؼ�ID
*/
void AnimationStart(u16 screen_id,u16 control_id);

/*! 
*  \brief      ֹͣ���Ŷ���
*  \param  screen_id ����ID
*  \param  control_id �ؼ�ID
*/
void AnimationStop(u16 screen_id,u16 control_id);

/*! 
*  \brief      ��ͣ���Ŷ���
*  \param  screen_id ����ID
*  \param  control_id �ؼ�ID
*/
void AnimationPause(u16 screen_id,u16 control_id);

/*! 
*  \brief     �����ƶ�֡
*  \param  screen_id ����ID
*  \param  control_id �ؼ�ID
*  \param  frame_id ֡ID
*/
void AnimationPlayFrame(u16 screen_id,u16 control_id,u8 frame_id);

/*! 
*  \brief     ������һ֡
*  \param  screen_id ����ID
*  \param  control_id �ؼ�ID
*/
void AnimationPlayPrev(u16 screen_id,u16 control_id);

/*! 
*  \brief     ������һ֡
*  \param  screen_id ����ID
*  \param  control_id �ؼ�ID
*/
void AnimationPlayNext(u16 screen_id,u16 control_id);

/*! 
*  \brief     ���߿ؼ�-���ͨ��
*  \param  screen_id ����ID
*  \param  control_id �ؼ�ID
*  \param  channel ͨ����
*  \param  color ��ɫ
*/
void GraphChannelAdd(u16 screen_id,u16 control_id,u8 channel,u16 color);

/*! 
*  \brief     ���߿ؼ�-ɾ��ͨ��
*  \param  screen_id ����ID
*  \param  control_id �ؼ�ID
*  \param  channel ͨ����
*/
void GraphChannelDel(u16 screen_id,u16 control_id,u8 channel);

/*! 
*  \brief     ���߿ؼ�-�������
*  \param  screen_id ����ID
*  \param  control_id �ؼ�ID
*  \param  channel ͨ����
*  \param  pData ��������
*  \param  nDataLen ���ݸ���
*/
void GraphChannelDataAdd(u16 screen_id,u16 control_id,u8 channel,u8 *pData,u16 nDataLen);

/*! 
*  \brief     ���߿ؼ�-�������
*  \param  screen_id ����ID
*  \param  control_id �ؼ�ID
*  \param  channel ͨ����
*/
void GraphChannelDataClear(u16 screen_id,u16 control_id,u8 channel);

/*! 
*  \brief     ���߿ؼ�-������ͼ����
*  \param  screen_id ����ID
*  \param  control_id �ؼ�ID
*  \param  x_offset ˮƽƫ��
*  \param  x_mul ˮƽ����ϵ��
*  \param  y_offset ��ֱƫ��
*  \param  y_mul ��ֱ����ϵ��
*/
void GraphSetViewport(u16 screen_id,u16 control_id,s16 x_offset,u16 x_mul,s16 y_offset,u16 y_mul);

/*! 
*  \brief     ��ʼ��������
*  \param  screen_id ����ID
*/
void BatchBegin(u16 screen_id);

/*! 
*  \brief     �������°�ť�ؼ�
*  \param  control_id �ؼ�ID
*  \param  value ��ֵ
*/
void BatchSetButtonValue(u16 control_id,u8 state);

/*! 
*  \brief     �������½������ؼ�
*  \param  control_id �ؼ�ID
*  \param  value ��ֵ
*/
void BatchSetProgressValue(u16 control_id,u32 value);

/*! 
*  \brief     �������»������ؼ�
*  \param  control_id �ؼ�ID
*  \param  value ��ֵ
*/
void BatchSetSliderValue(u16 control_id,u32 value);

/*! 
*  \brief     ���������Ǳ�ؼ�
*  \param  control_id �ؼ�ID
*  \param  value ��ֵ
*/
void BatchSetMeterValue(u16 control_id,u32 value);

/*! 
*  \brief      �����ַ�������
*/
u32 GetStringLen(u8 *str);

/*! 
*  \brief     ���������ı��ؼ�
*  \param  control_id �ؼ�ID
*  \param  strings �ַ���
*/
void BatchSetText(u16 control_id,u8 *strings);

/*! 
*  \brief     �������¶���\ͼ��ؼ�
*  \param  control_id �ؼ�ID
*  \param  frame_id ֡ID
*/
void BatchSetFrame(u16 control_id,u16 frame_id);

#if FIRMWARE_VER>=921

/*! 
*  \brief     �������ÿؼ��ɼ�
*  \param  control_id �ؼ�ID
*  \param  visible ֡ID
*/
void BatchSetVisible(u16 control_id,u8 visible);

/*! 
*  \brief     �������ÿؼ�ʹ��
*  \param  control_id �ؼ�ID
*  \param  enable ֡ID
*/
void BatchSetEnable(u16 control_id,u8 enable);

#endif

/*! 
*  \brief    ������������
*/
void BatchEnd(void);

/*! 
*  \brief     ���õ���ʱ�ؼ�
*  \param  screen_id ����ID
*  \param  control_id �ؼ�ID
*  \param  timeout ����ʱ(��)
*/
void SeTimer(u16 screen_id,u16 control_id,u32 timeout);

/*! 
*  \brief     ��������ʱ�ؼ�
*  \param  screen_id ����ID
*  \param  control_id �ؼ�ID
*/
void StartTimer(u16 screen_id,u16 control_id);

/*! 
*  \brief     ֹͣ����ʱ�ؼ�
*  \param  screen_id ����ID
*  \param  control_id �ؼ�ID
*/
void StopTimer(u16 screen_id,u16 control_id);

/*! 
*  \brief     ��ͣ����ʱ�ؼ�
*  \param  screen_id ����ID
*  \param  control_id �ؼ�ID
*/
void PauseTimer(u16 screen_id,u16 control_id);

/*! 
*  \brief     ���ÿؼ�����ɫ
*  \details  ֧�ֿؼ������������ı�
*  \param  screen_id ����ID
*  \param  control_id �ؼ�ID
*  \param  color ����ɫ
*/
void SetControlBackColor(u16 screen_id,u16 control_id,u16 color);

/*! 
*  \brief     ���ÿؼ�ǰ��ɫ
* \details  ֧�ֿؼ���������
*  \param  screen_id ����ID
*  \param  control_id �ؼ�ID
*  \param  color ǰ��ɫ
*/
void SetControlForeColor(u16 screen_id,u16 control_id,u16 color);

/*! 
*  \brief     ��ʾ\���ص����˵��ؼ�
*  \param  screen_id ����ID
*  \param  control_id �ؼ�ID
*  \param  show �Ƿ���ʾ��Ϊ0ʱfocus_control_id��Ч
*  \param  focus_control_id �������ı��ؼ�(�˵��ؼ�������������ı��ؼ�)
*/
void ShowPopupMenu(u16 screen_id,u16 control_id,u8 show,u16 focus_control_id);

/*! 
*  \brief     ��ʾ\����ϵͳ����
*  \param  show 0���أ�1��ʾ
*  \param  x ������ʾλ��X����
*  \param  y ������ʾλ��Y����
*  \param  type 0С���̣�1ȫ����
*  \param  option 0�����ַ���1���룬2ʱ������
*  \param  max_len ����¼���ַ���������
*/
void ShowKeyboard(u8 show,u16 x,u16 y,u8 type,u8 option,u8 max_len);

#if FIRMWARE_VER>=914
/*! 
*  \brief     ����������
*  \param  ui_lang �û���������0~9
*  \param  sys_lang ϵͳ��������-0���ģ�1Ӣ��
*/
void SetLanguage(u8 ui_lang,u8 sys_lang);
#endif

#if FIRMWARE_VER>=917
/*! 
*  \brief     ��ʼ����ؼ���ֵ��FLASH
*  \param  version ���ݰ汾�ţ�������ָ������16λΪ���汾�ţ���16λΪ�ΰ汾��
*  \param  address �������û��洢���Ĵ�ŵ�ַ��ע���ֹ��ַ�ص�����ͻ
*/
void FlashBeginSaveControl(u32 version,u32 address);

/*! 
*  \brief     ����ĳ���ؼ�����ֵ��FLASH
*  \param  screen_id ����ID
*  \param  control_id �ؼ�ID
*/
void FlashSaveControl(u16 screen_id,u16 control_id);

/*! 
*  \brief     �������
*/
void FlashEndSaveControl(void);

/*! 
*  \brief     ��FLASH�лָ��ؼ�����
*  \param  version ���ݰ汾�ţ����汾�ű�����洢ʱһ�£���������ʧ��
*  \param  address �������û��洢���Ĵ�ŵ�ַ
*/
void FlashRestoreControl(u32 version,u32 address);
#endif

#if FIRMWARE_VER>=921
/*! 
*  \brief     ������ʷ���߲�������ֵ(���ֽڣ�u8��int8)
*  \param  screen_id ����ID
*  \param  control_id �ؼ�ID
*  \param  value ����������
*  \param  channel ͨ����
*/
void HistoryGraph_SetValueInt8(u16 screen_id,u16 control_id,u8 *value,u8 channel);

/*! 
*  \brief     ������ʷ���߲�������ֵ(˫�ֽڣ�u16��s16)
*  \param  screen_id ����ID
*  \param  control_id �ؼ�ID
*  \param  value ����������
*  \param  channel ͨ����
*/
void HistoryGraph_SetValues16(u16 screen_id,u16 control_id,u16 *value,u8 channel);

/*! 
*  \brief     ������ʷ���߲�������ֵ(���ֽڣ�u32��int32)
*  \param  screen_id ����ID
*  \param  control_id �ؼ�ID
*  \param  value ����������
*  \param  channel ͨ����
*/
void HistoryGraph_SetValueInt32(u16 screen_id,u16 control_id,u32 *value,u8 channel);

/*! 
*  \brief     ������ʷ���߲�������ֵ(�����ȸ�����)
*  \param  screen_id ����ID
*  \param  control_id �ؼ�ID
*  \param  value ����������
*  \param  channel ͨ����
*/
void HistoryGraph_SetValueFloat(u16 screen_id,u16 control_id,float *value,u8 channel);

/*! 
*  \brief     ������ֹ��ʷ���߲���
*  \param  screen_id ����ID
*  \param  control_id �ؼ�ID
*  \param  enable 0-��ֹ��1-����
*/
void HistoryGraph_EnableSampling(u16 screen_id,u16 control_id,u8 enable);

/*! 
*  \brief     ��ʾ��������ʷ����ͨ��
*  \param  screen_id ����ID
*  \param  control_id �ؼ�ID
*  \param  channel ͨ�����
*  \param  show 0-���أ�1-��ʾ
*/
void HistoryGraph_ShowChannel(u16 screen_id,u16 control_id,u8 channel,u8 show);


/*! 
*  \brief     ������ʷ����ʱ�䳤��(����������)
*  \param  screen_id ����ID
*  \param  control_id �ؼ�ID
*  \param  sample_count һ����ʾ�Ĳ�������
*/
void HistoryGraph_SetTimeLength(u16 screen_id,u16 control_id,u16 sample_count);

/*! 
*  \brief     ��ʷ�������ŵ�ȫ��
*  \param  screen_id ����ID
*  \param  control_id �ؼ�ID
*/
void HistoryGraph_SetTimeFullScreen(u16 screen_id,u16 control_id);

/*! 
*  \brief     ������ʷ�������ű���ϵ��
*  \param  screen_id ����ID
*  \param  control_id �ؼ�ID
*  \param  zoom ���Űٷֱ�(zoom>100%ʱˮƽ������С�������Ŵ�)
*  \param  max_zoom �������ƣ�һ�������ʾ��������
*  \param  min_zoom �������ƣ�һ��������ʾ��������
*/
void HistoryGraph_SetTimeZoom(u16 screen_id,u16 control_id,u16 zoom,u16 max_zoom,u16 min_zoom);
#endif

#if SD_FILE_EN
/*! 
*  \brief     ���SD���Ƿ����
*/
void SD_IsInsert(void);

#define	FA_READ				   0x01  // �ɶ�ȡ
#define	FA_WRITE			   0x02  // ��д��
#define	FA_CREATE_NEW		   0x04  // �������ļ�������ļ��Ѿ����ڣ��򷵻�ʧ��
#define	FA_CREATE_ALWAYS	   0x08  // �������ļ�������ļ��Ѿ����ڣ��򸲸�
#define	FA_OPEN_EXISTING	   0x00  // ���ļ�������ļ������ڣ��򷵻�ʧ��
#define	FA_OPEN_ALWAYS		   0x10  // ���ļ�������ļ������ڣ��򴴽����ļ�

/*! 
*  \brief     �򿪻򴴽��ļ�
*  \param  filename �ļ�����(��ASCII����)
*  \param  mode ģʽ����ѡ���ģʽ����FA_XXXX
*/
void SD_CreateFile(u8 *filename,u8 mode);

/*! 
*  \brief     �Ե�ǰʱ�䴴���ļ�������:20161015083000.txt
*  \param  ext �ļ���׺������ txt
*/
void SD_CreateFileByTime(u8 *ext);

/*! 
*  \brief     �ڵ�ǰ�ļ�ĩβд������
*  \param  buffer ����
*  \param  dlc ���ݳ���
*/
void SD_WriteFile(u8 *buffer,u16 dlc);

/*! 
*  \brief     ��ȡ��ǰ�ļ�
*  \param  offset �ļ�λ��ƫ��
*  \param  dlc ���ݳ���
*/
void SD_ReadFile(u32 offset,u16 dlc);

/*! 
*  \brief     ��ȡ��ǰ�ļ�����
*/
void SD_GetFileSize();

/*! 
*  \brief     �رյ�ǰ�ļ�
*/
void SD_CloseFile();
#endif

/*! 
*  \brief     ��¼�ؼ�-��������
*  \param  screen_id ����ID
*  \param  control_id �ؼ�ID
*  \param  value �澯ֵ
*  \param  time �澯������ʱ�䣬Ϊ0ʱʹ����Ļ�ڲ�ʱ��
*/
void Record_SetEvent(u16 screen_id,u16 control_id,u16 value,u8 *time);

/*! 
*  \brief     ��¼�ؼ�-�������
*  \param  screen_id ����ID
*  \param  control_id �ؼ�ID
*  \param  value �澯ֵ
*  \param  time �澯�����ʱ�䣬Ϊ0ʱʹ����Ļ�ڲ�ʱ��
*/
void Record_ResetEvent(u16 screen_id,u16 control_id,u16 value,u8 *time);

/*! 
*  \brief    ��¼�ؼ�- ��ӳ����¼
*  \param  screen_id ����ID
*  \param  control_id �ؼ�ID
*  \param  record һ����¼(�ַ���)������ͨ���ֺŸ��������磺��һ��;�ڶ���;������;
*/
void Record_Add(u16 screen_id,u16 control_id,u8 *record);

/*! 
*  \brief     ��¼�ؼ�-�����¼����
*  \param  screen_id ����ID
*  \param  control_id �ؼ�ID
*/
void Record_Clear(u16 screen_id,u16 control_id);

/*! 
*  \brief     ��¼�ؼ�-���ü�¼��ʾƫ��
*  \param  screen_id ����ID
*  \param  control_id �ؼ�ID
*  \param  offset ��ʾƫ�ƣ�������λ��
*/
void Record_SetOffset(u16 screen_id,u16 control_id,u16 offset);

/*! 
*  \brief     ��¼�ؼ�-��ȡ��ǰ��¼��Ŀ
*  \param  screen_id ����ID
*  \param  control_id �ؼ�ID
*/
void Record_GetCount(u16 screen_id,u16 control_id);


/*! 
*  \brief     ��ȡ��ĻRTCʱ��
*/
void ReadRTC(void);

/*! 
*  \brief   ��������
*  \param   buffer ʮ�����Ƶ�����·��������
*/
void PlayMusic(u8 *buffer);

#endif      //_HMI_DRIVER_
