#include "../Entry/includes.h"
#include <math.h>

#define SWAP(a,b) tempr=(a);(a)=(b);(b)=tempr	// 1.15.2018

#define PI 3.14159265758	// 1.15.2018

#define SIGNAL_LEN 256


//public:
unsigned short               Aux_ACIMTest::Priority = PRIORITY4;
unsigned short               Aux_ACIMTest::TaskID = AUX_ACIM_Test_TASK;

// void  (*Aux_WriteDO::m_WriteDO)(void);
// void  (*Aux_WriteDO::m_InitDO)(void);
//private:
#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM32",zidata = "SRAM32"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x03E80000
#endif
bool    Aux_ACIMTest::b_ACIM_StartTest_Flag;
bool    Aux_ACIMTest::b_ACIM_TestFinished_Flag;
bool    Aux_ACIMTest::b_ACIM_DC_StartMea_Flag;
bool    Aux_ACIMTest::b_ACIM_FREQUENCY_High_Flag;  //04/18/2018
bool    Aux_ACIMTest::b_ACIM_FREQUENCY_Low_Flag;  //05/17/2018
bool     Aux_ACIMTest::b_ACIM_OpenCircuit_CheckFlag;
bool		Aux_ACIMTest::b_lastPoint;
unsigned char   Aux_ACIMTest::uc_test_counter;
float           Aux_ACIMTest::sum_Rac;
unsigned char   Aux_ACIMTest::ACIM_Status;
unsigned short  Aux_ACIMTest::TimeTicks;
float           Aux_ACIMTest::ACIM_I_Sum;
unsigned  char  Aux_ACIMTest::ACIM_I_Cnt;
float           Aux_ACIMTest::ACIM_DC_Value=0;
float           Aux_ACIMTest::ACIM_Irms;
float           Aux_ACIMTest::ACIM_Irms_Openload;
float           Aux_ACIMTest::ACIM_Vrms;
float   		Aux_ACIMTest::ACIM_Rac_Avg;
float			Aux_ACIMTest::ACIM_Pac_Avg;
float           Aux_ACIMTest::ACIM_Mac[MAX_ACIM_TEST_NUM];
float			Aux_ACIMTest::ACIM_Pac[MAX_ACIM_TEST_NUM];
float           Aux_ACIMTest::ACIM_Rac[ACIM_TEST_REPETITION];
float           Aux_ACIMTest::ACIM_5160_Value;
float           Aux_ACIMTest::f_Frequency;  //04/10/2018
float           Aux_ACIMTest::ACIM_SineAmplitude_value;
unsigned short   Aux_ACIMTest::Gain_coefficient;
unsigned char     Aux_ACIMTest::uc_ACIM_TEST_Source;
unsigned char     Aux_ACIMTest::uc_ACIM_CALI_OhmRange;
unsigned int	Aux_ACIMTest::Idata[256];
unsigned int	Aux_ACIMTest::Vdata[256];
unsigned int	Aux_ACIMTest::VBKdata[256];  //04/12/2018
unsigned int	Aux_ACIMTest::Vtemp[20];  //03/22/2018
unsigned int	Aux_ACIMTest::ACIMTestNum;

float 			Aux_ACIMTest::pC[];
float 			Aux_ACIMTest::pV[];
float 			Aux_ACIMTest::pVBK[];  //04/12/2018
float 			Aux_ACIMTest::IFFT[];
float 			Aux_ACIMTest::VFFT[];
//float 			Aux_ACIMTest::IFFT_LIST[];
//float 			Aux_ACIMTest::VFFT_LIST[];
float       	Aux_ACIMTest::pVtemp[];  //03/22/2018

float*			Aux_ACIMTest::pCurrent;
float*			Aux_ACIMTest::pVoltage;
float*			Aux_ACIMTest::p_IFFT;
float*			Aux_ACIMTest::p_VFFT;

float			Aux_ACIMTest::DCV;  //03/15/2018
float			Aux_ACIMTest::DCV1;  //03/16/2018
float			Aux_ACIMTest::pVmax;  //03/20/2018
float			Aux_ACIMTest::pVmin;  //03/20/2018
float   		Aux_ACIMTest::Frequency_List[MAX_ACIM_TEST_NUM];
float			Aux_ACIMTest::f_FrequencyH;
float			Aux_ACIMTest::f_FrequencyL;
unsigned int	Aux_ACIMTest::PointPerDecade;
unsigned int	Aux_ACIMTest::TestId;
const float Decade_Index[11] = {0, 10, 3.16227766f, 2.15443469f, 1.77827941f, 1.58489319f, 1.467799f, 1.38949549f, 1.33352143f, 1.29154966f, 1.25892541f};	// Decade_Index[x] = x index root of 10

#pragma arm section //结束外存变量定义 


////////
//    Init(void)
// Input:  N/A
// Return: N/A
///////
void Aux_ACIMTest::Init(void)
{
    b_ACIM_StartTest_Flag = false;
    b_ACIM_TestFinished_Flag = false;
    b_ACIM_DC_StartMea_Flag = false;
    b_ACIM_FREQUENCY_High_Flag = false;  //04/18/2018
    b_ACIM_FREQUENCY_Low_Flag = false;  //05/17/2018
    ACIM_Status =ACIM_IDLE;
    TimeTicks =0;
    ACIM_I_Sum=0;
    ACIM_I_Cnt=0;
    ACIM_Irms=0;
    ACIM_Vrms=0;
    uc_ACIM_TEST_Source =0;
    ACIM_5160_Value = 0.1;
    ACIM_SineAmplitude_value=0.1;
    Gain_coefficient =1;
    uc_test_counter = 0;
    sum_Rac =0;
    f_Frequency = 1000;
    f_FrequencyH = 0;
    f_FrequencyL = 0;
    PointPerDecade = 0;
    for(unsigned char i=0; i<10; i++)
        ACIM_Rac[i] = 0;
    for(unsigned char i=0; i<MAX_ACIM_TEST_NUM; i++)
    {
        ACIM_Mac[i] = 0;
        ACIM_Pac[i] = 0;
    }
    memset(&pC[0],0,sizeof(pC));
    memset(&pV[0],0,sizeof(pV));
    memset(&IFFT[0],0,sizeof(IFFT));
    memset(&VFFT[0],0,sizeof(VFFT));
    memset(&Frequency_List[0],0,MAX_ACIM_TEST_NUM*sizeof(float));

    pCurrent = &pC[0];
    pVoltage = &pV[0];
    p_IFFT = &IFFT[0];
    p_VFFT = &VFFT[0];
}
// The following function is for test only
void Aux_ACIMTest::ReportData(float value, MsTime time, int report_type)
{
    ST_AUX_DATA_TRIG DataTrigInfo;
    St_SDL_Point_Pack * st_Point;
    st_Point =(St_SDL_Point_Pack *) &EthernetParse::m_Point_Temp[0];
    st_Point->u32Second = 0;
    st_Point->u16Us100 = 0;

    st_Point->ucType = SDL_DATA_FOR_LOG; //SDL_DATA_TYPE_LOG;
    DataTrigInfo.m_IVChnIdx = 0;
    DataTrigInfo.m_IV_unitID = 0;
    DataTrigInfo.m_AuxType = AUX_VOLTAGE;
    DataTrigInfo.TrigTime = time;
    switch(report_type)
    {
    case ACIM_CURRENT:
        DataTrigInfo.m_AuxChnIdx  = 0; // ??????????
        break;
    case ACIM_VOLTAGE:
        DataTrigInfo.m_AuxChnIdx  = 1; // ??????????
        break;
    case ACIM_RESISTANCE:
        DataTrigInfo.m_AuxChnIdx  = 2; // ??????????
        break;
    case ACIM_PHASE:
        DataTrigInfo.m_AuxChnIdx  = 3; // ??????????
        break;
    case ACIM_IMPEDANCE:
        DataTrigInfo.m_AuxChnIdx  = 4; // ??????????
        break;
    }

    st_Point->u32Second = DataTrigInfo.TrigTime.Second;
    st_Point->u16Us100 =DataTrigInfo.TrigTime.Us100;

    st_Point->fValue = value;
    st_Point->bIV = 0;
    st_Point->ucValueType =  DataTrigInfo.m_AuxType;

    st_Point->ucChannelIdx = DataTrigInfo.m_AuxChnIdx;

    //st_Aux_Data.m_Chn_Info.m_AuxType = DataTrigInfo->m_AuxType;

    EthernetParse::MPReportData(0,SEND_AUX_LOGDATA);  // PC ??????
}
// The above function is for test only


