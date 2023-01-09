//AuxSafeCheck
//����IV������
//�� ��ǰaux mcu��map������iv mcu��  ����ѯ
//ÿ��ֻ����һ������
//updata coment out chen 20131211

#include "../Entry/includes.h"
#include "../../COMMON_os/Ext_Devices/LM75B.h"

//UInt16 safety_check::Priority = PRIORITY4;
//UInt16 safety_check::TaskID = SAFETYCHECK_TASK;			// should be a definition which positioning this task in OS_Task[]


UInt16 Aux_Safety_Check::Priority = PRIORITY4;
UInt16 Aux_Safety_Check::TaskID = SAFETYCHECK_TASK;


#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM32",zidata = "SRAM32"		   // ���ñ����洢���ⲿ��չ�洢����	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // ���ñ����洢���ⲿ��չ�洢����	   dynamic memory  0XA0000000  0x03E80000
#endif
unsigned char Aux_Safety_Check::sampleFlag[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU][MAX_AUX_TYPE_PER_MCU];   //������־


ST_AUX_SAFETY_CHECK Aux_Safety_Check::st_aux_safety[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU][MAX_AUX_TYPE_PER_MCU];    //[MAX_AUX_TYPE];

unsigned char Aux_Safety_Check::safety_id[MAX_IVMCU_PER_SYS];
unsigned char Aux_Safety_Check::safety_chn[MAX_IVMCU_PER_SYS];

// ���ñ������������

MsTime Aux_Safety_Check::safety_last_time[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU][MAX_AUX_TYPE_PER_MCU][MAX_AUXCH_PER_AUXMCU];
MsTime Aux_Safety_Check::safety_error_time[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU][MAX_AUX_TYPE_PER_MCU][MAX_AUXCH_PER_AUXMCU];
MsTime Aux_Safety_Check::safety_resume_time[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU][MAX_AUX_TYPE_PER_MCU][MAX_AUXCH_PER_AUXMCU];
ST_AUX_SAFETY_RECORD Aux_Safety_Check::st_check_result[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU][MAX_AUX_TYPE_PER_MCU];
unsigned char Process_SM_Ptr[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU][MAX_AUX_TYPE_PER_MCU][MAX_AUXCH_PER_AUXMCU];
//RedundantVoltage
St_IV_M4ADCChnMap Aux_Safety_Check::st_IV2M4ADCChnMap;
St_SafetyCeck Aux_Safety_Check::st_SafetyCheck[M4_MAX_CHANNELNO];
unsigned char  Aux_Safety_Check::EmergencyStopCheckEnable=0;     //0Ϊ���߱��˹��ܣ� 1Ϊ�߱���ͣDI�����⹦��

ST_OnlineRSM_Safety_RECORD  Aux_Safety_Check::st_OnlineRSM_UnsafeData[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU];
unsigned char Aux_Safety_Check::MonitorSafetyLedState[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU];//����״̬ 0 Nouse,1 moitoring,2protect.
unsigned char Aux_Safety_Check::Led_State[8];
MsTime       Aux_Safety_Check::LED_ProtectTime;
#pragma arm section //��������������


void Aux_Safety_Check::Init(void)
{

    for (unsigned char m = 0; m<MAX_IVMCU_PER_SYS; m++)
    {
        safety_id[m] = 0;    //����iv_chn
        safety_chn[m] = 0;

        for (unsigned char i = 0; i<MAX_IVCH_PER_IVMCU; i++)
        {
            for (unsigned char j = 0; j<MAX_AUX_TYPE_PER_MCU; j++)
            {
                for (unsigned char k = 0; k<MAX_AUXCH_PER_AUXMCU; k++)
                {
                    Process_SM_Ptr[m][i][j][k] = SAFE;
                }
            }
        }
    }
}


//����ʵ��
//1 m_AuxSafetyBank.m_safety_alive����check
//2 ����Щͨ���ڽ���check��������sampleFlag����check
//3 ��m_AuxDataBankȡ��pv��m_AuxSafetyBank�Ƚ�
//4 ��һ��ʱ��������unsafe�������m_AuxSafetyBank ��¼��IVid��IV����
//---------------------------------------------------------------
//������һ������  20131211 chen
//m_AuxSafetyBank�ǰ���IV�ţ�����ͨ������ �ŵ�  Ҳ��˵����ŵ���IV����Ϣ    Aux_AddSafety����    CAN_CMD_DOWNLOAD_SAFETY
//��Aux��������Ȼ�ǰ��ո�����ź�ͨ��������ŵģ�����������Ҫһ��ת��
//����Aux�İ�ȫ��鲻ͬ��iv��ÿ�ΰ���ivͨ�������
//��չ����ĸ�������Ҫ����MAP��IVMCU����顣 ��Ϊ����IVMCUʹ�õĸ���ͨ���᲻һ����

