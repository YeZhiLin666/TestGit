///////////////////////////////////////////////////////////////////////////////////
//File Name: AUX_BASE.cpp
//Version:		V1.0
//Date:				2013/11/18
//Description:
//						����������ز���
//Others:      dirui
//History��

///////////////////////////////////////////////////////////////////////////////////
#include "../Entry/includes.h"

// SSPI_Para BuiltIn_16T_409920_Base::BuiltIn_16V_409920_SSP0ADC;
SSPI_Para BuiltIn_16T_409920_Base::BuiltIn_16T_409920_SSP0ADC;

unsigned long BuiltIn_16T_409920_Base::RestADC;

const St_Port BuiltIn_16T_409920_Base::Pin_START      = {1,        20,        eCPTYPE_MCUGPIO,   GPIO_O,             0};
const St_Port BuiltIn_16T_409920_Base::Pin_DRDY_M     = {1,        25,        eCPTYPE_MCUGPIO,   GPIO_O,             0};
const St_Port BuiltIn_16T_409920_Base::Pin_DRDY_S     = {1,        26,        eCPTYPE_MCUGPIO,   GPIO_O,             0};
// const St_Port BuiltIn_16T_409920_Base::Pin_START_S     = {1,        22,        eCPTYPE_MCUGPIO,   GPIO_O,             0};

void BuiltIn_16T_409920_Base::IO_Init(void)
{
    //There is no speacial port for this board
    ControlPin::SetIObyCP(BuiltIn_16T_409920_Base::Pin_START);
// 	  ControlPin::SetIObyCP(BuiltIn_16T_409920_Base::Pin_DRDY_M);
// 	  ControlPin::SetIObyCP(BuiltIn_16T_409920_Base::Pin_DRDY_S);

}
void BuiltIn_16T_409920_Base::SPI_SET(void)
{
// 	BuiltIn_16V_409920_SSP0ADC.uc_Module_Name = SSP0_DEVICE0;
// 	BuiltIn_16V_409920_SSP0ADC.uc_Speed = 7;
// 	BuiltIn_16V_409920_SSP0ADC.uc_CPOL = 1;
// 	BuiltIn_16V_409920_SSP0ADC.uc_CPHA = 1;
// 	BuiltIn_16V_409920_SSP0ADC.uc_MSTR = 1;    //master
// 	BuiltIn_16V_409920_SSP0ADC.uc_BITS = 8;
// 	BuiltIn_16V_409920_SSP0ADC.uc_LSBF = 0;   //AD7175
// 	SSPIFunc::setPara(&BuiltIn_16T_409920_Base::BuiltIn_16V_409920_SSP0ADC,0);
// 	SSPIFunc::my_Init(0);


    BuiltIn_16T_409920_SSP0ADC.uc_Module_Name = SSP0_DEVICE4;
    BuiltIn_16T_409920_SSP0ADC.uc_Speed = 7;     //2
    BuiltIn_16T_409920_SSP0ADC.uc_CPOL = 0;
    BuiltIn_16T_409920_SSP0ADC.uc_CPHA = 0;
    BuiltIn_16T_409920_SSP0ADC.uc_MSTR = 1;    //Master
    BuiltIn_16T_409920_SSP0ADC.uc_BITS = 8;
    BuiltIn_16T_409920_SSP0ADC.uc_LSBF = 0;   //LTC2498

    SSPIFunc::setPara(&BuiltIn_16T_409920_Base::BuiltIn_16T_409920_SSP0ADC,0);
    SSPIFunc::my_Init(0);

}
void BuiltIn_16T_409920_Base::ADC_Init(unsigned char u_CSid)
{
    Adc24bit_AD7175::ADC_START = BuiltIn_16T_409920_Base::Pin_START ;
//      Adc24bit_AD7175::ADC_CS = LBT21084VB_Base::PinGroup[LBT21084VB_Pin_AdcCS1];
// 	   SSPIFunc::ConfirmDevice(0, &BuiltIn_16V_409920_SSP0ADC);
// 		 ARBUS_DELAY(10);
// 	   BuiltIn_DataManager::SettingCS_Low(u_CSid);
// 	   Adc24bit_AD7175::Init_diffmode_BUILTIN(eCONVCODETYPE_BIPOLAR,0); //��ʼ��ADC7175
// 	   BuiltIn_DataManager::SettingCS_High();

}
// void BuiltIn_16T_409920_Base::Func_Init(unsigned long BoardID,unsigned long ADCID,unsigned long ChannelID)
// {

