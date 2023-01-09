
#include "../Entry/includes.h"

const St_Port Aux_PowerADJ_2US6K_422108_Base::Pin_ERR1 = {1,        24,         eCPTYPE_MCUGPIO,   GPIO_I,             1};
const St_Port Aux_PowerADJ_2US6K_422108_Base::Pin_ERR2 = {1,        26,         eCPTYPE_MCUGPIO,   GPIO_I,             1};	
const St_Port Aux_PowerADJ_2US6K_422108_Base::Pin_PFCOK = {1,        13,        eCPTYPE_MCUGPIO,   GPIO_I,             1};
const St_Port Aux_PowerADJ_2US6K_422108_Base::Pin_FANALM = {1,        27,        eCPTYPE_MCUGPIO,   GPIO_I,             1};

const St_Port Aux_PowerADJ_2US6K_422108_Base::Pin_RST =  {1,        12,         eCPTYPE_MCUGPIO,   GPIO_O,             1};
const St_Port Aux_PowerADJ_2US6K_422108_Base::Pin_OFF =  {1,        22,        eCPTYPE_MCUGPIO,   GPIO_O,             1};
const St_Port Aux_PowerADJ_2US6K_422108_Base::Pin_FANPWM =  {3,        18,        eCPTYPE_MCUGPIO,		PWM0,					0};


const St_Port Aux_PowerADJ_2US6K_422108_Base::Pin_ERRP = {1,        5,       eCPTYPE_MCUGPIO,   GPIO_O,             0};
const St_Port Aux_PowerADJ_2US6K_422108_Base::Pin_RunLed = {1,        7,          eCPTYPE_MCUGPIO,   GPIO_O,             0};

const St_Port Aux_PowerADJ_2US6K_422108_Base::Pin_Mode0 = {3,        20,         eCPTYPE_MCUGPIO,   GPIO_I,             1};
const St_Port Aux_PowerADJ_2US6K_422108_Base::Pin_Mode1 = {3,        21,         eCPTYPE_MCUGPIO,   GPIO_I,             1};	
const St_Port Aux_PowerADJ_2US6K_422108_Base::Pin_Mode2 = {3,        22,        eCPTYPE_MCUGPIO,   GPIO_I,             1};
const St_Port Aux_PowerADJ_2US6K_422108_Base::Pin_Mode3 = {3,        23,        eCPTYPE_MCUGPIO,   GPIO_I,             1};

