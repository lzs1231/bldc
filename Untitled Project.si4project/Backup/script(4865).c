
/*
������on_init
���ܣ�ϵͳִ�г�ʼ��
*/
void on_init()
{	
	Array[0] = 2;
	Array[1] = 3;
	Array[2] = 3;
	Array[3] = 5;
	Array[4] = 11;
	Array[5] = 15;
	Array[6] = 15;
	Array[7] = 16;
	Array[8] = 16;
	Array[9] = 16;
	Array[10] = 17;
	Array[11] = 17;
	Array[12] = 17;
	Array[13] = 17;
	Array[14] = 18;
	Array[15] = 19;
	Array[16] = 19;
	Array[17] = 19;
	Array[18] = 20;
	Array[19] = 20; 
	Array[20] = 20;
	Array[21] = 21;
	Array[22] = 21;
	Array[23] = 21;
	Array[24] = 23; 
	PassArray[0] = 6;		
	PassArray[1] = 28;		
	PassArray[2] = 28;
	PolarArray[0] = 2;		
	PolarArray[1] = 11;
	SensorArray[0] = 4; 	
	SensorArray[1] = 9; 	
	SensorArray[2] = 23;
	TripArray[0] = 4;	
	TripArray[1] = 7;	
	TripArray[2] = 24;

	hide(2,50);
	hide(2,51);
	hide(2,52);

	hide(4,50);
	hide(4,51);
	hide(4,52);

	hide(5,50);
	hide(5,51);	
	hide(5,52);	

	sys.lang = Language;
	sys.beep_en = Buzzer;
}

/*
������on_systick
���ܣ�����ִ������(1��/��)
*/
void on_systick()
{
	StandByTime++;    //����һ����������¼ʱ��

	if(sys.current_screen == 0)
	{
		TimeS++;
		if(TimeS > 2)
		{
			if(OtherPara.WorkMode == 0)		sys.current_screen = 4;
			else	if(OtherPara.WorkMode == 1)		sys.current_screen = 2;
 			else	if(OtherPara.WorkMode == 2)		sys.current_screen = 5;	
			TimeS =0;
		}
	}	
	if(CurrentPara.CaliStep == 3)	
	{
		TimeS++;
		if(TimeS > 0)
		{
			sys.current_screen = 47;
			TimeS =0;
		}
	}
  	if("Ϣ��ʱ��" == 0)
	{
		if(StandByTime>=30)
		{
			sys.backlight = 0x33;    //һ������ʱ�䵽���ͽ�����Ļ����
		}else{
			if("��Ļ����" == 0){  			     	//����ʱ��δ�����ͻָ���Ļ����   40%
				sys.backlight = 0x66;
			}else	if("��Ļ����" == 1){    		//50%
				sys.backlight = 0x7F;
			}else	if("��Ļ����" == 2){  	    	//60%
				sys.backlight = 0x99;
			}else	if("��Ļ����" == 3){    		//70%
				sys.backlight = 0xB2;
			}else	if("��Ļ����" == 4){    		//80%
				sys.backlight = 0xCC;
			}else	if("��Ļ����" == 5){ 	    	//90%
				sys.backlight = 0xE5;
			}else	if("��Ļ����" == 6){    		//100%
				sys.backlight = 0xFF;
			}		
		}
	}else	if("Ϣ��ʱ��" == 1){
		if(StandByTime>=60)
		{
			sys.backlight = 0x33;    //һ������ʱ�䵽���ͽ�����Ļ����
		}else{
	 		if("��Ļ����" == 0){  			    	 //����ʱ��δ�����ͻָ���Ļ����   40%
				sys.backlight = 0x66;
			}else	if("��Ļ����" == 1){    		//50%
				sys.backlight = 0x7F;
			}else	if("��Ļ����" == 2){  	    	//60%
				sys.backlight = 0x99;
			}else	if("��Ļ����" == 3){    		//70%
				sys.backlight = 0xB2;
			}else	if("��Ļ����" == 4){    		//80%
				sys.backlight = 0xCC;
			}else	if("��Ļ����" == 5){ 	 	   //90%
				sys.backlight = 0xE5;
			}else	if("��Ļ����" == 6){    		//100%
				sys.backlight = 0xFF;
			}
		}
	}else	if("Ϣ��ʱ��" == 2){
		if(StandByTime>=300)
		{
			sys.backlight = 0x33;    //һ������ʱ�䵽���ͽ�����Ļ����
		}else{
	 		if("��Ļ����" == 0){  			     //����ʱ��δ�����ͻָ���Ļ����   40%
				sys.backlight = 0x66;
			}else	if("��Ļ����" == 1){    		//50%
				sys.backlight = 0x7F;
			}else	if("��Ļ����" == 2){  		    //60%
				sys.backlight = 0x99;
			}else	if("��Ļ����" == 3){    		//70%
				sys.backlight = 0xB2;
			}else	if("��Ļ����" == 4){    		//80%
				sys.backlight = 0xCC;
			}else	if("��Ļ����" == 5){ 	  	    //90%
				sys.backlight = 0xE5;
			}else	if("��Ļ����" == 6){    		//100%
				sys.backlight = 0xFF;
			}	
		}
	}else	if("Ϣ��ʱ��" == 3){
		if("��Ļ����" == 0){  			     //����ʱ��δ�����ͻָ���Ļ����   40%
			sys.backlight = 0x66;
		}else	if("��Ļ����" == 1){    		//50%
			sys.backlight = 0x7F;
		}else	if("��Ļ����" == 2){  	 	   //60%
			sys.backlight = 0x99;
		}else	if("��Ļ����" == 3){    		//70%
			sys.backlight = 0xB2;
		}else	if("��Ļ����" == 4){    		//80%
			sys.backlight = 0xCC;
		}else	if("��Ļ����" == 5){ 	 	   //90%
			sys.backlight = 0xE5;
		}else	if("��Ļ����" == 6){    		//100%
			sys.backlight = 0xFF;
		}	
	}		
}

