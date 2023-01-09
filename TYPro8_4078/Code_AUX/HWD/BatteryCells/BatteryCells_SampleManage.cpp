///////////////////////////////////////////////////////////////////////////////////
//File Name: BatteryCells_SampleManage.cpp
//Version:		V1.0
//Date:				2014/11/28
//Description:
//						BatteryCells辅助采样相关处理
//Others:      dirui
//History：

///////////////////////////////////////////////////////////////////////////////////
#include "../Entry/includes.h"


unsigned long BatteryCells_SampleManage::Sample_conter[MAX_BOARD_PER_AUXMCU]= {0,0,0,0,0,0,0,0};

st_Aux_Sample BatteryCells_SampleManage::st_aux_sample_setting[MAX_BOARD_PER_AUXMCU];
unsigned char BatteryCells_SampleManage::Use_ADCNUM;
MsTime BatteryCells_SampleManage::RecordTime;
MsTime BatteryCells_SampleManage::LastTime;

void BatteryCells_SampleManage::Init(unsigned char BoardID)
{

    for(unsigned char uc_i=0; uc_i<MAX_CH_IN_AUX_UNIT; uc_i++)
    {

        Sample_conter[BoardID]=0;
        BatteryCells_SampleManage::st_aux_sample_setting[BoardID].stateMachine=AUX_SAMPLE_IDLE_ENUM;
        BatteryCells_SampleManage::st_aux_sample_setting[BoardID].sspChn=1;
        BatteryCells_SampleManage::st_aux_sample_setting[BoardID].ADC_chn=0;
    }
    //BatteryCells_SampleManage::Use_ADCNUM=BatteryCells_Base::ReleaselConfig();
}

