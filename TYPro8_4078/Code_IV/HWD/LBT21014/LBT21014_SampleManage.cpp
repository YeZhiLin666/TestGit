
#include "../../Entry/includes.h"

//把LBT21014_Base内的采样ProcessADC转移进来
//同时更新SampleProcess类文件
//comment out chen 20130216
//打算增加一个特殊的采样支持的IR函数
//直接adcreset  把通道置于该通道
//把状态变成noopen
//运行后，再次还原


//for 接口函数
char LBT21014_SampleManage::m_chn;
unsigned long int LBT21014_SampleManage::m_value;
char LBT21014_SampleManage::m_type;
char LBT21014_SampleManage::m_ADC_chn;
unsigned char LBT21014_SampleManage::I_ADG_Address[MAXCHANNELNO];

char LBT21014_SampleManage::chn_history;
char LBT21014_SampleManage::chn_DoIR;
char LBT21014_SampleManage::ADC_Status;
unsigned char LBT21014_SampleManage::IR_CTRL_FLAG;
unsigned char LBT21014_SampleManage::IR_SWITCH_FLAG;

unsigned short LBT21014_SampleManage::TimeTicks;
const unsigned short LBT21014_SampleManage::DelayTickCount_ADG = 8; //12;  //600us Open ADG--->200us Open ADG seens more good
// const unsigned short LBT21014_SampleManage::DelayTickCount_AdcConv = 1;
// const unsigned short LBT21014_SampleManage::DelayTickCount_AdcReadData = 1;


unsigned char LBT21014_SampleManage::sample_count[2];
unsigned long LBT21014_SampleManage::sample_value[2][3];
unsigned long LBT21014_SampleManage::RestADC;
void LBT21014_SampleManage::Init(void)
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
    ADC_Status = LBT21014_ADC_IDLE;
    I_ADG_Address[0]=0;
    I_ADG_Address[1]=1;
    IR_CTRL_FLAG = 0;
    IR_SWITCH_FLAG = 0;
    for(unsigned char i=0; i<2; i++)
    {
        sample_count[i]=0;
        for(unsigned char j=0; j<3; j++)
            sample_value[i][j]=0;
    }
}