void Aux_ACIMTest::DoWork(void)
{
#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count[COUNT_ACIM_TEST] ++;
    CommonFunc::GetTimeTicksUpdate(&OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
	Time_Count::thread_time_init(COUNT_ACIM_TEST);
#endif
    if(Aux_DDS_ACIM_ACR_408888_Base::isCurrentSoruce)
				ProcessCurrentSource();
		else if(Aux_DDS_ACIM_ACR_408888_Base::isACIM)
        ProcessACIM();
    else
        ProcessACR();


#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count_Time[COUNT_ACIM_TEST] = CommonFunc::TimeDiff(OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
		Time_Count::thread_time(COUNT_ACIM_TEST);
#endif
}

void Aux_ACIMTest::ProcessACR()
{
    switch(ACIM_Status)
    {
    case ACR_IDLE:
        if(b_ACIM_StartTest_Flag)
        {
            ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ACIM_V);   //Turn on Voltage circuit first, or else cannot measure the base voltage
            ACIM_Status = ACR_WAIT_V_Relay;


            b_ACIM_StartTest_Flag = false;
            b_ACIM_TestFinished_Flag = false;
            b_ACIM_OpenCircuit_CheckFlag =false;
            TimeTicks = 0;
            uc_test_counter = 0;
            sum_Rac =0;
        }
        break;
    case ACR_WAIT_V_Relay:
        TimeTicks++;
        if(TimeTicks>1250)  //0.8*1250=1000ms =1 秒以上
        {
            b_ACIM_DC_StartMea_Flag =true;
            ACIM_I_Sum=0;
            ACIM_I_Cnt=0;
            ACIM_Status =ACR_DCMeasure_Start;
        }
        break;

    case ACR_DCMeasure_Start:
        if(!b_ACIM_DC_StartMea_Flag)
        {
            ACIM_Status = ACR_CalDC_Value;
        }
        break;

    case ACR_CalDC_Value:
        ACIM_DC_Value =ACIM_I_Sum/ACIM_I_Cnt;
        ACIM_I_Sum=0;
        ACIM_I_Cnt=0;
        ACIM_Status =ACR_DAC_Output;
        break;

    case ACR_DAC_Output:
        float DC_Value =ACIM_DC_Value*2; //（due to a 1/2 voltage dividor is used
        unsigned long data1 = Aux_DataManager::CalibrateVoltageDAC(2,0,DC_Value);
        Aux_DDS_ACIM_ACR_408888_Base::Dac_out(0,data1);
        ACIM_Status = ACR_SetResValue;
        break;

    case ACR_SetResValue:
        if(Aux_DDS_ACIM_ACR_408888_Base::Amplitude_Flag==0x01)
        {
            Aux_DDS_ACIM_ACR_408888_Base::AD5160_SPI_Init();
            Aux_DDS_ACIM_ACR_408888_Base::Write_AD5160(ACIM_5160_Value);//11:4.357K
        }
        else
        {
            float value= 0.9985f-ACIM_SineAmplitude_value;
            if(value<=0)
                DC_Value = 0;
            else
                DC_Value =	value/0.0938 ;
            unsigned long data2 = Aux_DataManager::CalibrateVoltageDAC(2,2,DC_Value);
            Aux_DDS_ACIM_ACR_408888_Base::Dac_out(2,data2);
        }
        if(b_ACIM_OpenCircuit_CheckFlag )
        {
            ACIM_Status = ACR_Output_SineWave;
// 								  b_ACIM_OpenCircuit_CheckFlag = true;
        }
        else
            ACIM_Status = ACR_CheckLoad_Open;
        break;
    case ACR_CheckLoad_Open:
        ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_SETS);
        TimeTicks =0;
        ACIM_Status = ACR_WAIT_10MS;
        break;
    case ACR_WAIT_10MS:
        TimeTicks++;
        if(TimeTicks>12)
        {
            ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_I_VS_RMS);  //选电流Rms
            TimeTicks =0;
            ACIM_Status = ACR_WAIT_IRMS_1S;
        }
        break;
    case ACR_WAIT_IRMS_1S:
        TimeTicks++;
        if(TimeTicks>1250)  //0.8*1250=1000ms =1 秒以上
        {
            if(!b_ACIM_OpenCircuit_CheckFlag)
                ACIM_Status = ACR_Read_Irsm_Open;
            else
                ACIM_Status =ACR_Read_Irsm;
        }
        break;
    case ACR_Read_Irsm_Open:
        if(Aux_DDS_ACIM_ACR_408888_Base::Amplitude_Flag==0x01)
            ACIM_Irms_Openload = Aux_DataManager::m_AuxDataBank[0][1].aux_data/10;
        else
            ACIM_Irms_Openload = Aux_DataManager::m_AuxDataBank[0][1].aux_data/20;

        ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ACIM_I);
        TimeTicks =0;
        ACIM_Status = ACR_WAIT;
        break;
    case ACR_WAIT:
        TimeTicks++;
        if(TimeTicks>1250)
        {
            TimeTicks =0;
            ACIM_Status = ACR_Read_Irsm;
        }
        break;
    case ACR_Output_SineWave:
        ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_SETS);
        ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ACIM_I);
        TimeTicks =0;
        ACIM_Status = ACR_WAIT_10MS;
        break;

    case ACR_Read_Irsm:
        if(Aux_DDS_ACIM_ACR_408888_Base::Amplitude_Flag==0x01)
            ACIM_Irms = Aux_DataManager::m_AuxDataBank[0][1].aux_data/10; //(10V -> 1A)
        else
            ACIM_Irms = Aux_DataManager::m_AuxDataBank[0][1].aux_data/20; //(10V -> 0.5A)

        if(!b_ACIM_OpenCircuit_CheckFlag)
        {
            b_ACIM_OpenCircuit_CheckFlag=true;
            if((ACIM_Irms < ACIM_Irms_Openload*0.5) ||(ACIM_Irms > ACIM_Irms_Openload*1.9))
            {
                if(uc_ACIM_TEST_Source == 0xAA)
                {
                    ACIM_Rac[uc_test_counter]= 1000000;

                }
                else
                {
                    ACIM_Rac_Avg =  1000000;
                    EthernetParse::MPReportData(0,SEND_AUX_ACIMDATA);
                }
                ACIM_Status = ACR_STOP_TEST;
                return;
            }
        }
        ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_I_VS_RMS);  //选电压Rms
        ACIM_Status = ACR_Set_Gain;
        Gain_coefficient =1;
        break;

    case ACR_Set_Gain:
        Aux_DDS_ACIM_ACR_408888_Base::Write_AD8253(Gain_coefficient);
        TimeTicks=0;
        ACIM_Status = ACR_WAIT_VRMS_1S;
        break;

    case ACR_WAIT_VRMS_1S:
        TimeTicks++;
        if(TimeTicks>1250*2)  //0.8*1250=1000ms =1 秒以上
        {
            ACIM_Status = ACR_Read_Vrsm;
        }
        break;

    case ACR_Read_Vrsm:
        ACIM_Vrms =Aux_DataManager::m_AuxDataBank[0][1].aux_data;
        if(ACIM_Vrms<0.15)
        {
            if(Gain_coefficient>=1000)
            {
                Gain_coefficient =1000;
                ACIM_Status = ACR_Cal_ACR;
            }
            else
            {
                Gain_coefficient *=10;
                ACIM_Status = ACR_Set_Gain;
            }
        }
        else if(ACIM_Vrms>=1.5)
        {
            if(Gain_coefficient<=1)
            {
                Gain_coefficient=1;
                ACIM_Status = ACR_Cal_ACR;
            }
            else
            {
                Gain_coefficient /=10;
                ACIM_Status = ACR_Set_Gain;
            }
        }
        else
        {
            ACIM_Status = ACR_Cal_ACR;
        }

        break;

    case ACR_Cal_ACR:
        float data = 0;
        if(Aux_DDS_ACIM_ACR_408888_Base::Amplitude_Flag==0x01)
            data = 2*ACIM_Vrms/(Gain_coefficient*ACIM_Irms);  // The AC Voltage is divieded by 10K-10K divider
        else
            //
            //	Load Vac -> 15k-5k divider (0.25) -> 2.55k-10k divider (0.8) -> 8k-3.2k divider(0.28) -> Rectifier -> ADC chip
            //
            //	Load Iac -> 0.5A/10V sample resistor (20) -> 8k-3.2k divider(0.28) -> Rectifier -> ADC chip
            //
            //	// note: ACIM_Irms = Aux_DataManager::m_AuxDataBank[0][1].aux_data/20; //(10V -> 0.5A)
            //
            data = 5*ACIM_Vrms/(Gain_coefficient*ACIM_Irms);

        float	ADC_Gain,ADC_Offset;


        if (uc_ACIM_TEST_Source == 0xAA)
        {
            if ( uc_ACIM_CALI_OhmRange == ACIM_FROM_10_TO_100_mOhm)
            {
                ADC_Gain =  Aux_DataManager::m_AuxDataBank[0][0].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Gain;
                ADC_Offset = Aux_DataManager::m_AuxDataBank[0][0].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Offset;

            }
            else
            {

                ADC_Gain =  Aux_DataManager::m_AuxDataBank[0][1].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Gain;
                ADC_Offset = Aux_DataManager::m_AuxDataBank[0][1].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Offset;

            }

            ACIM_Rac[uc_test_counter] =	ADC_Gain*data + ADC_Offset;

        }

        else
        {
            ADC_Gain =  Aux_DataManager::m_AuxDataBank[0][1].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Gain;
            ADC_Offset = Aux_DataManager::m_AuxDataBank[0][1].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Offset;

            ACIM_Rac[uc_test_counter] =	ADC_Gain*data + ADC_Offset;
            if (ACIM_Rac[uc_test_counter]<0.1)
            {
                ADC_Gain =  Aux_DataManager::m_AuxDataBank[0][0].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Gain;
                ADC_Offset = Aux_DataManager::m_AuxDataBank[0][0].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Offset;
                ACIM_Rac[uc_test_counter] =	ADC_Gain*data + ADC_Offset;


            }

        }

// 			           if(uc_ACIM_TEST_Source ==0xBB)
// 						{
//                           EthernetParse::MPReportData(0,SEND_AUX_ACIMDATA);
//                         }

        ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_SETS);
        Aux_DDS_ACIM_ACR_408888_Base::Write_AD8253(1);   //Add by DKQ to reset AD8253 gain to 1
        if(uc_ACIM_TEST_Source == 0xAA)
        {
            ACIM_Status = ACR_STOP_TEST;
//							uc_test_counter=0;
//                             b_ACIM_TestFinished_Flag = true;
//                            data1 = Aux_DataManager::CalibrateVoltageDAC(2,0,0);   //add by DKQ  to reset DAC output to 0 V
//						    Aux_DDS_ACIM_ACR_408888_Base::Dac_out(0,data1);
//							ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_ACIM_I);
//						    ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_ACIM_V);
        }
        else
        {
            uc_test_counter ++;

            if(uc_test_counter >=ACIM_TEST_REPETITION)
            {   float temp_max=ACIM_Rac[0], temp_min=ACIM_Rac[0];
                int i=0;
                for (i=0; i<ACIM_TEST_REPETITION; i++)
                {
                    if (ACIM_Rac[i] >temp_max )
                        temp_max = ACIM_Rac[i];
                    if (ACIM_Rac[i] <temp_min )
                        temp_min = ACIM_Rac[i];

                }

//							  for ( i=0; i<ACIM_TEST_REPETITION;i++)
//							     {
//								  if (ACIM_Rac[i] ==temp_max )
//								  {
//									  ACIM_Rac[i]=0;
//								      break;
//								  }
//							  }
//								  for ( i=0; i<ACIM_TEST_REPETITION;i++)
//								{  if (ACIM_Rac[i] == temp_min )
//								  {  ACIM_Rac[i]=0;
//								     break;
//								  }
//								 }
                sum_Rac =0;
                for ( i=0; i<ACIM_TEST_REPETITION; i++)
                {
                    sum_Rac +=ACIM_Rac[i];

                }
                ACIM_Rac_Avg = (sum_Rac - temp_max - temp_min) / (ACIM_TEST_REPETITION -2);


// 									if(uc_ACIM_TEST_Source ==0xBB)
// 						      {
                EthernetParse::MPReportData(0,SEND_AUX_ACIMDATA);
//                   }
//						      data1 = Aux_DataManager::CalibrateVoltageDAC(2,0,0);   //add by DKQ  to reset DAC output to 0 V
//						      Aux_DDS_ACIM_ACR_408888_Base::Dac_out(0,data1);
//						      ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_ACIM_I);
//						      ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_ACIM_V);
//
//						      b_ACIM_TestFinished_Flag = true;
//						      ACIM_Status = ACIM_IDLE;
//// 						      ACIM_Rac = sum_Rac/uc_test_counter;
//						      uc_test_counter = 0;
                ACIM_Status = ACR_STOP_TEST;
            }
            else
            {
                TimeTicks =0;
                ACIM_Status = ACR_WAIT_NEXT_TEST;
            }
        }
        break;

    case ACR_WAIT_NEXT_TEST:
        TimeTicks++;
        if(TimeTicks>1250)  //0.8*1250=1000ms =1 秒以上
        {
            // ACIM_Status = ACIM_Read_Vrsm;
            b_ACIM_DC_StartMea_Flag =true;
            ACIM_I_Sum=0;
            ACIM_I_Cnt=0;
            ACIM_Status = ACR_DCMeasure_Start;
            sum_Rac += ACIM_Rac[uc_test_counter-1];
            TimeTicks=0;
        }
        break;
    case ACR_STOP_TEST:
        ACIM_Status = ACR_IDLE;
        uc_test_counter=0;
        b_ACIM_TestFinished_Flag = true;
        ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_SETS);
        Aux_DDS_ACIM_ACR_408888_Base::Write_AD8253(1);   //Add by DKQ to reset AD8253 gain to 1
        data1 = Aux_DataManager::CalibrateVoltageDAC(2,0,0);   //add by DKQ  to reset DAC output to 0 V
        Aux_DDS_ACIM_ACR_408888_Base::Dac_out(0,data1);
        ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_ACIM_I);
        ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_ACIM_V);
        break;

    default:
        break;
    }
}



void Aux_ACIMTest::ProcessACIM()
{
    static int I_id;
    static int V_id;
    unsigned int data;
    unsigned char temp;
    static bool initial_Vdc_measured;
    static bool v_read_gain_already_set;  //03/23/2018
    static bool Sync_Phase;  //04/26/2018

    static int measured_point_id;  //04/25/2018
    //static int n;  //04/25/2018
    static int AC_V_Measure_Interval;  //05/22/2018
    static int FFT_Measure_Interval;  //05/22/2018
    //03/20/2018
    float V_phy_gain =  Aux_DataManager::m_AuxDataBank[0][0].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_Gain;
    float V_phy_offset = Aux_DataManager::m_AuxDataBank[0][0].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_Offset;

//	float f_IVdata = 0;  //05/17/2018
    float f_IVdata;

    switch(ACIM_Status)
    {
    case ACIM_IDLE:

        I_id = 0;
        V_id = 0;
        data = 0;
        temp = 0;
        TestId = 0;
        initial_Vdc_measured = false;
        v_read_gain_already_set = false;
        measured_point_id = 0;

        AC_V_Measure_Interval = 0;  //05/22/2018
        FFT_Measure_Interval = 0;
        f_IVdata = 0;

        ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_ACIM_V);   //04/25/2018
        ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_ACIM_I);

        if(b_ACIM_StartTest_Flag)
        {
            // init testing frequency
            if(PointPerDecade<1)
                PointPerDecade = 1;
            if(PointPerDecade>10)
                PointPerDecade = 10;
            float index = Decade_Index[PointPerDecade];
            float tempf = 0;
            Frequency_List[0] = f_FrequencyH;

            if(f_FrequencyH == f_FrequencyL)
            {
                ACIMTestNum = 1;
            }
            else
            {
                for(TestId=1; TestId<MAX_ACIM_TEST_NUM; TestId++)
                {
                    tempf = Frequency_List[TestId-1]/index;
                    if(tempf/f_FrequencyL<1.01f)
                    {
                        Frequency_List[TestId] = f_FrequencyL;
                        break;
                    }
                    Frequency_List[TestId] = tempf;
                }
                ACIMTestNum = TestId + 1;
            }

            TestId = 0;

            Aux_DDS_ACIM_ACR_408888_Base::Write_AD8253(1);
            unsigned long data1 = Aux_DataManager::CalibrateVoltageDAC(2,0,0);  //03/23/2018
            Aux_DDS_ACIM_ACR_408888_Base::Dac_out(0,data1);
            ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ACIM_V);   //03/16/2018, Turn on ACIM relays
            ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ACIM_I);

            b_ACIM_StartTest_Flag = false;
            b_ACIM_TestFinished_Flag = false;
            b_ACIM_FREQUENCY_High_Flag = false;  //04/18/2018
            b_ACIM_FREQUENCY_Low_Flag = false;  //05/17/2018

            TimeTicks = 0;

            ACIM_Status = ACIM_SET_DDS_FREQ;
        }
        break;

    case ACIM_SET_DDS_FREQ:
    {
        Aux_DDS_ACIM_ACR_408888_Base::DDS_SPI_Init();
        DDS_AD9834::SetFreq(Frequency_List[TestId]);
        f_Frequency = Frequency_List[TestId];
        ACIM_Status = ACIM_ADC_SAMPLE_RATE;  //04/18/2018
    }
    break;

