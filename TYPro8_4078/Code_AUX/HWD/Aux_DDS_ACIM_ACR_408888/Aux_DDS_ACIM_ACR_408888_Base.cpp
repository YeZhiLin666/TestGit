///////////////////////////////////////////////////////////////////////////////////
//File Name: AUX_BASE.cpp
//Version:		V1.0
//Date:				2013/11/18
//Description:
//						辅助配置相关操作
//Others:      dirui
//History：

///////////////////////////////////////////////////////////////////////////////////
#include "../Entry/includes.h"
#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM32",zidata = "SRAM32"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x03E80000
#endif
SSPI_Para Aux_DDS_ACIM_ACR_408888_Base::Aux_DDS_ACIM_ACR_408888_SSP0ADC;
SSPI_Para Aux_DDS_ACIM_ACR_408888_Base::Aux_DDS_ACIM_ACR_408888_SSP1ADC;
unsigned char Aux_DDS_ACIM_ACR_408888_Base::Amplitude_Flag;
// const unsigned char AD5160_Table[]={ 240,131,82,36,35,      //0.05A   0.1A  0.15A  0.281A  0.287A
//                                         34,33,32,31,30,   //0.292A    0.298A  0.305A  0.311A  0.3182A
//                                         29,28,27,26,25,   //0.449A   0.479A  0.507A  0.533A 0.56A
//                                         24,23,20,17,13   //0.837A   0.85A  0.90A  0.95A  1.00A
//                                      };

const float  f_Coefficient[6] = {  537.12, -3763.1, 12170, -19955, 15887, -4862.3  };
#pragma arm section //结束外存变量定义
//ADC  AD5754
const 	St_Port Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CONV= {2,        3,        eCPTYPE_MCUGPIO,   GPIO_O,             0};
const 	St_Port Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_DRDY1= {2,        5,        eCPTYPE_MCUGPIO,   GPIO_I,             0};
const 	St_Port Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_DRDY2= {2,        6,        eCPTYPE_MCUGPIO,   GPIO_I,             0};
const 	St_Port Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS1= {1,       19,        eCPTYPE_MCUGPIO,   GPIO_O,             0};
const 	St_Port Aux_DDS_ACIM_ACR_408888_Base::Pin_ADC_CS2= {1,        20,        eCPTYPE_MCUGPIO,   GPIO_O,             0};

//DAC AD5754
const 	St_Port Aux_DDS_ACIM_ACR_408888_Base::Pin_DacBIN2COMP = {2,        4,        eCPTYPE_MCUGPIO,   GPIO_O,             0};
const 	St_Port Aux_DDS_ACIM_ACR_408888_Base::Pin_DacLoad = {2,        7,        eCPTYPE_MCUGPIO,   GPIO_O,             0};
const 	St_Port Aux_DDS_ACIM_ACR_408888_Base::Pin_DacCLR = {2,        9,        eCPTYPE_MCUGPIO,   GPIO_O,             0};
const 	St_Port Aux_DDS_ACIM_ACR_408888_Base::Pin_DacCS = {1,        21,        eCPTYPE_MCUGPIO,   GPIO_O,             0};
// const 	St_Port Aux_DDS_ACIM_ACR_408888_Base::Pin_DacCS2 ={2,        3,        eCPTYPE_MCUGPIO,   GPIO_O,             0};

//DDS
const 	St_Port Aux_DDS_ACIM_ACR_408888_Base::Pin_DDS_CS = {1,        22,        eCPTYPE_MCUGPIO,   GPIO_O,             0};
const 	St_Port Aux_DDS_ACIM_ACR_408888_Base::Pin_DDS_CS6 = {1,        23,        eCPTYPE_MCUGPIO,   GPIO_O,             0};  //
const 	St_Port Aux_DDS_ACIM_ACR_408888_Base::Pin_DDS_SLEEP = {1,        18,        eCPTYPE_MCUGPIO,   GPIO_O,             0};