MsTime Aux_PowerADJ_2US6K_422108_Base::EngineDelayTime;
unsigned char Aux_PowerADJ_2US6K_422108_Base::PowerStatus = POWER_IDLE;
unsigned char Aux_PowerADJ_2US6K_422108_Base::PowerRunMode_Vpp = 0;
unsigned char Aux_PowerADJ_2US6K_422108_Base::PowerRunMode_Vmm = 0;
POWER_2US6K_IV_TypeDef Aux_PowerADJ_2US6K_422108_Base::Power_2US6K_IV;
unsigned char Aux_PowerADJ_2US6K_422108_Base::Power_ErrCnt = 0;
unsigned char Aux_PowerADJ_2US6K_422108_Base::Power_DacRampStatus = 0;
unsigned char Aux_PowerADJ_2US6K_422108_Base::ErrPinRiseNum = 0;
unsigned char Aux_PowerADJ_2US6K_422108_Base::ErrPinFallNum = 0;
unsigned char Aux_PowerADJ_2US6K_422108_Base::PowerConfigMode = VppSingleMode;
MsTime StartFanSafeTime;
MsTime StartFanUnsafeTime;
MsTime FanPassTime;
MsTime FanAdjustPeriodTime;
unsigned char FanCheckState;
unsigned char FanUnsafeConunter;
unsigned char FanElimination;
unsigned char FanJudgeState = FanSafe;
void Aux_PowerADJ_2US6K_422108_Base::IO_Init(void)
{
	ControlPin::SetIObyCP(Pin_ERR1);
	ControlPin::SetIObyCP(Pin_ERR2);
	ControlPin::SetIObyCP(Pin_PFCOK);
	ControlPin::SetIObyCP(Pin_FANALM);
	
	ControlPin::SetIObyCP(Pin_RST);
	ControlPin::SetIObyCP(Pin_OFF);
	ControlPin::SetIObyCP(Pin_ERRP);
	ControlPin::SetIObyCP(Pin_FANPWM);
	ControlPin::SetIObyCP(Pin_RunLed);
	ControlPin::SetIObyCP(Pin_Mode0);
	ControlPin::SetIObyCP(Pin_Mode1);
	ControlPin::SetIObyCP(Pin_Mode2);
	ControlPin::SetIObyCP(Pin_Mode3);
	
	ControlPin::SetMeHigh(Pin_RST);
	ControlPin::SetMeHigh(Pin_OFF);

	LPC4078PWM::SetPWM(PWM_HD0,PWM_CH3,100,30,1000);
	ControlPin::SetMeLow(Pin_ERRP);
	ControlPin::SetMeLow(Pin_RunLed);
	
	PowerConfigMode = ControlPin::ReadMe(Pin_Mode0) + (ControlPin::ReadMe(Pin_Mode1)<<1);	
	
}
void Aux_PowerADJ_2US6K_422108_Base::Hardware_Init(unsigned char BoardID)
{
		unsigned char AuxFunc;
	  IO_Init();
		ADC_Init();	  
		SetDacOut2Zero();	 
   
    for(unsigned char i = 0; i<MAX_DATA_GROUP; i++)
    {
        for(unsigned char j = 0; j<MAX_AUXCH_PER_DATA_GROUP; j++)
        {
            Aux_DataManager::m_PChn2EChn [i][j].e_data_group_id = 0xFF;
            Aux_DataManager::m_PChn2EChn [i][j].e_chan_id = 0xFF;
        }
    }
    AuxFunc=Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_globe.m_AuxFuncType;
    if( ((AuxFunc&0x01)==0x01)&&(Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_globe.m_Total_ADC!=0 )	)// 配置了ADC
    {            
        //分配 data_group 开始号
        Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start = 0;
        //标记data_group 有效
        Aux_DataManager::m_DataGroupMgr[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start] = true;

        for(int i = 0; i< 4; i++) //该板型最多4个电气通道，
        {   //拷贝板上通道信息到AuxDataBank中去。
            Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].m_Chn_Info =Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_chn[i];   //copy  通道信息到 AuxDataBank
                       
        }

    }
    if(((AuxFunc&0x80)==0x80) &&(Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_globe.m_Total_DAC !=0))  //配置了DAC通道
    {

        Aux_DataManager::m_DataGroupMgr[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+3] = true;
        for (unsigned char i=0; i<4; i++)
        {
            Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+3][i].m_Chn_Info =Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_DAC_chn[i];   //copy  通道信息到 AuxDataBank
            
        }
    }
		
}
void Aux_PowerADJ_2US6K_422108_Base::Software_Init(unsigned char Board_ID)
{

	board_config::Func_AUX_Dac_Out = &Aux_PowerADJ_2US6K_422108_Base::DAC_SET;
  POWERADJ::Func_Voltage_Dac_Out = &Aux_PowerADJ_2US6K_422108_Base::Aux_Power_dac_out;
	POWERADJ::PowerRunningState = &Aux_PowerADJ_2US6K_422108_Base::OpenProcess600A;
	POWERADJ::IndependentSample = &Aux_PowerADJ_2US6K_422108_Base::Aux_I2C_Sample;
	POWERADJ::VppSET_VOL_MAX = Aux_DataManager::m_AuxDataBank[3][ADJ_Vpp_CHAN].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_RangeH;
  POWERADJ::VppSET_VOL_MIN = Aux_DataManager::m_AuxDataBank[3][ADJ_Vpp_CHAN].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_RangeL; 
  POWERADJ::VmmSET_VOL_MAX = Aux_DataManager::m_AuxDataBank[3][ADJ_Vmm_CHAN].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_RangeH;
  POWERADJ::VmmSET_VOL_MIN = Aux_DataManager::m_AuxDataBank[3][ADJ_Vmm_CHAN].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_RangeL; 	
  CommonFunc::GetTimeTicksUpdate(&FanAdjustPeriodTime);	
}