//04/18/2018
    case ACIM_ADC_SAMPLE_RATE:
    {
        unsigned short SPI_Send_Buffer[3];
        Aux_DDS_ACIM_ACR_408888_Base::SPI_SET(1);

        SPI_Send_Buffer[0] = 0x28;
        SPI_Send_Buffer[1] = 0x02;     //Sinc3 Map Diabled , Enhanced filter disabled,    27SPS, 47dB

//05/11/2018

        if(f_Frequency < 0.01)
            f_Frequency = 0.01;

        if(f_Frequency > 10000)
            f_Frequency = 10000;

        if(f_Frequency <= 0.02)
        {
            SPI_Send_Buffer[2] =0x09;//DK 220207// 0x0E;  //filter type: sinc1+sinc5, 100 sps
            AC_V_Measure_Interval = floor(5/f_Frequency);	// 1 cycle
            FFT_Measure_Interval = ceil(300/(256*f_Frequency));
            b_ACIM_FREQUENCY_Low_Flag = true;
        }

        if(f_Frequency > 0.02 && f_Frequency <= 5)
        {
            SPI_Send_Buffer[2] =0x09;//DK 220207// 0x0E;  //filter type: sinc1+sinc5, 100 sps
            AC_V_Measure_Interval = floor(5/f_Frequency);	// 1 cycle
            FFT_Measure_Interval = ceil(600/(256*f_Frequency));
            b_ACIM_FREQUENCY_Low_Flag = true;
        }

        if(f_Frequency > 5 && f_Frequency <= 20)	// 0.5 - 2 cycle
        {
            SPI_Send_Buffer[2] =0x09;//DK 220207// 0x0D;  //filter type: sinc1+sinc5, 200 sps
        }

        if(f_Frequency > 20 && f_Frequency < 100)	// 0.8 - 4 cycle
        {
            SPI_Send_Buffer[2] = 0x09;//DK 220207//0x0B;  //filter type: sinc1+sinc5, 500 sps
        }

        if(f_Frequency >= 100 && f_Frequency < 500)	// 0.8 - 4 cycle
        {
            SPI_Send_Buffer[2] = 0x09;  //filter type: sinc1+sinc5, 2500 sps
        }

        if(f_Frequency >= 500 && f_Frequency <= 2000)	// 0.64 - 2.56 cycle
        {
            SPI_Send_Buffer[2] = 0x06;//0x62;     //filter type: sinc1+sinc5, 15625 sps
        }

        if(f_Frequency > 2000)	// >0.8 cycle
        {
            SPI_Send_Buffer[2] = 0x03;  //50,000 sps
            b_ACIM_FREQUENCY_High_Flag = true;
        }

        ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS1);
        ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS2);
        ARBUS_DELAY(1000);
        SSPIFunc::RxTx_Frames(SPI_Send_Buffer,3,1);    //send data
        ARBUS_DELAY(1000);
        ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS1);
        ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS2);

        Aux_DDS_ACIM_ACR_408888_Base::Write_AD8253(1);

        ACIM_Status = ACIM_WAIT_Relay_On;
        break;
    }

    case ACIM_WAIT_Relay_On:
        TimeTicks++;
        if(TimeTicks>1250)  //0.8*1250=1000ms =1 ??ò?é?
        {
            b_ACIM_DC_StartMea_Flag = true;
            ACIM_I_Sum=0;
            ACIM_I_Cnt=0;
//05/18/2018
            V_id=0;
            Aux_DDS_ACIM_ACR_408888_Base::SPI_SET(1);
            ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CONV);
            ARBUS_DELAY(1000);
            ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CONV);
            if(b_ACIM_FREQUENCY_Low_Flag == true)
                ACIM_Status = ACIM_DCMeasure_Start_L;
            else
                ACIM_Status = ACIM_DCMeasure_Start_H;
        }
        break;

//05/18/2018
    case ACIM_DCMeasure_Start_L:

    {
        Aux_DDS_ACIM_ACR_408888_Base::SPI_SET(1);
        // choose AD7175-2, voltage
        ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS1);
        ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS2);
        ARBUS_DELAY(1000);
        temp=ControlPin::ReadMe(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_DRDY2);

        if(temp == 0)
        {
            data = Adc24bit_AD7175::Read(1);
            Vtemp[V_id] = data;
            V_id++;
        }

        ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS2);

        if(V_id >=20)
        {
            V_id = 0;
            ACIM_Status = ACIM_CalDC_Value;
        }
    }

    break;

    case ACIM_DCMeasure_Start_H:

        Aux_DDS_ACIM_ACR_408888_Base::SPI_SET(1);  //03/15/2018
        ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CONV);
        ARBUS_DELAY(1000);
        ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CONV);

        V_id=0;
        unsigned long IRQ1,IRQ2;
        IRQFunc::DisableIRQ(&IRQ1,&IRQ2);  //03/15/2018

        while(true)
        {
            // choose AD7175-2, voltage
            ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS1);
            ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS2);
            temp=ControlPin::ReadMe(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_DRDY2);

            while(temp)
            {

                temp=ControlPin::ReadMe(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_DRDY2);
            }

            data = Adc24bit_AD7175::Read(1);
            Vtemp[V_id] = data;
            V_id++;
            ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS2);

            if(V_id >=20)
            {
                V_id = 0;
                break;
            }
        }

        IRQFunc::EnableIRQ(IRQ1,IRQ2);
        ACIM_Status = ACIM_CalDC_Value;  //03/15/2018

        break;

    case ACIM_CalDC_Value:

        DCV = 0;  //03/15/2018

        for(int id = 0; id<20; id++)
        {

            pVtemp[id] = V_phy_gain * Vtemp[id]  + V_phy_offset;
            DCV += pVtemp[id];
        }

        ACIM_Status = ACIM_DAC_Output;
        break;

    case ACIM_DAC_Output:
    {
        if(!initial_Vdc_measured)
        {
            DCV1 = (DCV/20.0f-2.5f)*5.0f;  //03/22/2018
            unsigned long data1 = Aux_DataManager::CalibrateVoltageDAC(2,0,DCV1);  //03/15/2018
            Aux_DDS_ACIM_ACR_408888_Base::Dac_out(0,data1);
            ARBUS_DELAY(20000);
            ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_VA0);  //03/16/2018
            ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_VA1);	// AD8253 Gain = 100
            initial_Vdc_measured = true;

            if(b_ACIM_FREQUENCY_Low_Flag == true)
                ACIM_Status = ACIM_DCMeasure_Start_L;
            else
                ACIM_Status = ACIM_DCMeasure_Start_H;
        }
        else
        {
            ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_VA0);  //03/16/2018
            ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_VA1);	// AD8253 Gain = 1

            DCV1 = DCV1+((DCV/20.0-2.5)*5.0)/99.0;  //03/16/2018
            unsigned long data1 = Aux_DataManager::CalibrateVoltageDAC(2,0,DCV1);  //03/15/2018
            Aux_DDS_ACIM_ACR_408888_Base::Dac_out(0,data1);
            ACIM_Status = ACIM_Output_SineWave;
        }

    }
    break;

    case ACIM_Output_SineWave:
        ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_SETS);	// open ADG1419(SB input)

        TimeTicks =0;
        ACIM_Status = ACIM_WAIT;
        break;

    case ACIM_WAIT:         //?ìμ??÷±?o?￡?μ??·μè′y10ms ×óóò
        TimeTicks++;

        if(TimeTicks>11)
        {
            ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_I_VS_RMS);  //??μ??1Rms
            TimeTicks =0;
            Gain_coefficient =1;

            //ACIM_Status = ACIM_Set_Gain;
            ACIM_Status = ACIM_SetResValue;
        }
        break;

    case ACIM_SetResValue:
    {
        //Aux_DDS_ACIM_ACR_408888_Base::AD5160_SPI_Init();
        //Aux_DDS_ACIM_ACR_408888_Base::Write_AD5160(ACIM_5160_Value);//11:4.357K
        //  Aux_DDS_ACIM_ACR_408888_Base::Write_AD5160(1);//11:4.357K
        //  Aux_DDS_ACIM_ACR_408888_Base::Write_AD5160(0.1);//11:4.357K
//06/11/2018
        float Ipp = ACIM_5160_Value*1.414*2;	// RMS to Peak to Peak
        float DACC = (1.13-1.13*Ipp)*9.06;	// DAC, VoutC value = 10.645 - 10.661*(Iac/Irange); Iac = Ipp, Irange = half peak current
        if(DACC <= 0) DACC = 0.0;
        if(DACC >= 10.0) DACC = 10.0;
        unsigned long data1 = Aux_DataManager::CalibrateVoltageDAC(2,2,DACC);
        Aux_DDS_ACIM_ACR_408888_Base::Dac_out(2,data1);
//05/18/2018
        ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_VA0);  //03/20/2018
        ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_VA1);	// AD8253 Gain = 10
        ARBUS_DELAY(20000);
        Aux_DDS_ACIM_ACR_408888_Base::SPI_SET(1);
        ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CONV);
        ARBUS_DELAY(1000);
        ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CONV);

        if(b_ACIM_FREQUENCY_Low_Flag == true)
            ACIM_Status = ACIM_ACMeasure_Start_L;
        else
            ACIM_Status = ACIM_ACMeasure_Start_H;
    }
    break;

    case ACIM_ACMeasure_Start_L:

    {
        Aux_DDS_ACIM_ACR_408888_Base::SPI_SET(1);
        // choose AD7175-2, voltage
        ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS1);
        ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS2);
        ARBUS_DELAY(1000);
        temp=ControlPin::ReadMe(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_DRDY2);

        if(temp == 0)
        {
            data = Adc24bit_AD7175::Read(1);
            measured_point_id++;
            if(measured_point_id >= AC_V_Measure_Interval)
            {
                Vtemp[V_id] = data;
                V_id++;
                measured_point_id = 0;
            }
        }

        ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS2);

        if(V_id >=20)
        {
            V_id = 0;
            measured_point_id = 0;
            ACIM_Status = ACIM_Set_Gain;
        }
    }

    break;

    case ACIM_ACMeasure_Start_H:
        ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_VA0);  //03/20/2018
        ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_VA1);
        ARBUS_DELAY(20000);
        Aux_DDS_ACIM_ACR_408888_Base::SPI_SET(1);
        ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CONV);
        ARBUS_DELAY(1000);
        ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CONV);

        V_id=0;
        IRQFunc::DisableIRQ(&IRQ1,&IRQ2);  //03/15/2018

        while(true)	// stuck 100ms!
        {
            ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS1);
            ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS2);
            temp=ControlPin::ReadMe(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_DRDY2);

            while(temp)
            {
                temp=ControlPin::ReadMe(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_DRDY2);
            }

            data = Adc24bit_AD7175::Read(1);
            Vtemp[V_id] = data;
            V_id++;
            ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS2);

            if(V_id >=20)
            {
                V_id = 0;
                break;
            }

            IRQFunc::EnableIRQ(IRQ1,IRQ2);

            ACIM_Status = ACIM_Set_Gain;
        }
        break;

    case ACIM_Set_Gain:

        if(!v_read_gain_already_set)
        {
            pVmax = 0.5;
            pVmin = 4.5;
            for(int id = 0; id<20; id++)
            {
                pVtemp[id] = V_phy_gain * Vtemp[id]  + V_phy_offset;
                if(pVtemp[id] > pVmax )
                    pVmax = pVtemp[id];
                if(pVtemp[id] < pVmin)
                    pVmin = pVtemp[id];
            }
            pVmax = (pVmax - pVmin)/2.0;
            if(pVmax>=2.0)
                Aux_DDS_ACIM_ACR_408888_Base::Write_AD8253(1);
            else if(pVmax<2.0 && pVmax>=0.2)
                Aux_DDS_ACIM_ACR_408888_Base::Write_AD8253(10);
            else if(pVmax<0.2 && pVmax>=0.02)  //04/02/2018
                Aux_DDS_ACIM_ACR_408888_Base::Write_AD8253(100);
            else
                Aux_DDS_ACIM_ACR_408888_Base::Write_AD8253(1000);

            v_read_gain_already_set = true;
        }
        else
        {
            if(Aux_DDS_ACIM_ACR_408888_Base::AD8253_VRead_Gain==10)
                Aux_DDS_ACIM_ACR_408888_Base::Write_AD8253(1);
            else if(Aux_DDS_ACIM_ACR_408888_Base::AD8253_VRead_Gain==100)
                Aux_DDS_ACIM_ACR_408888_Base::Write_AD8253(10);
            else if(Aux_DDS_ACIM_ACR_408888_Base::AD8253_VRead_Gain==1000)
                Aux_DDS_ACIM_ACR_408888_Base::Write_AD8253(100);
        }