// }
void BuiltIn_16T_409920_Base::Hardware_Init(unsigned char BoardID)
{
    IO_Init();
    SPI_SET();
    BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_abi_globe.m_Total_ADC=1;  //
    BuiltIn_DataManager::BuiltIn_ADC_Info[0].m_Valid=1;
    BuiltIn_DataManager::BuiltIn_ADC_Info[0].counter =0;
    BuiltIn_DataManager::BuiltIn_ADC_Info[0].b_Finished = 0;
    BuiltIn_DataManager::BuiltIn_ADC_Info[0].m_WorkingChn = 0;
    BuiltIn_DataManager::BuiltIn_ADC_Info[0].b_ADG_Exist_Flag = false;
    BuiltIn_DataManager::BuiltIn_ADC_Info[0].b_WAIT2_FLAG = false;
    BuiltIn_DataManager::BuiltIn_ADC_Info[0].m_StateMachine = BUILTIN_Sample_IDLE;

    BuiltIn_DataManager::BuiltIn_ADC_Info[0].sspChn = 0;
    BuiltIn_DataManager::BuiltIn_ADC_Info[0].m_CS_Address.m_Value = 1; //1110 EOE~ECODE1
    BuiltIn_DataManager::BuiltIn_ADC_Info[0].m_TickCount_ADG = 0;  //8*50=400us
    BuiltIn_DataManager::BuiltIn_ADC_Info[0].m_Wait_Count = 3340; //3340*50=167ms; 4000;  //4000*50=200ms;  //12*50=600us
    BuiltIn_DataManager::BuiltIn_ADC_Info[0].m_ADCCH_MaxCnt = 17;
    BuiltIn_DataManager::BuiltIn_ADC_Info[0].b_ADCCH_Valid[16]=1; //��֤�����¶�
// 	  BuiltIn_DataManager::BuiltIn_ADC_Info[0].m_BoardADC_OpenADG = &BuiltIn_16T_409920_Base::BuiltIn_2ndV_ADG_Open;
    BuiltIn_DataManager::BuiltIn_ADC_Info[0].m_BoardADC_Trigger = &BuiltIn_16T_409920_Base::BuiltIn_TC_Trigger;
    BuiltIn_DataManager::BuiltIn_ADC_Info[0].m_BoardADC_Rd = &BuiltIn_16T_409920_Base::BuiltIn_TC_Read;
    BuiltIn_DataManager::BuiltIn_ADC_Info[0].m_data_group_idx = 0;
    BuiltIn_DataManager::m_DataGroupMgr[0]=true;

    for(int i = 0; i< 16; i++) //�ð��͵װ���16ͨ���¶ȣ�������������֧�ֶ������͡�
    {
        BuiltIn_DataManager::m_BuiltInDataBank[0][i].m_Chn_Info = BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_abi_chn[i];
        BuiltIn_DataManager::BuiltIn_ADC_Info[0].b_ADCCH_Valid[i]= BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_abi_chn[i].m_Populated;
        BuiltIn_DataManager::m_PChn2EChn[0][i].e_data_group_id = 0;
        BuiltIn_DataManager::m_PChn2EChn[0][i].e_chan_id = i; //���ͨ�������ͨ�
    }
    /*  if(BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_assemble.m_WithSlave==1)
      {
       switch (BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_abi_globe.m_SlaveBoardType)
    	   {
            case  Aux_BI_VL_16V_409932:
    						    BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_abi_globe.m_Total_ADC=2;
    	              BuiltIn_DataManager::BuiltIn_ADC_Info[1].m_Valid=1;
    //                       BuiltIn_DataManager::BuiltIn_ADC_Info[BoardID].m_BoardType = Aux_BulitIn_16V_409386;
                    BuiltIn_DataManager::BuiltIn_ADC_Info[1].counter =0;
                    BuiltIn_DataManager::BuiltIn_ADC_Info[1].b_Finished = 0;
                    BuiltIn_DataManager::BuiltIn_ADC_Info[1].m_WorkingChn = 0;
    	              BuiltIn_DataManager::BuiltIn_ADC_Info[1].b_ADG_Exist_Flag = true;
    	              BuiltIn_DataManager::BuiltIn_ADC_Info[1].b_WAIT2_FLAG = true;
                    BuiltIn_DataManager::BuiltIn_ADC_Info[1].m_StateMachine = BUILTIN_Sample_IDLE;

                    BuiltIn_DataManager::BuiltIn_ADC_Info[1].sspChn = 0;
                    BuiltIn_DataManager::BuiltIn_ADC_Info[1].m_CS_Address.m_Value = 8; //1110 EOE~ECODE1
                    BuiltIn_DataManager::BuiltIn_ADC_Info[1].m_TickCount_ADG = 8;  //8*50=400us
                    BuiltIn_DataManager::BuiltIn_ADC_Info[1].m_ADCCH_MaxCnt = 16;
    					      BuiltIn_DataManager::BuiltIn_ADC_Info[1].m_BoardADC_OpenADG = &BuiltIn_16T_409920_Base::BuiltIn_2ndV_ADG_Open;
                    BuiltIn_DataManager::BuiltIn_ADC_Info[1].m_BoardADC_Trigger = &BuiltIn_16T_409920_Base::BuiltIn_2ndV_Trigger;
                    BuiltIn_DataManager::BuiltIn_ADC_Info[1].m_BoardADC_Rd = &BuiltIn_16T_409920_Base::BuiltIn_2ndV_Read;
                  	BuiltIn_DataManager::BuiltIn_ADC_Info[1].m_data_group_idx = 1;
                  BuiltIn_DataManager::m_DataGroupMgr[1]=true;
    					      ADC_Init(8);

    		            for(int i = 16; i< 32;i++)  //�ð������16������ͨ�����Ҹð��޸���
    	              {
                       BuiltIn_DataManager::m_BuiltInDataBank[1][i].m_Chn_Info = BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_abi_chn[i];
                       BuiltIn_DataManager::BuiltIn_ADC_Info[1].b_ADCCH_Valid[i-16]= BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_abi_chn[i].m_Populated;
                  }
    								if(BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_assemble.m_SlaveGoesFirst ==1)
    								{
                       for(unsigned char i=0;i<32;i++)
    									   {
    											    if(i<16)
                            {
    														  BuiltIn_DataManager::m_PChn2EChn[1][i].e_data_group_id =0;
                                BuiltIn_DataManager::m_PChn2EChn[1][i].e_chan_id = i;
                            }
    													else
    													{
    														  BuiltIn_DataManager::m_PChn2EChn[0][i].e_data_group_id =1;
    														  BuiltIn_DataManager::m_PChn2EChn[0][i-16].e_chan_id =i-16;
                            }
                       }
                  }
    								else
    								{
                       for(unsigned char i=16;i<32;i++)
                       {
    											    BuiltIn_DataManager::m_PChn2EChn[1][i].e_data_group_id =1;
                            BuiltIn_DataManager::m_PChn2EChn[1][i-16].e_chan_id = i-16;
                       }
                  }

    					      break;
    					case  Aux_BI_VH_8V_409936:
    						    BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_abi_globe.m_Total_ADC=9;
                  break;
    					case Aux_BI_TC_16T_409928:
    						    BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_abi_globe.m_Total_ADC=2;
    	              BuiltIn_DataManager::BuiltIn_ADC_Info[1].m_Valid=1;
    //                       BuiltIn_DataManager::BuiltIn_ADC_Info[BoardID].m_BoardType = Aux_BulitIn_16V_409386;
                    BuiltIn_DataManager::BuiltIn_ADC_Info[1].counter =0;
                    BuiltIn_DataManager::BuiltIn_ADC_Info[1].b_Finished = 0;
                    BuiltIn_DataManager::BuiltIn_ADC_Info[1].m_WorkingChn = 0;
    	              BuiltIn_DataManager::BuiltIn_ADC_Info[1].b_ADG_Exist_Flag = false;
    	              BuiltIn_DataManager::BuiltIn_ADC_Info[1].b_WAIT2_FLAG = false;
                    BuiltIn_DataManager::BuiltIn_ADC_Info[1].m_StateMachine = BUILTIN_Sample_IDLE;

                    BuiltIn_DataManager::BuiltIn_ADC_Info[1].sspChn = 0;
                    BuiltIn_DataManager::BuiltIn_ADC_Info[1].m_CS_Address.m_Value = 9; //1110 EOE~ECODE1
                    BuiltIn_DataManager::BuiltIn_ADC_Info[1].m_Wait_Count =  3340; //3340*50=167ms; 4000;  //4000*50=200ms;  //12*50=600us
                    BuiltIn_DataManager::BuiltIn_ADC_Info[1].m_ADCCH_MaxCnt = 17;
    					      BuiltIn_DataManager::BuiltIn_ADC_Info[1].b_ADCCH_Valid[16]=1; //��֤�����¶�
                    BuiltIn_DataManager::BuiltIn_ADC_Info[1].m_BoardADC_Trigger = &BuiltIn_16T_409920_Base::BuiltIn_TC_Trigger;
                    BuiltIn_DataManager::BuiltIn_ADC_Info[1].m_BoardADC_Rd = &BuiltIn_16T_409920_Base::BuiltIn_TC_Read;
                  BuiltIn_DataManager::BuiltIn_ADC_Info[1].m_data_group_idx = 1;
    		            for(int i = 16; i< 32;i++)  //�ð������16������ͨ�����Ҹð��޸���
    	              {
                       BuiltIn_DataManager::m_BuiltInDataBank[1][i].m_Chn_Info = BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_abi_chn[i];
                       BuiltIn_DataManager::BuiltIn_ADC_Info[1].b_ADCCH_Valid[i-16]= BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_abi_chn[i].m_Populated;
                  }
    								if(BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_assemble.m_SlaveGoesFirst ==1)
    								{
                       for(unsigned char i=0;i<32;i++)
    									   {
    											    if(i<16)
                            {
    														  BuiltIn_DataManager::m_PChn2EChn[1][i].e_data_group_id =0;
                                BuiltIn_DataManager::m_PChn2EChn[1][i].e_chan_id = i;
                            }
    													else
    													{
    														  BuiltIn_DataManager::m_PChn2EChn[0][i].e_data_group_id =1;
    														  BuiltIn_DataManager::m_PChn2EChn[0][i-16].e_chan_id =i-16;
                            }
                       }
                  }
    								else
    								{
                       for(unsigned char i=16;i<32;i++)
                       {
    											    BuiltIn_DataManager::m_PChn2EChn[1][i].e_data_group_id =1;
                            BuiltIn_DataManager::m_PChn2EChn[1][i-16].e_chan_id = i-16;
                       }
                  }
                  break;
    					default:

    					      break;
       }
    }*/
    BuiltIn_DataManager::m_BuiltInAuxInfo.TempExistFlag = true;
    BuiltIn_DataManager::m_BuiltInAuxInfo.VolExistFlag = false;
    BuiltIn_DataManager::m_BuiltInAuxInfo.TempDataBank =0;
    BuiltIn_DataManager::m_BuiltInAuxInfo.TempCntInOneIV = BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_abi_globe.m_Total_Channels/ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount;

}
// void BuiltIn_16T_409920_Base::Software_Init(unsigned char Board_ID)
// {
// There is no speacial software init
// }
// void BuiltIn_16T_409920_Base::BuiltIn_2ndV_ADG_Open(unsigned char u_CSid,unsigned char ChannelIdx)
// {
//     CPLD_FUNC::WriteADGBus(ChannelIdx,0x01);
// }