const  St_Port Aux_DDS_ACIM_ACR_408888_Base::Pin_I_VS_RMS = {1,        24,        eCPTYPE_MCUGPIO,   GPIO_O,             0};
const  St_Port Aux_DDS_ACIM_ACR_408888_Base::Pin_SETS = {1,        25,        eCPTYPE_MCUGPIO,   GPIO_O,             0};
const  St_Port Aux_DDS_ACIM_ACR_408888_Base::Pin_ACIM_I = {1,        26,        eCPTYPE_MCUGPIO,   GPIO_O,             0};
const  St_Port Aux_DDS_ACIM_ACR_408888_Base::Pin_ACIM_V = {1,        27,        eCPTYPE_MCUGPIO,   GPIO_O,             0};

const  St_Port Aux_DDS_ACIM_ACR_408888_Base::Pin_VA0 = {1,        28,        eCPTYPE_MCUGPIO,   GPIO_O,             0};
const  St_Port Aux_DDS_ACIM_ACR_408888_Base::Pin_VA1 = {1,        29,        eCPTYPE_MCUGPIO,   GPIO_O,             0};
const  St_Port Aux_DDS_ACIM_ACR_408888_Base::Pin_ARW = {1,        13,        eCPTYPE_MCUGPIO,   GPIO_O,             0};

const  St_Port Aux_DDS_ACIM_ACR_408888_Base::Pin_None = {0,        0,        eCPTYPE_NONE,   NA,             0};
// const  St_Port Aux_DDS_ACIM_ACR_408888_Base::Pin_ARW ={1,        3,        eCPTYPE_MCUGPIO,   GPIO_O,             0};
const  St_Port Aux_DDS_ACIM_ACR_408888_Base::Pin_DAC_BIAS_SET = {1,        6,        eCPTYPE_MCUGPIO,   GPIO_O,             0};
const  St_Port Aux_DDS_ACIM_ACR_408888_Base::Pin_PROT = {0,        16,        eCPTYPE_MCUGPIO,   GPIO_O,             0};
//temp one board

unsigned char Aux_DDS_ACIM_ACR_408888_Base::My_BoardID =0;
int Aux_DDS_ACIM_ACR_408888_Base::AD8253_VRead_Gain;
bool Aux_DDS_ACIM_ACR_408888_Base::isACIM;
bool Aux_DDS_ACIM_ACR_408888_Base::isCurrentSoruce;
// long Aux_DDS_ACIM_ACR_408888_Base::data[14];

