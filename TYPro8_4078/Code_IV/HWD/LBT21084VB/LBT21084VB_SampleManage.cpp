
#include "../../Entry/includes.h"

//把LBT21084VB_Base内的采样ProcessADC转移进来
//同时更新SampleProcess类文件
//comment out chen 20130216
//打算增加一个特殊的采样支持的IR函数
//直接adcreset  把通道置于该通道
//把状态变成noopen
//运行后，再次还原


//for 接口函数

char LBT21084VB_SampleManage::m_chn;
unsigned long int LBT21084VB_SampleManage::m_value;
char LBT21084VB_SampleManage::m_type;
char LBT21084VB_SampleManage::m_ADC_chn;
unsigned char LBT21084VB_SampleManage::I_ADG_Address[MAXCHANNELNO];

char LBT21084VB_SampleManage::chn_history;
char LBT21084VB_SampleManage::chn_DoIR;
char LBT21084VB_SampleManage::ADC_Status;
unsigned char LBT21084VB_SampleManage::IR_CTRL_FLAG;
unsigned char LBT21084VB_SampleManage::IR_SWITCH_FLAG;

unsigned short LBT21084VB_SampleManage::TimeTicks;
const unsigned short LBT21084VB_SampleManage::DelayTickCount_ADG = 8; //12;  //600us Open ADG--->200us Open ADG seens more good
const unsigned short LBT21084VB_SampleManage::DelayTickCount_AdcConv = 1;  
const unsigned short LBT21084VB_SampleManage::DelayTickCount_AdcReadData = 1; 
unsigned int LBT21084VB_SampleManage::Trigger_Counter = 0;

unsigned long LBT21084VB_SampleManage::RestADC;

void LBT21084VB_SampleManage::Init(void)
{
	m_chn = 0;
	m_value = 0;
	m_type = 0;

	//内部运转
	//m_I_chn = 0;    //I，V通道从0开始
	//m_V_chn = 0;
	m_ADC_chn = 0;

	chn_history = 0;
	//状态机
	ADC_Status = LBT21084VB_ADC_IDLE;
	for(unsigned char i=0;i<ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount;i++)
	{
	    I_ADG_Address[i] = i;
    }
	IR_CTRL_FLAG = 0;
	IR_SWITCH_FLAG = 0;
}

 
char LBT21084VB_SampleManage::SampleISR(unsigned long CH_Number,unsigned long type)
{
    //if((OS_ServeProcess::OS_Time.Us100 & 0x0F) == 0)     //每隔1.6毫秒重新计算一次时间？ 这个时间间隔是否合理呢？
		ChannelInfo_Manager::Set_TimeRecalculateSign();

//	  unsigned char bOK;
		unsigned char temp;
		unsigned long data;
		
	switch(ADC_Status)
	{
        case LBT21084VB_ADC_IDLE:
            ADC_Status = LBT21084VB_ADC_OpenADG;
            TimeTicks = 0;
            m_ADC_chn = 0;
					  Adc24bit_AD7175::ADC_CS= LBT21084VB_Base::PinGroup[LBT21084VB_Pin_AdcCS1];  //电流
			      ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_CS);  
            Adc24bit_AD7175::ADC_CS= LBT21084VB_Base::PinGroup[LBT21084VB_Pin_AdcCS2];   //电压
			      ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_CS);  	
            Adc24bit_AD7175::ADC_START = LBT21084VB_Base::PinGroup[LBT21084VB_Pin_AdcSTART];// 
	          ControlPin::SetMeLow(Adc24bit_AD7175::ADC_START);
            break;
        case LBT21084VB_ADC_OpenADG:
           
            TimeTicks = 0;
            ADC_Status = LBT21084VB_ADC_OPENING;

             if(IR_CTRL_FLAG == 1)    //特殊的IR控制
							 {
								 IR_SWITCH_FLAG = 1;  //need to switch to IR channel
					
							// LBT21084VB_Base::ADG_Reset();
							 m_ADC_chn = chn_DoIR;
							 //LBT21084VB_Base::ADC_ADG_open(m_ADC_chn);
							 LBT21084VB_Base::ADC_ADG_open(I_ADG_Address[m_ADC_chn]); //add by hpq,20150730
							// ADC_Status = LBT21084VB_ADC_NoWork;    //省去了开关模拟门的时序
						  
                            }
						 else
							  LBT21084VB_Base::ADC_ADG_open(I_ADG_Address[m_ADC_chn]);
						 
            if((ACR::ACR_TimerEn)&&(ACR::ACRChannelNo == m_ADC_chn)) // (ACR功能通道采样)
            ACR::Write_V(m_ADC_chn,ACR_ON);
            break;
        case LBT21084VB_ADC_OPENING:
            TimeTicks++;
            if(TimeTicks >= DelayTickCount_ADG)
            {
             if(IR_CTRL_FLAG == 1 && IR_SWITCH_FLAG == 1)    // already switch to IR channel
				
				{
				   ADC_Status = LBT21084VB_ADC_NoWork;    //Stop other channel's sampling, and waiting for the trigger from IR timer 								
					
				} 
			 else
				{
				ADC_Status = LBT21084VB_ADC_TriggerI_V;

				}	
            TimeTicks = 0;
            }
            break;
        case LBT21084VB_ADC_TriggerI_V:     //Trigger Current and Voltage at the same time
           
			SSPIFunc::ConfirmDevice(1, &LBT21084VB_Base::LBT21084VB_SSP1ADC); 
            Adc24bit_AD7175::ADC_CS =  LBT21084VB_Base::PinGroup[LBT21084VB_Pin_AdcCS1];
						ControlPin::SetMeLow(Adc24bit_AD7175::ADC_CS); 
            Adc24bit_AD7175::Trigger(1);
			 
			 SSPIFunc::ConfirmDevice(1, &LBT21084VB_Base::LBT21084VB_SSP1ADC); 
             Adc24bit_AD7175::ADC_CS = LBT21084VB_Base::PinGroup[LBT21084VB_Pin_AdcCS2];
			 ControlPin::SetMeLow(Adc24bit_AD7175::ADC_CS); 							
             Adc24bit_AD7175::Trigger(1);  // Prepare V trigger
			 
						ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_START);
	          TIMERSFunc::DELAY_SYS(20, 0);
	          ControlPin::SetMeLow(Adc24bit_AD7175::ADC_START);
			  RestADC = 0;
            ADC_Status = LBT21084VB_ADC_ReadI;
		      Adc24bit_AD7175::ADC_CS = LBT21084VB_Base::PinGroup[LBT21084VB_Pin_AdcCS2]; // Set Voltage CS high to read I
			  ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_CS); 		
					
             
				
				 
            break;
        case LBT21084VB_ADC_ReadI:
