///////////////////////////////////////////////////////////////////////////////////
//File Name: PULSE_Safety.cpp
//Version:        V1.0
//Date:                2014/8/21
//Description:
//           ���尲ȫ������񣬿���ֵ�쳣ֻ������������Mv�ĵ�����ѹ֮�⻹Ӧ��̨��
//			 ����m_Pulse_Stage_Data�Ƚϣ��κ�һ��Unsafe�˾���Ӧ
//			 �����Լ����ʱ����
//Others:
//History��

///////////////////////////////////////////////////////////////////////////////////
#include "../Entry/includes.h"


UInt16                  PULSE_safety_check::Priority = PRIORITY4;
UInt16                  PULSE_safety_check::TaskID = PULSE_SAFETY_TASK;			// should be a definition which positioning this task in OS_Task[]
#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM32",zidata = "SRAM32"		   // ���ñ����洢���ⲿ��չ�洢����	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // ���ñ����洢���ⲿ��չ�洢����	   dynamic memory  0XA0000000  0x03E80000
#endif
MsTime                  PULSE_safety_check::safety_last_time[MAXCHANNELNO];
// MsTime                  PULSE_safety_check::safety_error_time[MAXCHANNELNO];
MsTime                  PULSE_safety_check::safety_resume_time[MAXCHANNELNO];

unsigned char		PULSE_safety_check::Pulse_UnsafeType[MAXCHANNELNO];


ST_PULSE_SAFETYCHECK 	PULSE_safety_check::Pulse_SafetyCheck[MAXCHANNELNO];


//////////////////creat buf for test
//PULSE_RAW_DATA test_buf[2][2][30];

#pragma arm section //��������������
// unsigned char  PULSE_safety_check::RedundantSafetyFlg[MAXCHANNELNO];
// char                    PULSE_safety_check::PULSE_Safety_Check_status[MAXCHANNELNO];
unsigned long           PULSE_safety_check::ErrorFlag[MAXCHANNELNO];    //safety��־   bit 0 ��������  bit 1 ��ͨ���
// unsigned char           PULSE_safety_check::safety_chn;
// unsigned char           PULSE_safety_check::I_Sample_Flag[MAXCHANNELNO];
// unsigned char           PULSE_safety_check::V_Sample_Flag[MAXCHANNELNO];
// St_CtrlValueCheck       PULSE_safety_check::CtrlValueCheck[MAXCHANNELNO];
// St_IrregularCheck       PULSE_safety_check::IrregularCheck[MAXCHANNELNO];
// St_CtrlValueSetting     PULSE_safety_check::m_CtrlValueSetting;
// St_IrregularSetting     PULSE_safety_check::m_IrregularSetting;
unsigned char           PULSE_safety_check::Process_SM_Ptr[MAXCHANNELNO];	// process() status machine ptr.
float                   PULSE_safety_check::I_Record[MAXCHANNELNO];		// used for recording error values. each channel should has it's own buffer because
float                   PULSE_safety_check::V_Record[MAXCHANNELNO];		// - sometimes there's no new value for this channel, but there's a timeout  triggered
float                   PULSE_safety_check::P_Record[MAXCHANNELNO];		// - unsafe and if only one variable, it could be updated by other channels value already.
float                   PULSE_safety_check::C_Record[MAXCHANNELNO];		// - unsafe and if only one variable, it could be updated by other channels value already.
float                   PULSE_safety_check::t_Record[MAXCHANNELNO];		// - unsafe and if only one variable, it could be updated by other channels value already.
// unsigned char           PULSE_safety_check:: b_ClearError[MAXCHANNELNO];
float                   PULSE_safety_check::UD_Record[MAXCHANNELNO];                           //170122
int                   PULSE_safety_check::UD_UnsafeIndex[MAXCHANNELNO];                      //170122

PULSE_RAW_DATA		*CheckPulseSafetyBuf1_ptr;
PULSE_RAW_DATA		*CheckPulseSafetyBuf2_ptr;
PULSE_RAW_DATA		*BeCheckedPulseSafetyBuf_ptr;

unsigned char Unsafetype;
unsigned char PulseSafetyErrorflg;

///////////////////////////////////