//05/18/2018
        I_id = 0;
        V_id = 0;
        Sync_Phase = false;

        if(b_ACIM_FREQUENCY_High_Flag == true)
            ACIM_Status = ACIM_SAMPLE_SINWAVE_H;
        else if(b_ACIM_FREQUENCY_Low_Flag == true)
            ACIM_Status = ACIM_SAMPLE_SINWAVE_L;
        else
            ACIM_Status = ACIM_SAMPLE_SINWAVE_M;

        break;

//05/18/2018
    case ACIM_SAMPLE_SINWAVE_L:
        if(!Sync_Phase)	//í?2??à??
        {
            ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS1);
            ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS2);
            Aux_DDS_ACIM_ACR_408888_Base::DDS_SPI_Init();
            DDS_AD9834::FSYNC =Aux_DDS_ACIM_ACR_408888_Base::Pin_DDS_CS;
            DDS_AD9834::WriteToAD9834Word(0X0100,1); //02/01/2018
            DDS_AD9834::WriteToAD9834Word(0X2100,1);
            ARBUS_DELAY(2000);
            DDS_AD9834::WriteToAD9834Word(0X2000,1);
            Aux_DDS_ACIM_ACR_408888_Base::SPI_SET(1);

            ARBUS_DELAY(20000);

            ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CONV);
            TIMERSFunc::DELAY_SYS(20, 0);
            ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CONV);

            Sync_Phase = true;
        }

        //Aux_DDS_ACIM_ACR_408888_Base::SPI_SET(1);
        ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS2);
        ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS1);
        ARBUS_DELAY(1000);
        temp=ControlPin::ReadMe(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_DRDY1);

        if(temp == 0)
        {
            data = Adc24bit_AD7175::Read(1);
            measured_point_id++;
            if(measured_point_id >= FFT_Measure_Interval)
            {
                Idata[I_id] = data;
                I_id++;
                ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS1);

                ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS2);
                //Adc24bit_AD7175::ADC_DRDY = Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_DRDY2;

                data = Adc24bit_AD7175::Read(1);
                Vdata[V_id] = data;
                V_id++;
                ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS2);
                measured_point_id = 0;
            }
        }
        ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS1);

        if(V_id >=256)
        {
            I_id = 0;
            V_id = 0;
            Sync_Phase = false;
            ACIM_Status = ACIM_SAMPLE_VBK;
            //ACIM_Status = ACIM_CONV_DATA;  //05/22/2018
        }
        break;

    case ACIM_SAMPLE_VBK:

        if(!Sync_Phase)
        {
            unsigned long data1 = Aux_DataManager::CalibrateVoltageDAC(2,0,0);
            Aux_DDS_ACIM_ACR_408888_Base::Dac_out(0,data1);

            ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_ACIM_V);
            ARBUS_DELAY(20000);

            Aux_DDS_ACIM_ACR_408888_Base::DDS_SPI_Init();
            DDS_AD9834::FSYNC =Aux_DDS_ACIM_ACR_408888_Base::Pin_DDS_CS;
            DDS_AD9834::WriteToAD9834Word(0X0100,1); //02/01/2018
            DDS_AD9834::WriteToAD9834Word(0X2100,1);
            ARBUS_DELAY(2000);
            DDS_AD9834::WriteToAD9834Word(0X2000,1);
            Aux_DDS_ACIM_ACR_408888_Base::SPI_SET(1);

            ARBUS_DELAY(20000);

            ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CONV);
            TIMERSFunc::DELAY_SYS(20, 0);
            ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CONV);

            Sync_Phase = true;
        }
        Aux_DDS_ACIM_ACR_408888_Base::SPI_SET(1);
        ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS1);
        ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS2);
        ARBUS_DELAY(1000);
        temp=ControlPin::ReadMe(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_DRDY2);

        if(temp == 0)
        {
            data = Adc24bit_AD7175::Read(1);
            measured_point_id++;
            if(measured_point_id >= FFT_Measure_Interval)
            {
                VBKdata[V_id] = data;
                V_id++;
                ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS2);
                measured_point_id = 0;
            }
        }

        ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS2);

        if(V_id >=256)
        {
            V_id = 0;
            Sync_Phase = false;
            ACIM_Status = ACIM_CONV_DATA;
        }

        break;

//04/13/2018
    case ACIM_SAMPLE_SINWAVE_M:
    {
        ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS1);
        ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS2);

        unsigned long IRQ1,IRQ2;  //04/13/2018
        IRQFunc::DisableIRQ(&IRQ1,&IRQ2);

        I_id = 0;
        V_id = 0;

        Aux_DDS_ACIM_ACR_408888_Base::DDS_SPI_Init();
        DDS_AD9834::FSYNC =Aux_DDS_ACIM_ACR_408888_Base::Pin_DDS_CS;
        DDS_AD9834::WriteToAD9834Word(0X0100,1); //02/01/2018
        DDS_AD9834::WriteToAD9834Word(0X2100,1);
        ARBUS_DELAY(10000);  //04/19/2018
        DDS_AD9834::WriteToAD9834Word(0X2000,1);
        Aux_DDS_ACIM_ACR_408888_Base::SPI_SET(1);

        ARBUS_DELAY(20000);

        ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CONV);
        TIMERSFunc::DELAY_SYS(20, 0);
        ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CONV);

        while(true)	// stuck here for 16ms
        {
            // choose AD7175-1, current
            ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS2);
            ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS1);
            temp=ControlPin::ReadMe(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_DRDY1);

            while(temp)
            {
                temp=ControlPin::ReadMe(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_DRDY1);
            }

            data = Adc24bit_AD7175::Read(1);
            Idata[I_id] = data;
            I_id++;
            ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS1);

            // choose AD7175-2, voltage
            ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS2);
            //Adc24bit_AD7175::ADC_DRDY = Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_DRDY2;

            data = Adc24bit_AD7175::Read(1);
            Vdata[V_id] = data;
            V_id++;
            ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS2);

            if(I_id >=256)
            {
                I_id = 0;
                V_id = 0;
                break;
            }
        }

//Test, 04/12/2018
        ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_ACIM_V);  //04/27/2018
        unsigned long data1 = Aux_DataManager::CalibrateVoltageDAC(2,0,0);
        Aux_DDS_ACIM_ACR_408888_Base::Dac_out(0,data1);

        ARBUS_DELAY(20000);

        Aux_DDS_ACIM_ACR_408888_Base::DDS_SPI_Init();
        DDS_AD9834::FSYNC =Aux_DDS_ACIM_ACR_408888_Base::Pin_DDS_CS;
        DDS_AD9834::WriteToAD9834Word(0X0100,1); //02/01/2018
        DDS_AD9834::WriteToAD9834Word(0X2100,1);
        ARBUS_DELAY(10000);  //04/19/2018
        DDS_AD9834::WriteToAD9834Word(0X2000,1);
        Aux_DDS_ACIM_ACR_408888_Base::SPI_SET(1);

        ARBUS_DELAY(20000);

        ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CONV);
        TIMERSFunc::DELAY_SYS(20, 0);
        ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CONV);

        while(true)	// stuck here for 16ms
        {
            ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS1);
            ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS2);
            temp=ControlPin::ReadMe(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_DRDY2);

            while(temp)
            {
                temp=ControlPin::ReadMe(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_DRDY2);
            }

            data = Adc24bit_AD7175::Read(1);
            VBKdata[V_id] = data;
            V_id++;
            ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS2);

            if(V_id >=256)
            {
                V_id = 0;
                break;
            }
        }

        IRQFunc::EnableIRQ(IRQ1,IRQ2);

        ACIM_Status = ACIM_CONV_DATA;  //ACIM_GOTO_NORMAL_MODE;
    }
    break;

    case ACIM_SAMPLE_SINWAVE_H:
    {

        ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS1);
        ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS2);  // 12/21/2017

        unsigned long IRQ1,IRQ2;
        IRQFunc::DisableIRQ(&IRQ1,&IRQ2);

        I_id = 0;
        V_id = 0;

//Test, 01/31/2018
        Aux_DDS_ACIM_ACR_408888_Base::DDS_SPI_Init();
        DDS_AD9834::FSYNC =Aux_DDS_ACIM_ACR_408888_Base::Pin_DDS_CS;
        DDS_AD9834::WriteToAD9834Word(0X0100,1); //02/01/2018
        DDS_AD9834::WriteToAD9834Word(0X2100,1);
        ARBUS_DELAY(2000);
        DDS_AD9834::WriteToAD9834Word(0X2000,1);
        Aux_DDS_ACIM_ACR_408888_Base::SPI_SET(1);

        ARBUS_DELAY(20000);

        ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CONV);
        TIMERSFunc::DELAY_SYS(20, 0);
        ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CONV);

        while(true)	// stuck here for 16ms
        {
            ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS1);
            ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS2);
            temp=ControlPin::ReadMe(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_DRDY2);

            while(temp)
            {
                temp=ControlPin::ReadMe(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_DRDY2);
            }

            data = Adc24bit_AD7175::Read(1);
            Vdata[V_id] = data;
            V_id++;
            ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS2);

            if(V_id >=256)
            {
                V_id = 0;
                break;
            }
        }

        //Test, 01/31/2018
        Aux_DDS_ACIM_ACR_408888_Base::DDS_SPI_Init();
        DDS_AD9834::FSYNC =Aux_DDS_ACIM_ACR_408888_Base::Pin_DDS_CS;
        DDS_AD9834::WriteToAD9834Word(0X2100,1);
        ARBUS_DELAY(2000);
        DDS_AD9834::WriteToAD9834Word(0X2000,1);
        Aux_DDS_ACIM_ACR_408888_Base::SPI_SET(1);

        ARBUS_DELAY(20000);

        ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CONV);
        TIMERSFunc::DELAY_SYS(20, 0);
        ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CONV);

        while(true)	// stuck here for 16ms
        {
            ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS2);
            ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS1);
            temp=ControlPin::ReadMe(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_DRDY1);

            while(temp)
            {
                temp=ControlPin::ReadMe(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_DRDY1);
            }

            data = Adc24bit_AD7175::Read(1);
            Idata[I_id] = data;
            I_id++;
            ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS1);

            if(I_id >=256)
            {
                I_id = 0;
                break;
            }
        }