/*
������on_timer
���ܣ���ʱ����ʱ֪ͨ
������timer_id����ʱ��ID
��ز�����
������ʱ����start_timer(timer_id,timeout,countdown,repeat)
            timer_id-��ʱ��ID��0~9��
			timeout-��ʱʱ�䣬���뵥λ
			countdown-0˳��ʱ��1����ʱ������sys.timer������ݼ�
			repeat-�ظ�������0��ʾ����
ֹͣ��ʱ����stop_timer(timer_id)
��ʱ����ֵ��sys.timer0~sys.timer9�����뵥λ
*/
void on_timer(int timer_id)
{	
	if(timer_id == 0)  
	{
		TimeMs=TimeMs+1;
		if(TimeMs==10)
		{
			TimeMs=0;
			stop_timer(0);
			if(sys.current_screen == 34)
			{
				SBackup = 0;
	 			SRestore = 0;
				SFactorySet = 0;
				sys.current_screen = 32;
			}
			else
			{
				sys.current_screen = IntoFlag;
			}
		}
	}
}

/*
������on_variant_upate
���ܣ�����ͨ�ŵ��±�������ʱ��ִ�д˺���
*/
void on_variant_upate()
{
	if(@Language)
	{
		sys.lang = Language; 
	}
	if(@Buzzer)
	{
		sys.beep_en = Buzzer; 
	}	
	if(@SensorValue)
	{
		//ѡ��Ĵ�����ΪEPC1����EPC2
		if( SensorValue.SensorN== 0)					//ȡ���λ
		 {
			set("���������",EPC1);
		 }else	if(SensorValue.SensorN == 1){   		//ȡ���λ	
			set("���������",EPC2); 
		 }   
	}
	if(@DisPulseNum||@Language)
	{
		ManuPolar =DisPulseNum.ManuPolar +Language*2;
		AutoPolar =DisPulseNum.AutoPolar +Language*2;
		MotorType =DisPulseNum.MotorType +Language*2;
	}
	
    if(@SensorRate)
	{
		OccRate=SensorRate.PRate-SensorRate.NRate;
		OccRate1 = OccRate+100;

		if(SensorRate.PRate>SensorRate.NRate)  set(Rate,SensorRate.PRate);
		else								   set(Rate,-SensorRate.NRate);
	}
	if(@SensorMode)
	{
		if(SensorMode == 0)		   {set("������ģʽ",EPC1);}
 		else if(SensorMode == 1)	{set("������ģʽ",EPC2);}
		else if(SensorMode == 2)    {set("������ģʽ",CPC);}
		else if(SensorMode == 3)    {set("������ģʽ",SPC);}	
	}
	if(@LimitFun1||@Language)
	{
		LimitMode = LimitFun1.LimitMode+Language*3;
	}
	 if(@SPCFun1||@Language)
	{
		SPCMode = SPCFun1.SPCMode+Language*2;
	}	
	if(@NoMatFun1||@Language)
	{
		NoWaitEN = NoMatFun1.NoWaitEN+Language*2;
	}	
 	if(@FineTune||@Language)
	{
		WarmFlag    =    FineTune.WarmFlag       +Language*11;
		PowerOnMode = 	 FineTune.PowerOnMode	 +Language*4;
		SensorTips	=	 FineTune.SensorTips     +Language*2;
	}	
 	if(@LongIoMode||@Language)
	{
		LongIo0Mode =    LongIoMode.LongIo0Mode      +Language*11;
		LongIo1Mode =    LongIoMode.LongIo1Mode      +Language*11;
 		LongIo2Mode =    LongIoMode.LongIo2Mode      +Language*11;
		LongIo3Mode =    LongIoMode.LongIo3Mode      +Language*11;	
	}	
	if(@AdjustFlag||@Language)
	{
		"����ָʾ" = AdjustFlag +Language*25;
	}
	if(@CurrentPara||@Language)
	{
		if(CurrentPara.CaliCurrent==0)				    "У׼����"=1;
		else	if(CurrentPara.CaliCurrent==1)		  "У׼����"=15/10;
 		else	if(CurrentPara.CaliCurrent==2)		  "У׼����"=2;
		else	if(CurrentPara.CaliCurrent==3)		  "У׼����"=3;	

		"У׼����" =       CurrentPara.CaliStep      +Language*4;
	}	
	if(@FineTune.WarmFlag||@Buzzer)
	{
		if(FineTune.WarmFlag>2&&Buzzer==1)  "����������" = 1;
		else								"����������" = 0;
	}
}

