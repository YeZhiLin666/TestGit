#include "includes.h"

/*
//public
UInt16          TEC_Ctrl::Priority = PRIORITY4;
UInt16          TEC_Ctrl::TaskID   = TEC_CTRL_TASK;
UInt16          TEC_Ctrl::TEC_status;
TEC_Pid_Info	TEC_Ctrl::Pid_Info;
float           TEC_Ctrl::temperature;
MsTime last_time = OS_ServeProcess::OS_Time;
MsTime          TEC_Ctrl::m_Interval_LogTemp;    // 记录温度时间间隔器


void TEC_Ctrl::Init(void)
{
	Init_PWM();
	TEC_status = Read_Temp ;
    if (ChannelInfo_Manager::m_st_BoadType.m_u16BoardType == BOARDTYPE_HPT200)  // 05.03.2018  ZC
        Pid_Info.SetTemp = 38;   // 将HPT200的温度统一为38度。
    else if (ChannelInfo_Manager::m_st_BoadType.m_u16BoardType == BOARDTYPE_HPM4)
	Pid_Info.SetTemp = 30;


	Pid_Info.Last_Error = 0;
	Pid_Info.Last_Output = 0;

	CommonFunc::GetTimeTicksUpdate( &m_Interval_LogTemp); // 更新时间间隔器
}

void TEC_Ctrl::DoWork(void)
{
	static float pwm_output;
	//	float const Ki = 0.00001;
	float const Ki = 0.000035;
	//	static float TempReading[1024];
	//static int i = 0;
	switch ( TEC_status)
	{
		//        MsTime lasttime;
		float fKi;
	case Read_Temp:                                       //Read Temperature
		{
// 			float Temp0 = TMP75C::ReadTempf(0);
// 			float Temp1 = TMP75C::ReadTempf(1); // Temperature upstream, Ron 4-24-2015
// 			float Temp2 = TMP75C::ReadTempf(2);
// 			Pid_Info.SampleTemp0 = Temp0;
// 			Pid_Info.SampleTemp1 = Temp1;
// 			Pid_Info.SampleTemp2 = Temp2;
// 			temperature = Temp0;

            Pid_Info.SampleTemp0 = TMP75C::ReadTempf(0);
			Pid_Info.SampleTemp1 = TMP75C::ReadTempf(1); // Temperature upstream, Ron 4-24-2015
			Pid_Info.SampleTemp2 = TMP75C::ReadTempf(2);
            temperature = Pid_Info.SampleTemp0;

			TEC_status = Cal_PWM;

			// 间隔30秒上传一个温度值，由上位机记录入文本文件     yy  2015/04/09
			if(CommonFunc::GetTimeTicks( &m_Interval_LogTemp) >= LOG_TEMP_INTERVAL_TIME)    //超过30s
			{
				TEC_Ctrl::BuildInAux_TriggerLog_Net(AUX_TEMPERATURE, 0X0003);
				CommonFunc::GetTimeTicksUpdate( &m_Interval_LogTemp); // 更新时间间隔器
			}

			//added Aug 14 2014
			if(OS_ServeProcess::TimeDiff(last_time).Second >=5 )
			{
				//           TempReading[i] = Temp0;
// 				i++;
// 				if ( i > 1024 ) { i = 0 ;}
				last_time = OS_ServeProcess::OS_Time;
			};
			break;
		}
	case Cal_PWM:                                         //Calculate PWM based on PID function
		// 		  if(pwm_output>0)
		fKi = Ki;
		//       if(pwm_output<0)
		// 				fKi = 2*Ki;

		pwm_output = Pid_Output_Calculation( 0.2, fKi, 0.0, &Pid_Info);
		TEC_status = Output_PWM;
		break;

	case Output_PWM:                                      //Output PWM
		ControlPin::SetMeHigh(HPT200_Base::PinGroup[HPT200_Pin_TC_ENB]);
		//  LPC4078PWM::PWM_MatchUpdate(1, 6, pwm_output*3e4+3e4, 0);
		if(pwm_output >= -0.1)
		{
			ControlPin::SetMeHigh(HPT200_Base::PinGroup[HPT200_Pin_PWM_DIR]);
			if(pwm_output <= 0)
				pwm_output = 0;
			LPC4078PWM::PWM_MatchUpdate(1, 6, 0.35f* pwm_output * 6e4f, 0);
		}
		else
		{
			ControlPin::SetMeLow(HPT200_Base::PinGroup[HPT200_Pin_PWM_DIR]);
			LPC4078PWM::PWM_MatchUpdate(1, 6, -0.9f * (pwm_output + 0.1f) * 6e4f, 0);
		}

		TEC_status = Read_Temp;
		break;
	}

}

float TEC_Ctrl::Pid_Output_Calculation( float fKp,  float fKi, float fKd, TEC_Pid_Info * pPidInfo)
{
	float Output;
	float New_Error = pPidInfo->SetTemp - pPidInfo->SampleTemp0;
	// if (New_Error<0)
	//      fKi = 2* fKi;

	float Delta_Output = fKp * ( New_Error - pPidInfo->Last_Error)   +   fKi * New_Error ; // fKd sensitive -> 0
	if(Delta_Output > 0.1f)
		Delta_Output = 0.1f;
	if(Delta_Output < -0.1f)
		Delta_Output = -0.1f;
	Output = pPidInfo->Last_Output + Delta_Output;

	if( Output>0.99f)  {Output = 0.99f;}
	if(Output<-0.99f)  {Output = -0.99f;}

	//if(New_Error < 0.2 && New_Error > -0.2) Output = 0;	//test
	pPidInfo->Last_Output = Output;
	pPidInfo->Last_Error = New_Error;
	return Output;
}

void TEC_Ctrl::Init_PWM(void)
{
	//const St_Port pwm_output=	{1,	26, eCPTYPE_MCUGPIO, PWM1,	0	}; Ron 7-24-15
	//const St_Port pwm_output=	{2,	5, eCPTYPE_MCUGPIO, PWM1,	0	}; Ron 7-24-15
	//ControlPin::SetIObyCP(pwm_output);	Ron 7-24-15

	LPC4078PWM::CLKPWR_ConfigPPWR (1, ENABLE);
	LPC_PWM_TypeDef* pPwm = LPC4078PWM::PWM_GetPointer(1);                    // 03.28.2017 for HPM4：fix small bug

	pPwm->IR = 0xFF & ((uint32_t)(0x0000073F));
	pPwm->TCR = 0x00;
	pPwm->CTCR = 0x00;
	pPwm->MCR = 0x00;
	pPwm->CCR = 0x00;
	pPwm->PCR = 0x00;
	pPwm->LER = 0x00;

	LPC4078PWM::PWM_ChannelCmd(1, 6, ENABLE);
	LPC4078PWM::PWM_ChannelConfig(1,6,PWM_CHANNEL_SINGLE_EDGE);

	LPC4078PWM::PWM_MatchUpdate(1,0,6e4, 0);
	LPC4078PWM::PWM_MatchUpdate(1,6,3e4, 0);    //intialization, 50% duty cycle
	LPC4078PWM::PWM_ResetCounter(1);
	LPC4078PWM::PWM_CounterCmd(1, ENABLE);
	LPC4078PWM::PWM_Cmd(1, ENABLE);
}

void TEC_Ctrl::BuildInAux_TriggerLog_Net(unsigned char uc_AuxType, unsigned short uw16_AuxChnbit)
{
	//St_BUILDINAUX_VALUE * st_Point[8];  // 暂时用不到8，仅用1
	//st_Point[0] =(St_BUILDINAUX_VALUE *) &EthernetParse::m_Point_Temp[0];

	St_BUILDINAUX_VALUE * st_Point;
    memset(&EthernetParse::m_Point_Temp,0,sizeof(EthernetParse::m_Point_Temp));
	st_Point =(St_BUILDINAUX_VALUE *) &EthernetParse::m_Point_Temp[0];

	st_Point->m_ucLogType = SDL_DATA_FOR_LOG;

	unsigned char uc_ChCount = 0;
	unsigned short uw16_AuxChnbit_back = uw16_AuxChnbit;
	for(unsigned char uc_i=0; uc_i < 16; uc_i ++)
	{
		if(((uw16_AuxChnbit_back>>uc_i)&0X0001) == 0X0001)
			uc_ChCount++;     // 计算总通道数
	}
	st_Point->m_u16ChannelCount = uc_ChCount;
	st_Point->m_btChannelType =  uc_AuxType;  // AUX_TEMPERATURE
	if( uc_AuxType == AUX_TEMPERATURE)
	{
		st_Point->m_ucSensor = TC_T;   // 温度才有传感器K,J,T
	}
	else
	{
		st_Point->m_ucSensor = uc_AuxType;  // 否则填同通道类型
	}

	//     for(unsigned char uc_i=0; uc_i < 2; uc_i ++)
	//         st_Point->m_ucChannelIdx[uc_i] = uc_AuxType;

// 	for(unsigned char uc_i=0; uc_i < MAXCHANNELNO; uc_i ++)
// 		st_Point->m_fValue[uc_i] = 0;              // 其它通道无值

	st_Point->m_fValue[0] = Pid_Info.SampleTemp0;   // 仅有一个PID温度通道有值
	st_Point->m_fValue[1] = Pid_Info.SampleTemp1;   //Ron 4-24-2015
	st_Point->m_fValue[2] = Pid_Info.SampleTemp2;   //Ron 4-24-2015
	st_Point->m_ucChannelIdx[0] = uw16_AuxChnbit;
	st_Point->m_ucChannelIdx[1] = uw16_AuxChnbit>>8;     //  04/22/2015  yy//
	//    st_Point->m_ucChannelIdx[2] = uw16_AuxChnbit>>16;  //Ron 4-24-2015
	//    st_Point->m_fValue[0] =  55.8;   // Test to Display
	//    st_Point->m_fValue[2] =  55.9;
	//		st_Point->m_fValue[1] = st_Point->m_fValue[2];
	EthernetParse::MPReportData(0,SEND_IV_BUILDINAUX_LOGDATA);  // PC 以太网络发送
}
*/