char LBT21014_SampleManage::SampleISR(unsigned long CH_Number,unsigned long type)
{
    //if((OS_ServeProcess::OS_Time.Us100 & 0x0F) == 0)     //每隔1.6毫秒重新计算一次时间？ 这个时间间隔是否合理呢？
    ChannelInfo_Manager::Set_TimeRecalculateSign();
    if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion <=BOARDTYPE_VER_C)
    {
        switch(ADC_Status)
        {
        case LBT21014_ADC_IDLE:
            ADC_Status = LBT21014_ADC_OpenADG;
            TimeTicks = 0;
            m_ADC_chn = 0;
            break;
        case LBT21014_ADC_OpenADG:
            LBT21014_Base::ADC_ADG_open(I_ADG_Address[m_ADC_chn]);
            TimeTicks = 0;
            ADC_Status = LBT21014_ADC_OPENING;

            if(IR_CTRL_FLAG == 1)    //特殊的IR控制
            {
                LBT21014_Base::ADG_Reset();
                m_ADC_chn = chn_DoIR;
                LBT21014_Base::ADC_ADG_open(m_ADC_chn);
                ADC_Status = LBT21014_ADC_NoWork;    //省去了开关模拟门的时序
            }
            break;
        case LBT21014_ADC_OPENING:
            TimeTicks++;
            if(TimeTicks >= DelayTickCount_ADG)
            {
                ADC_Status = LBT21014_ADC_TriggerI;
                Adc24bit_CS5560::Convert();
                TimeTicks = 0;
            }
            break;
        case LBT21014_ADC_TriggerI:
            SSPIFunc::ConfirmDevice(1, &LBT21014_Base::LBT21014_SSP1ADC);
            Adc24bit_CS5560::Pin_CS = LBT21014_Base::PinGroup_ADC1[LBT21014_Pin_AdcCS1];
            Adc24bit_CS5560::Trigger();
            ADC_Status = LBT21014_ADC_ReadI;
            break;
        case LBT21014_ADC_ReadI:
        {
            SSPIFunc::ConfirmDevice(1, &LBT21014_Base::LBT21014_SSP1ADC);
            unsigned long adc_code_temp = Adc24bit_CS5560::ReadData();    //just for debug
            ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32CurrentAdc  = adc_code_temp;
            ADC_Status = LBT21014_ADC_TriggerV;
            break;
        }
        case LBT21014_ADC_TriggerV:
            SSPIFunc::ConfirmDevice(1, &LBT21014_Base::LBT21014_SSP1ADC);
            Adc24bit_CS5560::Pin_CS = LBT21014_Base::PinGroup_ADC1[LBT21014_Pin_AdcCS2];
            Adc24bit_CS5560::Trigger();
            ADC_Status = LBT21014_ADC_ReadV;
            break;
        case LBT21014_ADC_ReadV:
            SSPIFunc::ConfirmDevice(1, &LBT21014_Base::LBT21014_SSP1ADC);
            ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32VoltageAdc = Adc24bit_CS5560::ReadData();
            ADC_Status = LBT21014_ADC_CloseADG;
            Sample_Pro::NewCodeSampled[m_ADC_chn] = 1;
            break;
        case LBT21014_ADC_CloseADG:
            if(IR_CTRL_FLAG == 0)    //如果没有IR特殊控制
            {
                m_ADC_chn++;
                if(m_ADC_chn>=ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount)
                {
                    m_ADC_chn=0;
                }
                //关闭
                ADC_Status = LBT21014_ADC_OpenADG;
            }
            else
                ADC_Status = LBT21014_ADC_OpenADG;
            break;
        case LBT21014_ADC_NoWork:
            break;
        default:
            ADC_Status = LBT21014_ADC_IDLE;
            break;
        }
    }
    else
    {
        unsigned char temp;
        unsigned long data;

        switch(ADC_Status)
        {
        case LBT21014_ADC_IDLE:
            ADC_Status = LBT21014_ADC_OpenADG;
            TimeTicks = 0;
            m_ADC_chn = 0;
            Adc24bit_AD7175::ADC_CS= LBT21014_Base::PinGroup_ADC2[LBT21014_Pin_Adc2CS1];  //电流
            ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_CS);
            Adc24bit_AD7175::ADC_CS= LBT21014_Base::PinGroup_ADC2[LBT21014_Pin_Adc2CS2];   //电压
            ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_CS);
            Adc24bit_AD7175::ADC_START = LBT21014_Base::PinGroup_ADC2[LBT21014_Pin_Adc2START];//
            ControlPin::SetMeLow(Adc24bit_AD7175::ADC_START);
            break;
        case LBT21014_ADC_OpenADG:
