
/*! 
*  \brief  ��������
*/ 
void UpdateUI()
{
	static u8 i=0;	
    //�����Զ����棬����ID=0
    if(current_screen_id == 0)
    {
		switch(WorkMode)
		{
			case EPC1:   SetTextValue(0,9,"EPC1");  break;
			case EPC2:   SetTextValue(0,9,"EPC2");  break;
			case CPC:    SetTextValue(0,9,"CPC");   break;
			case SPC:    SetTextValue(0,9,"SPC");   break;
			default:     break;
		}
		switch(g_Auto_Polar)
		{
			case 1:		SetTextValue(0,25,"-");  break;
			case 2:		SetTextValue(0,25,"+");  break;
			default:     break;
		}
		SetTextInt32(0,14,g_Gain_Data,0,1); 
		SetTextInt32(0,18,g_Fine_Tuning,0,1); 
		
		SetTextValue(0,16,"Motor"); 
		switch(Sensor_calibration)
		{
			case 1:    
				SetProgressValue(0,23,Sensor1_Offset);                     //���ý�������ֵ
			break;          //���洫����1����ߣ���Сֵ
			case 2:    
				SetProgressValue(0,23,Sensor2_Offset);     
			break;          //���洫����2���ұߣ���Сֵ
			default :                                break;
		}
		SetProgressValue(0,24,g_Current_pulse_num/g_MAX_pulse_num);                    //���ý�������ֵ
    }
	
    //�����ֶ����棬����ID=1
    else if(current_screen_id == 1)
    {
		switch(WorkMode)
		{
			case EPC1:   SetTextValue(1,9,"EPC1");  break;
			case EPC2:   SetTextValue(1,9,"EPC2");  break;
			case CPC:    SetTextValue(1,9,"CPC");   break;
			case SPC:    SetTextValue(1,9,"SPC");   break;
			default:     break;
		}
		switch(g_Auto_Polar)
		{
			case 1:		SetTextValue(1,25,"-");     break;
			case 2:		SetTextValue(1,25,"+");     break;
			default:     break;
		}
		SetTextInt32(1,14,g_Gain_Data,0,1); 
		SetTextInt32(1,18,g_Fine_Tuning,0,1); 
		
		SetTextValue(1,8,"Motor"); 
		switch(Sensor_calibration)
		{
			case 1:    
				SetProgressValue(1,23,Sensor1_Offset);                     //���ý�������ֵ
			break;          //���洫����1����ߣ���Сֵ
			case 2:    
				SetProgressValue(1,23,Sensor2_Offset);     
			break;          //���洫����2���ұߣ���Сֵ
			default :                                break;
		}
		SetProgressValue(1,24,g_Current_pulse_num/g_MAX_pulse_num);                    //���ý�������ֵ
    }
	
    //�������Ľ��棬����ID=2
    else if(current_screen_id==2)                                   
    {
      
    }
	
    //�������ý����һҳ������ID=3
    else if(current_screen_id==3)                                   
    {
	                  //���ý�������ֵ
    }

    //������λ�����ֶ���λ
    else if(current_screen_id == 10)
    {
	
    }
    //��������ƫ��������
    else if(current_screen_id == 11)   
    {
		
    }
	//У׼������ѡ��
	else if(current_screen_id == 12)   
    {

    }
	//������У׼δ�ڵ�ʵʱ����
	else if(current_screen_id == 13)   
    {
//		GetScreen();
		switch(Sensor_calibration)
		{
			case 1:    
				g_Sensor_Reference_L_H = Sensor1_value;       //���洫����1����ߣ����ֵ
				SetTextValue(13,3,"EPC1");                    //���µ����������ı���
			break;          
			case 2:    
				g_Sensor_Reference_R_H = Sensor2_value;       //���洫����2���ұߣ����ֵ
			    SetTextValue(14,3,"EPC2");                    //���µ����������ı���
			break;          
			default :                                break;
		}
		SetTextInt32(13,1,Sensor1_value,0,1);        //���µ����������ı���
    }
	//������У׼�ڵ�ʵʱ����
	else if(current_screen_id == 14)   
    {
//		GetScreen();
		switch(Sensor_calibration)
		{
			case 1:    
				g_Sensor_Reference_L_L = Sensor1_value;       //���洫����1����ߣ���Сֵ
				SetTextValue(14,3,"EPC1");                    //���µ����������ı���
			break;         
			case 2:    
				g_Sensor_Reference_R_L = Sensor2_value;       //���洫����2���ұߣ���Сֵ
			SetTextValue(14,3,"EPC2");                        //���µ����������ı���
			break;        
			default :                                break;
		}
		SetTextInt32(14,1,Sensor1_value,0,1);        //���µ����������ı���
    }
	//������紫����У׼���ƶ����ϣ��鿴ƫ�����仯        ʵʱ��ȡ����������ֵ
	else if(current_screen_id == 15)   
    {
//		GetScreen();
		switch(Sensor_calibration)
		{
			case 1:      
				SetTextInt32(15,4,Sensor1_Offset,1,1);        //���µ����������ı���
				SetProgressValue(15,2,Sensor1_Offset);                     //���ý�������ֵ
			break;          //���洫����1����ߣ���Сֵ
			case 2:          
				SetTextInt32(15,4,Sensor2_Offset,1,1);        //���µ����������ı���
				SetProgressValue(15,2,Sensor2_Offset);     
			break;          //���洫����2���ұߣ���Сֵ
			default :                                break;
		}
    }
	//������Ļ���Ƚ���
    else if(current_screen_id == 28)
    {
		SetTextInt32(28,3,Screen_brightness,0,1);        //���µ����������ı���
    }
	//�����Զ������������
//    else if(current_screen_id == 39)
//    {
//		SetTextInt32(39,5,g_Gain_Data,0,1);        //���µ����������ı���
//    }
	//�����Զ������н����������
    else if(current_screen_id == 41)
    {
		SetTextInt32(41,5,g_Gain_Data,0,1);        //���µ����������ı���
    }
	
	

}