// unsigned long BuiltIn_16T_409920_Base::BuiltIn_2ndV_Trigger(unsigned char u_CSid,unsigned char ADCchn)
// {
// 	  SSPIFunc::ConfirmDevice(0, &BuiltIn_16V_409920_SSP0ADC);
//     Adc24bit_AD7175::Trigger(0);

// 	  Adc24bit_AD7175::ADC_START = BuiltIn_16T_409920_Base::Pin_START ;
//     ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_START);
// 	  TIMERSFunc::DELAY_SYS(20, 0);
// 	  ControlPin::SetMeLow(Adc24bit_AD7175::ADC_START);
// 	  RestADC = 0;
// 	return 1;
// }

/*unsigned long BuiltIn_16T_409920_Base::BuiltIn_2ndV_Read(unsigned char u_CSid, unsigned char ADCchn)
{
	  unsigned char temp;
	  unsigned long data;
	  SSPIFunc::ConfirmDevice(0, &BuiltIn_16V_409920_SSP0ADC);
		Adc24bit_AD7175::ADC_DRDY =  BuiltIn_16T_409920_Base::Pin_DRDY_S;
	  temp=ControlPin::ReadMe(Adc24bit_AD7175::ADC_DRDY);
		if(temp==0)
		{
	          data= Adc24bit_AD7175::Read(0);
	          if(data!=0)
					  {
									BuiltIn_DataManager::m_BuiltInDataBank[BuiltIn_DataManager::BuiltIn_ADC_Info[1].m_data_group_idx][ADCchn].aux_instant_code = data;
									BuiltIn_DataManager::m_BuiltInDataBank[BuiltIn_DataManager::BuiltIn_ADC_Info[1].m_data_group_idx][ADCchn].updated = true;
						}
// 						ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_CS);
		}
			else
			{
						 if(RestADC++>=0xFF)
						 {

							    Adc24bit_AD7175::Reset2(0);
									ADC_Init(u_CSid);
							    BuiltIn_DataManager::BuiltIn_ADC_Info[u_CSid].m_StateMachine = BUILTIN_Sample_IDLE;

              }
       }
	    return 1;
}*/

