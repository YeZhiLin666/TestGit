
#include "../../Entry/includes.h"


char LBT21XXX_SampleManage::m_ADC_chn;
unsigned char LBT21XXX_SampleManage::I_ADG_Address[MAXCHANNELNO];

char LBT21XXX_SampleManage::chn_history;
char LBT21XXX_SampleManage::chn_DoIR;
char LBT21XXX_SampleManage::ADC_Status;
unsigned char LBT21XXX_SampleManage::IR_CTRL_FLAG;
unsigned char LBT21XXX_SampleManage::IR_SWITCH_FLAG;   //170207,hpq

unsigned short LBT21XXX_SampleManage::TimeTicks;
unsigned short LBT21XXX_SampleManage::DelayTickCount_ADG = eADG_DelayTickCountFor400us; //8;  //400us Open ADG Open ADG seems better

unsigned long LBT21XXX_SampleManage::RestADC;

unsigned char LBT21XXX_SampleManage::SampleChanNumber;

void LBT21XXX_SampleManage::SetADG_DelayTickCount(eADG_DelayTickCount count)
{
	DelayTickCount_ADG = count;
}

void LBT21XXX_SampleManage::Init(void)
{
    m_ADC_chn = 0;

    chn_history = 0;
    //状态机
    ADC_Status = LBT21XXX_ADC_IDLE;
    if(board_config::DacInfo.m_bHaveBuildInVolt == 1)//MSTAT21044
    {
        SampleChanNumber = 8;
    }
    else
    {
        SampleChanNumber = ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount;
    }
    for(unsigned char i=0; i<SampleChanNumber; i++)
    {
        I_ADG_Address[i] = i;
    }
    IR_CTRL_FLAG = 0;
    IR_SWITCH_FLAG = 0;
}

void LBT21XXX_SampleManage::ResetSampleChanNumber(void)
{
		switch(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType)
		{
		case BOARDTYPE_MSTAT21044:
		case BOARDTYPE_LBT21162:
		case BOARDTYPE_LBT21084:
		case BOARDTYPE_LBT21024:
		case BOARDTYPE_LBT21014:
		case BOARDTYPE_LBT20162:
		case BOARDTYPE_LBT20084:
				LBT21XXX_SampleManage::Init();
				break;
		}
}

char LBT21XXX_SampleManage::SampleISR(unsigned long CH_Number,unsigned long type)
{
// 	 ChannelInfo_Manager::Set_TimeRecalculateSign();

    if(LBT21XXX_Base::BoardFeature.BoolFlag.Adc24bit_CS5560_Enable )
    {
#if ADC_CS5560_ENABLE == 1
        Adc24bit_CS5560_Work();
#endif
    }

    else if(LBT21XXX_Base::BoardFeature.BoolFlag.Adc24bit_AD7175_Enable)
    {
        Adc24bit_AD7175_Work();
    }
    return 0;    //error
}