//            LBT21014_Base::ADC_ADG_open(I_ADG_Address[m_ADC_chn]);
            TimeTicks = 0;
            ADC_Status = LBT21014_ADC_OPENING;

            if(IR_CTRL_FLAG == 1)    //特殊的IR控制
            {
                IR_SWITCH_FLAG = 1;
//                LBT21014_Base::ADG_Reset();
                m_ADC_chn = chn_DoIR;
//                LBT21014_Base::ADC_ADG_open(m_ADC_chn);
                LBT21014_Base::ADC_ADG_open(I_ADG_Address[m_ADC_chn]);
//                ADC_Status = LBT21014FRVB_ADC_NoWork;    //省去了开关模拟门的时序
            }
            else
                LBT21014_Base::ADC_ADG_open(I_ADG_Address[m_ADC_chn]);
            break;
        case LBT21014_ADC_OPENING:
            TimeTicks++;
            if(TimeTicks >= DelayTickCount_ADG)
            {
                if(IR_CTRL_FLAG == 1 && IR_SWITCH_FLAG == 1)    // already switch to IR channel

                {
                    ADC_Status = LBT21014_ADC_NoWork;    //Stop other channel's sampling, and waiting for the trigger from IR timer

                }
                else
                    ADC_Status = LBT21014_ADC_TriggerI_V;
//                 Adc24bit_CS5560::Convert();
                TimeTicks = 0;
            }
            break;
        case LBT21014_ADC_TriggerI_V:
            SSPIFunc::ConfirmDevice(1, &LBT21014_Base::LBT21014_SSP1ADC);
            Adc24bit_AD7175::ADC_CS =  LBT21014_Base::PinGroup_ADC2[LBT21014_Pin_Adc2CS1];
            ControlPin::SetMeLow(Adc24bit_AD7175::ADC_CS);
            Adc24bit_AD7175::Trigger(1);

            SSPIFunc::ConfirmDevice(1, &LBT21014_Base::LBT21014_SSP1ADC);
            Adc24bit_AD7175::ADC_CS = LBT21014_Base::PinGroup_ADC2[LBT21014_Pin_Adc2CS2];
            ControlPin::SetMeLow(Adc24bit_AD7175::ADC_CS);
            Adc24bit_AD7175::Trigger(1);  // Prepare V trigger

            Adc24bit_AD7175::ADC_START = LBT21014_Base::PinGroup_ADC2[LBT21014_Pin_Adc2START];
            ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_START);
            TIMERSFunc::DELAY_SYS(20,0);
            ControlPin::SetMeLow(Adc24bit_AD7175::ADC_START);
            ADC_Status = LBT21014_ADC_ReadI;
            RestADC = 0;
            Adc24bit_AD7175::ADC_CS = LBT21014_Base::PinGroup_ADC2[LBT21014_Pin_Adc2CS2];
            ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_CS);
            break;
        case LBT21014_ADC_ReadI:
            Adc24bit_AD7175::ADC_CS =  LBT21014_Base::PinGroup_ADC2[LBT21014_Pin_Adc2CS1];  //Set CS to Current ADC
            Adc24bit_AD7175::ADC_DRDY = LBT21014_Base::PinGroup_ADC2[LBT21014_Pin_Adc2DRDY1];
            temp=ControlPin::ReadMe(Adc24bit_AD7175::ADC_DRDY);
            if(temp==0)
            {
                data= Adc24bit_AD7175::Read(1);
                if(data!=0)
                {
                    ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32CurrentAdc=data;
                }
//							  ADC_Status = LBT21014FRVB_ADC_TriggerV;
                ADC_Status = LBT21014_ADC_ReadV;
                ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_CS);
            }
            else
            {
                if(RestADC++>=0xFF)
                {
                    LBT21014_Base::ADC_Init();
                    LBT21014_SampleManage::Init();
                }
            }
            break;
//        case LBT21014FRVB_ADC_TriggerV:
// 		        SSPIFunc::ConfirmDevice(1, &LBT21014_Base::LBT21014_SSP1ADC);
// 						Adc24bit_AD7175::ADC_CS = LBT21014_Base::PinGroup[LBT21014FRVB_Pin_AdcCS2];
// 						ControlPin::SetMeLow(Adc24bit_AD7175::ADC_CS);
//             Adc24bit_AD7175::Trigger(1);
// 						ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_START);
// 	          TIMERSFunc::DELAY_SYS(20,0);
// 	          ControlPin::SetMeLow(Adc24bit_AD7175::ADC_START);
//             ADC_Status = LBT21014FRVB_ADC_ReadV;
//             break;
        case LBT21014_ADC_ReadV:
            Adc24bit_AD7175::ADC_CS = LBT21014_Base::PinGroup_ADC2[LBT21014_Pin_Adc2CS2];  // Set Voltage CS low to read V
            ControlPin::SetMeLow(Adc24bit_AD7175::ADC_CS);
            Adc24bit_AD7175::ADC_DRDY = LBT21014_Base::PinGroup_ADC2[LBT21014_Pin_Adc2DRDY2];
            temp=ControlPin::ReadMe(Adc24bit_AD7175::ADC_DRDY);
            if(temp==0)
            {
                data= Adc24bit_AD7175::Read(1);
                if(data!=0)
                {
                    ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32VoltageAdc=data;
                    Sample_Pro::NewCodeSampled[m_ADC_chn] = 1;
                }
                ADC_Status = LBT21014_ADC_CloseADG;

// 								Adc24bit_AD7175::ADC_CS = LBT21014_Base::PinGroup[LBT21014FRVB_Pin_AdcCS2];

                ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_CS);
            }
            else
            {
                if(RestADC++>=0xFF)
                {
                    LBT21014_Base::ADC_Init();
                    LBT21014_SampleManage::Init();
                }
            }
            break;
        case LBT21014_ADC_CloseADG:
            if(IR_CTRL_FLAG == 0)    //如果没有IR特殊控制
            {
                m_ADC_chn++;
                if(m_ADC_chn>=ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount)
                {
                    m_ADC_chn=0;
                }
                //关闭
                ADC_Status = LBT21014_ADC_OpenADG;
            }
            else
            {
                if(IR::ADG_Ready ==1 )  //If alreday at the IR channel, then wait for next trigger from IR timer
                    ADC_Status = LBT21014_ADC_NoWork;
                else
                {
                    m_ADC_chn = chn_DoIR;
                    ADC_Status = LBT21014_ADC_OpenADG;
                }
            }
            break;
        case LBT21014_ADC_NoWork:
            IR::ADG_Ready =1;
            break;
        default:
            ADC_Status = LBT21014_ADC_IDLE;
            break;
        }
    }
    return 0;    //error
}
//处理ADC     自我调度运行
/*int LBT21014_SampleManage::ProcessADC(char adc_device_chn)
{
	return -1;    //error
}

char LBT21014_SampleManage::getCHN(char adc_device_chn)
{
	return m_chn;
}

//返回的是码字
unsigned long int LBT21014_SampleManage::getValue(char adc_device_chn)
{
	return m_value;
}

char LBT21014_SampleManage::getType(char adc_device_chn)
{
	return m_type;
}
*/
////////////////////////////////////////////////////////////////////////////////////////
//触发该chn的特殊采样，同时记录，并关闭其他通道
char LBT21014_SampleManage::IR_ADC(char chn)
{
    chn_history = m_ADC_chn;    //记录历史
    chn_DoIR = chn;
    IR_CTRL_FLAG = 1;
    IR::ADG_Ready =0;

    return 1;
}