//Test, 04/12/2018
        ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_ACIM_V);  //04/27/2018
        unsigned long data1 = Aux_DataManager::CalibrateVoltageDAC(2,0,0);
        Aux_DDS_ACIM_ACR_408888_Base::Dac_out(0,data1);

        ARBUS_DELAY(20000);

        Aux_DDS_ACIM_ACR_408888_Base::DDS_SPI_Init();
        DDS_AD9834::FSYNC =Aux_DDS_ACIM_ACR_408888_Base::Pin_DDS_CS;
        DDS_AD9834::WriteToAD9834Word(0X0100,1); //02/01/2018
        DDS_AD9834::WriteToAD9834Word(0X2100,1);
        ARBUS_DELAY(2000);
        DDS_AD9834::WriteToAD9834Word(0X2000,1);
        Aux_DDS_ACIM_ACR_408888_Base::SPI_SET(1);

        ARBUS_DELAY(20000);

        ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CONV);
        TIMERSFunc::DELAY_SYS(20, 0);
        ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CONV);

        while(true)	// stuck here for 16ms
        {
            ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS1);
            ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS2);
            temp=ControlPin::ReadMe(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_DRDY2);

            while(temp)
            {
                temp=ControlPin::ReadMe(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_DRDY2);
            }

            data = Adc24bit_AD7175::Read(1);
            VBKdata[V_id] = data;
            V_id++;
            ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS2);

            if(V_id >=256)
            {
                V_id = 0;
                break;
            }
        }

        IRQFunc::EnableIRQ(IRQ1,IRQ2);

        ACIM_Status = ACIM_CONV_DATA;
    }
    break;

    case ACIM_CONV_DATA:
    {
        // Current ADC spec
        float I_phy_gain =  Aux_DataManager::m_AuxDataBank[0][0].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_Gain;
        float I_phy_offset = Aux_DataManager::m_AuxDataBank[0][0].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_Offset;

        // Voltage ADC spec(to be changed to [0][1])
        float V_phy_gain =  Aux_DataManager::m_AuxDataBank[0][0].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_Gain;
        float V_phy_offset = Aux_DataManager::m_AuxDataBank[0][0].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_Offset;

        I_id = 0;
        V_id = 0;
        ACIM_Status = ACIM_CAL_FFT;

        for(int id = 0; id<SIGNAL_LEN; id++)
        {
            if(Idata[id] > CONVERTER_MAX)
                Idata[id] = CONVERTER_MAX;

            f_IVdata = I_phy_gain *Idata[id]  + I_phy_offset;
            pC[id] = f_IVdata;	// save float current value
            //ReportData(data,t,ACIM_CURRENT);	// Current

            f_IVdata = V_phy_gain *Vdata[id]  + V_phy_offset;

//03/23/2018, if capacitor charged by DC current
            if((f_IVdata > 4.7 || f_IVdata < 0.3) && (Aux_DDS_ACIM_ACR_408888_Base::AD8253_VRead_Gain >= 10) )
            {
                unsigned long data1 = Aux_DataManager::CalibrateVoltageDAC(2,0,DCV1);
                Aux_DDS_ACIM_ACR_408888_Base::Dac_out(0,data1);
                ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ACIM_V);  //04/12/2018, 04/17/2018
                ACIM_Status = ACIM_Set_Gain;
                break;
            }

            unsigned long data1 = Aux_DataManager::CalibrateVoltageDAC(2,2,10.0);  //07/23/2018
            Aux_DDS_ACIM_ACR_408888_Base::Dac_out(2,data1);

            f_IVdata /= Aux_DDS_ACIM_ACR_408888_Base::AD8253_VRead_Gain;
            pV[id] = f_IVdata;	// save float voltage value

            //if(b_ACIM_FREQUENCY_Low_Flag == false)
            //{
            f_IVdata = V_phy_gain *VBKdata[id]  + V_phy_offset;  //04/12/2018
            f_IVdata /= Aux_DDS_ACIM_ACR_408888_Base::AD8253_VRead_Gain;
            pVBK[id] = f_IVdata;	// save float back ground voltage value
            pV[id] -= pVBK[id];  // save corrected float voltage value
            //}
            //ReportData(data,t,ACIM_VOLTAGE);	// Voltage
        }
    }
    break;

    case ACIM_CAL_FFT:
    {
        int NUM_ACIREAD = 1;
        bool bOK;
        WORD maxIdx = 0;
        float maxIFFT;
        float ftemp;

        //
        //FFT for current and tage, real time signal
        //

        float fAci_temp = 0;
        float fAciPhase_temp = 0;
        for( int kk=0; kk < NUM_ACIREAD; kk++)
        {   //10.16
            FFT_TwoReal( pCurrent+kk*SIGNAL_LEN, pVoltage+kk*SIGNAL_LEN, p_IFFT, p_VFFT, SIGNAL_LEN); // FFT

            //find the peak of FT that is the sin's:
            maxIFFT = 0;	//p_IFFT[0]*p_IFFT[0] + p_IFFT[1]* p_IFFT[1];
            for ( WORD k = 1; k< SIGNAL_LEN / 2; k++) // skip base frequency(DC value)
            {
                ftemp = p_IFFT[2*k]*p_IFFT[2*k] + p_IFFT[2*k+1]* p_IFFT[2*k+1];
                if (ftemp > maxIFFT )
                {
                    maxIdx = k;
                    maxIFFT = ftemp;
                }
            }

            float temptemp = sqrt((p_VFFT[maxIdx*2]*p_VFFT[maxIdx*2]+p_VFFT[maxIdx*2+1]*p_VFFT[maxIdx*2+1])/maxIFFT );
            fAci_temp += sqrt((p_VFFT[maxIdx*2]*p_VFFT[maxIdx*2]+p_VFFT[maxIdx*2+1]*p_VFFT[maxIdx*2+1])/maxIFFT );
            ftemp = atan2( p_VFFT[maxIdx*2+1], p_VFFT[maxIdx*2] )
                    - atan2( p_IFFT[maxIdx*2+1], p_IFFT[maxIdx*2] );//radian

            if (ftemp > PI) {
                ftemp -= 2*PI;
            }
            if ( ftemp < -PI) {
                ftemp += 2*PI;
            }
            fAciPhase_temp += ftemp;
        }
        fAci_temp /= NUM_ACIREAD;
        fAciPhase_temp /= NUM_ACIREAD; //radian
        ACIM_Pac[TestId] = fAciPhase_temp / PI * 180;// degree
				if(Aux_DataManager::m_AuxDataBank[2][0].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_RangeH>19 && Aux_DataManager::m_AuxDataBank[2][0].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_RangeH<21)
					ACIM_Mac[TestId] = fAci_temp * 20;
				else
					ACIM_Mac[TestId] = fAci_temp * 80;//93.8;

        //AC_R = AC_I*cos(AC_P*PI/180)*1000;
        //printf("**************************************************************************\n");  //10.16
        //printf("\n*    ACI-Phase = %2.2f (degree); ACI = %.4f mOhm; R = %.4f mOhm\n", fAciPhase, fAci*1000, fAci*cos(fAciPhase*PI/180)*1000); //10.16
        //printf("\n**************************************************************************\n\n");  //10.16


        // turn off AC current output
        Aux_DDS_ACIM_ACR_408888_Base::AD5160_SPI_Init();
        Aux_DDS_ACIM_ACR_408888_Base::Write_AD5160(0.1);//11:4.357K, 0 -> 0.1 04/17/2018
        ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_SETS);  //03/16/2018
        ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_ACIM_I);  //04/18/2018, Don't let V-- afect the DUT.
        ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_VA0);  //For test timing, 04/17/2018
        ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_VA1);
        ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ACIM_V);

        ACIM_Status = ACIM_CONFIRM_VALUE;
    }
    break;

    case ACIM_CONFIRM_VALUE:
    {
        TestId++;

        if(TestId<ACIMTestNum)
        {
            ACIM_Status = ACIM_WAIT_NEXT_TEST;
        }
        else
        {
            TestId = 0;
            EthernetParse::MPReportData(0,SEND_AUX_ACIMDATA);
            ACIM_Status = ACIM_STOP_TEST;
        }
    }
    break;

    case ACIM_WAIT_NEXT_TEST:
    {
        initial_Vdc_measured = false;
        v_read_gain_already_set = false;
        measured_point_id = 0;

        Aux_DDS_ACIM_ACR_408888_Base::Write_AD8253(1);
        unsigned long data1 = Aux_DataManager::CalibrateVoltageDAC(2,0,0);  //03/23/2018
        Aux_DDS_ACIM_ACR_408888_Base::Dac_out(0,data1);
        ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ACIM_V);   //03/16/2018, Turn on ACIM relays
        ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ACIM_I);

        b_ACIM_StartTest_Flag = false;
        b_ACIM_TestFinished_Flag = false;
        b_ACIM_FREQUENCY_High_Flag = false;  //04/18/2018
        b_ACIM_FREQUENCY_Low_Flag = false;  //05/17/2018
        ACIM_Status = ACIM_SET_DDS_FREQ;
    }
    break;

    case ACIM_STOP_TEST:
    {
        TimeTicks=0;
        b_ACIM_TestFinished_Flag = true;
        Aux_DDS_ACIM_ACR_408888_Base::Write_AD8253(1);   //Add by DKQ to reset AD8253 gain to 1
        unsigned long data1 = Aux_DataManager::CalibrateVoltageDAC(2,0,0);
        Aux_DDS_ACIM_ACR_408888_Base::Dac_out(0,data1);

        ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_SETS);  //03/16/2018
        ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_ACIM_I);  //04/18/2018, Don't let V-- afect the DUT.
        ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_ACIM_V);

        ACIM_Status = ACIM_DELAY;  //04/17/2018
    }
    break;

    //Test, 04/17/2018
    case ACIM_DELAY:
    {
        TimeTicks++;
        if(TimeTicks>120)
        {
            TimeTicks =0;
            ACIM_Status = ACIM_IDLE;
        }
    }
    break;

    default:

        break;
    }
}

void Aux_ACIMTest::ProcessCurrentSource()
{
	static int I_id;
    static int V_id;
    unsigned int data;
    unsigned char temp;
    static bool initial_Vdc_measured;
    static bool v_read_gain_already_set;  //03/23/2018
    static bool Sync_Phase;  //04/26/2018

    static int measured_point_id;  //04/25/2018
    //static int n;  //04/25/2018
    static int AC_V_Measure_Interval;  //05/22/2018
    static int FFT_Measure_Interval;  //05/22/2018
    //03/20/2018
    float V_phy_gain =  Aux_DataManager::m_AuxDataBank[0][0].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_Gain;
    float V_phy_offset = Aux_DataManager::m_AuxDataBank[0][0].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_Offset;

//	float f_IVdata = 0;  //05/17/2018
    float f_IVdata;

    switch(ACIM_Status)
    {
    case ACIM_IDLE:

        I_id = 0;
        V_id = 0;
        data = 0;
        temp = 0;
        TestId = 0;
        initial_Vdc_measured = false;
        v_read_gain_already_set = false;
        measured_point_id = 0;

        AC_V_Measure_Interval = 0;  //05/22/2018
        FFT_Measure_Interval = 0;
        f_IVdata = 0;

        ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_ACIM_V);   //04/25/2018
        ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_ACIM_I);

        if(b_ACIM_StartTest_Flag)
        {
            // init testing frequency
            if(PointPerDecade<1)
                PointPerDecade = 1;
            if(PointPerDecade>10)
                PointPerDecade = 10;
            float index = Decade_Index[PointPerDecade];
            float tempf = 0;
            Frequency_List[0] = f_FrequencyH;

            if(f_FrequencyH == f_FrequencyL)
            {
                ACIMTestNum = 1;
            }
            else
            {
                for(TestId=1; TestId<MAX_ACIM_TEST_NUM; TestId++)
                {
                    tempf = Frequency_List[TestId-1]/index;
                    if(tempf/f_FrequencyL<1.01f)
                    {
                        Frequency_List[TestId] = f_FrequencyL;
                        break;
                    }
                    Frequency_List[TestId] = tempf;
                }
                ACIMTestNum = TestId + 1;
            }

            TestId = 0;

            Aux_DDS_ACIM_ACR_408888_Base::Write_AD8253(1);
            unsigned long data1 = Aux_DataManager::CalibrateVoltageDAC(2,0,0);  //03/23/2018
            Aux_DDS_ACIM_ACR_408888_Base::Dac_out(0,data1);
            ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ACIM_V);   //03/16/2018, Turn on ACIM relays
            ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ACIM_I);

            b_ACIM_StartTest_Flag = false;
            b_ACIM_TestFinished_Flag = false;
            b_ACIM_FREQUENCY_High_Flag = false;  //04/18/2018
            b_ACIM_FREQUENCY_Low_Flag = false;  //05/17/2018

            TimeTicks = 0;

            ACIM_Status = ACIM_SET_DDS_FREQ;
        }
        break;

    case ACIM_SET_DDS_FREQ:
    {
        Aux_DDS_ACIM_ACR_408888_Base::DDS_SPI_Init();
        DDS_AD9834::SetFreq(Frequency_List[TestId]);
        f_Frequency = Frequency_List[TestId];
        ACIM_Status = ACIM_ADC_SAMPLE_RATE;  //04/18/2018
		//ACIM_Status = ACIM_Output_SineWave;	// 20191212
    }
    break;