void PULSE_safety_check::Init(void)
{
    //��0
    for(unsigned char i = 0; i<MAXCHANNELNO; i++)
    {
// 		PULSE_Safety_Check_status[i] = 0;    //״̬��
        ErrorFlag[i] = 0;                   //�����־
        Process_SM_Ptr[i] = SAFE;

        //may be wrong  ��Ϊ�������board config�����л������������
        ChannelInfo_Manager::GetSystemSafty(i,MP_SYS_SAFTY_TYPE_CURRENT,ChannelInfo_Manager::CurrRngPhysicalRange1,& safety_check::Check_S[i].checkSYS_I);
        ChannelInfo_Manager::GetSystemSafty(i,MP_SYS_SAFTY_TYPE_VOLTAGE,VOLTAGE_RANGE_HIGH,& safety_check::Check_S[i].checkSYS_V);
        ChannelInfo_Manager::GetSystemSafty(i,MP_SYS_SAFTY_TYPE_POWER,0,& safety_check::Check_S[i].checkSYS_P);

        //����ˢ�±�־
// 		I_Sample_Flag[i] = 0;
// 		V_Sample_Flag[i] = 0;


        //��¼����
        I_Record[i] = 0;
        V_Record[i] = 0;
        P_Record[i] = 0;
        C_Record[i] = 0;
        t_Record[i] = 0;
// 		RedundantSafetyFlg[i]=0;
// 		b_ClearError[i]=0;
// 		Pulse_SafetyCheck[i].UnsafeCount=0;
// 		Pulse_SafetyCheck[i].CheckCount=0;
// 		Pulse_SafetyCheck[i].RecordCount=0;
// 		Pulse_SafetyCheck[i].CheckStageID=1;
// 		Pulse_SafetyCheck[i].UnsafeStageCount=0;
// 		Pulse_SafetyCheck[i].BufOffset=0;
        memset(Pulse_SafetyCheck,0,sizeof(ST_PULSE_SAFETYCHECK));
        Pulse_SafetyCheck[i].CheckStageID=1;
    }

// 	safety_chn = 0;

// 	if(m_CtrlValueSetting.fErrorFactor <= 0)
// 		m_CtrlValueSetting.fErrorFactor = DEF_ACCURACY * DEF_CTRL_VALUE_ERROR_FACTOR;


// 	memset(CtrlValueCheck,0,sizeof(CtrlValueCheck));
// 	memset(IrregularCheck,0,sizeof(IrregularCheck));
// 	m_CtrlValueSetting.bChecking  = 0;
// 	m_CtrlValueSetting.NextTime   = OS_ServeProcess::OS_Time;
// 	m_IrregularSetting.bChecking  = 0;
// 	m_IrregularSetting.NextTime   = OS_ServeProcess::OS_Time;





    CheckPulseSafetyBuf1_ptr=&Pulse::raw_data[0][0][0];
    CheckPulseSafetyBuf2_ptr=&Pulse::raw_data[1][0][0];

    //   	CheckPulseSafetyBuf1_ptr=&test_buf[0][0][0];  //test buf check
    //  	CheckPulseSafetyBuf2_ptr=&test_buf[1][0][0];

    // 	for(unsigned char i=0;i<2;i++)
    // 	{
    // 		for(unsigned char j=0;j<2;j++)
    // 		{
    // 			for(unsigned char k=0;k<2;k++)
    // 			{
    // 				test_buf[i][j][k].m_bDataConverted=1;
    // 				test_buf[i][j][k].m_bSafetyChecked=0;
    // 				test_buf[i][j][k].m_bSampleUpdated=1;
    // 				test_buf[i][j][k].m_fCurrent=1;
    // 				test_buf[i][j][k].m_fVoltage=2;
    // 				test_buf[i][j][k].m_uStageNum=k;
    // 			}
    // 			for(unsigned char k=2;k<30;k++)
    // 			{
    // 				test_buf[i][j][k].m_bDataConverted=0;
    // 				test_buf[i][j][k].m_bSafetyChecked=0;
    // 				test_buf[i][j][k].m_bSampleUpdated=0;
    // 				test_buf[i][j][k].m_fCurrent=1;
    // 				test_buf[i][j][k].m_fVoltage=2;
    // 				test_buf[i][j][k].m_uStageNum=k;
    // 			}
    // 		}
    // 	}

    BeCheckedPulseSafetyBuf_ptr=CheckPulseSafetyBuf1_ptr;


}