/*
������on_control_notify
���ܣ��ؼ�ֵ����֪ͨ
������screen_id������ID
������control_id���ؼ�ID
������value����ֵ
*/
void on_control_notify(int screen_id,int control_id,int value)
{
	StandByTime = 0;      //ֻҪ�а������£�Ϣ����������

	if(screen_id ==1)
	{
		if(control_id == 2 && value == 1)
		{
			sys.current_screen =BackFlag;
		}
	}else if(screen_id ==2){
 		BackFlag = 2;
		if(control_id == 1 && value == 1) 
		{
			sys.current_screen =1;
		}
		if(control_id == 4)
		{
			if(value == 1)   		   set(HWarmFlag,3);	
			else if(value == 0)      set(HWarmFlag,0);	
		}else if(control_id == 5	&&	value == 1){
			PolarFlag = 0;		
		}else if(control_id == 7	&&	value == 1 ){
 			AdjustFlag = 0;	
			set(HAdjustFlag ,0);
		}else if(control_id == 8) {
			IntoFlag = 6;
			if(value == 1)			     {start_timer(0,100,0,0);}
 			else if(value == 0)		   {stop_timer(0);	TimeMs=0;}
		}
	}	
 	else if(screen_id == 3)
	{
		if(control_id == 6 	&&	value == 1)
		{
			AdjustFlag = 1;		
			set(HAdjustFlag ,1);
		}else if(control_id == 7 &&	value == 1){
			AdjustFlag = 2;		
			set(HAdjustFlag ,2);
		}	
	}	
	else if(screen_id == 4)
	{
		BackFlag = 4;
		if(control_id == 1 && value == 1) 
		{
			sys.current_screen =1;
		}else if(control_id == 8){
			IntoFlag = 6;
			if(value == 1)			  {start_timer(0,100,0,0);}
			else if(value == 0)		  {stop_timer(0);TimeMs=0;}
		}else if(control_id == 9){
			IntoFlag = 40;
			if(value == 1)			{start_timer(0,100,0,0);	SensorFlag = 0;	 }
 			else if(value == 0)		{stop_timer(0);TimeMs=0;	}
		}else if(control_id == 10){
			IntoFlag = 47;
	
			if(value == 1)			{start_timer(0,100,0,0);	TripFlag = 0;}
 			else if(value == 0)		{stop_timer(0);TimeMs=0;	}
		}	
	}
	else if(screen_id == 5)
	{
		BackFlag = 5;	
 		if(control_id == 1 && value == 1) 
		{
			sys.current_screen =1;
		}else if(control_id == 8){
			IntoFlag = 6;
			if(value == 1)			{start_timer(0,100,0,0);}
 			else if(value == 0)		 {stop_timer(0);TimeMs=0;}
		}else if(control_id == 24	&&	value == 1 ){
 			AdjustFlag = 3;		
			set(HAdjustFlag ,3);
		}
	}
	else if(screen_id == 6)
	{
		if(control_id == 9 && value == 1)
		{
			sys.current_screen = BackFlag;
		}else if(control_id == 2	&&	value == 1 ){
 			set("���������ʾ",0+Language*4);	
			set("����������ʾ",0+Language*3);
 			MPassWord=0;	
 			PassFlag=0;
		}else if(control_id == 4	 ){
			if(value == 1)   		 set(HWarmFlag,1);
			else if(value == 0)  	 set(HWarmFlag,0);
		}	
	}
 	else if(screen_id == 7)
	{
		if(control_id == 3 	&&	value == 1)
		{
 			TripFlag=1;	
		}
	}	
	else if(screen_id == 9)
	{
		if(control_id == 3 	&&	value == 1)
		{
			SensorFlag=1;
		}
	}
 	else if(screen_id == 11)
	{
		if(control_id == 5 	&&	value == 1)
		{
			AdjustFlag =4;		
			set(HAdjustFlag ,4);
		}else  if(control_id == 4 	&&	value == 1) {
 			PolarFlag=1;
		}		
	}	
 	else if(screen_id == 15)
	{
		if(control_id == 6	&&	value == 1)
		{
 			AdjustFlag = 5;		
			set(HAdjustFlag ,5);
		}else	if(control_id == 7	&&	value == 1 ){
 			AdjustFlag = 6;		
			set(HAdjustFlag ,6);
		}		
	}	
 	else if(screen_id == 16)
	{
		if(control_id == 11 &&	value == 1)
		{
 			AdjustFlag = 7;		
			set(HAdjustFlag ,7);
		}else if(control_id == 7 &&	value == 1){
			AdjustFlag = 8;		
			set(HAdjustFlag ,8);
		}else if(control_id == 8 &&	value == 1 ){
 			AdjustFlag = 9;		
			set(HAdjustFlag ,9);
		}		
	}	
  	else if(screen_id == 17)
	{
		if(control_id == 8 	&&	value == 1)
		{
 			AdjustFlag = 10;		
			set(HAdjustFlag ,10);
		}else if(control_id == 5	&&	value == 1){
			AdjustFlag = 11;		
			set(HAdjustFlag ,11);
		}else if(control_id == 19 	&&	value == 1){
 			AdjustFlag = 12;		
			set(HAdjustFlag ,12);
		}	else if(control_id == 26	&&	value == 1){
 			AdjustFlag = 13;		
		    set(HAdjustFlag ,13);
		}			
	}		
	else if(screen_id == 18)
	{
		if(control_id == 4	&&	value == 1)
		{
			AdjustFlag = 14;		
			set(HAdjustFlag ,14);
		}	
	}	
	else if(screen_id == 19)
	{
		if(control_id == 3	&&	value == 1 )
		{
			AdjustFlag = 15;		
			set(HAdjustFlag ,15);
		}
 		else	if(control_id == 4	&&	value == 1 )
		{
			AdjustFlag = 16;		
			set(HAdjustFlag ,16);
		}	
 		else	if(control_id == 8	&&	value == 1 )
		{
			AdjustFlag = 17;		
			set(HAdjustFlag ,17);
		}			
	}	
	else if(screen_id == 20)
	{
		if(control_id == 7	&&	value == 1 )
		{
 			AdjustFlag = 18;		
			set(HAdjustFlag ,18);
		}
 		else	if(control_id == 8 	&&	value == 1)
		{
			AdjustFlag = 19;		
			set(HAdjustFlag ,19);
		}	
 		else	if(control_id == 9 	&&	value == 1)
		{
 			AdjustFlag = 20;		
			set(HAdjustFlag ,20);
		}		
	}	
	else if(screen_id == 21)
	{
		if(control_id == 6	&&	value == 1)
		{
			AdjustFlag = 21;		
			set(HAdjustFlag ,21);
		}else if(control_id == 7 	&&	value == 1){
 			AdjustFlag = 22;		
			set(HAdjustFlag ,22);
		}else if(control_id == 8 	&&	value == 1){
			AdjustFlag = 23;		
			set(HAdjustFlag ,23);
		}		
	}	
	else if(screen_id == 23)
	{
		if(control_id == 3 	&&	value == 1)
		{
			SensorFlag = 2;		
		}else if(control_id == 13	&&	value == 1 ){
 			AdjustFlag = 24;		
			set(HAdjustFlag ,24);
		}	
	}		
	else if(screen_id == 24)
	{
		if(control_id == 3 	&&	value == 1)
		{
 			TripFlag = 2;		
		}
	}
/////////////////////////////////////�����������//////////////////////////////////
	else if(screen_id == 28)
	{
		if(control_id == 4	&&	value == 1 )
		{
			set("���������ʾ",0+Language*4);	
		 	set("����������ʾ",1+Language*3);	
 			MPassWord=0;		
			PassFlag = 1;		
		}else if(control_id == 6 	&&	value == 1){
 			set("���������ʾ",0+Language*4);	
 			set("����������ʾ",2+Language*3);	
 			MPassWord=0;		
			PassFlag = 2;		
		}	
	}	
	else if(screen_id == 36)
	{
		if(control_id == 16 && value == 1 )
		{
			sys.current_screen = PassArray[PassFlag];       //���ص�����������Ľ���
		}else if(control_id == 14 && value == 1){
			if(PassFlag == 0)	
			{
				if(MPassWord==PassWord || MPassWord==SysPassWord)	
				{
					sys.current_screen = 12;    			//�ж������Ƿ���ȷ
				}else{
					set("���������ʾ",1+Language*4);
				}
			}else  if(PassFlag	==	1){
				set(tMPassWord,MPassWord);
				sys.current_screen = 28;
			}else  if(PassFlag	==	2){
				if(tMPassWord == MPassWord)
				{
					set(PassWord,MPassWord);
					sys.current_screen = 28;
				}else{
					set("���������ʾ",3+Language*4);
				}
			}
		}		
	}	
//////////////////////////////////////////���Ը��Ľ���///////////////////////////////////////////	
 	else if(screen_id == 37)
	{
		if(control_id ==	2  && value == 1)
		{
		//	if(DisPulseNum.AutoPolar == 1)	 set(HAutoPolar,0);     //����Ϊ1���������л�Ϊ0������
		//	else						     set(HAutoPolar,1);
		}
		else	if(control_id == 3  && value == 1)
		{
			sys.current_screen = PolarArray[PolarFlag];
		}	
	}		
//////////////////////////////////////////����ȷ�ϸ��Ľ���///////////////////////////////////////////
	else if(screen_id == 39)
	{
		if(control_id == 3 && value == 1)
		{
			sys.current_screen = Array[HAdjustFlag];
		}else if(control_id == 4 && value == 1){
			sys.current_screen = Array[HAdjustFlag];
		}
	}		
/////////////////////////////////////////////������У׼ѡ��/////////////////////////////////////////////////
	else if(screen_id == 40)
	{
		if(control_id == 21 && value == 1)
		{
			sys.current_screen = SensorArray[SensorFlag];     //���ص��ô�����У׼�Ľ���
		}	
	}	
	else if(screen_id == 42)
	{
		if(control_id == 2 && value == 1)
		{
			if(OtherPara.WorkMode == 0)  sys.current_screen = 44;
			else                         sys.current_screen = 43;
		}	
		else	if(control_id == 1 && value == 1)
		{
			set(HSensorNum,0);
		}	
 		else	if(control_id == 3  &&  value == 1)
		{
			set(HSensorNum,1);
		}		
	}
/////////////////////////////////////////////////////�г�У׼/////////////////////////////////////////////////
	else if(screen_id == 47)
	{
		if(control_id == 9 && value == 1)
		{
			sys.current_screen = TripArray[TripFlag];         //���ص����г�У׼�Ľ���
		}	
		else	if(control_id == 1 && value == 1)
		{
 			if(OtherPara.WorkMode == 0)  {sys.current_screen = 49;  set(HTCaliFlag,1);}	
			else                          sys.current_screen = 48;	
		}	
	}		
 	else if(screen_id == 48)
	{
		if(control_id == 3 && value == 1)
		{
			set(HWorkMode,0);	
			set(HTCaliFlag,1);
		}	
	}		
	else if(screen_id == 49)
	{
		if(control_id == 2 && value == 1)
		{
 			set(HTCaliCancel,1);	
		}	
	}	
	//////////////////////////////////////////////////////////////////////////////////////////////////
	if(SBackup == 1)			{DisSys = 0+Language*3;}
	else if(SRestore == 1)		{DisSys = 1+Language*3;}
	else if(SFactorySet == 1)	{DisSys = 2+Language*3;}
	
}