unsigned char Aux_PowerADJ_2US6K_422108_Base::OpenProcess600A(void)
{	
	unsigned long long tempTicks = 0;
	switch(PowerStatus)
	{			
		case POWER_IDLE:
//			PowerStatus = POWER_ERROR;
			tempTicks = CommonFunc::GetTimeTicks(&EngineDelayTime);
			if(tempTicks > 20*10000)	//500ms
			{
				PowerStatus = POWER_WAIT_FOR_REST;
				ControlPin::SetMeHigh(Pin_OFF);
				ControlPin::SetMeHigh(Pin_RST);
				CommonFunc::GetTimeTicksUpdate(&EngineDelayTime); 
			}		     
			break;
		case POWER_WAIT_FOR_REST:
			tempTicks = CommonFunc::GetTimeTicks(&EngineDelayTime);
			if(tempTicks > 5000)	//500ms
			{
				PowerStatus = POWER_SLOW_START_ON;				
				ControlPin::SetMeLow(Pin_RST);
				CommonFunc::GetTimeTicksUpdate(&EngineDelayTime);  
			}
			break;
			
		case POWER_SLOW_START_ON:
			tempTicks = CommonFunc::GetTimeTicks(&EngineDelayTime);
			if(tempTicks > 5*10000)//5S
			{
				PowerStatus = POWER_SET_DAC;
				ControlPin::SetMeLow(Pin_OFF);						
				CommonFunc::GetTimeTicksUpdate(&EngineDelayTime);  
			}
			break;
			
		case POWER_SET_DAC:
			tempTicks = CommonFunc::GetTimeTicks(&EngineDelayTime);		
			if(Get_ErrPinStatus() == true)
			{
				if(tempTicks > 1000 && Power_DacRampStatus == 0)						//100ms
				{ 				
// 					DAC6571::DAC6571_SET(I2C0,0,(uint16_t)(3.25*109.5187));
// 					DAC6571::DAC6571_SET(I2C0,1,(uint16_t)(1.125*206.8687));
 					Aux_Power_dac_out(ADJ_Vpp_CHAN,Aux_DataManager::m_AuxDataBank[3][ADJ_Vpp_CHAN].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_RangeH/2);
 					Aux_Power_dac_out(ADJ_Vmm_CHAN,Aux_DataManager::m_AuxDataBank[3][ADJ_Vmm_CHAN].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_RangeH/2);
					Aux_Power_dac_out(ADJ_Cpp_CHAN,Aux_DataManager::m_AuxDataBank[3][ADJ_Cpp_CHAN].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_RangeH/2);
 					Aux_Power_dac_out(ADJ_Cmm_CHAN,Aux_DataManager::m_AuxDataBank[3][ADJ_Cmm_CHAN].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_RangeH/2);

					Power_DacRampStatus = 1;
					CommonFunc::GetTimeTicksUpdate(&EngineDelayTime);
				}				  
				else if(tempTicks > 3000 && Power_DacRampStatus == 1)						//300ms
				{ 				
// 					DAC6571::DAC6571_SET(I2C0,0,(uint16_t)(5.5*109.5187));
// 					DAC6571::DAC6571_SET(I2C0,1,(uint16_t)(4.5*206.8687));
 					Aux_Power_dac_out(ADJ_Vpp_CHAN,Aux_DataManager::m_AuxDataBank[3][ADJ_Vpp_CHAN].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_RangeH);
					Aux_Power_dac_out(ADJ_Vmm_CHAN,Aux_DataManager::m_AuxDataBank[3][ADJ_Vmm_CHAN].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_RangeH);
 					Aux_Power_dac_out(ADJ_Cpp_CHAN,Aux_DataManager::m_AuxDataBank[3][ADJ_Cpp_CHAN].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_RangeH);
 					Aux_Power_dac_out(ADJ_Cmm_CHAN,Aux_DataManager::m_AuxDataBank[3][ADJ_Cmm_CHAN].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_RangeH);

					PowerStatus = POWER_RUN;
					ControlPin::SetMeHigh(Pin_RunLed);
					Power_DacRampStatus = 0;
					CommonFunc::GetTimeTicksUpdate(&EngineDelayTime);
				}
			}
			else
			{
				if(tempTicks > 1000)					//100ms
				{ 
					PowerStatus = POWER_ERROR;
					ControlPin::SetMeHigh(Pin_OFF);
					ControlPin::SetMeLow(Pin_RunLed);
					SetDacOut2Zero();
					ControlPin::SetMeLow(Pin_RunLed);					
					Power_DacRampStatus = 0;
					CommonFunc::GetTimeTicksUpdate(&EngineDelayTime);
				}
			}					
			break;		
		case POWER_RUN:
			tempTicks = CommonFunc::GetTimeTicks(&EngineDelayTime);	
      Fan_PWM_Adjust();		
			if((Get_ErrPinStatus() == false)||(CurrentSafetyCheck()==false))
			{
				if(tempTicks > 1000)				//500ms
				{
				  PowerStatus = POWER_ERROR;	
					ControlPin::SetMeHigh(Pin_OFF);
					ControlPin::SetMeLow(Pin_RunLed);
					SetDacOut2Zero();
					ControlPin::SetMeLow(Pin_RunLed);					
				}
			}
			else
			{												
				CommonFunc::GetTimeTicksUpdate(&EngineDelayTime);				
			}				
			break;		
			
		case POWER_ERROR:			
			
			break;		
	}

  return PowerStatus;	
}