//���������֪������ʱ�����  ����limit�ľ�����ʲô
//��Ҫ���Ӽ���error counter������
//�㷨��������ʼ��¼һ��ʱ���last��Ȼ��check
//��������� ��¼��ǰʱ��Ϊerrorʱ�䣬���errorʱ����last����һ��ʱ�䣬��˵�����ʱ���ڶ����limit����ʱ�ж�ͨ��
//���û�о������ƶ�lastʱ��㵽��ǰʱ��
//comment out chen 20130531    ��ȡ�����õ�����ȥ
void PULSE_safety_check::DoWork(void)
{
#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count[COUNT_SAFTY_PULSE] ++;
    CommonFunc::GetTimeTicksUpdate(&OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
	Time_Count::thread_time_init(COUNT_SAFTY_PULSE);
#endif
    uint16 checkitem;


    checkitem =V_CHECK | PI_CHECK;
    check(0,checkitem);
// 	b_ClearError[0]=0;


// 	ClearSchUnsafeIfNotRunning(0);
    StatusProcess(0, checkitem);		// ����״̬������ؽ��

#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count_Time[COUNT_SAFTY_PULSE] = CommonFunc::TimeDiff(OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
	Time_Count::thread_time(COUNT_SAFTY_PULSE);
#endif
}

// void PULSE_safety_check::EndTask(void)
// {

// }

void PULSE_safety_check::SetUnsafe(unsigned char chn, uint32 unsafeType, float value)
{   // each time only one unsafe type is set, and meantime, data recorded.
    //ErrorFlag��־λ    I_Record��������¼
    //ErrorFlag ������λ��ƥ��
    ErrorFlag[chn] |= unsafeType;
    switch(unsafeType)		// record data here
    {
    case CURRENT_SCH_UNSAFE:
    case CURRENT_SYS_UNSAFE:
        I_Record[chn] = value;
        break;
    case VOLTAGE_SCH_UNSAFE:
    case VOLTAGE_SYS_UNSAFE:
        V_Record[chn] = value;
        break;
    case POWER_SCH_UNSAFE:
    case POWER_SYS_UNSAFE:
        P_Record[chn] = value;
        break;
    case SCH_UserDefined_UNSAFE:    //170122
        UD_Record[chn] = value;
        break;
    case CAPACITY_SCH_UNSAFE:
        C_Record[chn] = value;
        break;
    case STEPTIME_SCH_UNSAFE:
        t_Record[chn] = value;
        break;
    }
}
// void inline PULSE_safety_check::ClrUnsafe(unsigned char chn, uint16 unsafeType)
// {
// 	if(	b_ClearError[chn])   // Can't clear the Error flag ,when Pulse status.      dirui 20140902
// 		ErrorFlag[chn] &= ~unsafeType;
// }
void PULSE_safety_check::check(unsigned char chn, uint16 checkItem)
{
    float current, voltage, power;
    unsigned char type = PULSE_safety_check_sch_is_ready(chn);
    unsigned char m_ActiveBuf=0;   //��¼��ǰbuf�������ڼ�鵱ǰѭ����
    Unsafetype = 0;
    PulseSafetyErrorflg=0;
    ////////////////////////////��ͨ��� ÿ�ζ�Ҫ��
    if(type==SAFETY_CHECK_SCH)
    {
        PulseSafetyErrorflg=0;
        current = ChannelInfo_Manager::Get_MetaVariable(chn, MP_DATA_TYPE_METAVALUE,DEFAULT_OPERATOR,DEFAULT_COEFFICIENT, MetaCode_PV_Current);
        voltage = ChannelInfo_Manager::Get_MetaVariable(chn, MP_DATA_TYPE_METAVALUE,DEFAULT_OPERATOR,DEFAULT_COEFFICIENT, MetaCode_PV_Voltage);
        power   = current*voltage;
        if((checkItem & PI_CHECK) != 0)    //P and I
        {

// 			if(safety_check::checkSCH_I[chn].m_bIfCheck == 1)
// 			{
// 				if((current > safety_check::checkSCH_I[chn].m_fHigh )|| (current < safety_check::checkSCH_I[chn].m_fLow))
// 				{
// 					PulseSafetyErrorflg=1;
// 					SetUnsafe(chn, CURRENT_SCH_UNSAFE, current);
// 				}
// 			}
            BeNormalSafetyPIValueCheck(chn,current,power);


        }
// 		else
// 		{
// 			if((safety_check::checkSYS_I[chn].m_bIfCheck == 1) && (current> safety_check::checkSYS_I[chn].m_fHigh || current < safety_check::checkSYS_I[chn].m_fLow))
// 			{
// 				PulseSafetyErrorflg=1;
// 				SetUnsafe(chn, CURRENT_SYS_UNSAFE, current);
// 				Unsafetype=CURRENT_SYS_UNSAFE;
// 			}
        NormalSafetyValueCheck(chn,CURRENT_SYS_UNSAFE,current);
// 		}

        if((checkItem & V_CHECK) != 0)
        {
// 			if((checkItem & V_CHECK) != 0)
// 			{
// 				if((voltage > safety_check::checkSCH_V[chn].m_fHigh )|| (voltage < safety_check::checkSCH_V[chn].m_fLow))
// 				{
// 					PulseSafetyErrorflg=1;
// 					SetUnsafe(chn, VOLTAGE_SCH_UNSAFE, voltage);
// 				}
// 			}
            NormalSafetyValueCheck(chn,VOLTAGE_SCH_UNSAFE,voltage);
// 			if((safety_check::checkSYS_V[chn].m_bIfCheck == 1) && (voltage > safety_check::checkSYS_V[chn].m_fHigh || voltage < safety_check::checkSYS_V[chn].m_fLow))
// 			{
// 				PulseSafetyErrorflg=1;
// 				Unsafetype=VOLTAGE_SYS_UNSAFE;
// 				SetUnsafe(chn, VOLTAGE_SYS_UNSAFE, voltage);
// 			}
        }
// 		else
// 		{
// 			if((safety_check::checkSYS_V[chn].m_bIfCheck == 1) && (voltage > safety_check::checkSYS_V[chn].m_fHigh || voltage < safety_check::checkSYS_V[chn].m_fLow))
// 			{
// 				PulseSafetyErrorflg=1;
// 				Unsafetype=VOLTAGE_SYS_UNSAFE;
// 				SetUnsafe(chn, VOLTAGE_SYS_UNSAFE, voltage);
// 			}
        NormalSafetyValueCheck(chn,VOLTAGE_SYS_UNSAFE,voltage);
// 		}
        if(PulseSafetyErrorflg)
        {
            Pulse_SafetyCheck[chn].UnsafeCount++; //�쳣�ۼ�
            Pulse_UnsafeType[chn]=UNSAFE_COUNTER;
        }

        //170122 start
        St_Safety_UD  *    pUDsafety    =    &ChannelInfo_Manager::ChannelSchedule[chn].m_Settings.m_Safety_UD[0];
        for(int UD_Idx = 0; UD_Idx < MAX_USERDEFINED_SAFETY; UD_Idx++)
        {
            if(pUDsafety->m_bValid)
            {
                float fValue = ChannelInfo_Manager::Get_MetaVariable(chn,pUDsafety->m_DataType,DEFAULT_OPERATOR,DEFAULT_COEFFICIENT,pUDsafety->m_MetaCode);
                if(fValue > pUDsafety->m_fHigh || fValue < pUDsafety->m_fLow)
                {
                    SetUnsafe(chn,SCH_UserDefined_UNSAFE,fValue);
                    UD_UnsafeIndex[chn] = UD_Idx;
                    break;
                }
            }
//       else
//         ClrUnsafe(chn, SCH_UserDefined_UNSAFE);
            pUDsafety++;
        } // 170122 end
    }

    ///////////////////////////////////////////����Ϊ��������Buf���
    ST_PULSE_SAFETYCHECK *pSafety = &Pulse_SafetyCheck[chn];
    for(unsigned char i=0; i<30; i++) //ÿ��ֻ���30�����ݣ���Լ��Դ
    {
        PulseSafetyErrorflg=0;//���������
        if(pSafety->CheckStageID==Pulse::m_Info.m_StageCount) //���Ѿ�����һ��Cycle��Ҫ��Buf
        {
            if(m_ActiveBuf)
                pSafety->BufOffset=0;  //buf0��ƫ����
            else
                pSafety->BufOffset=60;  //buf1��ƫ����

            pSafety->CheckStageID=1;  //���¼�¼Stage��
        }

        if((pSafety->CheckCount%120)==0)
            pSafety->BufOffset=0;  //����buf��������

        if(pSafety->BufOffset>59)
        {
            if((pSafety->CheckCount>0)&&((pSafety->CheckCount&1)==1))//��һ�β����ˣ��´ν��뻹�ô��׵����鿪ʼ�������ֻ����β����
                BeCheckedPulseSafetyBuf_ptr=CheckPulseSafetyBuf2_ptr+((pSafety->BufOffset-60)/2)+30;
            else
                BeCheckedPulseSafetyBuf_ptr=CheckPulseSafetyBuf2_ptr+((pSafety->BufOffset-60)-1)/2;
            m_ActiveBuf=1;
        }
        else
        {
            if((pSafety->CheckCount>0)&&((pSafety->CheckCount&1)==1))//��һ�β����ˣ��´ν��뻹�ô��׵����鿪ʼ�������ֻ����β����
                BeCheckedPulseSafetyBuf_ptr=CheckPulseSafetyBuf1_ptr+(pSafety->BufOffset/2)+30;
            else
            {
                if(pSafety->BufOffset>0)
                    BeCheckedPulseSafetyBuf_ptr=CheckPulseSafetyBuf1_ptr+(pSafety->BufOffset-1)/2;
                else
                    BeCheckedPulseSafetyBuf_ptr=CheckPulseSafetyBuf1_ptr;
            }
            m_ActiveBuf=0;
        }

        if(BeCheckedPulseSafetyBuf_ptr->m_bSafetyChecked)
        {
            pSafety->BufOffset++;
            continue;
        }

        if(!BeCheckedPulseSafetyBuf_ptr->m_bDataConverted)  //�Ƿ�����Чֵ
        {
            if((BeCheckedPulseSafetyBuf_ptr==(&Pulse::raw_data[m_ActiveBuf][0][0]+Pulse::m_Info.m_StageCount)) \
                    ||  (BeCheckedPulseSafetyBuf_ptr==(&Pulse::raw_data[m_ActiveBuf][0][1]+Pulse::m_Info.m_StageCount))  \
                    ||  (BeCheckedPulseSafetyBuf_ptr==(&Pulse::raw_data[m_ActiveBuf][1][0]+Pulse::m_Info.m_StageCount))  \
                    ||  (BeCheckedPulseSafetyBuf_ptr==(&Pulse::raw_data[m_ActiveBuf][1][1]+Pulse::m_Info.m_StageCount))  )    //һ��ѭ�������һ����
            {
                m_ActiveBuf=!m_ActiveBuf;
                if(m_ActiveBuf)
                    pSafety->BufOffset=60;
                else
                    pSafety->BufOffset=0;
                return;
            }
            if(pSafety->CheckStageID<Pulse::m_Info.m_StageCount)  //�������û��ˢ����ϲ������
                continue;
        }
        else  //������Ч�Ļ�
        {
            BeCheckedPulseSafetyBuf_ptr->m_bSafetyChecked=1;
            pSafety->CheckCount++;  //�����ۼ�

            if(type==SAFETY_CHECK_SCH)
            {
                if((checkItem & PI_CHECK) != 0)    //P and I
                {
                    power=BeCheckedPulseSafetyBuf_ptr->m_fCurrent*BeCheckedPulseSafetyBuf_ptr->m_fVoltage;
// 					if(safety_check::checkSCH_I[chn].m_bIfCheck == 1)
// 					{
// 						if ((BeCheckedPulseSafetyBuf_ptr->m_fCurrent> safety_check::checkSCH_I[chn].m_fHigh)
// 							||(BeCheckedPulseSafetyBuf_ptr->m_fCurrent< safety_check::checkSCH_I[chn].m_fLow))        //���������
// 						{
// 							PulseSafetyErrorflg=1;
// 							Unsafetype=CURRENT_SCH_UNSAFE;
// 							SetUnsafe(chn, CURRENT_SCH_UNSAFE, BeCheckedPulseSafetyBuf_ptr->m_fCurrent);
// 						}
// 					}
                    BeNormalSafetyPIValueCheck(chn,current,power);
                }
                if((checkItem & V_CHECK) != 0)    //V
                {
// 					if(safety_check::checkSCH_V[chn].m_bIfCheck==1)
// 					{
// 						if ((BeCheckedPulseSafetyBuf_ptr->m_fVoltage> safety_check::checkSCH_V[chn].m_fHigh)||(BeCheckedPulseSafetyBuf_ptr->m_fVoltage<safety_check::checkSCH_V[chn].m_fLow)) //���������
// 						{
// 							PulseSafetyErrorflg=1;
// 							Unsafetype=VOLTAGE_SCH_UNSAFE;
// 							SetUnsafe(chn, VOLTAGE_SCH_UNSAFE,BeCheckedPulseSafetyBuf_ptr->m_fVoltage);
// 						}
// 					}
                    NormalSafetyValueCheck(chn,VOLTAGE_SCH_UNSAFE,BeCheckedPulseSafetyBuf_ptr->m_fVoltage);
                }
            }
            else
            {
// 				if((safety_check::checkSYS_I[chn].m_bIfCheck == 1) && (BeCheckedPulseSafetyBuf_ptr->m_fCurrent> safety_check::checkSYS_I[chn].m_fHigh || BeCheckedPulseSafetyBuf_ptr->m_fCurrent < safety_check::checkSYS_I[chn].m_fLow))
// 				{
// 					PulseSafetyErrorflg=1;
// 					SetUnsafe(chn, CURRENT_SYS_UNSAFE, BeCheckedPulseSafetyBuf_ptr->m_fCurrent);
// 					Unsafetype=CURRENT_SYS_UNSAFE;
// 				}
                NormalSafetyValueCheck(chn,CURRENT_SYS_UNSAFE,BeCheckedPulseSafetyBuf_ptr->m_fCurrent);
// 				if((safety_check::checkSYS_V[chn].m_bIfCheck == 1) && (BeCheckedPulseSafetyBuf_ptr->m_fVoltage > safety_check::checkSYS_V[chn].m_fHigh || BeCheckedPulseSafetyBuf_ptr->m_fVoltage < safety_check::checkSYS_V[chn].m_fLow))
// 				{
// 					PulseSafetyErrorflg=1;
// 					Unsafetype=VOLTAGE_SYS_UNSAFE;
// 					SetUnsafe(chn, VOLTAGE_SYS_UNSAFE, BeCheckedPulseSafetyBuf_ptr->m_fVoltage);
// 				}
                NormalSafetyValueCheck(chn,VOLTAGE_SYS_UNSAFE,BeCheckedPulseSafetyBuf_ptr->m_fVoltage);
            }

            if(PulseSafetyErrorflg)
            {
                Pulse_SafetyCheck[chn].UnsafeCount++; //�쳣�ۼ�
                Pulse_UnsafeType[chn]=UNSAFE_COUNTER;
                //����������������쳣
                if(pSafety->UnsafeCycleID!= Pulse::buf_info[m_ActiveBuf].m_CycleIdx)//��û�б���¼
                {
                    if(pSafety->UnsafeCycleID==(Pulse::buf_info[m_ActiveBuf].m_CycleIdx-1))
                        pSafety->UnsafeCycleCount++;//ѭ�����ۼ�
                    else
                        pSafety->UnsafeCycleCount=0;  //��������ѭ���Ͳ��ۼ���
                    pSafety->UnsafeCycleID= Pulse::buf_info[m_ActiveBuf].m_CycleIdx;
                    pSafety->UnsafeStageCount=0;  //���¼�¼Stage�쳣
                    if(Pulse_SafetyCheck[chn]. UnsafeCycleCount>=2)  //��һ���Ѿ���ͳ�Ƶ��ˣ����Լ�¼���ξͿ�����
                    {
                        //�������γ��ޱ���Unsafe���˳����
                        switch(Unsafetype)
                        {
                        case CURRENT_SYS_UNSAFE:
                        case CURRENT_SCH_UNSAFE:
                            SetUnsafe(chn, Unsafetype, BeCheckedPulseSafetyBuf_ptr->m_fCurrent);
                            break;
                        case VOLTAGE_SYS_UNSAFE:
                        case VOLTAGE_SCH_UNSAFE:
                            SetUnsafe(chn, Unsafetype, BeCheckedPulseSafetyBuf_ptr->m_fVoltage);
                            break;
                        case POWER_SCH_UNSAFE:
                            SetUnsafe(chn, Unsafetype, power);
                            break;
                        default:
                            break;
                        }

                        Pulse_UnsafeType[chn]=UNSAFE_CYCLE;
                        return;   //�������ֱ���˳�
                    }
                }
                else if(pSafety->UnsafeCycleID==Pulse::buf_info[m_ActiveBuf].m_CycleIdx)//һ��ѭ��֮��Ҫ���Stage�Ƿ�ﵽ�쳣��׼
                {
                    if((pSafety->CheckCount&1))  //һ��Cycle֮����3��stage����������
                    {   //������׵�Ļ�
                        pSafety->CheckCount++;  //�����ۼ�
                        i++;//ѭ���ۼ�
                        BeCheckedPulseSafetyBuf_ptr+=30;//��β��
                        pSafety->BufOffset++;
                        if(!BeCheckedPulseSafetyBuf_ptr->m_bDataConverted)  //������Чֵ
                            continue;  // ������
                        BeCheckedPulseSafetyBuf_ptr->m_bSafetyChecked=1;
                        //���¶�β���һ��
                        if(type==SAFETY_CHECK_SCH)
                        {
                            if((checkItem & PI_CHECK) != 0)    //P and I
                            {
                                power=BeCheckedPulseSafetyBuf_ptr->m_fCurrent*BeCheckedPulseSafetyBuf_ptr->m_fVoltage;
// 								if(safety_check::checkSCH_I[chn].m_bIfCheck == 1)
// 								{
// 									if ((BeCheckedPulseSafetyBuf_ptr->m_fCurrent> safety_check::checkSCH_I[chn].m_fHigh)||(BeCheckedPulseSafetyBuf_ptr->m_fCurrent<         \
// 										safety_check::checkSCH_I[chn].m_fLow))        //���������
// 									{
// 										PulseSafetyErrorflg=1;
// 										Unsafetype=CURRENT_SCH_UNSAFE;
// 										SetUnsafe(chn, CURRENT_SCH_UNSAFE, BeCheckedPulseSafetyBuf_ptr->m_fCurrent);
// 									}
// 								}
                                NormalSafetyValueCheck(chn,CURRENT_SCH_UNSAFE,BeCheckedPulseSafetyBuf_ptr->m_fCurrent);
// 								if(safety_check::checkSCH_P[chn].m_bIfCheck == 1)
// 								{
// 									if ((power> safety_check::checkSCH_P[chn].m_fHigh)||(power<safety_check::checkSCH_P[chn].m_fLow)) //���������
// 									{
// 										PulseSafetyErrorflg=1;
// 										Unsafetype=POWER_SCH_UNSAFE;
// 										SetUnsafe(chn, POWER_SCH_UNSAFE,power);
// 									}
// 								}
                                NormalSafetyValueCheck(chn,POWER_SCH_UNSAFE,power);
                            }

                            if((checkItem & V_CHECK) != 0)    //V
                            {
// 								if(safety_check::checkSCH_V[chn].m_bIfCheck==1)
// 								{
// 									if ((BeCheckedPulseSafetyBuf_ptr->m_fVoltage> safety_check::checkSCH_V[chn].m_fHigh)||(BeCheckedPulseSafetyBuf_ptr->m_fVoltage<safety_check::checkSCH_V[chn].m_fLow)) //���������
// 									{
// 										PulseSafetyErrorflg=1;
// 										Unsafetype=VOLTAGE_SCH_UNSAFE;
// 										SetUnsafe(chn, POWER_SCH_UNSAFE,power);
// 									}
// 								}
                                NormalSafetyValueCheck(chn,VOLTAGE_SCH_UNSAFE,BeCheckedPulseSafetyBuf_ptr->m_fVoltage);
                            }
                        }
                        else
                        {
// 							if((safety_check::checkSYS_I[chn].m_bIfCheck == 1) && (BeCheckedPulseSafetyBuf_ptr->m_fCurrent> safety_check::checkSYS_I[chn].m_fHigh || BeCheckedPulseSafetyBuf_ptr->m_fCurrent < safety_check::checkSYS_I[chn].m_fLow))
// 							{
// 								PulseSafetyErrorflg=1;
// 								SetUnsafe(chn, CURRENT_SYS_UNSAFE, BeCheckedPulseSafetyBuf_ptr->m_fCurrent);
// 								Unsafetype=CURRENT_SYS_UNSAFE;
// 							}
                            NormalSafetyValueCheck(chn,CURRENT_SYS_UNSAFE,BeCheckedPulseSafetyBuf_ptr->m_fCurrent);
// 							if((safety_check::checkSYS_V[chn].m_bIfCheck == 1) && (BeCheckedPulseSafetyBuf_ptr->m_fVoltage > safety_check::checkSYS_V[chn].m_fHigh || BeCheckedPulseSafetyBuf_ptr->m_fVoltage < safety_check::checkSYS_V[chn].m_fLow))
// 							{
// 								PulseSafetyErrorflg=1;
// 								Unsafetype=VOLTAGE_SYS_UNSAFE;
// 								SetUnsafe(chn, VOLTAGE_SYS_UNSAFE, voltage);
// 							}
                            NormalSafetyValueCheck(chn,VOLTAGE_SYS_UNSAFE,BeCheckedPulseSafetyBuf_ptr->m_fVoltage);
                        }
                    }

                    if(PulseSafetyErrorflg)//β�㳬��
                    {
                        pSafety->UnsafeCount++; //�쳣�ۼ�
                        pSafety->UnsafeStageCount++;//stage���޼�¼һ��
                    }
                    if(pSafety->UnsafeStageCount>=3)
                    {   //����3��Stage�������򱨸�Unsafe
                        switch(Unsafetype)
                        {
                        case CURRENT_SYS_UNSAFE:
                        case CURRENT_SCH_UNSAFE:
                            SetUnsafe(chn, Unsafetype, BeCheckedPulseSafetyBuf_ptr->m_fCurrent);
                            break;
                        case VOLTAGE_SYS_UNSAFE:
                        case VOLTAGE_SCH_UNSAFE:
                            SetUnsafe(chn, Unsafetype, BeCheckedPulseSafetyBuf_ptr->m_fVoltage);
                            break;
                        case POWER_SCH_UNSAFE:
                            SetUnsafe(chn, Unsafetype, power);
                            break;
                        default:
                            break;
                        }
                        Pulse_UnsafeType[chn]=UNSAFE_STAGE;
                        return;  //���stage�쳣��ֱ���˳�
                    }
                }
            }

            if(!(pSafety->CheckCount&1))  //ͳ�Ƽ���stage����
                pSafety->CheckStageID++;   // ���Ǽ����һ��Stage��

            pSafety->BufOffset++;
        }
    }
}
void PULSE_safety_check::NormalSafetyValueCheck(unsigned char chn,DWORD type,float value)
{
// 	unsigned char m_bIfCheck;
    float m_fHigh,m_fLow;
    St_SaftyCheckItem  * Check = &safety_check::Check_S[chn].checkSYS_V;
    unsigned char check_offset = CHECK_SYS_V;
    switch(type)
    {
    case CURRENT_SYS_UNSAFE:
// 			m_bIfCheck = safety_check::checkSYS_I[chn].m_bIfCheck;
// 		  m_fHigh = safety_check::checkSYS_I[chn].m_fHigh;
// 		  m_fLow = safety_check::checkSYS_I[chn].m_fLow;
        check_offset = CHECK_SYS_I;
        break;
    case CURRENT_SCH_UNSAFE:
// 			m_bIfCheck = safety_check::checkSCH_I[chn].m_bIfCheck;
// 		  m_fHigh = safety_check::checkSCH_I[chn].m_fHigh;
// 		  m_fLow = safety_check::checkSCH_I[chn].m_fLow;
        check_offset = CHECK_SCH_I;
        break;
    case VOLTAGE_SYS_UNSAFE:
// 			m_bIfCheck = safety_check::checkSYS_V[chn].m_bIfCheck;
// 		  m_fHigh = safety_check::checkSYS_V[chn].m_fHigh;
// 		  m_fLow = safety_check::checkSYS_V[chn].m_fLow;
        break;
    case VOLTAGE_SCH_UNSAFE:
// 			m_bIfCheck = safety_check::checkSCH_V[chn].m_bIfCheck;
// 		  m_fHigh = safety_check::checkSCH_V[chn].m_fHigh;
// 		  m_fLow = safety_check::checkSCH_V[chn].m_fLow;
        check_offset = CHECK_SCH_V;
        break;
    case POWER_SCH_UNSAFE:
// 			m_bIfCheck = safety_check::checkSCH_P[chn].m_bIfCheck;
//       m_fHigh = safety_check::checkSCH_P[chn].m_fHigh;
// 		  m_fLow = safety_check::checkSCH_P[chn].m_fLow;
        check_offset = CHECK_SCH_P;
        break;
    case CAPACITY_SCH_UNSAFE:
        check_offset = CHECK_SCH_C;
        break;
    case STEPTIME_SCH_UNSAFE:
        check_offset = CHECK_SCH_t;
        break;
    default:
        //break;
        return;
    }

    Check += check_offset;
    // m_bIfCheck = Check->m_bIfCheck;//180726 zyx
    if(type == CAPACITY_SCH_UNSAFE || type == STEPTIME_SCH_UNSAFE)
    {
        m_fHigh = Check->m_fHigh;
        if(value > m_fHigh) //?????
        {
            PulseSafetyErrorflg = 1;
            Unsafetype = type;
			if(type == CAPACITY_SCH_UNSAFE)
			{
				if(fabs(safety_check::g_dChargeCapacity[chn]) < fabs(safety_check::g_dDischargeCapacity[chn]))
				    value = -value;
			}
            SetUnsafe(chn, type, value);
        }
    }
    else
    {
        m_fHigh = Check->m_fHigh;
        m_fLow = Check->m_fLow;

//     if(m_bIfCheck==1)//180726 zyx
//     {
        if ((value> m_fHigh)||(value<m_fLow)) //���������
        {
            PulseSafetyErrorflg=1;
            Unsafetype=type;
            SetUnsafe(chn, type, value);
        }
//     }
    }
}
// void PULSE_safety_check::ClearSchUnsafeIfNotRunning(unsigned char chn)
// {
// 	unsigned char type = PULSE_safety_check_sch_is_ready(chn);

// 	if(type == SAFETY_CHECK_SYS)	// if no schedule is running, clear the schedule unsafe status.
// 	{
// 		ClrUnsafe(chn, CURRENT_SCH_UNSAFE);
// 		ClrUnsafe(chn, VOLTAGE_SCH_UNSAFE);
// 		ClrUnsafe(chn, POWER_SCH_UNSAFE);
// 		ClrUnsafe(chn, SCH_UserDefined_UNSAFE);    //170122
// 	}
// }
void PULSE_safety_check::StatusProcess(unsigned char chn, uint16 checkItem)		// ����״̬������ؽ��
{
    MsTime Time;
    unsigned char unsafe = 0;

    if(ErrorFlag[chn] != 0)    //if have error unsafe = 1
        unsafe = 1;

    switch(Process_SM_Ptr[chn])
    {
    case SAFE:
        if(unsafe)
        {
            switch (Pulse_UnsafeType[chn])
            {
            case UNSAFE_COUNTER:    //��Ҫ10ms�ȴ��ж�
                CommonFunc::GetTimeTicksUpdate(&safety_last_time[chn]);
                Process_SM_Ptr[chn] = SAFE2UNSAFE;
                Pulse_SafetyCheck[chn].RecordCount++;  //�����ۼ�
                ErrorFlag[chn]=0;
                break;
            case UNSAFE_CYCLE:    //����3��ѭ�����쳣��ֱ�ӱ���
            case UNSAFE_STAGE:    //1��cycle��3��Stage���쳣
                Process_SM_Ptr[chn] =UNSAFE;
                break;
            default:
                break;
            }
        }
        break;
    case SAFE2UNSAFE:
        //�����ж��Ƿ񵽴�10ms������80%�����쳣��
        //�ڴ��ڼ仹�п��ܳ������������쳣
        if((Pulse_UnsafeType[chn]==UNSAFE_CYCLE)||(Pulse_UnsafeType[chn]==UNSAFE_STAGE))
        {
            Schedule_Manager_task::LockModeSwitch[chn]=1;
            Process_SM_Ptr[chn] =UNSAFE;
            break;
        }
        Pulse_SafetyCheck[chn].RecordCount++; //�����ۼ�

// 		OS_ServeProcess::recordCurTime(&safety_error_time[chn]);
// 		Time = TimeT(safety_last_time[chn],safety_error_time[chn]);    //�Ƚ���2��ʱ��  safety_error_time>safety_last_time
        //Time = OS_ServeProcess::TimeDiff(safety_last_time[chn]);
        Time = CommonFunc::TimeDiff(safety_last_time[chn]);
        //if(Time.Us100 >= PULSESAFETY_CHECK_PERIOD)//&&(Pulse_SafetyCheck[chn].UnsafeCount*100/Pulse_SafetyCheck[chn].CheckCount>80))
        if(Time.Us100 >= PULSESAFETY_CHECK_PERIOD)//&&(Pulse_SafetyCheck[chn].UnsafeCount*100/Pulse_SafetyCheck[chn].CheckCount>80))
        {

            if(Pulse_SafetyCheck[chn].UnsafeCount*100/(Pulse_SafetyCheck[chn].RecordCount+Pulse_SafetyCheck[chn].CheckCount)>80)
            {
                Schedule_Manager_task::LockModeSwitch[chn]=1; //����unsafe����ֻ��Schedule����һ�Σ���ô��״̬�л��ˣ�����û�лظ������Ļ�������Ҫ�б�Ǳ�ʾ���ڵ�Unasfe״̬�� 0901 dirui
                Process_SM_Ptr[chn] = UNSAFE;
            }
            else
            {
                Process_SM_Ptr[chn] = SAFE;
                ErrorFlag[chn]=0;
            }
        }
        break;
    case UNSAFE:
        sendError(chn);		// trigger a sending
        Process_SM_Ptr[chn] = UNSAFE2SAFE;
        break;
    case UNSAFE2SAFE:
        Schedule_Manager_task::LockModeSwitch[chn]=0;
        //	Process_SM_Ptr[chn] = SAFE;
        break;

    default:
        Process_SM_Ptr[chn] = SAFE;
    }
}
void PULSE_safety_check::sendError(unsigned char chn)  //���������ش���  dirui 20140819
{   //������������
    //����ErrorFlag ����� // m_btSafe_Code need to be re-explained in Schedule management task
    St_ScheduleMessage sch_msg;
    memset((void *)&sch_msg,0,sizeof(St_ScheduleMessage));
    //		unsigned short temp = 0;
    //		temp = ErrorFlag[chn];
    unsigned long err_flg = ErrorFlag[chn];
    // Packing Msg
    sch_msg.MsgNumber = Sch_OnUnsafe_Msg;    //lock sch
    sch_msg.m_Message.m_Assign.m_u16ChannelIdx = chn;    //ͨ��
    sch_msg.st_status_sch_running.m_btCMD_Source = ST_SAFECHECK;
    sch_msg.st_status_sch_running.m_uw32Safe_Code = err_flg;
//     sch_msg.st_status_sch_running.m_btLost_Unit = 0;
    //important    comment out chen 20130814
    ChannelInfo_Manager::ChannelInfo[chn].m_SafetyStatus = err_flg;
    ChannelInfo_Manager::ChannelInfo[chn].m_UnsafeType = IV_UNSAFE_TYPE;
    //���ݲ�ͬ������������sch�㱨m_btSafe_ClassType  ������ο�    enum MP_SCH_SAFTY_TYPE in SF_Hwf.h
    //����������λ����ƥ�������

// 	if(err_flg & CTRL_VALUE_CHK_UNSAFE)
// 		sch_msg.st_status_sch_running.m_btSubCode = CtrlValueCheck[chn].ErrorCode;
// 	if(err_flg & IRREGULAR_UNSAFE)
// 		sch_msg.st_status_sch_running.m_btSubCode = IrregularCheck[chn].ErrorCode;

    if((err_flg & CURRENT_SYS_UNSAFE) || (err_flg & CURRENT_SCH_UNSAFE))
    {
        sch_msg.st_status_sch_running.m_Value.st_UnsafeMetaValue.f_UnsafeCurrent = I_Record[chn];
    }
    else if((err_flg & VOLTAGE_SYS_UNSAFE) || (err_flg & VOLTAGE_SCH_UNSAFE))
    {
        sch_msg.st_status_sch_running.m_Value.st_UnsafeMetaValue.f_UnsafeVoltage = V_Record[chn];
    }
    else if((err_flg & POWER_SYS_UNSAFE) || (err_flg & POWER_SCH_UNSAFE))
    {
        sch_msg.st_status_sch_running.m_Value.st_UnsafeMetaValue.f_UnsafePower = P_Record[chn];
    }
    else if(ErrorFlag[chn] & SCH_UserDefined_UNSAFE)  //170122
    {
        sch_msg.st_status_sch_running.m_Value.st_UnsafeUD_Value.f_Value = UD_Record[chn];
        int UD_Idx = UD_UnsafeIndex[chn];
        if( (UD_Idx < 0) || (UD_Idx >= MAX_USERDEFINED_SAFETY) )
            UD_Idx = 0;
        St_Safety_UD  *pUDsafety = &ChannelInfo_Manager::ChannelSchedule[chn].m_Settings.m_Safety_UD[UD_Idx];
        sch_msg.st_status_sch_running.m_btSafetyUD_DataType = pUDsafety->m_DataType;
        sch_msg.st_status_sch_running.m_btSafetyUD_MetaCode = pUDsafety->m_MetaCode;
    }
    else if(err_flg & CAPACITY_SCH_UNSAFE)
    {
        sch_msg.st_status_sch_running.m_Value.st_UnsafeMetaValue.f_UnsafeCapacity = C_Record[chn];
    }
    else if(err_flg & STEPTIME_SCH_UNSAFE)
    {
        sch_msg.st_status_sch_running.m_Value.st_UnsafeMetaValue.f_UnsafeStepTime = t_Record[chn];
    }
    else
    {
        ErrorFlag[chn]=0;   //���ڸ����쳣
    }

    // Send to Schedule manager. Here, there might not be schedule running, but msg need to be sent each time safe->unsafe happens.
    Schedule_Manager_task::UnsafeContent[chn]=sch_msg;
    Schedule_Manager_task::SendMsgToSchedule(chn,sch_msg);

//#if REDUNDANT_SAFETY_MODE == 1
//     if(board_config::Feature_Enable_init.Redundant_Safety_Mode==1)
// 	{
// 	    if((PULSE_safety_check::ErrorFlag[chn]&REDUNDANT_SAFETY)!=0)
// 		    RedundantSafetyFlg[chn]=1;
// 	}
//#endif
    //	}
// 	return 1;
}
//char PULSE_safety_check::resumeError(unsigned char chn)
//{
//	//���ͻָ�
//	St_ScheduleMessage sch_msg;
//	sch_msg.m_Message.m_Assign.m_u16ChannelIdx = chn;    //ͨ��
//	//ChannelInfo_Manager::ChannelInfo[chn].m_SafetyStatus = 0;  //Resume��ʱ���������û�б�Ҫ����� Unsafe��ʱ��ֱ�ӿ��Ը��� 0902 dirui
//	sch_msg.MsgNumber = Sch_OnSafe_Msg;    //unlock
//	Schedule_Manager_task::LockModeSwitch[chn]=0; //�����ȫ���  0902
//	memset(&Schedule_Manager_task::UnsafeContent[chn],0,sizeof(St_ScheduleMessage));
//	Schedule_Manager_task::SendMsgToSchedule(chn,sch_msg);

//	return 1;
//}

// MsTime PULSE_safety_check::TimeT(MsTime a,MsTime b)    // a<b
// {///////////////////////////////////////////////////////////////////// Above By Philip ///////////////////////////////////////////////////
// 	//����򵥵ıȽ�ʱ����...
// 	//һ�㶼��ms����ģ�����û��Ҫ�Ƚ�s����
// 	//comment out chen 20130812  need modify    <<<LATER>>>
// 	MsTime tempDiff;
// 	signed short temp;

// 	tempDiff.Second = b.Second - a.Second;
// 	temp = b.Us100 - a.Us100;
// 	if(temp < 0)
// 	{
// 		temp = temp + 10000;
// 		tempDiff.Second --;
// 	}
// 	tempDiff.Us100 = temp;
// 	return tempDiff;
// }
// unsigned char PULSE_safety_check::getSafetyFlag(unsigned char chn)
// {
// 	return ErrorFlag[chn];
// }
char PULSE_safety_check::PULSE_safety_check_sch_is_ready(unsigned char chn)
{
    //��鵱ǰ���Ƿ�������sch�͵���������idle
    //�����sch  �򷵻�1
    //�������������
    //comment out chen 20130219
    //return ChannelInfo_Manager::ChannelInfo[chn].ScheduleStatus;
    //if((ChannelInfo_Manager::ChannelInfo[chn].ScheduleStatus == IDLE)
    //	||(ChannelInfo_Manager::ChannelInfo[chn].ScheduleStatus == NOTREADY))
    if(ChannelInfo_Manager::ChannelInfo[chn].chn_Running_Flag != CHN_LOCK_SCH)
    {
        return SAFETY_CHECK_SYS;
    }
    else    //˵����sch״̬
    {
        return SAFETY_CHECK_SCH;
    }
}
void PULSE_safety_check::BeNormalSafetyPIValueCheck(unsigned char chn,float current,float power)
{
    NormalSafetyValueCheck(chn,CURRENT_SCH_UNSAFE,current);


    NormalSafetyValueCheck(chn,POWER_SCH_UNSAFE,power);

//     float fChargeCapacity = ChannelInfo_Manager::Get_MetaVariable(chn, MP_DATA_TYPE_METAVALUE,DEFAULT_OPERATOR,DEFAULT_COEFFICIENT, MetaCode_PV_ChargeCapacity);
//     float fDischargeCapacity = ChannelInfo_Manager::Get_MetaVariable(chn, MP_DATA_TYPE_METAVALUE,DEFAULT_OPERATOR,DEFAULT_COEFFICIENT, MetaCode_PV_DischargeCapacity);
//     if(fChargeCapacity > fDischargeCapacity)
//         NormalSafetyValueCheck( chn, CAPACITY_SCH_UNSAFE, fChargeCapacity);
//     else
//         NormalSafetyValueCheck(chn, CAPACITY_SCH_UNSAFE, fDischargeCapacity);
	NormalSafetyValueCheck(chn, CAPACITY_SCH_UNSAFE, fabs(safety_check::g_dChargeCapacity[chn] - safety_check::g_dDischargeCapacity[chn]));
    float fStepTime = ChannelInfo_Manager::Get_MetaVariable(chn, MP_DATA_TYPE_METAVALUE,DEFAULT_OPERATOR,DEFAULT_COEFFICIENT, MetaCode_PV_Steptime);
    NormalSafetyValueCheck(chn, STEPTIME_SCH_UNSAFE, fStepTime);
}