void Aux_DDS_ACIM_ACR_408888_Base::IO_Init(void)
{
    ControlPin::SetIObyCP(Pin_DacBIN2COMP);
    ControlPin::SetIObyCP(Pin_DacLoad);
    ControlPin::SetIObyCP(Pin_DacCLR);
    ControlPin::SetIObyCP(Pin_ADC_CONV);
    ControlPin::SetIObyCP(Pin_ADC_DRDY1);
    ControlPin::SetIObyCP(Pin_ADC_DRDY2);
    ControlPin::SetIObyCP(Pin_ADC_CS1);
    ControlPin::SetIObyCP(Pin_ADC_CS2);
    ControlPin::SetIObyCP(Pin_DDS_CS);
    ControlPin::SetIObyCP(Pin_DDS_CS6);
    ControlPin::SetIObyCP(Pin_DDS_SLEEP);
    ControlPin::SetIObyCP(Pin_I_VS_RMS);
    ControlPin::SetIObyCP(Pin_SETS);
    ControlPin::SetIObyCP(Pin_ACIM_I);
    ControlPin::SetIObyCP(Pin_ACIM_V);
    ControlPin::SetIObyCP(Pin_VA0);
    ControlPin::SetIObyCP(Pin_VA1);
    ControlPin::SetIObyCP(Pin_ARW);
    ControlPin::SetIObyCP(Pin_None);
		ControlPin::SetIObyCP(Pin_DAC_BIAS_SET);
		ControlPin::SetIObyCP(Pin_PROT);

    ControlPin::SetMeHigh(Pin_DDS_CS );
    ControlPin::SetMeHigh(Pin_DDS_CS6);
// 	ControlPin::SetMeHigh(Pin_I_VS_RMS);
    ControlPin::SetMeHigh(Pin_ADC_CS1);
    ControlPin::SetMeHigh(Pin_ADC_CS2);
    ControlPin::SetMeHigh(Pin_DacCS);
    ControlPin::SetMeHigh(Pin_ACIM_V);
// 	ControlPin::SetMeHigh(Pin_ARW);
// 	ControlPin::SetMeHigh(Pin_VA0);
// 	ControlPin::SetMeHigh(Pin_VA1);

    ControlPin::SetMeLow(Pin_I_VS_RMS);  //选电流Rms
    ControlPin::SetMeLow(Pin_ACIM_I);
// 	ControlPin::SetMeLow(Pin_ACIM_V);
    ControlPin::SetMeLow(Pin_SETS);
    ControlPin::SetMeLow(Pin_ARW);
    ControlPin::SetMeLow(Pin_VA0);
    ControlPin::SetMeLow(Pin_VA1);
		ControlPin::SetMeLow(Pin_DAC_BIAS_SET);
		ControlPin::SetMeLow(Pin_PROT);		
}
void Aux_DDS_ACIM_ACR_408888_Base::SPI_SET(unsigned char spiChn)
{
    switch (spiChn)
    {

    case  0:
    {
        Aux_DDS_ACIM_ACR_408888_SSP0ADC.uc_Module_Name = SSP0_DEVICE3;
        Aux_DDS_ACIM_ACR_408888_SSP0ADC.uc_Speed = 7;
        Aux_DDS_ACIM_ACR_408888_SSP0ADC.uc_CPOL = 0;
        Aux_DDS_ACIM_ACR_408888_SSP0ADC.uc_CPHA = 1;
        Aux_DDS_ACIM_ACR_408888_SSP0ADC.uc_MSTR = 1;    //master
        Aux_DDS_ACIM_ACR_408888_SSP0ADC.uc_BITS = 8;
        Aux_DDS_ACIM_ACR_408888_SSP0ADC.uc_LSBF = 0;    //DAC  AD5754
        SSPIFunc::setPara(&Aux_DDS_ACIM_ACR_408888_Base::Aux_DDS_ACIM_ACR_408888_SSP0ADC,0);
        SSPIFunc::my_Init(0);
    }
    break;
    case 1:
    {
        Aux_DDS_ACIM_ACR_408888_SSP1ADC.uc_Module_Name = SSP1_DEVICE1;
        Aux_DDS_ACIM_ACR_408888_SSP1ADC.uc_Speed = 7;
        Aux_DDS_ACIM_ACR_408888_SSP1ADC.uc_CPOL = 1;
        Aux_DDS_ACIM_ACR_408888_SSP1ADC.uc_CPHA = 1;
        Aux_DDS_ACIM_ACR_408888_SSP1ADC.uc_MSTR = 1;    //master
        Aux_DDS_ACIM_ACR_408888_SSP1ADC.uc_BITS = 8;
        Aux_DDS_ACIM_ACR_408888_SSP1ADC.uc_LSBF = 0;   //ADC7175
        SSPIFunc::setPara(&Aux_DDS_ACIM_ACR_408888_Base::Aux_DDS_ACIM_ACR_408888_SSP1ADC,1);
        SSPIFunc::my_Init(1);
    }
    break;
    default:
        break;
    }
}
void Aux_DDS_ACIM_ACR_408888_Base::DDS_SPI_Init(void)
{
    Aux_DDS_ACIM_ACR_408888_SSP1ADC.uc_Module_Name = SSP1_DEVICE1;
    Aux_DDS_ACIM_ACR_408888_SSP1ADC.uc_Speed = 8;
    Aux_DDS_ACIM_ACR_408888_SSP1ADC.uc_CPOL = 1;  //0;
    Aux_DDS_ACIM_ACR_408888_SSP1ADC.uc_CPHA = 0;  //1;
    Aux_DDS_ACIM_ACR_408888_SSP1ADC.uc_MSTR = 1;    //master
    Aux_DDS_ACIM_ACR_408888_SSP1ADC.uc_BITS = 16;
    Aux_DDS_ACIM_ACR_408888_SSP1ADC.uc_LSBF = 0;   //AD9834
    SSPIFunc::setPara(&Aux_DDS_ACIM_ACR_408888_Base::Aux_DDS_ACIM_ACR_408888_SSP1ADC,1);
    SSPIFunc::my_Init(1);
}
void Aux_DDS_ACIM_ACR_408888_Base::AD5160_SPI_Init(void)
{
    Aux_DDS_ACIM_ACR_408888_SSP1ADC.uc_Module_Name = SSP1_DEVICE1;
    Aux_DDS_ACIM_ACR_408888_SSP1ADC.uc_Speed = 8;
    Aux_DDS_ACIM_ACR_408888_SSP1ADC.uc_CPOL = 0;
    Aux_DDS_ACIM_ACR_408888_SSP1ADC.uc_CPHA = 0;
    Aux_DDS_ACIM_ACR_408888_SSP1ADC.uc_MSTR = 1;    //master
    Aux_DDS_ACIM_ACR_408888_SSP1ADC.uc_BITS = 8;
    Aux_DDS_ACIM_ACR_408888_SSP1ADC.uc_LSBF = 0;   //AD5160
    SSPIFunc::setPara(&Aux_DDS_ACIM_ACR_408888_Base::Aux_DDS_ACIM_ACR_408888_SSP1ADC,1);
    SSPIFunc::my_Init(1);

}
void Aux_DDS_ACIM_ACR_408888_Base::ADC_Init()
{
    if(isACIM) // ACIM board
    {
        Adc24bit_AD7175::ADC_START = Pin_ADC_CONV;
        Adc24bit_AD7175::ADC_CS = Pin_None;		// disable ADC_CS use in AD7175::Read()

        SPI_SET(1);
        ControlPin::SetMeLow(Pin_ADC_CS1);
        ARBUS_DELAY(1000);
        Adc24bit_AD7175::Init_FFT_mode(eCONVCODETYPE_BIPOLAR,1);  //初始化电流采样ADC7175
        ARBUS_DELAY(1000);
        ControlPin::SetMeHigh(Pin_ADC_CS1);

        ControlPin::SetMeLow(Pin_ADC_CS2);
        ARBUS_DELAY(1000);
        Adc24bit_AD7175::Init_FFT_mode(eCONVCODETYPE_BIPOLAR,1);  //初始化电压采样ADC7175
        ARBUS_DELAY(1000);
        ControlPin::SetMeHigh(Pin_ADC_CS2);
    }
    else
    {
        Adc24bit_AD7175::ADC_START = Pin_ADC_CONV;
        Adc24bit_AD7175::ADC_CS  =AUX_Unit_Base::Pin_ECODE2;
        SPI_SET(1);
        ARBUS_DELAY(100);
        Adc24bit_AD7175::Init_Single_mode(eCONVCODETYPE_BIPOLAR,1);  //初始化电流采样ADC7175
    }
}