unsigned long BatteryCells_SampleManage::Aux_V_Trigger(unsigned char Bid,unsigned char Bchn)
{

    ADC16bit_LTC68042::LTC6804_adcv(1);		 //发送开始转换命令
    return 1;
}
unsigned long BatteryCells_SampleManage::Aux_V_Read(unsigned char Bid)
{

    return 1;
}
char BatteryCells_SampleManage::SampleISR(unsigned long BoardID,unsigned long ADCID,unsigned long ChannelID)
{
//	unsigned char bOK;
    MsTime Time;    //用于时间比较
    if(Aux_DataManager::m_Aux_BoardCFG[BoardID].BeUsed==0)
        return 0;
    SSPIFunc::setPara(&BatteryCells_Base::BatteryCells_SSP1ADC,1);
    SSPIFunc::my_Init(1);              //初始化

//	static unsigned char status_68042 = SLEEP;
    static MsTime time1 =CommonFunc::GetSystemTime();//OS_Time;
    switch (st_aux_sample_setting[BoardID].stateMachine)
    {
    case AUX_SAMPLE_IDLE_ENUM:
        BatteryCells_Base::beUsedCHannel=BatteryCells_Base::Sum_BatteryCellChannel/12;
        if((BatteryCells_Base::Sum_BatteryCellChannel%12)!=0)
            BatteryCells_Base::beUsedCHannel++;
        BatteryCells_SampleManage::Use_ADCNUM=BatteryCells_Base::beUsedCHannel;

        if(Use_ADCNUM==0)
            return 0;
        if(Use_ADCNUM>AUX_BC_MAXCHIP)
            Use_ADCNUM=AUX_BC_MAXCHIP;

        sampleSettingSet_CS(BoardID);   // 片选信号输出进行过滤处理
        st_aux_sample_setting[BoardID].stateMachine = AUX_SAMPLE_WAITTING_TRIGGER;
        BatteryCells_Base::m_ADC_ChipAddr[BoardID]=0;

        BatteryCells_Base::m_ADC_MaxCnt[BoardID]=Use_ADCNUM;


        break;
    case AUX_SAMPLE_WAITTING_TRIGGER:

        if(	ADC16bit_LTC68042::wakeup_sleep(&time1)==1)   //500us

            st_aux_sample_setting[BoardID].stateMachine = AUX_SAMPLE_WAITTING_TRIGGER;
        else

            st_aux_sample_setting[BoardID].stateMachine = AUX_SAMPLE_WAITTING_SAMPLE;
        break;
    case AUX_SAMPLE_WAITTING_SAMPLE:
        Aux_V_Trigger(BatteryCells_Base::m_ADC_ChipAddr[BoardID],0);
        st_aux_sample_setting[BoardID].stateMachine = AUX_SAMPLE_WAITTING_POLL0;
        break;
    case AUX_SAMPLE_WAITTING_POLL0:
        if (ADC16bit_LTC68042::LTC6804_poll(BatteryCells_Base::m_ADC_ChipAddr[BoardID],1))
        {
            st_aux_sample_setting[BoardID].stateMachine =AUX_SAMPLE_WAITTING_READ;
            CommonFunc::GetTimeTicksUpdate(&RecordTime);
        }
        else
            st_aux_sample_setting[BoardID].stateMachine =AUX_SAMPLE_WAITTING_POLL0;
        break;

    case AUX_SAMPLE_WAITTING_READ:

        ADC16bit_LTC68042::wakeup_idle();
        if(ADC16bit_LTC68042::LTC6804_rd_single_cv(0, BatteryCells_Base::m_ADC_ChipAddr[BoardID],BatteryCells_Base::BC_ReadData,1)==0)
            st_aux_sample_setting[BoardID].stateMachine = AUX_SAMPLE_PROCESSING_DATA;
        else
        {
            CommonFunc::GetTimeTicksUpdate(&LastTime);
            Time = TimeT(LastTime,RecordTime);
            if(Time.Us100 >= MAXSAMPLETIME)
                st_aux_sample_setting[BoardID].stateMachine = AUX_SAMPLE_PROCESSING_DATA; //超时就不要死等了
            else
                st_aux_sample_setting[BoardID].stateMachine = AUX_SAMPLE_WAITTING_READ;   //是否进POLL还是直接read，调试看
        }
        break;


    case AUX_SAMPLE_PROCESSING_DATA:
        unsigned char m_BoardID,m_AuxChID;
        for(unsigned char uc_i=0; uc_i<AUX_BC_MAXCHANNELPERCHIP; uc_i++)
        {
            m_BoardID=BatteryCells_Base::m_MapBattToDat[BatteryCells_Base::m_ADC_ChipAddr[BoardID]*12+uc_i].m_BoardID;
            m_AuxChID=BatteryCells_Base::m_MapBattToDat[BatteryCells_Base::m_ADC_ChipAddr[BoardID]*12+uc_i].m_AuxChannelNum;
            if (BatteryCells_Base::BC_ReadData[BatteryCells_Base::m_ADC_ChipAddr[BoardID]][uc_i]!=0)
            {

                st_aux_sample_setting[m_BoardID].ADC_chn = m_AuxChID ;
                Aux_DataManager::st_Aux_info[m_BoardID][m_AuxChID].AdcCode = BatteryCells_Base::BC_ReadData[BatteryCells_Base::m_ADC_ChipAddr[BoardID]][uc_i];
                Aux_DataManager::st_Aux_info[m_BoardID][m_AuxChID].NewAdcCode = 1;

                Aux_DataManager::m_AuxDataBank[m_BoardID][m_AuxChID].aux_instant_code = Aux_DataManager::st_Aux_info[m_BoardID][m_AuxChID].AdcCode;	//add by Yifei, 2015.06.26
                Aux_DataManager::m_AuxDataBank[m_BoardID][m_AuxChID].updated = 1;
            }
            else
            {
                Aux_DataManager::st_Aux_info[m_BoardID][m_AuxChID].NewAdcCode = 0;
            }
        }
        BatteryCells_Base::m_ADC_ChipAddr[BoardID]++;
        if(BatteryCells_Base::m_ADC_ChipAddr[BoardID]<BatteryCells_Base::m_ADC_MaxCnt[BoardID])
            st_aux_sample_setting[BoardID].stateMachine = AUX_SAMPLE_WAITTING_READ;   //是否进POLL还是直接read，调试看
        else
        {
            st_aux_sample_setting[BoardID].stateMachine = AUX_SAMPLE_WAITTING_SAMPLE;
            BatteryCells_Base::m_ADC_ChipAddr[BoardID]=0;
        }
        break;

    }
    //}
    return 1;
}
void BatteryCells_SampleManage::sampleSettingSet_CS(unsigned char Bid)
{
    switch(Bid)
    {
    case 7:
        ControlPin::SetMeLow(BatteryCells_Base::Pin_ECODE1); // 7号板
        ControlPin::SetMeLow(BatteryCells_Base::Pin_ECODE2);
        ControlPin::SetMeLow(BatteryCells_Base::Pin_ECODE3);
        break;
    case 6:
        ControlPin::SetMeHigh(BatteryCells_Base::Pin_ECODE1);
        ControlPin::SetMeLow(BatteryCells_Base::Pin_ECODE2);
        ControlPin::SetMeLow(BatteryCells_Base::Pin_ECODE3);
        break;
    case 5:
        ControlPin::SetMeLow(BatteryCells_Base::Pin_ECODE1);
        ControlPin::SetMeHigh(BatteryCells_Base::Pin_ECODE2);
        ControlPin::SetMeLow(BatteryCells_Base::Pin_ECODE3);
        break;
    case 4:
        ControlPin::SetMeHigh(BatteryCells_Base::Pin_ECODE1);
        ControlPin::SetMeHigh(BatteryCells_Base::Pin_ECODE2);
        ControlPin::SetMeLow(BatteryCells_Base::Pin_ECODE3);
        break;
    case 3:
        ControlPin::SetMeLow(BatteryCells_Base::Pin_ECODE1);
        ControlPin::SetMeLow(BatteryCells_Base::Pin_ECODE2);
        ControlPin::SetMeHigh(BatteryCells_Base::Pin_ECODE3);
        break;
    case 2:
        ControlPin::SetMeHigh(BatteryCells_Base::Pin_ECODE1);
        ControlPin::SetMeLow(BatteryCells_Base::Pin_ECODE2);
        ControlPin::SetMeHigh(BatteryCells_Base::Pin_ECODE3);
        break;
    case 1:
        ControlPin::SetMeLow(BatteryCells_Base::Pin_ECODE1);
        ControlPin::SetMeHigh(BatteryCells_Base::Pin_ECODE2);
        ControlPin::SetMeHigh(BatteryCells_Base::Pin_ECODE3);
        break;
    case 0:
        ControlPin::SetMeHigh(BatteryCells_Base::Pin_ECODE1);   // 0号板
        ControlPin::SetMeHigh(BatteryCells_Base::Pin_ECODE2);
        ControlPin::SetMeHigh(BatteryCells_Base::Pin_ECODE3);
        break;
    default:
        break;
    }
}
MsTime BatteryCells_SampleManage::TimeT(MsTime a,MsTime b)    // a<b
{
    MsTime tempDiff;
    signed short temp;

    tempDiff.Second = b.Second - a.Second;
    temp = b.Us100 - a.Us100;
    if(temp < 0)
    {
        temp = temp + 10000;
        tempDiff.Second --;
    }
    tempDiff.Us100 = temp;
    return tempDiff;
}