//04/18/2018
    case ACIM_ADC_SAMPLE_RATE:
    {
        unsigned short SPI_Send_Buffer[3];
        Aux_DDS_ACIM_ACR_408888_Base::SPI_SET(1);

        SPI_Send_Buffer[0] = 0x28;
        SPI_Send_Buffer[1] = 0x02;     //Sinc3 Map Diabled , Enhanced filter disabled,    27SPS, 47dB

//05/11/2018

        if(f_Frequency < 0.01)
            f_Frequency = 0.01;

        if(f_Frequency > 10000)
            f_Frequency = 10000;

        if(f_Frequency <= 0.02)
        {
            SPI_Send_Buffer[2] = 0x0E;  //filter type: sinc1+sinc5, 100 sps
            AC_V_Measure_Interval = floor(5/f_Frequency);	// 1 cycle
            FFT_Measure_Interval = ceil(300/(256*f_Frequency));
            b_ACIM_FREQUENCY_Low_Flag = true;
        }

        if(f_Frequency > 0.02 && f_Frequency <= 5)
        {
            SPI_Send_Buffer[2] = 0x0E;  //filter type: sinc1+sinc5, 100 sps
            AC_V_Measure_Interval = floor(5/f_Frequency);	// 1 cycle
            FFT_Measure_Interval = ceil(600/(256*f_Frequency));
            b_ACIM_FREQUENCY_Low_Flag = true;
        }

        if(f_Frequency > 5 && f_Frequency <= 20)	// 0.5 - 2 cycle
        {
            SPI_Send_Buffer[2] = 0x0D;  //filter type: sinc1+sinc5, 200 sps
        }

        if(f_Frequency > 20 && f_Frequency < 100)	// 0.8 - 4 cycle
        {
            SPI_Send_Buffer[2] = 0x0B;  //filter type: sinc1+sinc5, 500 sps
        }

        if(f_Frequency >= 100 && f_Frequency < 500)	// 0.8 - 4 cycle
        {
            SPI_Send_Buffer[2] = 0x09;  //filter type: sinc1+sinc5, 2500 sps
        }

        if(f_Frequency >= 500 && f_Frequency <= 2000)	// 0.64 - 2.56 cycle
        {
            SPI_Send_Buffer[2] = 0x06;//0x62;     //filter type: sinc1+sinc5, 15625 sps
        }

        if(f_Frequency > 2000)	// >0.8 cycle
        {
            SPI_Send_Buffer[2] = 0x03;  //50,000 sps
            b_ACIM_FREQUENCY_High_Flag = true;
        }

        ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS1);
        ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS2);
        ARBUS_DELAY(1000);
        SSPIFunc::RxTx_Frames(SPI_Send_Buffer,3,1);    //send data
        ARBUS_DELAY(1000);
        ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS1);
        ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS2);

        Aux_DDS_ACIM_ACR_408888_Base::Write_AD8253(1);

        //ACIM_Status = ACIM_WAIT_Relay_On;
		ACIM_Status = ACIM_Output_SineWave;
        break;
    }
/*
    case ACIM_WAIT_Relay_On:
        TimeTicks++;
        if(TimeTicks>1250)  //0.8*1250=1000ms =1 ??ò?é?
        {
            b_ACIM_DC_StartMea_Flag = true;
            ACIM_I_Sum=0;
            ACIM_I_Cnt=0;
//05/18/2018
            V_id=0;
            Aux_DDS_ACIM_ACR_408888_Base::SPI_SET(1);
            ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CONV);
            ARBUS_DELAY(1000);
            ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CONV);
            if(b_ACIM_FREQUENCY_Low_Flag == true)
                ACIM_Status = ACIM_DCMeasure_Start_L;
            else
                ACIM_Status = ACIM_DCMeasure_Start_H;
        }
        break;

//05/18/2018
    case ACIM_DCMeasure_Start_L:
    {
        Aux_DDS_ACIM_ACR_408888_Base::SPI_SET(1);
        // choose AD7175-2, voltage
        ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS1);
        ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS2);
        ARBUS_DELAY(1000);
        temp=ControlPin::ReadMe(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_DRDY2);

        if(temp == 0)
        {
            data = Adc24bit_AD7175::Read(1);
            Vtemp[V_id] = data;
            V_id++;
        }

        ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS2);

        if(V_id >=20)
        {
            V_id = 0;
            ACIM_Status = ACIM_CalDC_Value;
        }
    }

    break;

    case ACIM_DCMeasure_Start_H:

        Aux_DDS_ACIM_ACR_408888_Base::SPI_SET(1);  //03/15/2018
        ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CONV);
        ARBUS_DELAY(1000);
        ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CONV);

        V_id=0;
        unsigned long IRQ1,IRQ2;
        IRQFunc::DisableIRQ(&IRQ1,&IRQ2);  //03/15/2018

        while(true)
        {
            // choose AD7175-2, voltage
            ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS1);
            ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS2);
            temp=ControlPin::ReadMe(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_DRDY2);

            while(temp)
            {

                temp=ControlPin::ReadMe(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_DRDY2);
            }

            data = Adc24bit_AD7175::Read(1);
            Vtemp[V_id] = data;
            V_id++;
            ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS2);

            if(V_id >=20)
            {
                V_id = 0;
                break;
            }
        }

        IRQFunc::EnableIRQ(IRQ1,IRQ2);
        ACIM_Status = ACIM_CalDC_Value;  //03/15/2018

        break;

    case ACIM_CalDC_Value:

        DCV = 0;  //03/15/2018

        for(int id = 0; id<20; id++)
        {

            pVtemp[id] = V_phy_gain * Vtemp[id]  + V_phy_offset;
            DCV += pVtemp[id];
        }

        ACIM_Status = ACIM_DAC_Output;
        break;

    case ACIM_DAC_Output:
    {
        if(!initial_Vdc_measured)
        {
            DCV1 = (DCV/20.0f-2.5f)*5.0f;  //03/22/2018
            unsigned long data1 = Aux_DataManager::CalibrateVoltageDAC(2,0,DCV1);  //03/15/2018
            Aux_DDS_ACIM_ACR_408888_Base::Dac_out(0,data1);
            ARBUS_DELAY(20000);
            ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_VA0);  //03/16/2018
            ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_VA1);	// AD8253 Gain = 100
            initial_Vdc_measured = true;

            if(b_ACIM_FREQUENCY_Low_Flag == true)
                ACIM_Status = ACIM_DCMeasure_Start_L;
            else
                ACIM_Status = ACIM_DCMeasure_Start_H;
        }
        else
        {
            ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_VA0);  //03/16/2018
            ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_VA1);	// AD8253 Gain = 1

            DCV1 = DCV1+((DCV/20.0-2.5)*5.0)/99.0;  //03/16/2018
            unsigned long data1 = Aux_DataManager::CalibrateVoltageDAC(2,0,DCV1);  //03/15/2018
            Aux_DDS_ACIM_ACR_408888_Base::Dac_out(0,data1);
            ACIM_Status = ACIM_Output_SineWave;
        }

    }
    break;
*/
    case ACIM_Output_SineWave:
        ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_SETS);	// open ADG1419(SB input)

        TimeTicks =0;
        ACIM_Status = ACIM_WAIT;
        break;

    case ACIM_WAIT:         //?ìμ??÷±?o?￡?μ??·μè′y10ms ×óóò
        TimeTicks++;

        if(TimeTicks>11)
        {
            ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_I_VS_RMS);  //??μ??1Rms
            TimeTicks =0;
            Gain_coefficient =1;

            //ACIM_Status = ACIM_Set_Gain;
            ACIM_Status = ACIM_SetResValue;
        }
        break;

    case ACIM_SetResValue:
    {
        //Aux_DDS_ACIM_ACR_408888_Base::AD5160_SPI_Init();
        //Aux_DDS_ACIM_ACR_408888_Base::Write_AD5160(ACIM_5160_Value);//11:4.357K
        //  Aux_DDS_ACIM_ACR_408888_Base::Write_AD5160(1);//11:4.357K
        //  Aux_DDS_ACIM_ACR_408888_Base::Write_AD5160(0.1);//11:4.357K
//06/11/2018
		// 20191212
		if(ACIM_5160_Value>=0.000001f)
		{
            /*		
			float Ipp = ACIM_5160_Value*1.414f*2.0f/33.0f;	// RMS to Peak to Peak (33A pp -> 1A pp)
			float DACC = (1.13-1.13*Ipp)*9.06;	// DAC, VoutC value = 10.645 - 10.661*(Iac/Irange); Iac = Ipp, Irange = half peak current
			if(DACC <= 0) DACC = 0.0;
			if(DACC >= 10.0) DACC = 10.0;
			*/      //comment out by DKQ 03032021 after calibration  
		//	unsigned long data1 = Aux_DataManager::CalibrateVoltageDAC(2,2,DACC);
			unsigned long data1 = Aux_DataManager::CalibrateDAC_for_DDS(2,1,ACIM_5160_Value);  //Add by DKQ 
			Aux_DDS_ACIM_ACR_408888_Base::Dac_out(2,data1);
	//05/18/2018
			ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_VA0);  //03/20/2018
			ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_VA1);	// AD8253 Gain = 10
			ARBUS_DELAY(20000);
			Aux_DDS_ACIM_ACR_408888_Base::SPI_SET(1);
			ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CONV);
			ARBUS_DELAY(1000);
			ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CONV);
			ACIM_Status = ACIM_HANGUP;
		}
		else
		{
			ACIM_Status = ACIM_STOP_TEST;
		}
		TestId = 0;
		Aux_ACIMTest::ACIMTestNum = 1;
		ACIM_Pac[0]=0;
		ACIM_Mac[0]=0;
        EthernetParse::MPReportData(0,SEND_AUX_ACIMDATA);
        
		
//        if(b_ACIM_FREQUENCY_Low_Flag == true)
//            ACIM_Status = ACIM_ACMeasure_Start_L;
//        else
//            ACIM_Status = ACIM_ACMeasure_Start_H;
    }
    break;
	case ACIM_HANGUP:
		if(b_ACIM_StartTest_Flag)
			ACIM_Status = ACIM_IDLE;
		break;