unsigned long BuiltIn_16T_409920_Base::BuiltIn_TC_Trigger(unsigned char u_ADCid,unsigned char ADCchn)
{
    SSPIFunc::ConfirmDevice(0, &BuiltIn_16T_409920_SSP0ADC);
    Adc32bit_LTC2498::Trigger(0,ADCchn);
    return 1;
}

unsigned long BuiltIn_16T_409920_Base::BuiltIn_TC_Read(unsigned char u_ADCid,unsigned char ADCchn)
{
    unsigned long temp;
    SSPIFunc::ConfirmDevice(0, &BuiltIn_16T_409920_SSP0ADC);
    if (ADCchn>=16)
    {
        unsigned long ul_Inchip_temperature = Adc32bit_LTC2498::Read(0);
        if(ul_Inchip_temperature!=0)
        {
            if(u_ADCid ==0)
            {
                BuiltIn_DataManager::m_InChipTemperature[0].InChipTemp_instant_code = ul_Inchip_temperature ;//�����ݷŵ�һ��ͳһ�����Ƭ���¶�QUE���Ա���ƽ������
                BuiltIn_DataManager::m_InChipTemperature[0].updated= true ;//�����ݷŵ�һ��ͳһ�����Ƭ���¶�QUE���Ա���ƽ������
            }
            else
            {
                BuiltIn_DataManager::m_InChipTemperature[1].InChipTemp_instant_code = ul_Inchip_temperature ;//�����ݷŵ�һ��ͳһ�����Ƭ���¶�QUE���Ա���ƽ������
                BuiltIn_DataManager::m_InChipTemperature[1].updated= true ;//�����ݷŵ�һ��ͳһ�����Ƭ���¶�QUE���Ա���ƽ������
            }
        }
        else
        {
            if(u_ADCid ==0)
                BuiltIn_DataManager::m_InChipTemperature[0].updated= false ;
            else
                BuiltIn_DataManager::m_InChipTemperature[1].updated= false ;
        }
    }
    else
    {
        temp=Adc32bit_LTC2498::Read(0);
        if(temp!=0)
        {
            if(u_ADCid ==0)
            {
                BuiltIn_DataManager::m_BuiltInDataBank[BuiltIn_DataManager::BuiltIn_ADC_Info[0].m_data_group_idx][ADCchn].aux_instant_code = temp;
								BuiltIn_DataManager::m_BuiltInDataBank[BuiltIn_DataManager::BuiltIn_ADC_Info[0].m_data_group_idx][ADCchn].value_update_time = CommonFunc::GetSystemTime();
                BuiltIn_DataManager::m_BuiltInDataBank[BuiltIn_DataManager::BuiltIn_ADC_Info[0].m_data_group_idx][ADCchn].updated = true;
            }
            else
            {
                BuiltIn_DataManager::m_BuiltInDataBank[BuiltIn_DataManager::BuiltIn_ADC_Info[1].m_data_group_idx][ADCchn].aux_instant_code = temp;
								BuiltIn_DataManager::m_BuiltInDataBank[BuiltIn_DataManager::BuiltIn_ADC_Info[1].m_data_group_idx][ADCchn].value_update_time = CommonFunc::GetSystemTime();
                BuiltIn_DataManager::m_BuiltInDataBank[BuiltIn_DataManager::BuiltIn_ADC_Info[1].m_data_group_idx][ADCchn].updated = true;
            }
        }
    }
    return 1;
}