void Aux_Safety_Check::DoWork(void)
{
#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count[COUNT_SAFTY] ++;
    CommonFunc::GetTimeTicksUpdate(&OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
	Time_Count::thread_time_init(COUNT_SAFTY);
#endif

// 	unsigned char checkItem = CheckAndSetCandidate(safety_id);
    unsigned char checkItem;
    unsigned char uc_IVunitBufIndex;
    unsigned char CheckValue;
    for (uc_IVunitBufIndex =0; uc_IVunitBufIndex < MAX_IVMCU_PER_SYS; uc_IVunitBufIndex ++)
    {
        checkItem = CheckAndSetCandidate(safety_id[uc_IVunitBufIndex]);
        if(Aux_DataManager::m_IVMCU_ID_Info[uc_IVunitBufIndex].m_Valid)	//��û��MAP����IVMCU����
        {
            check(uc_IVunitBufIndex,safety_id[uc_IVunitBufIndex],checkItem);			// �����Ӧ��ȫ    checkItemֻ��������IV�����ﲢ��������

            statusProcess(uc_IVunitBufIndex,safety_id[uc_IVunitBufIndex]);		// ����״̬������ؽ��

            safety_id[uc_IVunitBufIndex]++;    //next

            if(safety_id[uc_IVunitBufIndex] > MAX_IVCH_PER_IVMCU)
            {
                safety_id[uc_IVunitBufIndex] = 0;
            }
        }
    }
    if(EmergencyStopCheckEnable==1)
    {
        CheckValue = Aux_ExtCharge_601618_Base::EmergencyStop_Check();
        Aux_ExtCharge_601618_Base::EmergencyStop_Set(CheckValue);
    }
    else if(EmergencyStopCheckEnable==2)
    {
        CheckValue = Aux_Monitor_601618_Base::EmergencyStop_Check();
        Aux_Monitor_601618_Base::EmergencyStop_Set(CheckValue);
    }
    if(Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_abi_globe.m_BoardType==Aux_OnlineRSM_602026)
    {
        for (unsigned char uc_LedIdex =0; uc_LedIdex < 8; uc_LedIdex ++)
        {
            Aux_OnlineRSM_602026_Base::Set_LEDAlarm(Led_State[uc_LedIdex], uc_LedIdex);
        }
    }

    if(Aux_MTC_Base::Aux_MTC_Board == true)  //add by zc for MTC
    {
        for (uc_IVunitBufIndex =0; uc_IVunitBufIndex < MAX_IVMCU_PER_SYS; uc_IVunitBufIndex ++)
        {
            for (int m = 0; m < MAX_IVCH_PER_IVMCU; m++)
            {
                for( int n = 0; n < Aux_DataManager::m_MapFromPC[uc_IVunitBufIndex][m][0].m_Counts_ByScheduleMap; n++)
                {
                    if( Aux_DataManager::m_MapFromPC[uc_IVunitBufIndex][m][0].m_MapInfo[n].m_Index_InAuxUnit == Aux_MTC_Base::index)
                    {
                        if(Aux_MTC_Base::Aux_MTC_TemperatureSafetyCheckFlag[Aux_MTC_Base::index] == 1) {
                            Aux_MTC_Base::TemperatureCheck(Aux_DataManager::m_IVMCU_ID_Info[uc_IVunitBufIndex].m_ID, m, AUX_TEMPERATURE, Aux_MTC_Base::index);
                        }
                        Aux_MTC_Base::FanLockCheck(Aux_DataManager::m_IVMCU_ID_Info[uc_IVunitBufIndex].m_ID, m, AUX_TEMPERATURE, Aux_MTC_Base::index);
                    }
                }
            }
        }
    }

#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count_Time[COUNT_SAFTY] = CommonFunc::TimeDiff(OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
		Time_Count::thread_time(COUNT_SAFTY);
#endif
}

void Aux_Safety_Check::EndTask(void)
{

}


//private
//Aux_DataManager::m_AuxSafetyBank������һ��range��������ά����
unsigned char Aux_Safety_Check::CheckAndSetCandidate(unsigned char uc_IVch)
{
    //if (Aux_DataManager::m_AuxSafetyBank[Bid][chn][0].m_bUnsafe == 0)    //IVMCU������ʵ��ʱ����IVͨ����safety�趨��CAN�´���AuxMCU
    //{
    //	return AUX_NO_CHECK;
    //}
    //else
    //{
    //	if(Aux_DataManager::m_AuxSafetyBank[Bid][chn][0].m_aux_type == AUX_TEMPERATURE)
    //	{
    //		return AUX_T_CHECK;
    //	}
    //	else    //���ε�ѹ
    //	{
    //		return AUX_V_CHECK;
    //	}
    //}


    //ȥ�������ÿ�α�check
    return 1;
}

//Aux_AddSafety (ST_AUX_SAFETY st_safety)  ֻ�ܸ����������ͳһ�Ĳ���
//ÿ�μ��һ��IVMCU�е�һ��IVCH
unsigned char Aux_Safety_Check::check(unsigned char uc_IVunitBufIndex, unsigned char uc_IVch,uint16 checkItem)
{

    for (unsigned char uc_AuxType =0; uc_AuxType < MAX_AUX_TYPE; uc_AuxType++)  //�Ը�IVͨ�����������͵ĸ�������ѭ��
    {
        unsigned char TempAuxTypeIdex = Aux_DataManager::Aux_LocateAuxTypeArrayIndex(uc_AuxType);
        if(TempAuxTypeIdex == 0xFF)//��������
            continue;

        if (uc_AuxType==AUX_SAFETY)  //���������Safety�� ��ִ�����´���
        {
            OnlineRSMSafetyCheck(uc_IVunitBufIndex,uc_IVch);
            OnlineRSMStateScan(uc_IVunitBufIndex, uc_IVch);
            continue;
        }
        if(Aux_DataManager::m_AuxSafetyBank[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_safety_alive[AUX_SAFETY_HIGH_END]&&
                Aux_DataManager::m_AuxSafetyBank[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_safety_alive[AUX_SAFETY_LOW_END])   //��������͵ĸ���ͨ�����˰�ȫ��������
        {
            for (unsigned char i =0; i< Aux_DataManager::m_MapFromPC[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_Counts_ByScheduleMap; i++) //�������͵���ͨ��
                //�ҳ���IV�����͸���������ͨ��������ѯ	,
            {

                ST_AUX_DATA  st_auxdata;
                unsigned char uc_AuxCh = Aux_DataManager::m_MapFromPC[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_MapInfo[i].m_Index_InAuxUnit;
                Aux_DataManager::Aux_GetData(Aux_DataManager::m_IVMCU_ID_Info[uc_IVunitBufIndex].m_ID, uc_IVch, uc_AuxType, uc_AuxCh, &st_auxdata);
                float f_PresentValue = st_auxdata.aux_data;

                //[0] �൱��low   [1]�൱��high
                if( (f_PresentValue >= Aux_DataManager::m_AuxSafetyBank[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_safe_value[AUX_SAFETY_HIGH_END]  )
                        || (f_PresentValue <= Aux_DataManager::m_AuxSafetyBank[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_safe_value[AUX_SAFETY_LOW_END])	)
                {
                    if(uc_AuxCh<=63)
                    {
                        st_check_result[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].NotSafeFlag[0] |= ((U64)(1)<<uc_AuxCh);
                    }
                    else
                    {
                        st_check_result[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].NotSafeFlag[1] |= ((U64)(1)<<(uc_AuxCh-64));
                    }

                    st_check_result[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].f_record[uc_AuxCh] = f_PresentValue;
                    st_aux_safety[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].ErrorFlag = uc_AuxType;    //����IV��˵�������һ��auxͨ������unsafe������unsafe
                    st_aux_safety[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].f_record = f_PresentValue;    //��IV�㱨��ֵ

                }
                else  //���������׼
                {
                    if(i<=63)
                    {
                        st_check_result[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].NotSafeFlag[0] &= ~((U64)(1)<<uc_AuxCh);
                    }
                    else
                    {
                        st_check_result[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].NotSafeFlag[1] &= ~((U64)(1)<<(uc_AuxCh-64));
                    }
                }
            }
        }


    }

    return 1;
}

void Aux_Safety_Check::CleanSafetyCheckResults( unsigned char uc_IVunitBufIndex,unsigned char uc_IVch)
{
    for (unsigned char uc_AuxType =0; uc_AuxType < MAX_AUX_TYPE; uc_AuxType++)
    {
        unsigned char TempAuxTypeIdex = Aux_DataManager::Aux_LocateAuxTypeArrayIndex(uc_AuxType);
        if(TempAuxTypeIdex == 0xFF)//��������
            continue;

        st_check_result[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].NotSafeFlag[0] = 0;
        st_check_result[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].NotSafeFlag[1] = 0;     //����ҲӦ������

        for(unsigned char j = 0; j< Aux_DataManager::m_MapFromPC[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_Counts_ByScheduleMap; j++)
        {
            unsigned char uc_AuxCh =Aux_DataManager:: m_MapFromPC[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_MapInfo[j].m_Index_InAuxUnit;
            Process_SM_Ptr[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex][uc_AuxCh] = SAFE;
        }

    }
}


unsigned char Aux_Safety_Check::statusProcess(unsigned char uc_IVunitBufIndex, unsigned char uc_IVch)
{
    unsigned char unsafe = 0;    //���û��ᵼ�º���ı���  20131218
    MsTime Time;    //����ʱ��Ƚ�

    for(unsigned char uc_AuxType = 0; uc_AuxType < MAX_AUX_TYPE; uc_AuxType ++)
    {
        unsigned char TempAuxTypeIdex = Aux_DataManager::Aux_LocateAuxTypeArrayIndex(uc_AuxType);
        if(TempAuxTypeIdex == 0xFF)//��������
            continue;

        if( (!Aux_DataManager::m_AuxSafetyBank[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_safety_alive[AUX_SAFETY_HIGH_END])  ||  \
                (!Aux_DataManager::m_AuxSafetyBank[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_safety_alive[AUX_SAFETY_LOW_END]) )    //��������͵ĸ���ͨ��û�谲ȫ��������
            continue;
        for(unsigned char i =0; i< Aux_DataManager::m_MapFromPC[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_Counts_ByScheduleMap; i++)    // DKQ 01032014    //1��IVͨ�������128��Aux
        {
            unsigned char uc_AuxCh = Aux_DataManager::m_MapFromPC[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_MapInfo[i].m_Index_InAuxUnit;  //�ҵ��ڸ������ͨ����

            if(uc_AuxCh <64)
            {
                if(	st_check_result[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].NotSafeFlag[0]&((U64)(1)<<uc_AuxCh))
                {
                    unsafe = 1;
                }
                else
                {
                    unsafe = 0;
                }
            }
            else if(uc_AuxCh>=64 )    //else�����ڼӿ������ٶ�
            {
                if(	st_check_result[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].NotSafeFlag[1]&((U64)(1)<<(uc_AuxCh-64)))
                {
                    unsafe = 1;
                }
                else
                {
                    unsafe = 0;
                }
            }

            switch(Process_SM_Ptr[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex][uc_AuxCh])
            {
            case SAFE:
                if (unsafe)
                {
                    //��¼ʱ��
                    CommonFunc::GetTimeTicksUpdate(&safety_last_time[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex][uc_AuxCh]);
                    //st_check_result[uc_IVch][uc_AuxType].m_state[uc_AuxCh] = SAFE2UNSAFE;
                    Process_SM_Ptr[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex][uc_AuxCh] = SAFE2UNSAFE;
                }
                else
                {
                    //do nothing
                }
                break;
            case SAFE2UNSAFE:
                if (unsafe)
                {
                    //�Ƚ�ʱ��
                    CommonFunc::GetTimeTicksUpdate(&safety_error_time[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex][uc_AuxCh]);
                    Time = TimeT(safety_last_time[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex][uc_AuxCh],safety_error_time[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex][uc_AuxCh]);    //�Ƚ���2��ʱ��  safety_error_time>safety_last_time
                    if(Time.Us100 >= SAFETY_CHECK_PERIOD)
                    {
                        sendError(uc_IVunitBufIndex,uc_IVch,uc_AuxType,uc_AuxCh);		// trigger a sending //�����Ҫ�����µ�MAP�޸�
                        //Schedule_Manager_task::LockModeSwitch[chn]=1; //����unsafe����ֻ��Schedule����һ�Σ���ô��״̬�л��ˣ�����û�лظ������Ļ�������Ҫ�б�Ǳ�ʾ���ڵ�Unasfe״̬�� 0901 dirui
                        Process_SM_Ptr[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex][uc_AuxCh] = UNSAFE;

                        break;
                    }
                }
                else
                {
                    Process_SM_Ptr[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex][uc_AuxCh] = SAFE;
                }
                break;
            case UNSAFE:
                if(unsafe)
                {
                    //do nothing
                }
                else
                {
                    //��¼�ָ�ʱ��
                    CommonFunc::GetTimeTicksUpdate(&safety_resume_time[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex][uc_AuxCh]);
                    Process_SM_Ptr[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex][uc_AuxCh] = UNSAFE2SAFE;
                }
                break;
            case UNSAFE2SAFE:
                if (unsafe)
                {
                    Process_SM_Ptr[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex][uc_AuxCh] = UNSAFE;
                }
                else
                {
                    //�Ƚϻָ�ʱ�䣬׼���ָ�����
                    CommonFunc::GetTimeTicksUpdate(&safety_last_time[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex][uc_AuxCh]);
                    Time = TimeT(safety_last_time[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex][uc_AuxCh],safety_resume_time[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex][uc_AuxCh]);    //resume time �Ƚ�
                    //if(Time.Us100 >= SAFETY_CHECK_RESUME)
                    if(Time.Us100+Time.Second*10000 >= SAFETY_CHECK_RESUME)
                    {
                        resumeError(uc_IVunitBufIndex,uc_IVch,uc_AuxType,uc_AuxCh);		//��Ҫ��д�������
                        Process_SM_Ptr[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex][uc_AuxCh] = SAFE;
                        break;
                    }
                }
                break;

            default:
                Process_SM_Ptr[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex][uc_AuxCh] = SAFE;
                break;
            }

        }
    }
    return 1;
}

//����ͻָ�
//ͨ��can parse ��ǡ����IVMCU�ģɣֱ�����֣գΣӣ��ƣ�״̬
//�д���д
unsigned char Aux_Safety_Check::sendError(unsigned char uc_IVunitBufIndex, unsigned char uc_IVch,unsigned char uc_AuxType,unsigned char uc_AuxCh)
{
    unsigned char IVMCU_ID ;
    unsigned char TempAuxTypeIdex = Aux_DataManager::Aux_LocateAuxTypeArrayIndex(uc_AuxType);
    if(TempAuxTypeIdex == 0xFF)//��������
        return 1;
    if(uc_AuxType==AUX_SAFETY)
    {
        IVMCU_ID = Aux_DataManager::m_AuxSafetyBank[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_Source_CANID;
        if((Aux_DataManager::m_AuxDataBank[1][8+uc_AuxCh].updated_aux_data.m_Char & 0x02))
        {
            CANBusParse::Aux_RptUnsafe(IVMCU_ID, uc_IVch,uc_AuxCh,AUX_SAFETY,OnlineRSMVol_UNSAFE,st_OnlineRSM_UnsafeData[uc_IVunitBufIndex][uc_IVch].f_SafetyVoltage[uc_AuxCh]);
        }
        if((Aux_DataManager::m_AuxDataBank[1][8+uc_AuxCh].updated_aux_data.m_Char & 0x04))
        {
            CANBusParse::Aux_RptUnsafe(IVMCU_ID, uc_IVch,uc_AuxCh,AUX_SAFETY,OnlineRSMCur_UNSAFE,st_OnlineRSM_UnsafeData[uc_IVunitBufIndex][uc_IVch].f_SafetyCurrent[uc_AuxCh]);
        }
        if((Aux_DataManager::m_AuxDataBank[1][8+uc_AuxCh].updated_aux_data.m_Char & 0x08))
        {
            CANBusParse::Aux_RptUnsafe(IVMCU_ID, uc_IVch,uc_AuxCh,AUX_SAFETY,OnlineRSMTemp_UNSAFE,st_OnlineRSM_UnsafeData[uc_IVunitBufIndex][uc_IVch].f_SafetyTemperature[uc_AuxCh]);
        }
    }
    else
    {
        IVMCU_ID = Aux_DataManager::m_AuxSafetyBank[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_Source_CANID;
        CANBusParse::Aux_RptUnsafe(IVMCU_ID, uc_IVch,uc_AuxCh,uc_AuxType,AUX_UNSAFE_TOIV,st_check_result[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].f_record[uc_AuxCh]);
    }
    return 1;
}

//��IV����ظ����ӣ��ƣ�״̬
unsigned char Aux_Safety_Check::resumeError(unsigned char uc_IVunitBufIndex,unsigned char uc_IVch,unsigned char uc_AuxType,unsigned char uc_AuxCh)
{
    unsigned char IVMCU_ID;
    unsigned char TempAuxTypeIdex = Aux_DataManager::Aux_LocateAuxTypeArrayIndex(uc_AuxType);
    if(TempAuxTypeIdex == 0xFF)//��������
        return 1;
    IVMCU_ID = Aux_DataManager::m_AuxSafetyBank[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_Source_CANID;
    CANBusParse::Aux_RptUnsafe(IVMCU_ID,uc_IVch,uc_AuxCh,uc_AuxType,AUX_SAFE,0);  //�ظ�����ȫ״̬

    return 1;
}

//����򵥵ıȽ�ʱ����...
//һ�㶼��ms����ģ�����û��Ҫ�Ƚ�s����
//comment out chen 20130812  need modify    <<<LATER>>>
MsTime Aux_Safety_Check::TimeT(MsTime a,MsTime b)    // a<b
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

//RedundantVoltage:
bool Aux_Safety_Check::SetSafetyInfo(const St_Uart_SetSafety *safetyInfo)
{
    unsigned char IVChnIdx = safetyInfo->m_IVChnNo;
    if(IVChnIdx>=M4_MAX_CHANNELNO)
        return false;
    st_IV2M4ADCChnMap.m_IVChnNo[safetyInfo->m_M4ADCChnIdx] = IVChnIdx;  //set map
    st_IV2M4ADCChnMap.m_bValid[safetyInfo->m_M4ADCChnIdx] = 1;
    if( safetyInfo->m_SafetyH>=999 &&  safetyInfo->m_SafetyL<=-999)
        AuxRedundantSafetyCheck::st_RedundantSafetyCheck[IVChnIdx].m_bReleaseCheck = true;
    AuxRedundantSafetyCheck::st_RedundantSafetyCheck[IVChnIdx].m_SafetyH = safetyInfo->m_SafetyH;
    AuxRedundantSafetyCheck::st_RedundantSafetyCheck[IVChnIdx].m_SafetyL = safetyInfo->m_SafetyL;
    AuxRedundantSafetyCheck::st_RedundantSafetyCheck[IVChnIdx].m_SafetyValid = true;
    AuxRedundantSafetyCheck::st_RedundantSafetyCheck[IVChnIdx].m_safeStatus = en_Safe;  //DKQ 03.11.2015
    return true;
    //st_SafetyCheck
}
void Aux_Safety_Check::OnlineRSMSafetyCheck(unsigned char uc_IVunitBufIndex, unsigned char uc_IVch)
{
    unsigned char TempAuxTypeIdex = Aux_DataManager::Aux_LocateAuxTypeArrayIndex(AUX_SAFETY);
    if(TempAuxTypeIdex == 0xFF)//��������
        return;

    if(!Aux_DataManager::m_AuxSafetyBank[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_safety_alive[AUX_SAFETY_HIGH_END]&&
            !Aux_DataManager::m_AuxSafetyBank[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_safety_alive[AUX_SAFETY_LOW_END])
        return;


//���´��봦������ȫ��ļ��
    if(Aux_DataManager::m_AuxSafetyBank[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_safety_alive[AUX_SAFETY_Voltage_HIGH]&&
            Aux_DataManager::m_AuxSafetyBank[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_safety_alive[AUX_SAFETY_Voltage_LOW])   //��������͵ĸ���ͨ�����˰�ȫ��������
    {
        for (unsigned char i =0; i< Aux_DataManager::m_MapFromPC[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_Counts_ByScheduleMap; i++) //�������͵���ͨ��
            //�ҳ���IV�����͸���������ͨ��������ѯ	,
        {
            ST_AUX_DATA  st_auxdata;
            unsigned char uc_AuxCh = Aux_DataManager::m_MapFromPC[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_MapInfo[i].m_Index_InAuxUnit;
            unsigned char uc_ch= uc_AuxCh*2+1;
            st_auxdata =Aux_DataManager::m_AuxDataBank[0][uc_ch];
            float f_PresentValue = st_auxdata.aux_data;
            //  f_PresentValue = 11;
            //[2] �൱��low   [3]�൱��high
            if( (f_PresentValue >= Aux_DataManager::m_AuxSafetyBank[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_safe_value[AUX_SAFETY_Voltage_HIGH]  )
                    || (f_PresentValue <= Aux_DataManager::m_AuxSafetyBank[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_safe_value[AUX_SAFETY_Voltage_LOW])	)
            {
                Aux_DataManager::m_AuxDataBank[1][8+uc_AuxCh].updated_aux_data.m_Char |=(1<<1);

                st_check_result[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].NotSafeFlag[0] |= ((U64)(1)<<uc_AuxCh);
//
                st_OnlineRSM_UnsafeData[uc_IVunitBufIndex][uc_IVch].f_SafetyVoltage[uc_AuxCh]=f_PresentValue;

            }
            else
            {
                Aux_DataManager::m_AuxDataBank[1][8+uc_AuxCh].updated_aux_data.m_Char &=~(1<<1);
            }

        }
    }
    if(Aux_DataManager::m_AuxSafetyBank[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_safety_alive[AUX_SAFETY_Current_HIGH]&&
            Aux_DataManager::m_AuxSafetyBank[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_safety_alive[AUX_SAFETY_Current_LOW])   //��������͵ĸ���ͨ�����˰�ȫ��������
    {
        for (unsigned char i =0; i< Aux_DataManager::m_MapFromPC[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_Counts_ByScheduleMap; i++) //�������͵���ͨ��
            //�ҳ���IV�����͸���������ͨ��������ѯ	,
        {
            ST_AUX_DATA  st_auxdata;
            unsigned char uc_AuxCh = Aux_DataManager::m_MapFromPC[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_MapInfo[i].m_Index_InAuxUnit;
            unsigned char uc_ch= uc_AuxCh*2;
            st_auxdata =Aux_DataManager::m_AuxDataBank[0][uc_ch];
            float f_PresentValue = st_auxdata.aux_data;

            //[2] �൱��low   [3]�൱��high
            if( (f_PresentValue >= Aux_DataManager::m_AuxSafetyBank[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_safe_value[AUX_SAFETY_Current_HIGH]  )
                    || (f_PresentValue <= Aux_DataManager::m_AuxSafetyBank[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_safe_value[AUX_SAFETY_Current_LOW])	)
            {
                Aux_DataManager::m_AuxDataBank[1][8+uc_AuxCh].updated_aux_data.m_Char |=(1<<2);;
                st_check_result[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].NotSafeFlag[0] |= ((U64)(1)<<uc_AuxCh);
                st_OnlineRSM_UnsafeData[uc_IVunitBufIndex][uc_IVch].f_SafetyCurrent[uc_AuxCh]=f_PresentValue;

            }
            else
            {
                Aux_DataManager::m_AuxDataBank[1][8+uc_AuxCh].updated_aux_data.m_Char &=~(1<<2);
            }
        }
    }
    if(Aux_DataManager::m_AuxSafetyBank[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_safety_alive[AUX_SAFETY_Temperature_HIGH]&&
            Aux_DataManager::m_AuxSafetyBank[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_safety_alive[AUX_SAFETY_Temperature_LOW])   //��������͵ĸ���ͨ�����˰�ȫ��������
    {
        for (unsigned char i =0; i< Aux_DataManager::m_MapFromPC[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_Counts_ByScheduleMap; i++) //�������͵���ͨ��
            //�ҳ���IV�����͸���������ͨ��������ѯ	,
        {
            ST_AUX_DATA  st_auxdata;
            unsigned char uc_AuxCh = Aux_DataManager::m_MapFromPC[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_MapInfo[i].m_Index_InAuxUnit;
            unsigned char uc_ch= uc_AuxCh;
            st_auxdata =Aux_DataManager::m_AuxDataBank[1][uc_ch]; //0�ĳ�1,�¶��ڵڶ�group
            float f_PresentValue = st_auxdata.aux_data;

            //[2] �൱��low   [3]�൱��high
            if( (f_PresentValue >= Aux_DataManager::m_AuxSafetyBank[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_safe_value[AUX_SAFETY_Temperature_HIGH]  )
                    || (f_PresentValue <= Aux_DataManager::m_AuxSafetyBank[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_safe_value[AUX_SAFETY_Temperature_LOW])	)
            {
                Aux_DataManager::m_AuxDataBank[1][8+uc_AuxCh].updated_aux_data.m_Char |=(1<<3);;
                st_check_result[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].NotSafeFlag[0] |= ((U64)(1)<<uc_AuxCh);
                st_OnlineRSM_UnsafeData[uc_IVunitBufIndex][uc_IVch].f_SafetyTemperature[uc_AuxCh]=f_PresentValue;

            }
            else
            {
                Aux_DataManager::m_AuxDataBank[1][8+uc_AuxCh].updated_aux_data.m_Char &=~(1<<3);
            }
        }
    }


    for (unsigned char i =0; i< Aux_DataManager::m_MapFromPC[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_Counts_ByScheduleMap; i++)
    {
        unsigned char uc_AuxCh = Aux_DataManager::m_MapFromPC[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_MapInfo[i].m_Index_InAuxUnit;
        if(Aux_DataManager::m_AuxDataBank[1][8+uc_AuxCh].updated_aux_data.m_Char ==0)
            st_check_result[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].NotSafeFlag[0] &= ~((U64)(1)<<uc_AuxCh);
    }

}
//�ж����߼��״̬;δʹ�ã���أ���������״̬
void Aux_Safety_Check::OnlineRSMStateScan(unsigned char uc_IVunitBufIndex, unsigned char uc_IVch)
{
    unsigned char TempAuxTypeIdex = Aux_DataManager::Aux_LocateAuxTypeArrayIndex(AUX_SAFETY);
    if(TempAuxTypeIdex == 0xFF)//��������
        return;

    unsigned char uc_AuxCh = Aux_DataManager::m_MapFromPC[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_MapInfo[0].m_Index_InAuxUnit;
    switch(MonitorSafetyLedState[uc_IVunitBufIndex][uc_IVch])
    {
    case NoUse:
        if(Aux_DataManager::m_AuxSafetyBank[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_safety_alive[AUX_SAFETY_HIGH_END]&&
                Aux_DataManager::m_AuxSafetyBank[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_safety_alive[AUX_SAFETY_LOW_END])
        {
            Led_State[uc_AuxCh]= Monitoring;
            MonitorSafetyLedState[uc_IVunitBufIndex][uc_IVch] = Monitoring;
        }
        break;
    case  Monitoring:
        if((st_check_result[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].NotSafeFlag[0]!=0)&&
                (!Aux_DataManager::m_AuxSafetyBank[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_safety_alive[AUX_SAFETY_HIGH_END]&&
                 !Aux_DataManager::m_AuxSafetyBank[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_safety_alive[AUX_SAFETY_LOW_END]))
        {
            Led_State[uc_AuxCh] = Protect;
            MonitorSafetyLedState[uc_IVunitBufIndex][uc_IVch] = Protect;
            LED_ProtectTime = CommonFunc::GetSystemTime();
        }
        else if(!Aux_DataManager::m_AuxSafetyBank[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_safety_alive[AUX_SAFETY_HIGH_END]&&
                !Aux_DataManager::m_AuxSafetyBank[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_safety_alive[AUX_SAFETY_LOW_END])
        {
            Led_State[uc_AuxCh] = NoUse;
            MonitorSafetyLedState[uc_IVunitBufIndex][uc_IVch] = NoUse;
        }
        break;
    case Protect:
        if((st_check_result[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].NotSafeFlag[0]==0)&&
                (Aux_DataManager::m_AuxSafetyBank[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_safety_alive[AUX_SAFETY_HIGH_END]&&
                 Aux_DataManager::m_AuxSafetyBank[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_safety_alive[AUX_SAFETY_LOW_END]))
        {
            Led_State[uc_AuxCh] = Monitoring;
            MonitorSafetyLedState[uc_IVunitBufIndex][uc_IVch] = Monitoring;
        }
        if(CommonFunc::TimeDiff(LED_ProtectTime).Second>60)
        {
            Led_State[uc_AuxCh] = NoUse;
            MonitorSafetyLedState[uc_IVunitBufIndex][uc_IVch] = NoUse;
        }
        break;

    }

}