/*
    case ACIM_ACMeasure_Start_L:

    {
        Aux_DDS_ACIM_ACR_408888_Base::SPI_SET(1);
        // choose AD7175-2, voltage
        ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS1);
        ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS2);
        ARBUS_DELAY(1000);
        temp=ControlPin::ReadMe(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_DRDY2);

        if(temp == 0)
        {
            data = Adc24bit_AD7175::Read(1);
            measured_point_id++;
            if(measured_point_id >= AC_V_Measure_Interval)
            {
                Vtemp[V_id] = data;
                V_id++;
                measured_point_id = 0;
            }
        }

        ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS2);

        if(V_id >=20)
        {
            V_id = 0;
            measured_point_id = 0;
            ACIM_Status = ACIM_Set_Gain;
        }
    }

    break;

    case ACIM_ACMeasure_Start_H:
        ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_VA0);  //03/20/2018
        ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_VA1);
        ARBUS_DELAY(20000);
        Aux_DDS_ACIM_ACR_408888_Base::SPI_SET(1);
        ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CONV);
        ARBUS_DELAY(1000);
        ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CONV);

        V_id=0;
        IRQFunc::DisableIRQ(&IRQ1,&IRQ2);  //03/15/2018

        while(true)	// stuck 100ms!
        {
            ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS1);
            ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS2);
            temp=ControlPin::ReadMe(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_DRDY2);

            while(temp)
            {
                temp=ControlPin::ReadMe(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_DRDY2);
            }

            data = Adc24bit_AD7175::Read(1);
            Vtemp[V_id] = data;
            V_id++;
            ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS2);

            if(V_id >=20)
            {
                V_id = 0;
                break;
            }

            IRQFunc::EnableIRQ(IRQ1,IRQ2);

            ACIM_Status = ACIM_Set_Gain;
        }
        break;

    case ACIM_Set_Gain:

        if(!v_read_gain_already_set)
        {
            pVmax = 0.5;
            pVmin = 4.5;
            for(int id = 0; id<20; id++)
            {
                pVtemp[id] = V_phy_gain * Vtemp[id]  + V_phy_offset;
                if(pVtemp[id] > pVmax )
                    pVmax = pVtemp[id];
                if(pVtemp[id] < pVmin)
                    pVmin = pVtemp[id];
            }
            pVmax = (pVmax - pVmin)/2.0;
            if(pVmax>=2.0)
                Aux_DDS_ACIM_ACR_408888_Base::Write_AD8253(1);
            else if(pVmax<2.0 && pVmax>=0.2)
                Aux_DDS_ACIM_ACR_408888_Base::Write_AD8253(10);
            else if(pVmax<0.2 && pVmax>=0.02)  //04/02/2018
                Aux_DDS_ACIM_ACR_408888_Base::Write_AD8253(100);
            else
                Aux_DDS_ACIM_ACR_408888_Base::Write_AD8253(1000);

            v_read_gain_already_set = true;
        }
        else
        {
            if(Aux_DDS_ACIM_ACR_408888_Base::AD8253_VRead_Gain==10)
                Aux_DDS_ACIM_ACR_408888_Base::Write_AD8253(1);
            else if(Aux_DDS_ACIM_ACR_408888_Base::AD8253_VRead_Gain==100)
                Aux_DDS_ACIM_ACR_408888_Base::Write_AD8253(10);
            else if(Aux_DDS_ACIM_ACR_408888_Base::AD8253_VRead_Gain==1000)
                Aux_DDS_ACIM_ACR_408888_Base::Write_AD8253(100);
        }

//05/18/2018
        I_id = 0;
        V_id = 0;
        Sync_Phase = false;

        if(b_ACIM_FREQUENCY_High_Flag == true)
            ACIM_Status = ACIM_SAMPLE_SINWAVE_H;
        else if(b_ACIM_FREQUENCY_Low_Flag == true)
            ACIM_Status = ACIM_SAMPLE_SINWAVE_L;
        else
            ACIM_Status = ACIM_SAMPLE_SINWAVE_M;

        break;

//05/18/2018
    case ACIM_SAMPLE_SINWAVE_L:
        if(!Sync_Phase)	//í?2??à??
        {
            ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS1);
            ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS2);
            Aux_DDS_ACIM_ACR_408888_Base::DDS_SPI_Init();
            DDS_AD9834::FSYNC =Aux_DDS_ACIM_ACR_408888_Base::Pin_DDS_CS;
            DDS_AD9834::WriteToAD9834Word(0X0100,1); //02/01/2018
            DDS_AD9834::WriteToAD9834Word(0X2100,1);
            ARBUS_DELAY(2000);
            DDS_AD9834::WriteToAD9834Word(0X2000,1);
            Aux_DDS_ACIM_ACR_408888_Base::SPI_SET(1);

            ARBUS_DELAY(20000);

            ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CONV);
            TIMERSFunc::DELAY_SYS(20, 0);
            ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CONV);

            Sync_Phase = true;
        }

        //Aux_DDS_ACIM_ACR_408888_Base::SPI_SET(1);
        ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS2);
        ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS1);
        ARBUS_DELAY(1000);
        temp=ControlPin::ReadMe(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_DRDY1);

        if(temp == 0)
        {
            data = Adc24bit_AD7175::Read(1);
            measured_point_id++;
            if(measured_point_id >= FFT_Measure_Interval)
            {
                Idata[I_id] = data;
                I_id++;
                ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS1);

                ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS2);
                //Adc24bit_AD7175::ADC_DRDY = Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_DRDY2;

                data = Adc24bit_AD7175::Read(1);
                Vdata[V_id] = data;
                V_id++;
                ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS2);
                measured_point_id = 0;
            }
        }
        ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS1);

        if(V_id >=256)
        {
            I_id = 0;
            V_id = 0;
            Sync_Phase = false;
            ACIM_Status = ACIM_SAMPLE_VBK;
            //ACIM_Status = ACIM_CONV_DATA;  //05/22/2018
        }
        break;

    case ACIM_SAMPLE_VBK:

        if(!Sync_Phase)
        {
            unsigned long data1 = Aux_DataManager::CalibrateVoltageDAC(2,0,0);
            Aux_DDS_ACIM_ACR_408888_Base::Dac_out(0,data1);

            ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_ACIM_V);
            ARBUS_DELAY(20000);

            Aux_DDS_ACIM_ACR_408888_Base::DDS_SPI_Init();
            DDS_AD9834::FSYNC =Aux_DDS_ACIM_ACR_408888_Base::Pin_DDS_CS;
            DDS_AD9834::WriteToAD9834Word(0X0100,1); //02/01/2018
            DDS_AD9834::WriteToAD9834Word(0X2100,1);
            ARBUS_DELAY(2000);
            DDS_AD9834::WriteToAD9834Word(0X2000,1);
            Aux_DDS_ACIM_ACR_408888_Base::SPI_SET(1);

            ARBUS_DELAY(20000);

            ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CONV);
            TIMERSFunc::DELAY_SYS(20, 0);
            ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CONV);

            Sync_Phase = true;
        }
        Aux_DDS_ACIM_ACR_408888_Base::SPI_SET(1);
        ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS1);
        ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS2);
        ARBUS_DELAY(1000);
        temp=ControlPin::ReadMe(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_DRDY2);

        if(temp == 0)
        {
            data = Adc24bit_AD7175::Read(1);
            measured_point_id++;
            if(measured_point_id >= FFT_Measure_Interval)
            {
                VBKdata[V_id] = data;
                V_id++;
                ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS2);
                measured_point_id = 0;
            }
        }

        ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS2);

        if(V_id >=256)
        {
            V_id = 0;
            Sync_Phase = false;
            ACIM_Status = ACIM_CONV_DATA;
        }

        break;

//04/13/2018
    case ACIM_SAMPLE_SINWAVE_M:
    {
        ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS1);
        ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS2);

        unsigned long IRQ1,IRQ2;  //04/13/2018
        IRQFunc::DisableIRQ(&IRQ1,&IRQ2);

        I_id = 0;
        V_id = 0;

        Aux_DDS_ACIM_ACR_408888_Base::DDS_SPI_Init();
        DDS_AD9834::FSYNC =Aux_DDS_ACIM_ACR_408888_Base::Pin_DDS_CS;
        DDS_AD9834::WriteToAD9834Word(0X0100,1); //02/01/2018
        DDS_AD9834::WriteToAD9834Word(0X2100,1);
        ARBUS_DELAY(10000);  //04/19/2018
        DDS_AD9834::WriteToAD9834Word(0X2000,1);
        Aux_DDS_ACIM_ACR_408888_Base::SPI_SET(1);

        ARBUS_DELAY(20000);

        ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CONV);
        TIMERSFunc::DELAY_SYS(20, 0);
        ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CONV);

        while(true)	// stuck here for 16ms
        {
            // choose AD7175-1, current
            ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS2);
            ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS1);
            temp=ControlPin::ReadMe(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_DRDY1);

            while(temp)
            {
                temp=ControlPin::ReadMe(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_DRDY1);
            }

            data = Adc24bit_AD7175::Read(1);
            Idata[I_id] = data;
            I_id++;
            ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS1);

            // choose AD7175-2, voltage
            ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS2);
            //Adc24bit_AD7175::ADC_DRDY = Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_DRDY2;

            data = Adc24bit_AD7175::Read(1);
            Vdata[V_id] = data;
            V_id++;
            ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS2);

            if(I_id >=256)
            {
                I_id = 0;
                V_id = 0;
                break;
            }
        }

//Test, 04/12/2018
        ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_ACIM_V);  //04/27/2018
        unsigned long data1 = Aux_DataManager::CalibrateVoltageDAC(2,0,0);
        Aux_DDS_ACIM_ACR_408888_Base::Dac_out(0,data1);

        ARBUS_DELAY(20000);

        Aux_DDS_ACIM_ACR_408888_Base::DDS_SPI_Init();
        DDS_AD9834::FSYNC =Aux_DDS_ACIM_ACR_408888_Base::Pin_DDS_CS;
        DDS_AD9834::WriteToAD9834Word(0X0100,1); //02/01/2018
        DDS_AD9834::WriteToAD9834Word(0X2100,1);
        ARBUS_DELAY(10000);  //04/19/2018
        DDS_AD9834::WriteToAD9834Word(0X2000,1);
        Aux_DDS_ACIM_ACR_408888_Base::SPI_SET(1);

        ARBUS_DELAY(20000);

        ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CONV);
        TIMERSFunc::DELAY_SYS(20, 0);
        ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CONV);

        while(true)	// stuck here for 16ms
        {
            ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS1);
            ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS2);
            temp=ControlPin::ReadMe(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_DRDY2);

            while(temp)
            {
                temp=ControlPin::ReadMe(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_DRDY2);
            }

            data = Adc24bit_AD7175::Read(1);
            VBKdata[V_id] = data;
            V_id++;
            ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS2);

            if(V_id >=256)
            {
                V_id = 0;
                break;
            }
        }

        IRQFunc::EnableIRQ(IRQ1,IRQ2);

        ACIM_Status = ACIM_CONV_DATA;  //ACIM_GOTO_NORMAL_MODE;
    }
    break;

    case ACIM_SAMPLE_SINWAVE_H:
    {

        ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS1);
        ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS2);  // 12/21/2017

        unsigned long IRQ1,IRQ2;
        IRQFunc::DisableIRQ(&IRQ1,&IRQ2);

        I_id = 0;
        V_id = 0;

//Test, 01/31/2018
        Aux_DDS_ACIM_ACR_408888_Base::DDS_SPI_Init();
        DDS_AD9834::FSYNC =Aux_DDS_ACIM_ACR_408888_Base::Pin_DDS_CS;
        DDS_AD9834::WriteToAD9834Word(0X0100,1); //02/01/2018
        DDS_AD9834::WriteToAD9834Word(0X2100,1);
        ARBUS_DELAY(2000);
        DDS_AD9834::WriteToAD9834Word(0X2000,1);
        Aux_DDS_ACIM_ACR_408888_Base::SPI_SET(1);

        ARBUS_DELAY(20000);

        ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CONV);
        TIMERSFunc::DELAY_SYS(20, 0);
        ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CONV);

        while(true)	// stuck here for 16ms
        {
            ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS1);
            ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS2);
            temp=ControlPin::ReadMe(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_DRDY2);

            while(temp)
            {
                temp=ControlPin::ReadMe(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_DRDY2);
            }

            data = Adc24bit_AD7175::Read(1);
            Vdata[V_id] = data;
            V_id++;
            ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS2);

            if(V_id >=256)
            {
                V_id = 0;
                break;
            }
        }

        //Test, 01/31/2018
        Aux_DDS_ACIM_ACR_408888_Base::DDS_SPI_Init();
        DDS_AD9834::FSYNC =Aux_DDS_ACIM_ACR_408888_Base::Pin_DDS_CS;
        DDS_AD9834::WriteToAD9834Word(0X2100,1);
        ARBUS_DELAY(2000);
        DDS_AD9834::WriteToAD9834Word(0X2000,1);
        Aux_DDS_ACIM_ACR_408888_Base::SPI_SET(1);

        ARBUS_DELAY(20000);

        ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CONV);
        TIMERSFunc::DELAY_SYS(20, 0);
        ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CONV);

        while(true)	// stuck here for 16ms
        {
            ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS2);
            ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS1);
            temp=ControlPin::ReadMe(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_DRDY1);

            while(temp)
            {
                temp=ControlPin::ReadMe(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_DRDY1);
            }

            data = Adc24bit_AD7175::Read(1);
            Idata[I_id] = data;
            I_id++;
            ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS1);

            if(I_id >=256)
            {
                I_id = 0;
                break;
            }
        }

//Test, 04/12/2018
        ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_ACIM_V);  //04/27/2018
        unsigned long data1 = Aux_DataManager::CalibrateVoltageDAC(2,0,0);
        Aux_DDS_ACIM_ACR_408888_Base::Dac_out(0,data1);

        ARBUS_DELAY(20000);

        Aux_DDS_ACIM_ACR_408888_Base::DDS_SPI_Init();
        DDS_AD9834::FSYNC =Aux_DDS_ACIM_ACR_408888_Base::Pin_DDS_CS;
        DDS_AD9834::WriteToAD9834Word(0X0100,1); //02/01/2018
        DDS_AD9834::WriteToAD9834Word(0X2100,1);
        ARBUS_DELAY(2000);
        DDS_AD9834::WriteToAD9834Word(0X2000,1);
        Aux_DDS_ACIM_ACR_408888_Base::SPI_SET(1);

        ARBUS_DELAY(20000);

        ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CONV);
        TIMERSFunc::DELAY_SYS(20, 0);
        ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CONV);

        while(true)	// stuck here for 16ms
        {
            ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS1);
            ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS2);
            temp=ControlPin::ReadMe(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_DRDY2);

            while(temp)
            {
                temp=ControlPin::ReadMe(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_DRDY2);
            }

            data = Adc24bit_AD7175::Read(1);
            VBKdata[V_id] = data;
            V_id++;
            ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS2);

            if(V_id >=256)
            {
                V_id = 0;
                break;
            }
        }

        IRQFunc::EnableIRQ(IRQ1,IRQ2);

        ACIM_Status = ACIM_CONV_DATA;
    }
    break;

    case ACIM_CONV_DATA:
    {
        // Current ADC spec
        float I_phy_gain =  Aux_DataManager::m_AuxDataBank[0][0].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_Gain;
        float I_phy_offset = Aux_DataManager::m_AuxDataBank[0][0].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_Offset;

        // Voltage ADC spec(to be changed to [0][1])
        float V_phy_gain =  Aux_DataManager::m_AuxDataBank[0][0].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_Gain;
        float V_phy_offset = Aux_DataManager::m_AuxDataBank[0][0].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_Offset;

        I_id = 0;
        V_id = 0;
        ACIM_Status = ACIM_CAL_FFT;

        for(int id = 0; id<SIGNAL_LEN; id++)
        {
            if(Idata[id] > CONVERTER_MAX)
                Idata[id] = CONVERTER_MAX;

            f_IVdata = I_phy_gain *Idata[id]  + I_phy_offset;
            pC[id] = f_IVdata;	// save float current value
            //ReportData(data,t,ACIM_CURRENT);	// Current

            f_IVdata = V_phy_gain *Vdata[id]  + V_phy_offset;

//03/23/2018, if capacitor charged by DC current
            if((f_IVdata > 4.7 || f_IVdata < 0.3) && (Aux_DDS_ACIM_ACR_408888_Base::AD8253_VRead_Gain >= 10) )
            {
                unsigned long data1 = Aux_DataManager::CalibrateVoltageDAC(2,0,DCV1);
                Aux_DDS_ACIM_ACR_408888_Base::Dac_out(0,data1);
                ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ACIM_V);  //04/12/2018, 04/17/2018
                ACIM_Status = ACIM_Set_Gain;
                break;
            }

            unsigned long data1 = Aux_DataManager::CalibrateVoltageDAC(2,2,10.0);  //07/23/2018
            Aux_DDS_ACIM_ACR_408888_Base::Dac_out(2,data1);

            f_IVdata /= Aux_DDS_ACIM_ACR_408888_Base::AD8253_VRead_Gain;
            pV[id] = f_IVdata;	// save float voltage value

            //if(b_ACIM_FREQUENCY_Low_Flag == false)
            //{
            f_IVdata = V_phy_gain *VBKdata[id]  + V_phy_offset;  //04/12/2018
            f_IVdata /= Aux_DDS_ACIM_ACR_408888_Base::AD8253_VRead_Gain;
            pVBK[id] = f_IVdata;	// save float back ground voltage value
            pV[id] -= pVBK[id];  // save corrected float voltage value
            //}
            //ReportData(data,t,ACIM_VOLTAGE);	// Voltage
        }
    }
    break;

    case ACIM_CAL_FFT:
    {
        int NUM_ACIREAD = 1;
        bool bOK;
        WORD maxIdx = 0;
        float maxIFFT;
        float ftemp;

        //
        //FFT for current and tage, real time signal
        //

        float fAci_temp = 0;
        float fAciPhase_temp = 0;
        for( int kk=0; kk < NUM_ACIREAD; kk++)
        {   //10.16
            FFT_TwoReal( pCurrent+kk*SIGNAL_LEN, pVoltage+kk*SIGNAL_LEN, p_IFFT, p_VFFT, SIGNAL_LEN); // FFT

            //find the peak of FT that is the sin's:
            maxIFFT = 0;	//p_IFFT[0]*p_IFFT[0] + p_IFFT[1]* p_IFFT[1];
            for ( WORD k = 1; k< SIGNAL_LEN / 2; k++) // skip base frequency(DC value)
            {
                ftemp = p_IFFT[2*k]*p_IFFT[2*k] + p_IFFT[2*k+1]* p_IFFT[2*k+1];
                if (ftemp > maxIFFT )
                {
                    maxIdx = k;
                    maxIFFT = ftemp;
                }
            }

            float temptemp = sqrt((p_VFFT[maxIdx*2]*p_VFFT[maxIdx*2]+p_VFFT[maxIdx*2+1]*p_VFFT[maxIdx*2+1])/maxIFFT );
            fAci_temp += sqrt((p_VFFT[maxIdx*2]*p_VFFT[maxIdx*2]+p_VFFT[maxIdx*2+1]*p_VFFT[maxIdx*2+1])/maxIFFT );
            ftemp = atan2( p_VFFT[maxIdx*2+1], p_VFFT[maxIdx*2] )
                    - atan2( p_IFFT[maxIdx*2+1], p_IFFT[maxIdx*2] );//radian

            if (ftemp > PI) {
                ftemp -= 2*PI;
            }
            if ( ftemp < -PI) {
                ftemp += 2*PI;
            }
            fAciPhase_temp += ftemp;
        }
        fAci_temp /= NUM_ACIREAD;
        fAciPhase_temp /= NUM_ACIREAD; //radian
        ACIM_Pac[TestId] = fAciPhase_temp / PI * 180;// degree
        ACIM_Mac[TestId] = fAci_temp * 80;//93.8;

        //AC_R = AC_I*cos(AC_P*PI/180)*1000;
        //printf("**************************************************************************\n");  //10.16
        //printf("\n*    ACI-Phase = %2.2f (degree); ACI = %.4f mOhm; R = %.4f mOhm\n", fAciPhase, fAci*1000, fAci*cos(fAciPhase*PI/180)*1000); //10.16
        //printf("\n**************************************************************************\n\n");  //10.16


        // turn off AC current output
        Aux_DDS_ACIM_ACR_408888_Base::AD5160_SPI_Init();
        Aux_DDS_ACIM_ACR_408888_Base::Write_AD5160(0.1);//11:4.357K, 0 -> 0.1 04/17/2018
        ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_SETS);  //03/16/2018
        ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_ACIM_I);  //04/18/2018, Don't let V-- afect the DUT.
        ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_VA0);  //For test timing, 04/17/2018
        ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_VA1);
        ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ACIM_V);

        ACIM_Status = ACIM_CONFIRM_VALUE;
    }
    break;

    case ACIM_CONFIRM_VALUE:
    {
        TestId++;

        if(TestId<ACIMTestNum)
        {
            ACIM_Status = ACIM_WAIT_NEXT_TEST;
        }
        else
        {
            TestId = 0;
            EthernetParse::MPReportData(0,SEND_AUX_ACIMDATA);
            ACIM_Status = ACIM_STOP_TEST;
        }
    }
    break;

    case ACIM_WAIT_NEXT_TEST:
    {
        initial_Vdc_measured = false;
        v_read_gain_already_set = false;
        measured_point_id = 0;

        Aux_DDS_ACIM_ACR_408888_Base::Write_AD8253(1);
        unsigned long data1 = Aux_DataManager::CalibrateVoltageDAC(2,0,0);  //03/23/2018
        Aux_DDS_ACIM_ACR_408888_Base::Dac_out(0,data1);
        ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ACIM_V);   //03/16/2018, Turn on ACIM relays
        ControlPin::SetMeHigh(Aux_DDS_ACIM_ACR_408888_Base::Pin_ACIM_I);

        b_ACIM_StartTest_Flag = false;
        b_ACIM_TestFinished_Flag = false;
        b_ACIM_FREQUENCY_High_Flag = false;  //04/18/2018
        b_ACIM_FREQUENCY_Low_Flag = false;  //05/17/2018
        ACIM_Status = ACIM_SET_DDS_FREQ;
    }
    break;
*/
    case ACIM_STOP_TEST:
    {
        TimeTicks=0;
        b_ACIM_TestFinished_Flag = true;
        Aux_DDS_ACIM_ACR_408888_Base::Write_AD8253(1);   //Add by DKQ to reset AD8253 gain to 1
        unsigned long data1 = Aux_DataManager::CalibrateVoltageDAC(2,0,0);
        Aux_DDS_ACIM_ACR_408888_Base::Dac_out(0,data1);
		data1 = Aux_DataManager::CalibrateVoltageDAC(2,2,10.0f);
		Aux_DDS_ACIM_ACR_408888_Base::Dac_out(2,data1);

        ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_SETS);  //03/16/2018
        ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_ACIM_I);  //04/18/2018, Don't let V-- afect the DUT.
        ControlPin::SetMeLow(Aux_DDS_ACIM_ACR_408888_Base::Pin_ACIM_V);

        ACIM_Status = ACIM_DELAY;  //04/17/2018
    }
    break;

    //Test, 04/17/2018
    case ACIM_DELAY:
    {
        TimeTicks++;
        if(TimeTicks>120)
        {
            TimeTicks =0;
            ACIM_Status = ACIM_IDLE;
        }
    }
    break;

    default:

        break;
    }
}