//				Adc24bit_AD7175::ADC_CS = LBT21084VB_Base::PinGroup[LBT21084VB_Pin_AdcCS2]; // Set Voltage CS high to read I
//			    ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_CS); 
			
		       Adc24bit_AD7175::ADC_CS =  LBT21084VB_Base::PinGroup[LBT21084VB_Pin_AdcCS1];  //Set CS to Current ADC

					  Adc24bit_AD7175::ADC_DRDY = LBT21084VB_Base::PinGroup[LBT21084VB_Pin_AdcDRDY1];
				    temp=ControlPin::ReadMe(Adc24bit_AD7175::ADC_DRDY);
				    if(temp==0)
				    {
	              data= Adc24bit_AD7175::Read(1);
	              if(data!=0)
							  {
								   ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32CurrentAdc=data;
							  }
								  //ADC_Status = LBT21084VB_ADC_TriggerV;
									ADC_Status = LBT21084VB_ADC_ReadV; 
						    ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_CS);		
					}
					  else
						{ 
								if(RestADC++>=0xFF)
								{
									LBT21084VB_Base::ADC_Init();
									LBT21084VB_SampleManage::Init();
                }   						 
             }						
            break;
						 
//	   case  LBT21084VB_ADC_TriggerV:		//Merged to Trigger_I_V  to reduce the time comsuming 				 