void Aux_DDS_ACIM_ACR_408888_Base::DAC_Init(void)
{
    //AD5754
    DAC16bit_AD5754::Pin_DacBin = Aux_DDS_ACIM_ACR_408888_Base::Pin_DacBIN2COMP;
// 	DAC16bit_AD5754::Pin_DacCS = AUX_Unit_Base::Pin_EOE;
    DAC16bit_AD5754::Pin_DacLoad =Aux_DDS_ACIM_ACR_408888_Base::Pin_DacLoad;
    DAC16bit_AD5754::Pin_DacClr = Aux_DDS_ACIM_ACR_408888_Base::Pin_DacCLR;
    DAC16bit_AD5754::SPI_Idx = 0;

    //初始化，power up AD5754
    SPI_SET(0);

    DAC16bit_AD5754::Init(eCONVCODETYPE_UNIPOLAR);    // 1.27 BIN       BIN COMP



}
void Aux_DDS_ACIM_ACR_408888_Base::Func_Init(void)
{
// 	for(unsigned char i=0;i<14;i++)
// 		Aux_DDS_ACIM_ACR_408888_Base::data[i]=0;
    if(isACIM)
    {
        //force set ADC param
        Aux_DataManager::m_AuxDataBank[0][0].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_Gain = 0.0000005960464f;	// -5V ~ 5V = 0 - 2^24
        Aux_DataManager::m_AuxDataBank[0][0].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_Offset = -5.0f;
				Aux_DataManager::m_AuxDataBank[0][0].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Gain = 1.0f;
				Aux_DataManager::m_AuxDataBank[0][0].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Offset = 0.0f;
			
        // force set DAC param
        Aux_DataManager::m_AuxDataBank[2][0].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Gain = 838860.8f;
        Aux_DataManager::m_AuxDataBank[2][0].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Offset = 8388608.0f;
				Aux_DataManager::m_AuxDataBank[2][0].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_Gain= 1.0f;
				Aux_DataManager::m_AuxDataBank[2][0].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_Offset = 0.0f;
			
        Aux_DataManager::m_AuxDataBank[2][2].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Gain = 838860.8f;
        Aux_DataManager::m_AuxDataBank[2][2].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Offset = 8388608.0f;
				Aux_DataManager::m_AuxDataBank[2][2].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_Gain = 1.0f;
				Aux_DataManager::m_AuxDataBank[2][2].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_Offset = 0.0f;			
    }
}
void Aux_DDS_ACIM_ACR_408888_Base::Hardware_Init(unsigned char BoardID)
{
    unsigned char AuxFunc;
    IO_Init();

    if(Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_globe.m_BoardType==Aux_DDS_ACIM_ACR_408888 || Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_globe.m_BoardType == Aux_DDS_ACIM_415466 || Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_globe.m_BoardType == Aux_DDS_Sinwave_Current_Source_604700)
    {
        Amplitude_Flag= 0x01;
        AD5160_SPI_Init();
        Write_AD5160(0.01);
    }
    else
        Amplitude_Flag= 0x02;

		if(Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_globe.m_BoardType == Aux_DDS_Sinwave_Current_Source_604700)
		{
			isACIM = true;
			isCurrentSoruce = true;
		}
		else if(Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_globe.m_BoardType == Aux_DDS_ACIM_415466)
		{
			isACIM = true;
			isCurrentSoruce = false;
		}
		else
		{
					isACIM = false;
			isCurrentSoruce = false;
		}

    Write_AD8253(1);


    DDS_SPI_Init();
    DDS_AD9834::FSYNC =Pin_DDS_CS;
    DDS_AD9834::Init();

// 		ControlPin::SetMeHigh(Pin_ACIM_I);
//     ControlPin::SetMeHigh(Pin_SETS);
//   Write_AD8253(10);
// 	Write_AD8253(100);
    ADC_Init();

    for(unsigned char i = 0; i<MAX_DATA_GROUP; i++)
    {
        for(unsigned char j = 0; j<MAX_AUXCH_PER_DATA_GROUP; j++)
        {
            Aux_DataManager::m_PChn2EChn [i][j].e_data_group_id = 0xFF;
            Aux_DataManager::m_PChn2EChn [i][j].e_chan_id = 0xFF;
        }
    }
    My_BoardID = BoardID;   //记录自己的BoardID；
    AuxFunc=Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_globe.m_AuxFuncType;
    if( ((AuxFunc&0x01)==0x01)&&(Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_globe.m_Total_ADC!=0 )	)// 配置了ADC
    {
//       for(unsigned char i =0;i<Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_globe.m_Total_ADC;i++)
// 	    {
// 		     Aux_DataManager::Aux_ADC_CS_Info[i].m_Valid=1;
// 		     Aux_DataManager::Aux_ADC_CS_Info[i].m_BoardType = Aux_WMADC_AIO_DIO_602548;
//
// 		     Aux_DataManager::Aux_ADC_CS_Info[i].counter =0;
// 	    	 Aux_DataManager::Aux_ADC_CS_Info[i].b_Finished = 0;
// 		     Aux_DataManager::Aux_ADC_CS_Info[i].m_WorkingChn = 0;
// 		     Aux_DataManager::Aux_ADC_CS_Info[i].m_StateMachine = AUX_Sample_IDLE;
//
// 		     Aux_DataManager::Aux_ADC_CS_Info[i].sspChn = 1;
// // 	       Aux_DataManager::Aux_ADC_CS_Info[i].m_CS_Address.m_Value = 0x00;//    0010 EOE~ECODE1   0x0E; //1110 EOE~ECODE1
// 	       Aux_DataManager::Aux_ADC_CS_Info[i].m_Wait_Count = 10;  //10*50=500us
// 		     Aux_DataManager::Aux_ADC_CS_Info[0].m_ADCCH_MaxCnt = 4;
// 		     Aux_DataManager::Aux_ADC_CS_Info[0].m_BoardADC_Trigger =  &Aux_DDS_ACIM_ACR_408888_Base::Aux_Trigger;
// 	       Aux_DataManager::Aux_ADC_CS_Info[0].m_BoardADC_Rd = &Aux_DDS_ACIM_ACR_408888_Base::Aux_Read;
//          Aux_DataManager::Aux_ADC_CS_Info[0].m_data_group_idx = 0;
// 			}
// 			Aux_DataManager::Aux_ADC_CS_Info[0].m_CS_Address.m_Value = 0x13;//    7175A_Current
// 			Aux_DataManager::Aux_ADC_CS_Info[1].m_CS_Address.m_Value = 0x11;//    7175B_Voltage
//          Aux_DataManager::m_DGIdx2BIdx[BoardID] = 0;

        //获取通道信息并建立面板通道与电气通道的联系

        //分配 data_group 开始号
        Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start = 0;
        //标记data_group 有效
        Aux_DataManager::m_DataGroupMgr[0] = true;
        Aux_DataManager::m_DataGroupMgr[1] = true;
        for(unsigned char i = 0; i< 8; i++) //该板型有两片ADC ，每个四通道
        {
            if(i<4)
            {
                Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].m_Chn_Info =Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_chn[i];   //copy  通道信息到 AuxDataBank
                Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].e_data_group_id = Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start;
                Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].e_chan_id = i; //面板通道与电气通道同号
            }
            else
            {
                Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+1][i-4].m_Chn_Info =Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_chn[i];   //copy  通道信息到 AuxDataBank
                Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+1][i-4].e_data_group_id = Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+1;
                Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+1][i].e_chan_id = i-4; //面板通道与电气通道同号
            }
        }
    }

    if(((AuxFunc&0x80)==0x80) &&(Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_globe.m_Total_DAC !=0))  //配置了DAC通道
    {
        DAC16bit_AD5754::Pin_DacCS = Aux_DDS_ACIM_ACR_408888_Base::Pin_DacCS;
        DAC_Init();

        Aux_DataManager::m_DataGroupMgr[2] = true;
        for (unsigned char i=0; i<Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_globe.m_Total_DAC; i++)
        {
            Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+2][i].m_Chn_Info =Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_DAC_chn[i];   //copy  通道信息到 AuxDataBank
            Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+2][i]. m_Chn_Info.m_Populated = true;
            Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+2][i]. m_Chn_Info.m_AuxType = AUX_AO;
            Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+2][i].e_data_group_id = Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+2;
            Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start+2][i].e_chan_id = i; //面板通道与电气通道同号
        }
    }


}
void Aux_DDS_ACIM_ACR_408888_Base::Software_Init(unsigned char Board_ID)
{
    Func_Init();
// 	board_config::Func_AuxSample = &Aux_DDS_ACIM_ACR_408888_SampleManage::SampleISR;
    board_config::Func_AUX_Dac_Out = &Aux_DDS_ACIM_ACR_408888_Base::Dac_out;
}