void Aux_ACIMTest::Set_ACIM_Status( unsigned char status)
{
    ACIM_Status = status;

}

void Aux_ACIMTest::FFT_RealSamp( float* data, unsigned short nn ) //KL ACI FFT 06.27 09.04
{
    unsigned short n,mmax,m,j,istep,i;
    float wtemp,wr,wpr,wpi,wi,theta;
    float tempr,tempi;

    n=nn << 1;
    j=0;

    // SHUFFLING:
    for (i=0; i<n; i+=2) {
        if (j > i) {
            SWAP(data[j],data[i]);
            SWAP(data[j+1],data[i+1]);
        }
        m=nn;
        while (m >= 2 && j >= m) {
            j -= m;
            m >>= 1;
        }
        j += m;
    }
    //check: CORRECT

    mmax=2;
    while (n > mmax) {
        istep=mmax << 1;
        theta = - 6.28318530717959/mmax; // 2Pi/2, 2Pi/4,2Pi/8
        wtemp=sin(0.5*theta);  // sin(2Pi/4), sin(2Pi/8), sin(2Pi/16)
        wpr = -2.0*wtemp*wtemp;// -2*sin(2Pi/4)*sin(2Pi/4), -2*sin(2Pi/8)*sin(2Pi/8), -2*sin(2Pi/16)*sin(2Pi/16)
        wpi=sin(theta);        // -2*sin(2Pi/4)*cos(2Pi/4), -2*sin(2Pi/8)*cos(2Pi/8), -2*sin(2Pi/16)*cos(2Pi/16)
        wr=1.0;
        wi=0.0;
        for (m=1; m<mmax; m+=2) { //mmax-pt FFT //mmax=2;  m=1        //mmax=4;  m=1,3       //mmax=8;   m=1,3,5,7

            for (i=m; i<n; i+=istep) {          //istep=4; i=1,5,9,13 //istep=8; i=1,9;3,11  //istep=16; i=1;3;5;7
                j=i+mmax; //the butterfly wing  //j=3,7,11,15         //j=5,9; 7,15          //j=9; 11; 13; 15
                tempr=wr*data[j-1]-wi*data[j];
                tempi=wr*data[j]+wi*data[j-1];
                data[j-1] = data[i-1] - tempr;
                data[j]   = data[i]   - tempi;
                data[i-1] += tempr;
                data[i]   += tempi;

            }
            wr=(wtemp=wr)*wpr-wi*wpi+wr;
            wi=wi*wpr+wtemp*wpi+wi;
        }
        mmax=istep;
    }

    for (i=0; i<nn*2; i++) {
        data[i] /= float(nn);
    }
    return;
}


void Aux_ACIMTest::FFT_TwoReal(float* data1, float* data2,
                               float * fft1, float* fft2,
                               unsigned short n) //KL ACI FFT 06.27 09.04
{
    //FFT_RealSamp( float* data, unsigned short nn );
    unsigned short nn3,nn2,jj,j;
    float R1,I1,R2,I2;

    for (j=0,jj=1; j<n; j++,jj+=2) {
        fft1[jj-1] = data1[j];
        fft1[jj]   = data2[j];
    }

    FFT_RealSamp(fft1, n);  //correct. based on fft butterfly algorithm

    fft2[0]=fft1[1];
    fft1[1]=fft2[1]=0.0;

    nn3=(nn2 = n<<1)+1;

    for (j=2; j<=n; j+=2) {  //2,4,6,8,...n

        R1=0.5*(fft1[j]   + fft1[nn2-j]); //REAL{XR}  (Re{X(k)}+Re{X*(N-k)})/2, k=j/2 ||(0&2n),  2&2n-2, 4&2n-4
        I1=0.5*(fft1[j+1] - fft1[nn3-j]); //IMAG{XR}  (Im{X(k)}-Im{X*(N-k)})/2, k=j/2 ||(1&2n-1), 3&2n-3

        R2=0.5*(fft1[j+1] + fft1[nn3-j]); //REAL{XI}
        I2=0.5*(fft1[j]   - fft1[nn2-j]); //IMAG{XI}

        fft1[j]     = R1;
        fft1[j+1]   = I1;
        fft1[nn2-j] = R1;
        fft1[nn3-j] = -I1;

        fft2[j]     = R2;
        fft2[j+1]   = -I2;
        fft2[nn2-j] = R2;
        fft2[nn3-j] = I2;
    }

    return;
}