void LBT21014_SampleManage::IR_Trigger_ADC(void)
{
    ADC_Status = LBT21014_ADC_TriggerI_V;

}
//恢复
char LBT21014_SampleManage::renew_IR_ADC(void)
{
    m_ADC_chn = chn_history;    //还原
    IR_CTRL_FLAG = 0;    //clear

    ADC_Status = LBT21014_ADC_IDLE;
    IR_SWITCH_FLAG = 0;

    return 1;
}

///////////////////
void LBT21014_SampleManage::IR_Read_ADC(char chn,unsigned long *SampleV,unsigned long *SampleI)
{
    if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion<=BOARDTYPE_VER_C )
    {
        Adc24bit_CS5560::Convert();    //conv
        ARBUS_DELAY(300);    //等待
    }

    *SampleI = ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32CurrentAdc;//LBT21014FRVB_ADC5DAC::ADC_rd(chn,LBT21014FRVB_ADC_TYPE_I);
    *SampleV = ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32VoltageAdc;//LBT21014FRVB_ADC5DAC::ADC_rd(chn,LBT21014FRVB_ADC_TYPE_V);     //电压
// 	 Adc24bit_AD7175::ADC_START = LBT21014_Base::PinGroup[LBT21014FRVB_Pin_AdcSTART];//
// 	Adc24bit_AD7175::ADC_CS= LBT21014_Base::PinGroup[LBT21014FRVB_Pin_AdcCS1];  //电流
// 	ControlPin::SetMeLow(Adc24bit_AD7175::ADC_CS);
// 	Adc24bit_AD7175::Trigger(1);
// 	ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_START);
// 	 TIMERSFunc::DELAY_SYS(20,0);
// 	ControlPin::SetMeLow(Adc24bit_AD7175::ADC_START);
// 	 TIMERSFunc::DELAY_SYS(100,0);
// 	*SampleI=Adc24bit_AD7175::Read(1);
// 		ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_CS);
//   Adc24bit_AD7175::ADC_CS= LBT21014_Base::PinGroup[LBT21014FRVB_Pin_AdcCS2];   //电压
// 		ControlPin::SetMeLow(Adc24bit_AD7175::ADC_CS);
//  	Adc24bit_AD7175::Trigger(1);
// 	ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_START);
// 	 TIMERSFunc::DELAY_SYS(20,0);
// 	ControlPin::SetMeLow(Adc24bit_AD7175::ADC_START);
// 	 TIMERSFunc::DELAY_SYS(100,0);
// 	*SampleV=Adc24bit_AD7175::Read(1);
// 		ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_CS);
}