#if ADC_CS5560_ENABLE == 1
void LBT21XXX_SampleManage::Adc24bit_CS5560_Work(void)
{
    switch(ADC_Status)
    {
    case LBT21XXX_ADC_IDLE:
        ADC_Status = LBT21XXX_ADC_OpenADG;
        TimeTicks = 0;
        m_ADC_chn = 0;
        break;
    case LBT21XXX_ADC_OpenADG:
        LBT21XXX_Base::ADC_ADG_open(I_ADG_Address[m_ADC_chn]);
        TimeTicks = 0;
        ADC_Status = LBT21XXX_ADC_OPENING;

        if(IR_CTRL_FLAG == 1)    //特殊的IR控制
        {   //
            m_ADC_chn = chn_DoIR;
            LBT21XXX_Base::ADC_ADG_open(I_ADG_Address[m_ADC_chn]); //add by hpq,20150730
            IR_SWITCH_FLAG = 1;  //need to switch to IR channel
        }
        break;
    case LBT21XXX_ADC_OPENING:
        TimeTicks++;
        if(TimeTicks >= DelayTickCount_ADG)
        {
            if(IR_CTRL_FLAG == 1&& IR_SWITCH_FLAG == 1)
            {
                ADC_Status = LBT21XXX_ADC_NoWork;
                break;
            }
            ADC_Status = LBT21XXX_ADC_TriggerI;
            Adc24bit_CS5560::Convert();
            TimeTicks = 0;
        }
        break;
    case LBT21XXX_ADC_TriggerI:
        SSPIFunc::ConfirmDevice(1, &LBT21XXX_Base::LBT21XXX_SSP1_ADC_5560); //Add by DKQ 03.08.2016
        Adc24bit_CS5560::Pin_CS = LBT21XXX_Base::PinGroup_ADC[LBT21XXX_Pin_AdcCS1];
        Adc24bit_CS5560::Trigger();
        ADC_Status = LBT21XXX_ADC_ReadI;
        break;
    case LBT21XXX_ADC_ReadI:
    {
        SSPIFunc::ConfirmDevice(1, &LBT21XXX_Base::LBT21XXX_SSP1_ADC_5560); //Add by DKQ 03.08.2016
        unsigned long adc_code_temp = Adc24bit_CS5560::ReadData();    //just for debug

        ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32CurrentAdc  = adc_code_temp;

        ADC_Status = LBT21XXX_ADC_TriggerV;
        break;
    }
    case LBT21XXX_ADC_TriggerV:
        SSPIFunc::ConfirmDevice(1, &LBT21XXX_Base::LBT21XXX_SSP1_ADC_5560); ////Add by DKQ 03.08.2016
        Adc24bit_CS5560::Pin_CS = LBT21XXX_Base::PinGroup_ADC[LBT21XXX_Pin_AdcCS2];
        Adc24bit_CS5560::Trigger();
        ADC_Status = LBT21XXX_ADC_ReadV;
        break;
    case LBT21XXX_ADC_ReadV:
        SSPIFunc::ConfirmDevice(1, &LBT21XXX_Base::LBT21XXX_SSP1_ADC_5560); ////Add by DKQ 03.08.2016

        ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32VoltageAdc = Adc24bit_CS5560::ReadData();
        Sample_Pro::NewCodeSampled[m_ADC_chn] = 1;
        Sample_Pro::NewIRSampled[m_ADC_chn] = 1;

        ADC_Status = LBT21XXX_ADC_CloseADG;

        break;
    case LBT21XXX_ADC_CloseADG:
        if(IR_CTRL_FLAG == 0)    //如果没有IR特殊控制
        {
            m_ADC_chn++;
            if(m_ADC_chn>=SampleChanNumber)
            {
                m_ADC_chn=0;
            }
            ADC_Status = LBT21XXX_ADC_OpenADG;
        }
        else
        {
            ADC_Status = LBT21XXX_ADC_OpenADG;
        }
        break;
    case LBT21XXX_ADC_NoWork:
        IR::ADG_Ready =1;  //Add by DKQ 01.27.2017
        break;
    default:
        ADC_Status = LBT21XXX_ADC_IDLE;
        break;
    }
}
#endif
void LBT21XXX_SampleManage::Adc24bit_AD7175_Work(void)
{
    unsigned char temp;
    unsigned long data;

    switch(ADC_Status)
    {
    case LBT21XXX_ADC_IDLE:
        ADC_Status = LBT21XXX_ADC_OpenADG;
        TimeTicks = 0;
        m_ADC_chn = 0;
        Adc24bit_AD7175::ADC_CS= LBT21XXX_Base::PinGroup_ADC[LBT21XXX_Pin_Adc2CS1];  //电流
        ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_CS);
        Adc24bit_AD7175::ADC_CS= LBT21XXX_Base::PinGroup_ADC[LBT21XXX_Pin_Adc2CS2];   //电压
        ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_CS);
        Adc24bit_AD7175::ADC_START = LBT21XXX_Base::PinGroup_ADC[LBT21XXX_Pin_Adc2START];//
        ControlPin::SetMeLow(Adc24bit_AD7175::ADC_START);
        break;
    case LBT21XXX_ADC_OpenADG:
        TimeTicks = 0;
        ADC_Status = LBT21XXX_ADC_OPENING;
        if(IR_CTRL_FLAG == 1)    //特殊的IR控制
        {
            IR_SWITCH_FLAG = 1;  //need to switch to IR channel
            m_ADC_chn = chn_DoIR;
            //LBT21XXXVB_Base::ADC_ADG_open(m_ADC_chn);
            LBT21XXX_Base::ADC_ADG_open(I_ADG_Address[m_ADC_chn]); //add by hpq,20150730
        }
        else
            LBT21XXX_Base::ADC_ADG_open(I_ADG_Address[m_ADC_chn]);

        break;
    case LBT21XXX_ADC_OPENING:
        TimeTicks++;
        if(TimeTicks >= DelayTickCount_ADG)
        {
            if(IR_CTRL_FLAG == 1 && IR_SWITCH_FLAG == 1)    // already switch to IR channel
            {
                ADC_Status = LBT21XXX_ADC_NoWork;    //Stop other channel's sampling, and waiting for the trigger from IR timer
            }
            else
            {
                ADC_Status = LBT21XXX_ADC_TriggerI_V;
            }
            TimeTicks = 0;
        }
        break;
    case LBT21XXX_ADC_TriggerI_V:     //Trigger Current and Voltage at the same time
        BuiltIn_DataManager::m_SuspendFlg=1;
        SSPIFunc::ConfirmDevice(1, &LBT21XXX_Base::LBT21XXX_SSP1_ADC_7175);
        Adc24bit_AD7175::ADC_CS =  LBT21XXX_Base::PinGroup_ADC[LBT21XXX_Pin_Adc2CS1];
        ControlPin::SetMeLow(Adc24bit_AD7175::ADC_CS);
        Adc24bit_AD7175::Trigger(1);

        SSPIFunc::ConfirmDevice(1, &LBT21XXX_Base::LBT21XXX_SSP1_ADC_7175);
        Adc24bit_AD7175::ADC_CS = LBT21XXX_Base::PinGroup_ADC[LBT21XXX_Pin_Adc2CS2];
        ControlPin::SetMeLow(Adc24bit_AD7175::ADC_CS);
        Adc24bit_AD7175::Trigger(1);  // Prepare V trigger