void Aux_PowerADJ_2US6K_422108_Base::SetDacOut2Zero(void)
{	
	Aux_Power_dac_out(ADJ_Vpp_CHAN,0);
	Aux_Power_dac_out(ADJ_Vmm_CHAN,0);
}

void Aux_PowerADJ_2US6K_422108_Base::ADC_Init(void)
{
	 ADC101C027::ADC101C027_Init(I2C0);
}
void Aux_PowerADJ_2US6K_422108_Base::DAC_SET(unsigned char chan,unsigned long data)
{
	unsigned char I2Cx = I2C0;
	if(PowerConfigMode == VmmSingleMode)
	{
		unsigned char ChanIndexInI2C = chan%2;
    if(ChanIndexInI2C == 1)
		 I2Cx = I2C0;
	  else
		 I2Cx = I2C1;
	  DAC6571::DAC6571_SET(I2Cx,chan/2,data);
  }
	else
	{
	 unsigned char ChanIndexInI2C = chan%2;
   if(ChanIndexInI2C == 0)
		 I2Cx = I2C0;
	 else
		 I2Cx = I2C1;
	 DAC6571::DAC6571_SET(I2Cx,chan/2,data);
 }
	 
	 
}

void Aux_PowerADJ_2US6K_422108_Base::Aux_Power_dac_out(unsigned char chan,float Vol_Vset)
{    
    if(Aux_DataManager::m_AuxDataBank[3][chan].m_Chn_Info.m_Populated!=1)
        return;
// 	Vol_Vset = 15;
    uint16 DAC_CODE = 0;
    if(Vol_Vset > Aux_DataManager::m_AuxDataBank[3][chan].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_RangeH)
        Vol_Vset = Aux_DataManager::m_AuxDataBank[3][chan].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_RangeH;
    else if(Vol_Vset < Aux_DataManager::m_AuxDataBank[3][chan].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_RangeL)
        Vol_Vset = Aux_DataManager::m_AuxDataBank[3][chan].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_RangeL;
    Aux_DataManager::m_AuxDataBank[3][chan].aux_data = Vol_Vset;
    Aux_DataManager::m_AuxDataBank[3][chan].updated = 1;
    Aux_DataManager::m_AuxDataBank[3][chan].value_update_time = CommonFunc::GetSystemTime();//OS_ServeProcess::OS_Time;  //160429
    DAC_CODE=Aux_DataManager::CalibrateVoltageDAC(3,chan,Vol_Vset);
    if(DAC_CODE>0x3FF)
        DAC_CODE = 0x3FF;
    DAC_SET(chan,DAC_CODE);

}

