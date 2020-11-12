
/*! 
*  \brief  更新数据
*/ 
void UpdateUI()
{
	static u8 i=0;	
    //进到自动界面，画面ID=0
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
				SetProgressValue(0,23,Sensor1_Offset);                     //设置进度条的值
			break;          //保存传感器1（左边）最小值
			case 2:    
				SetProgressValue(0,23,Sensor2_Offset);     
			break;          //保存传感器2（右边）最小值
			default :                                break;
		}
		SetProgressValue(0,24,g_Current_pulse_num/g_MAX_pulse_num);                    //设置进度条的值
    }
	
    //进到手动界面，画面ID=1
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
				SetProgressValue(1,23,Sensor1_Offset);                     //设置进度条的值
			break;          //保存传感器1（左边）最小值
			case 2:    
				SetProgressValue(1,23,Sensor2_Offset);     
			break;          //保存传感器2（右边）最小值
			default :                                break;
		}
		SetProgressValue(1,24,g_Current_pulse_num/g_MAX_pulse_num);                    //设置进度条的值
    }
	
    //进到中心界面，画面ID=2
    else if(current_screen_id==2)                                   
    {
      
    }
	
    //进到设置界面第一页，画面ID=3
    else if(current_screen_id==3)                                   
    {
	                  //设置进度条的值
    }

    //进到限位设置手动限位
    else if(current_screen_id == 10)
    {
	
    }
    //进到增益偏移量设置
    else if(current_screen_id == 11)   
    {
		
    }
	//校准传感器选择
	else if(current_screen_id == 12)   
    {

    }
	//传感器校准未遮挡实时更新
	else if(current_screen_id == 13)   
    {
//		GetScreen();
		switch(Sensor_calibration)
		{
			case 1:    
				g_Sensor_Reference_L_H = Sensor1_value;       //保存传感器1（左边）最大值
				SetTextValue(13,3,"EPC1");                    //更新到进度条和文本框
			break;          
			case 2:    
				g_Sensor_Reference_R_H = Sensor2_value;       //保存传感器2（右边）最大值
			    SetTextValue(14,3,"EPC2");                    //更新到进度条和文本框
			break;          
			default :                                break;
		}
		SetTextInt32(13,1,Sensor1_value,0,1);        //更新到进度条和文本框
    }
	//传感器校准遮挡实时更新
	else if(current_screen_id == 14)   
    {
//		GetScreen();
		switch(Sensor_calibration)
		{
			case 1:    
				g_Sensor_Reference_L_L = Sensor1_value;       //保存传感器1（左边）最小值
				SetTextValue(14,3,"EPC1");                    //更新到进度条和文本框
			break;         
			case 2:    
				g_Sensor_Reference_R_L = Sensor2_value;       //保存传感器2（右边）最小值
			SetTextValue(14,3,"EPC2");                        //更新到进度条和文本框
			break;        
			default :                                break;
		}
		SetTextInt32(14,1,Sensor1_value,0,1);        //更新到进度条和文本框
    }
	//进到光电传感器校准，移动材料，查看偏移量变化        实时读取传感器采样值
	else if(current_screen_id == 15)   
    {
//		GetScreen();
		switch(Sensor_calibration)
		{
			case 1:      
				SetTextInt32(15,4,Sensor1_Offset,1,1);        //更新到进度条和文本框
				SetProgressValue(15,2,Sensor1_Offset);                     //设置进度条的值
			break;          //保存传感器1（左边）最小值
			case 2:          
				SetTextInt32(15,4,Sensor2_Offset,1,1);        //更新到进度条和文本框
				SetProgressValue(15,2,Sensor2_Offset);     
			break;          //保存传感器2（右边）最小值
			default :                                break;
		}
    }
	//进到屏幕亮度界面
    else if(current_screen_id == 28)
    {
		SetTextInt32(28,3,Screen_brightness,0,1);        //更新到进度条和文本框
    }
	//进到自动界面增益调节
//    else if(current_screen_id == 39)
//    {
//		SetTextInt32(39,5,g_Gain_Data,0,1);        //更新到进度条和文本框
//    }
	//进到自动试运行界面增益调节
    else if(current_screen_id == 41)
    {
		SetTextInt32(41,5,g_Gain_Data,0,1);        //更新到进度条和文本框
    }
	
	

}