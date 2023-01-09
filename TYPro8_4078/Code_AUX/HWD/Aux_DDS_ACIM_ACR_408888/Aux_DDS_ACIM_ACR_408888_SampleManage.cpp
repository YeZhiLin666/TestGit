
#include "includes.h"




//for 接口函数
char Aux_DDS_ACIM_ACR_408888_SampleManage::m_chn;
unsigned long int Aux_DDS_ACIM_ACR_408888_SampleManage::m_value;
char Aux_DDS_ACIM_ACR_408888_SampleManage::m_type;


char Aux_DDS_ACIM_ACR_408888_SampleManage::m_ADC_chn;
char Aux_DDS_ACIM_ACR_408888_SampleManage::chn_history;
char Aux_DDS_ACIM_ACR_408888_SampleManage::ADC_Status;

unsigned short Aux_DDS_ACIM_ACR_408888_SampleManage::TimeTicks;
unsigned long  Aux_DDS_ACIM_ACR_408888_SampleManage::Sampledata[3][4];

unsigned long Aux_DDS_ACIM_ACR_408888_SampleManage::SampleDelayCounter;
unsigned long Aux_DDS_ACIM_ACR_408888_SampleManage::WaitForSampleCounter;
unsigned char Aux_DDS_ACIM_ACR_408888_SampleManage::SampleCnt;
const unsigned short Aux_DDS_ACIM_ACR_408888_SampleManage::DelayTickCount_ADG = 7;   //50*7=350us

unsigned long Aux_DDS_ACIM_ACR_408888_SampleManage::ResetADC=0;
unsigned long Aux_DDS_ACIM_ACR_408888_SampleManage::ResetADC_Counter=0;

void Aux_DDS_ACIM_ACR_408888_SampleManage::Init(void)
{
    m_chn = 0;
    m_value = 0;
    m_type = 0;

    m_ADC_chn = 0;
    SampleCnt =0;

    chn_history = 0;
    //状态机
    ADC_Status = ADC_IDLE;
    SampleDelayCounter=0;
    for(unsigned char i=0; i<3; i++)
    {
        for(unsigned char j=0; j<4; j++)
            Sampledata[i][j] = 0;
    }
}

char Aux_DDS_ACIM_ACR_408888_SampleManage::SampleISR(void)
{
    unsigned char temp;
    bool Triggerflag=0;
    unsigned long data;

//     switch(ADC_Status)
//   	{
// 	      case ADC_IDLE:
// 		         ADC_Status = ADC_AssignIV;
// 		         m_ADC_chn = 0;
// 		         TimeTicks=0;
// 			       Adc24bit_AD7175::ADC_CS= AUX_Unit_Base::Pin_ECODE2;
// 			       ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_CS);
//              Adc24bit_AD7175::ADC_CS= AUX_Unit_Base::Pin_ECODE3;
// 			       ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_CS);
//              Adc24bit_AD7175::ADC_START = Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CONV;
// 	           ControlPin::SetMeLow(Adc24bit_AD7175::ADC_START);
// 		         break;
// 	      case ADC_AssignIV:
// 		         Adc24bit_AD7175::ADC_CS= AUX_Unit_Base::Pin_ECODE2;
// 	           ControlPin::SetMeLow(Adc24bit_AD7175::ADC_CS);
// 	           Adc24bit_AD7175::Trigger(1);
// 	           Adc24bit_AD7175::ADC_CS= AUX_Unit_Base::Pin_ECODE3;
// 	           ControlPin::SetMeLow(Adc24bit_AD7175::ADC_CS);
// 	           Adc24bit_AD7175::Trigger(1);
// 	           TimeTicks=0;
// 	           ADC_Status = ADC_TriggerIV;
// 	           break;
// 	      case ADC_TriggerIV:
// 		         ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_START);
// 	           TIMERSFunc::DELAY_SYS(20, 0);
// 	           ControlPin::SetMeLow(Adc24bit_AD7175::ADC_START);
// 	           ADC_Status =ADC_WAIT;
// 		         break;
// 	      case  ADC_WAIT:
// 		         if(!Triggerflag)
// 				     {
//                   Adc24bit_AD7175::ADC_CS= AUX_Unit_Base::Pin_ECODE2;
// 			            ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_CS);
//                   Adc24bit_AD7175::ADC_CS= AUX_Unit_Base::Pin_ECODE3;
// 			            ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_CS);
// 					        Triggerflag =1;
//              }
// 		         TimeTicks++;
// 	           if(TimeTicks >= DelayTickCount_ADG)
//              {
// 					        ControlPin::SetMeLow(Adc24bit_AD7175::ADC_CS);
//                   ADC_Status = ADC_ReadI;
//                   TimeTicks = 0;
//              }
// 				     break;
// 	      case ADC_ReadI:
// 	           Adc24bit_AD7175::ADC_DRDY= Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_DRDY1;
// 				     temp=ControlPin::ReadMe(Adc24bit_AD7175::ADC_DRDY);
// 				     if(temp==0)
// 				     {
// 	              data= Adc24bit_AD7175::Read(1);
// 	              if(data!=0)
// 							  {
// 									   Aux_DataManager::m_AuxDataBank[0][m_ADC_chn ].aux_instant_code = data; //将数据放到Aux_DataBank 中
// 	                   Aux_DataManager::m_AuxDataBank[0][m_ADC_chn].updated = true;
// 							  }
// 						    ADC_Status = ADC_ReadV;
// 						    ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_CS);
//                 Adc24bit_AD7175::ADC_CS= AUX_Unit_Base::Pin_ECODE3;
// 			          ControlPin::SetMeLow(Adc24bit_AD7175::ADC_CS);
// 					  }
// 				    break;
// 	      case ADC_ReadV:
// 	           Adc24bit_AD7175::ADC_DRDY= Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_DRDY2;
// 				     temp=ControlPin::ReadMe(Adc24bit_AD7175::ADC_DRDY);
// 				     if(temp==0)
// 				     {
// 	                data= Adc24bit_AD7175::Read(1);
// 	                if(data!=0)
// 							    {
// 										  Aux_DataManager::m_AuxDataBank[1][m_ADC_chn ].aux_instant_code = data; //将数据放到Aux_DataBank 中
// 	                    Aux_DataManager::m_AuxDataBank[1][m_ADC_chn].updated = true;
// 							    }
// 					        m_ADC_chn++;
// 	                if(m_ADC_chn>3)
// 				          {
// 								       m_ADC_chn =0;
// 								       ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_CS);
// 								       Adc24bit_AD7175::ADC_CS= AUX_Unit_Base::Pin_ECODE3;
// 				               ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_CS);
// 								       ADC_Status = ADC_AssignIV;
// 							    }
// 							    else
// 							    {
// 								       Adc24bit_AD7175::ADC_CS= AUX_Unit_Base::Pin_ECODE2;
// 			                 ControlPin::SetMeLow(Adc24bit_AD7175::ADC_CS);
// 								       Triggerflag =0;
// 							         ADC_Status = ADC_TriggerIV;
//                    }
// 					    }
// 				break;
// 	case ADC_NoWork:
// 			  break;
// 	default:
// 		ADC_Status = ADC_IDLE;
// 		break;
// 	}
// 	return 0;    //error
    switch(ADC_Status)
    {
    case ADC_IDLE:
// 		         m_ADC_chn = 1;
        Adc24bit_AD7175::ADC_CS= AUX_Unit_Base::Pin_ECODE2;
        ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_CS);
        Adc24bit_AD7175::ADC_START = Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CONV;
        ControlPin::SetMeLow(Adc24bit_AD7175::ADC_START);
        ADC_Status = ADC_START;
    case ADC_START:
        Adc24bit_AD7175::ADC_CS= AUX_Unit_Base::Pin_ECODE2;
        ControlPin::SetMeLow(Adc24bit_AD7175::ADC_CS);
        Adc24bit_AD7175::Trigger(1);
        ADC_Status = ADC_Trigger;
        break;
    case ADC_Trigger:
        ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_START);
        TIMERSFunc::DELAY_SYS(20, 0);
        ControlPin::SetMeLow(Adc24bit_AD7175::ADC_START);
        // ADC_Status =ADC_Read;
        ADC_Status =ADC_WAIT;
        break;
    case  ADC_WAIT:
// 						if(!Triggerflag)
// 				     {
//                  Adc24bit_AD7175::ADC_CS= AUX_Unit_Base::Pin_ECODE2;
// 			           ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_CS);
// 					       Triggerflag =1;
//              }
// 		         TimeTicks++;
// 	          if(TimeTicks >= 1)
//             {
// 					        ControlPin::SetMeLow(Adc24bit_AD7175::ADC_CS);
        ADC_Status = ADC_Read;
//                   TimeTicks = 0;
//              }
        break;
    case ADC_Read:
        Adc24bit_AD7175::ADC_DRDY= Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_DRDY1;
        temp=ControlPin::ReadMe(Adc24bit_AD7175::ADC_DRDY);
        if(temp==0)
        {
            ResetADC = 0;
            data= Adc24bit_AD7175::Read(1);
            if(data!=0)
            {
                Sampledata[m_ADC_chn][SampleCnt] = data;  //将数据放到缓冲区
// 									       Aux_DataManager::m_AuxDataBank[0][m_ADC_chn].aux_instant_code =data;  //将数据放到缓冲区
// 									       Aux_DataManager::m_AuxDataBank[0][m_ADC_chn].updated = true;
            }
            m_ADC_chn++;
            if(m_ADC_chn>2)
            {
                m_ADC_chn =0;
                ADC_Status = ADC_IDLE;
                SampleCnt++;
                if(SampleCnt>3)
                {
                    SampleCnt=0;
                    for(unsigned char i=0; i<3; i++)
                    {
                        unsigned long dataSum =0;
                        for(unsigned char j=0; j<4; j++)
                        {
                            dataSum +=Sampledata[i][j];
                            Aux_DataManager::m_AuxDataBank[0][i].aux_instant_code = (dataSum/4); //将数据放到Aux_DataBank 中
                            Aux_DataManager::m_AuxDataBank[0][i].updated = true;
                        }

                    }
                }
            }
            else
            {
                ADC_Status = ADC_Trigger;
                Triggerflag	 =0;
            }
        }
        else
        {
            if(ResetADC++>=0xFFFF)
            {
                Adc24bit_AD7175::Reset2(1);
                Aux_DDS_ACIM_ACR_408888_Base::ADC_Init();
                Aux_DDS_ACIM_ACR_408888_SampleManage::Init();
                ResetADC = 0;
                ResetADC_Counter ++;
            }
        }

        break;
    default:
        ADC_Status = ADC_IDLE;
        break;
    }
    return 0;    //error
}



// MsTime  Aux_DDS_ACIM_ACR_408888_SampleManage::TimeT(MsTime a,MsTime b)    // a<b
// {
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