//            SSPIFunc::ConfirmDevice(1, &LBT21084VB_Base::LBT21084VB_SSP1ADC); 
//            Adc24bit_AD7175::ADC_CS = LBT21084VB_Base::PinGroup[LBT21084VB_Pin_AdcCS2];
//						ControlPin::SetMeLow(Adc24bit_AD7175::ADC_CS); 
//            Adc24bit_AD7175::Trigger(1);
//						ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_START);
//	          TIMERSFunc::DELAY_SYS(20, 0);
//	          ControlPin::SetMeLow(Adc24bit_AD7175::ADC_START);
//            ADC_Status = LBT21084VB_ADC_ReadV;
//				    RestADC = 0; 						 
//            break;
       
        case LBT21084VB_ADC_ReadV:
			 Adc24bit_AD7175::ADC_CS = LBT21084VB_Base::PinGroup[LBT21084VB_Pin_AdcCS2];  // Set Voltage CS low to read V
			 ControlPin::SetMeLow(Adc24bit_AD7175::ADC_CS);	
					  Adc24bit_AD7175::ADC_DRDY = LBT21084VB_Base::PinGroup[LBT21084VB_Pin_AdcDRDY2];
				    temp=ControlPin::ReadMe(Adc24bit_AD7175::ADC_DRDY);
				    if(temp==0)
				    {
	              data= Adc24bit_AD7175::Read(1);
	              if(data!=0)
							  {
								   ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32VoltageAdc=data;
									 Sample_Pro::NewCodeSampled[m_ADC_chn] = 1;
							  }
							  ADC_Status = LBT21084VB_ADC_CloseADG;
                              //ACR::Write_V(m_ADC_chn,ACR_OFF);
							  
							    Adc24bit_AD7175::ADC_CS = LBT21084VB_Base::PinGroup[LBT21084VB_Pin_AdcCS2];
		  
						    ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_CS);		
					  }
						 else
						{ 
								if(RestADC++>=0xFF)
								{
									LBT21084VB_Base::ADC_Init();
									LBT21084VB_SampleManage::Init();
                }   						 
					}
            break;
        case LBT21084VB_ADC_CloseADG:

            if(IR_CTRL_FLAG == 0)    //如果没有IR特殊控制
            {
                m_ADC_chn++;	
                if(m_ADC_chn>=ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount)
                {
                    m_ADC_chn=0;
                }		
                ADC_Status = LBT21084VB_ADC_OpenADG;
            }
            else
						{
				if(IR::ADG_Ready ==1 )  //If alreday at the IR channel, then wait for next trigger from IR timer
					ADC_Status = LBT21084VB_ADC_NoWork;
				else   //other wise, switch to the IR channel
				{
							  m_ADC_chn = chn_DoIR;   //这里直接切换到内阻通道
                ADC_Status = LBT21084VB_ADC_OpenADG;
						}
			}
            break;
        case LBT21084VB_ADC_NoWork:
			 IR::ADG_Ready =1;  //Add by DKQ 01.27.2017
            break;
        default:
            ADC_Status = LBT21084VB_ADC_IDLE;
            break;
	}

	return 0;    //error	
}

////////////////////////////////////////////////////////////////////////////////////////
//触发该chn的特殊采样，同时记录，并关闭其他通道
char LBT21084VB_SampleManage::IR_ADC(char chn)
{
	chn_history = m_ADC_chn;    //记录历史
    chn_DoIR = chn;
	IR_CTRL_FLAG = 1;
	IR::ADG_Ready =0;
	return 1;
}

void LBT21084VB_SampleManage::IR_Trigger_ADC(void)
{
	ADC_Status = LBT21084VB_ADC_TriggerI_V;

}

//恢复
char LBT21084VB_SampleManage::renew_IR_ADC(void) 
{
	m_ADC_chn = chn_history;    //还原
    IR_CTRL_FLAG = 0;    //clear
	ADC_Status = LBT21084VB_ADC_IDLE;
	IR_SWITCH_FLAG = 0;

	return 1;
}

///////////////////
void LBT21084VB_SampleManage::IR_Read_ADC(char chn,unsigned long *SampleV,unsigned long *SampleI)
{
// 	Adc24bit_CS5560::Convert();    //conv

// 	ARBUS_DELAY(300);    //等待
//     
// 	//add by hpq,20150730
// 	Adc24bit_CS5560::Pin_CS = LBT21084VB_Base::PinGroup[LBT21084VB_Pin_AdcCS1];
// 	ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32CurrentAdc  = Adc24bit_CS5560::Read();
// 	
// 	Adc24bit_CS5560::Pin_CS = LBT21084VB_Base::PinGroup[LBT21084VB_Pin_AdcCS2];
// 	ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32VoltageAdc  = Adc24bit_CS5560::Read();
	*SampleI = ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32CurrentAdc;
	*SampleV = ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32VoltageAdc;     //电压
}