/*
������on_screen_change
���ܣ������л�֪ͨ����ǰ����ID�����仯ʱִ�д˺���
������screen_id����ǰ����ID
*/
void on_screen_change(int screen_id)
{
	if(screen_id == 0)
	{
		TimeS = 0;
	}
	else	if(screen_id == 2 || screen_id == 11)
	{
		HWorkMode=1;
	}
	else	if(screen_id == 4 || screen_id == 8|| screen_id == 10)
	{
		HWorkMode=0;
	}	
 	else	if(screen_id == 5)
	{
		HWorkMode=2;
	}		

	else if(screen_id == 34)
	{
		start_timer(0,100,0,0);  //�򿪶�ʱ��0
	}	

 /////////////////////////////////////////////////////������У׼3���ж�/////////////////////////////////////////////////
	else if(screen_id == 44)
	{
		set(HWorkMode,0);	 //�ǰ�ť���£��л�Ϊ�ֶ�������¼У׼����Ϊ��0����
		set(HSCaliStep,0);   //��һ����ť���£�����¼У׼����Ϊ��1����
	}
	else if(screen_id == 45)
	{
		set(HSCaliStep,1);   //��һ����ť���£�����¼У׼����Ϊ��2����
	}	
	else if(screen_id == 46)
	{
		set(HSCaliStep,2);   //��һ����ť���£�����¼У׼����Ϊ��2����
	}	

}