void Aux_PowerADJ_2US6K_422108_Base::Aux_Read(unsigned char ADCchn)
{
// 	 unsigned char I2Cx = I2C0;
// 	 unsigned char ChanIndexInI2C = ADCchn%2;
// 	if(PowerConfigMode == VmmSingleMode)
// 	{
// 		if(ChanIndexInI2C == 1)
// 		 I2Cx = I2C0;
// 	 else
// 		 I2Cx = I2C1;
//   }
// 	else
// 	{
//    if(ChanIndexInI2C == 0)
// 		 I2Cx = I2C0;
// 	 else
// 		 I2Cx = I2C1;
//  }
//     Aux_DataManager::m_AuxDataBank[0][ADCchn].aux_instant_code = ADC101C027::ADC101C027_Read(I2Cx, ADCchn/2).Result; //将数据放到Aux_DataBank 中
//     Aux_DataManager::m_AuxDataBank[0][ADCchn].updated = true; //标记已经更新   
  if((PowerConfigMode == VmmSingleMode)||(PowerConfigMode == VppSingleMode))
	{
		Aux_DataManager::m_AuxDataBank[0][ADCchn].aux_instant_code = ADCFunc::ReadADC_Code(ADCchn/2);
  }	  
	else
	{
		Aux_DataManager::m_AuxDataBank[0][ADCchn].aux_instant_code = ADCFunc::ReadADC_Code(ADCchn);
  }		
  Aux_DataManager::m_AuxDataBank[0][ADCchn].updated = true; //标记已经更新   	
}
void Aux_PowerADJ_2US6K_422108_Base::Aux_I2C_Sample(void)
{
	 static unsigned char ADCChanIndex = 0;	 
	 	 
	 for(unsigned char i = 0; i<4; i++)
		{
			 ADCChanIndex = ADCChanIndex%4;
			 if(Aux_DataManager::m_AuxDataBank[0][ADCChanIndex].m_Chn_Info.m_Populated)
			 {				 
				 Aux_Read(ADCChanIndex);
				 ADCChanIndex++;
				 return;
       }
			 else
			 {
				 if(ADCChanIndex == ADJ_Vpp_CHAN)
				   Aux_DataManager::m_AuxDataBank[0][ADCChanIndex].aux_data = POWERADJ::Vpp_Set;
				 else if(ADCChanIndex == ADJ_Vmm_CHAN)
					 Aux_DataManager::m_AuxDataBank[0][ADCChanIndex].aux_data = POWERADJ::Vmm_Set;
       }
			 ADCChanIndex++;
		}
	 
	 
	
}
bool Aux_PowerADJ_2US6K_422108_Base::Get_ErrPinStatus(void)
{		
	static bool ErrPinState = false;
 	Fan_Safety_Check();
	unsigned char a = 0;

	if(ControlPin::ReadMe(Pin_ERR1) == 0x01 && ControlPin::ReadMe(Pin_ERR2) == 0x01 
		 && ControlPin::ReadMe(Pin_PFCOK) == 0x00 /*&& FanJudgeState == FanSafe*/)
	{
		if(ErrPinRiseNum++ >= 10)
		{
			ErrPinState = true;
			ErrPinRiseNum = 10;
		}		
		ErrPinFallNum = 0;			
  }
	else
	{
		if(ErrPinFallNum++ >= 10)
		{
			ErrPinState = false;
			ErrPinFallNum = 10;
		}		
		ErrPinRiseNum = 0;
  }
	return ErrPinState;
}
bool Aux_PowerADJ_2US6K_422108_Base::CurrentSafetyCheck(void)
{
	if(Aux_DataManager::m_AuxDataBank[0][ADJ_Cpp_CHAN].m_Chn_Info.m_Populated == 1)
	{
		if(Aux_DataManager::m_AuxDataBank[0][ADJ_Cpp_CHAN].aux_data > Aux_DataManager::m_AuxDataBank[0][ADJ_Cpp_CHAN].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_RangeH*1.05)
			return false;
  }
	if(Aux_DataManager::m_AuxDataBank[0][ADJ_Cmm_CHAN].m_Chn_Info.m_Populated == 1)
	{
		if(Aux_DataManager::m_AuxDataBank[0][ADJ_Cmm_CHAN].aux_data > Aux_DataManager::m_AuxDataBank[0][ADJ_Cmm_CHAN].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_RangeH*1.05)
			return false;
  }
	
	return true;
}
unsigned char Aux_PowerADJ_2US6K_422108_Base::FanSafetyPinCheck(void)
{
    unsigned char temp;
    temp = ControlPin::ReadMe(Pin_FANALM);    
    return temp;
}
void Aux_PowerADJ_2US6K_422108_Base::Fan_Safety_Check(void)
{
    switch(FanCheckState)
    {
    case FanSafe:
        FanPassTime = CommonFunc::TimeDiff(StartFanSafeTime);
        if(FanPassTime.Second >= 3 )//3秒之内没有电平异常
        {
//             ClrUnsafe(chn, FAN_FAULT_UNSAFE,IV_UNSAFE_TYPE);
					  FanJudgeState= FanSafe;
            CommonFunc::GetTimeTicksUpdate(&StartFanSafeTime);
            FanUnsafeConunter = 0;
            FanElimination = 0;
        }
        else
        {
            if(FanSafetyPinCheck()==1)
            {
                FanElimination++;
                if(FanElimination >= 5)//去抖处理，连续监测到5次错误电平。
                {
                    FanCheckState = FanUnsafe;
                    FanUnsafeConunter++;
                    CommonFunc::GetTimeTicksUpdate(&StartFanUnsafeTime);
                }
                if(FanUnsafeConunter >= 2)//连续监测到两个周期错误。
                {
//                     SetUnsafe(chn, FAN_FAULT_UNSAFE, 0,IV_UNSAFE_TYPE);
									  FanJudgeState= FanUnsafe; 
                    FanUnsafeConunter = 0;
                }
            }
            else
            {
                FanElimination = 0;
            }
        }
        break;
    case FanUnsafe:
        FanPassTime = CommonFunc::TimeDiff(StartFanUnsafeTime);
        if(FanPassTime.Second>=3 )//3秒只能全是电平异常
        {
//             SetUnsafe(chn, FAN_FAULT_UNSAFE, 0,IV_UNSAFE_TYPE);
					  FanJudgeState= FanUnsafe;
            FanUnsafeConunter = 0;
            FanElimination = 0;
            CommonFunc::GetTimeTicksUpdate(&StartFanUnsafeTime);
        }
        else
        {
            if(FanSafetyPinCheck()==0)
            {
                FanCheckState = FanSafe;
                FanElimination = 0;
                CommonFunc::GetTimeTicksUpdate(&StartFanSafeTime);
            }
        }
        break;
    default:
        break;
    }
}
void Aux_PowerADJ_2US6K_422108_Base::Fan_PWM_Adjust(void)
{
	if(CommonFunc::TimeDiff(FanAdjustPeriodTime).Second>=1)
	{
		CommonFunc::GetTimeTicksUpdate(&FanAdjustPeriodTime);
		float Cpp_Current = 0;float Cmm_Current = 0;float CurrentRatio = 0;
		if(Aux_DataManager::m_AuxDataBank[0][ADJ_Cpp_CHAN].m_Chn_Info.m_Populated == 1)
		{
			Cpp_Current = Aux_DataManager::m_AuxDataBank[0][ADJ_Cpp_CHAN].aux_data;			
		}
		if(Aux_DataManager::m_AuxDataBank[0][ADJ_Cmm_CHAN].m_Chn_Info.m_Populated == 1)
		{
			Cmm_Current = Aux_DataManager::m_AuxDataBank[0][ADJ_Cmm_CHAN].aux_data;			
		}
		if((Cpp_Current == 0)&&(Cmm_Current == 0))
			return;
		
		if(Cpp_Current>Cmm_Current)	
		{
			CurrentRatio = Cpp_Current*100.0f/(Aux_DataManager::m_AuxDataBank[0][ADJ_Cpp_CHAN].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_RangeH \
			- Aux_DataManager::m_AuxDataBank[0][ADJ_Cpp_CHAN].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_RangeL);
		}
		else
		{
			CurrentRatio = Cmm_Current*100.0f/(Aux_DataManager::m_AuxDataBank[0][ADJ_Cmm_CHAN].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_RangeH \
			- Aux_DataManager::m_AuxDataBank[0][ADJ_Cmm_CHAN].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_RangeL);
		}
		CurrentRatio += 30;
		if(CurrentRatio>100)
			CurrentRatio = 100;
		if(CurrentRatio<30)
			CurrentRatio = 30;
		
		LPC4078PWM::SetPWM(PWM_HD0,PWM_CH3,100,(unsigned long)(CurrentRatio),1000);
// 		LPC4078PWM::PWM_MatchUpdate(PWM_HD0,PWM_CH3, (unsigned long)(CurrentRatio), PWM_MATCH_UPDATE_NOW);
  }
	
}