// unsigned long Aux_DDS_ACIM_ACR_408888_Base::Aux_Trigger(unsigned char u_CSid,unsigned char ADCchn)
// {
// 	SPI_SET(1);
// 	Adc32bit_LTC2449::Trigger(Aux_DataManager::Aux_ADC_CS_Info[u_CSid].sspChn,ADCchn);
// 	return 1;
// }

// unsigned long Aux_DDS_ACIM_ACR_408888_Base::Aux_Read(unsigned char u_CSid, unsigned char ADCchn)
// {
// 	SPI_SET(1);
// 	Aux_DataManager::m_AuxDataBank[Aux_DataManager::Aux_ADC_CS_Info[u_CSid].m_data_group_idx][ADCchn].aux_instant_code = Adc32bit_LTC2449::Read(Aux_DataManager::Aux_ADC_CS_Info[u_CSid].sspChn); //将数据放到Aux_DataBank 中
// 	Aux_DataManager::m_AuxDataBank[Aux_DataManager::Aux_ADC_CS_Info[u_CSid].m_data_group_idx][ADCchn].updated = true; //标记已经更新
// 	return 1;
// }
void Aux_DDS_ACIM_ACR_408888_Base::Write_AD5160(float f_Value)
{
// 	   unsigned char Index =0, value;
// 	   value = (char)::abs(data*100);
// 	   if(value<5)
// 		 {
// 	      Index =0;
// 		 }
// 		 else if(value>100)
// 		 {
//         Index =19;
//      }
// 	   else
// 		 {
//          Index = value/5;
// 			   Index--;
//      }
    unsigned short datasent[1];
    float  f_Temp = 1;
    float  f_resistor_BinCode = 0;
    const float  *CoE;

    CoE = &f_Coefficient[0];
    f_resistor_BinCode =(*CoE);
    for( unsigned char uc_i=1; uc_i<6; uc_i++)
    {
        CoE++;
        f_Temp*= f_Value;
        f_resistor_BinCode +=f_Temp * (*CoE);
    }

//	   f_resistor *=256;
//		 f_resistor /=100;
    unsigned short temp_code =(unsigned short)(f_resistor_BinCode);
    if (temp_code <=13)
        temp_code = 13;
    if (temp_code>255)
        temp_code = 255;
    datasent[0]= temp_code;

    ControlPin::SetMeLow(Pin_DDS_CS6);
    ARBUS_DELAY(10);
    SSPIFunc::RxTx_Frames(datasent,1,1);    //send data
    TIMERSFunc::DELAY_SYS(20, 0);
    ControlPin::SetMeHigh(Pin_DDS_CS6);

}
void Aux_DDS_ACIM_ACR_408888_Base::Dac_out(unsigned char chn,unsigned long value)
{
    SPI_SET(0);
    DAC16bit_AD5754::Pin_DacCS = Aux_DDS_ACIM_ACR_408888_Base::Pin_DacCS;
    DAC16bit_AD5754::Dac_Out(chn,value);
}
void Aux_DDS_ACIM_ACR_408888_Base::Write_AD8253(unsigned short Gain)
{
    ControlPin::SetMeHigh(Pin_ARW);
// 	  ControlPin::SetMeLow(Pin_ARW);
    ARBUS_DELAY(1000);
    switch(Gain)
    {
    case 1:	// gain = 1
        AD8253_VRead_Gain = 1;
        ControlPin::SetMeLow(Pin_VA0);
        ControlPin::SetMeLow(Pin_VA1);
        break;
    case 10:	// gain = 10
        AD8253_VRead_Gain = 10;
        ControlPin::SetMeHigh(Pin_VA0);
        ControlPin::SetMeLow(Pin_VA1);
        break;
    case 100:	// gain = 100
        AD8253_VRead_Gain = 100;
        ControlPin::SetMeLow(Pin_VA0);
        ControlPin::SetMeHigh(Pin_VA1);
        break;
    case 1000:	// gain = 1000
        AD8253_VRead_Gain = 1000;
        ControlPin::SetMeHigh(Pin_VA0);
        ControlPin::SetMeHigh(Pin_VA1);
        break;
    default:
        ControlPin::SetMeLow(Pin_VA0);
        ControlPin::SetMeLow(Pin_VA1);
        break;
    }
    ARBUS_DELAY(1000);
    ControlPin::SetMeLow(Pin_ARW);
// 		 ARBUS_DELAY(50);
// 		 for(unsigned char i=0;i<10;i++)
// 		 {
//           ControlPin::SetMeHigh(Pin_ARW);
// 			    ARBUS_DELAY(1000);
//           ControlPin::SetMeLow(Pin_ARW);
//      }

}