// 				     TIMERSFunc::DELAY_SYS(100,0);

        Adc24bit_AD7175::ADC_START = LBT21XXX_Base::PinGroup_ADC[LBT21XXX_Pin_Adc2START];
        ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_START);
        TIMERSFunc::DELAY_SYS(20, 0);
        ControlPin::SetMeLow(Adc24bit_AD7175::ADC_START);
        RestADC = 0;
// 				TIMERSFunc::DELAY_SYS
        ADC_Status = LBT21XXX_ADC_WAIT;
//              ADC_Status = LBT21XXX_ADC_ReadI;
        Adc24bit_AD7175::ADC_CS = LBT21XXX_Base::PinGroup_ADC[LBT21XXX_Pin_Adc2CS2]; // Set Voltage CS high to read I
        ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_CS);
        break;
    case LBT21XXX_ADC_WAIT:
        ADC_Status = LBT21XXX_ADC_ReadI;
        break;
    case LBT21XXX_ADC_ReadI:
        Adc24bit_AD7175::ADC_CS =  LBT21XXX_Base::PinGroup_ADC[LBT21XXX_Pin_Adc2CS1];  //Set CS to Current ADC

        Adc24bit_AD7175::ADC_DRDY = LBT21XXX_Base::PinGroup_ADC[LBT21XXX_Pin_Adc2DRDY1];
        temp=ControlPin::ReadMe(Adc24bit_AD7175::ADC_DRDY);
        if(temp==0)
        {
            data= Adc24bit_AD7175::Read(1);
            if(data!=0)
            {
                if(board_config::DacInfo.m_bHaveBuildInVolt == 1)//MSTAT21044
                {
                    if(m_ADC_chn<4)
                        ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32CurrentAdc  = data;
                }
                else
                    ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32CurrentAdc  = data;

            }
            ADC_Status = LBT21XXX_ADC_ReadV;
            ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_CS);
        }
        else
        {
            if(RestADC++>=0xFF)
            {
                Adc24bit_AD7175::ADC_CS = LBT21XXX_Base::PinGroup_ADC[LBT21XXX_Pin_Adc2CS1];
                Adc24bit_AD7175::Reset2(1);
                SSPIFunc::setPara(&LBT21XXX_Base::LBT21XXX_SSP1_ADC_7175,1);
                SSPIFunc::my_Init(1);
                ARBUS_DELAY(10);
                Adc24bit_AD7175::Init_diffmode(eCONVCODETYPE_BIPOLAR,1); //初始化电流采样ADC7175
// 									Adc24bit_AD7175::Reset2(1);
// 									LBT21XXX_Base::ADC_Init();
                LBT21XXX_SampleManage::Init();
            }
        }
        break;
    case LBT21XXX_ADC_ReadV:
        Adc24bit_AD7175::ADC_CS = LBT21XXX_Base::PinGroup_ADC[LBT21XXX_Pin_Adc2CS2];  // Set Voltage CS low to read V
        ControlPin::SetMeLow(Adc24bit_AD7175::ADC_CS);
        Adc24bit_AD7175::ADC_DRDY = LBT21XXX_Base::PinGroup_ADC[LBT21XXX_Pin_Adc2DRDY2];
        temp=ControlPin::ReadMe(Adc24bit_AD7175::ADC_DRDY);
        if(temp==0)
        {
            data= Adc24bit_AD7175::Read(1);
            if(data!=0)
            {
                if(board_config::DacInfo.m_bHaveBuildInVolt == 1)
                {
                    if(m_ADC_chn>=4)
                    {
                        ChannelInfo_Manager::ChannelInfo[m_ADC_chn - 4].m_u32BuildInVolt[0] = data;
                        ChannelInfo_Manager::ChannelInfo[m_ADC_chn - 4].m_bBuildInVoltNewCode[0] = 1;
                    }
                    else
                    {
                        ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32VoltageAdc = data;
                        Sample_Pro::NewCodeSampled[m_ADC_chn] = 1;
                        Sample_Pro::NewIRSampled[m_ADC_chn] = 1;
                    }
                }
                else
                {
                    ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32VoltageAdc = data;
                    Sample_Pro::NewCodeSampled[m_ADC_chn] = 1;
                    Sample_Pro::NewIRSampled[m_ADC_chn] = 1;
                }

            }
            ADC_Status = LBT21XXX_ADC_CloseADG;
            //				BuiltIn_DataManager::m_SuspendFlg=0;                                                       //Delete for Tesla Aux T issue by Qjm on 180611
            //ACR::Write_V(m_ADC_chn,ACR_OFF);

            Adc24bit_AD7175::ADC_CS = LBT21XXX_Base::PinGroup_ADC[LBT21XXX_Pin_Adc2CS2];

            ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_CS);
            if((ChannelInfo_Manager::m_st_BoadType.m_u16BoardType == BOARDTYPE_LBT21162)&&(IR_CTRL_FLAG == 0))//21162放电处理
                LBT21XXX_Base::ADC_ADG_open(0x7F);
        }
        else
        {
            if(RestADC++>=0xFF)
            {
                Adc24bit_AD7175::ADC_CS = LBT21XXX_Base::PinGroup_ADC[LBT21XXX_Pin_Adc2CS2];
                Adc24bit_AD7175::Reset2(1);
                SSPIFunc::setPara(&LBT21XXX_Base::LBT21XXX_SSP1_ADC_7175,1);
                SSPIFunc::my_Init(1);
                ARBUS_DELAY(10);
                Adc24bit_AD7175::Init_diffmode(eCONVCODETYPE_BIPOLAR,1);//初始化电压采样ADC7175
// 									LBT21XXX_Base::ADC_Init();
                LBT21XXX_SampleManage::Init();
            }
        }
        break;
    case LBT21XXX_ADC_CloseADG:
        BuiltIn_DataManager::m_SuspendFlg=0;                                                               //Add for Tesla Aux T issue by Qjm on 180611
        if(IR_CTRL_FLAG == 0)    //如果没有IR特殊控制
        {
            m_ADC_chn++;
            if(m_ADC_chn>=SampleChanNumber)
            {
                m_ADC_chn=0;
            }
            ADC_Status = LBT21XXX_ADC_OpenADG;
        }
        else
        {
            if(IR::ADG_Ready ==1 )  //If alreday at the IR channel, then wait for next trigger from IR timer
                ADC_Status = LBT21XXX_ADC_NoWork;
            else   //other wise, switch to the IR channel
            {
                m_ADC_chn = chn_DoIR;   //这里直接切换到内阻通道
                ADC_Status = LBT21XXX_ADC_OpenADG;
            }
        }
        break;
    case LBT21XXX_ADC_NoWork:
        IR::ADG_Ready =1;  //Add by DKQ 01.27.2017
        break;
    default:
        ADC_Status = LBT21XXX_ADC_IDLE;
        break;
    }
}
////////////////////////////////////////////////////////////////////////////////////////
//触发该chn的特殊采样，同时记录，并关闭其他通道
char LBT21XXX_SampleManage::IR_ADC(char chn)
{
    chn_history = m_ADC_chn;    //记录历史
    chn_DoIR = chn;
    IR_CTRL_FLAG = 1;
    IR::ADG_Ready =0;
    BuiltIn_DataManager::m_SuspendFlg=1;
    return 1;
}

