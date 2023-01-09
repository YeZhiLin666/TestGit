
#include "../../Entry/includes.h"


void LBT22013_ADC5DAC::Adc_Init(void)
{
    if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_A)
    {
#if ADC_CS5560_ENABLE == 1
        Adc24bit_CS5560::Pin_Busy = LBT22013_Base::PinGroupVA[LBT22013_VA_Pin_AdcBusy1];
        Adc24bit_CS5560::Pin_Conver = LBT22013_Base::PinGroupVA[LBT22013_VA_Pin_AdcConver];
        Adc24bit_CS5560::Pin_CS = LBT22013_Base::PinGroupVA[LBT22013_VA_Pin_AdcCS1];
        Adc24bit_CS5560::Pin_Rst = LBT22013_Base::PinGroupVA[LBT22013_VA_Pin_AdcRst];
        Adc24bit_CS5560::SPI_Idx = 1;
        Adc24bit_CS5560::Init(eCONVCODETYPE_BIPOLAR);

        Adc24bit_CS5560::Pin_Busy = LBT22013_Base::PinGroupVA[LBT22013_VA_Pin_AdcBusy2];
        Adc24bit_CS5560::Pin_CS = LBT22013_Base::PinGroupVA[LBT22013_VA_Pin_AdcCS2];
        Adc24bit_CS5560::Init(eCONVCODETYPE_BIPOLAR);
#endif
    }
    else
    {
        Adc16bit_AD7606::OS0=LBT22013_Base::PinGroupVB[LBT22013_VB_Pin_OS0];       //No use
        Adc16bit_AD7606::OS1=LBT22013_Base::PinGroupVB[LBT22013_VB_Pin_OS1];       //No use
        Adc16bit_AD7606::OS2=LBT22013_Base::PinGroupVB[LBT22013_VB_Pin_OS2];       //No use
        Adc16bit_AD7606::BUSY=LBT22013_Base::PinGroupVB[LBT22013_VB_Pin_AdcBusy];  //No use
        Adc16bit_AD7606::FDATA=LBT22013_Base::PinGroupVB[LBT22013_VB_Pin_FDATA];   //No use

        Adc16bit_AD7606::CONVA=LBT22013_Base::PinGroupVB[LBT22013_VB_Pin_AdcConverA];
        Adc16bit_AD7606::CONVB=LBT22013_Base::PinGroupVB[LBT22013_VB_Pin_AdcConverB];
        Adc16bit_AD7606::Rst =LBT22013_Base::PinGroupVB[LBT22013_VB_Pin_AdcRst];
        Adc16bit_AD7606::Address=CPLD_BASE_ADDR+2;

        Adc16bit_AD7606::Init(eCONVCODETYPE_BIPOLAR);
    }
    //LBT22013_Base::ADG_Reset();
}
void LBT22013_ADC5DAC::Dac_Init(void)
{
    if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_A)
    {
        //AD5754
        DAC16bit_AD5754::Pin_DacCS = LBT22013_Base::PinGroupVA[LBT22013_VA_Pin_Dac_CS];
        DAC16bit_AD5754::Pin_DacLoad = LBT22013_Base::PinGroupVA[LBT22013_VA_Pin_Dac_Load];
        DAC16bit_AD5754::Pin_DacClr = LBT22013_Base::PinGroupVA[LBT22013_VA_Pin_Dac_Clr];
    }
    else
    {
        //AD5754
        DAC16bit_AD5754::Pin_DacBin = LBT22013_Base::PinGroupVB[LBT22013_VB_Pin_DacBIN2COMP];
        DAC16bit_AD5754::Pin_DacCS = LBT22013_Base::PinGroupVB[LBT22013_VB_Pin_DacCS];
        DAC16bit_AD5754::Pin_DacLoad = LBT22013_Base::PinGroupVB[LBT22013_VB_Pin_DacLoad];
        DAC16bit_AD5754::Pin_DacClr = LBT22013_Base::PinGroupVB[LBT22013_VB_Pin_DacCLR];
    }
    DAC16bit_AD5754::SPI_Idx = 0;

    //初始化，power up AD5754
    SSPIFunc::ConfirmDevice(0, &LBT22013_Base::LBT22013_SSP0DAC);
    DAC16bit_AD5754::Init(eCONVCODETYPE_UNIPOLAR);    // 1.27 BIN       BIN COMP
}

// unsigned long LBT22013_ADC5DAC::ADC_rd(unsigned long CH_Number,unsigned long type)
// {
// 	unsigned long Result = 0;

// 	unsigned char bOK = SSPIFunc::getModuleName(1);
// 	if(bOK  != SSP1_DEVICE1)
// 	{
// 		SSPIFunc::setPara(&LBT22013_Base::LBT22013_SSP1ADC,1);
// 		SSPIFunc::my_Init(1);
// 		ARBUS_DELAY(10);
// 	}
//
// 	switch(type)
// 	{
//         case ADC_TYPE_I:
//             Adc24bit_CS5560::Pin_Busy = LBT22013_Base::Pin_AdcBusy1;
//             Adc24bit_CS5560::Pin_CS = LBT22013_Base::Pin_AdcCS1;
//             Result = Adc24bit_CS5560::Read();
//             break;
//         case ADC_TYPE_V:
//             Adc24bit_CS5560::Pin_Busy = LBT22013_Base::Pin_AdcBusy2;
//             Adc24bit_CS5560::Pin_CS = LBT22013_Base::Pin_AdcCS2;
//             Result = Adc24bit_CS5560::Read();
//             break;
//         default:
//             break;
// 	}

// 	return Result;
// }
void LBT22013_ADC5DAC::Dac_out_Vlt(char chn,unsigned long value)
{
	//value = 0xFFFFFF-value;
	DAC16bit_AD5754::Dac_Out(chn,value);	
}
void LBT22013_ADC5DAC::Dac_out_Cur(char chn,unsigned long value)
{

    SSPIFunc::ConfirmDevice(0, &LBT22013_Base::LBT22013_SSP0DAC);
    value = 0xFFFFFF-value;
    DAC16bit_AD5754::Dac_Out(chn,value);
}

//clamp voltage
void LBT22013_ADC5DAC::Dac_out_Clamp_Vlt(char chn,unsigned long value)
{
// 	unsigned long temp = value;   // 20140613  电压保护
    SSPIFunc::ConfirmDevice(0, &LBT22013_Base::LBT22013_SSP0DAC);
    DAC16bit_AD5754::Dac_Out(chn,value);    //AD5754
}

/*********************************************************************************
**                            End Of File
*********************************************************************************/