void LBT21XXX_SampleManage::IR_Trigger_ADC(void)
{
    //ADC_Status = LBT21XXXVB_ADC_TriggerI_V;

//     if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion>BOARDTYPE_VER_C)
// 	{
//         ADC_Status = LBT21XXX_ADC_TriggerI_V;
//     }

    if(LBT21XXX_Base::BoardFeature.BoolFlag.Adc24bit_AD7175_Enable == true)
        ADC_Status = LBT21XXX_ADC_TriggerI_V;
}

//恢复
char LBT21XXX_SampleManage::renew_IR_ADC(void)
{
    m_ADC_chn = chn_history;    //还原
    IR_CTRL_FLAG = 0;    //clear
    ADC_Status = LBT21XXX_ADC_IDLE;
    IR_SWITCH_FLAG = 0;
    return 1;
}

///////////////////
void LBT21XXX_SampleManage::IR_Read_ADC(char chn,unsigned long *SampleV,unsigned long *SampleI)
{
#if ADC_CS5560_ENABLE == 1
    if(LBT21XXX_Base::BoardFeature.BoolFlag.Adc24bit_CS5560_Enable )
    {
        Adc24bit_CS5560::Convert();    //conv
        ARBUS_DELAY(300);    //等待
        //add by hpq,20150730
        Adc24bit_CS5560::Pin_CS = LBT21XXX_Base::PinGroup_ADC[LBT21XXX_Pin_AdcCS1];
        ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32CurrentAdc  = Adc24bit_CS5560::Read();

        Adc24bit_CS5560::Pin_CS = LBT21XXX_Base::PinGroup_ADC[LBT21XXX_Pin_AdcCS2];
        ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32VoltageAdc  = Adc24bit_CS5560::Read();
    }
#endif
    *SampleI = ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32CurrentAdc;
    *SampleV = ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32VoltageAdc;     //电压
}
