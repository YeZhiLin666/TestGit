#include "includes.h"
////////////////////////////////////////////////////DAC


void HPT200_ADC5DAC::Adc_Init(void)
{
    unsigned short AdcSSP;
    AdcSSP=0x0004;   //后续要从配置文件中读出   dirui  2015/03/20

    //SETTING TIME(tset)(ms)
    //			 										sinc1                   sinc2
    // SPS    10								100											200
    //				16.6							60.3										120
    //				50								20.3										40.4
    //				60								17.0										33.7
    //				400								2.85										5.42
    //				1200							1.18										2.10
    //				3600							0.632										0.980
    //				14400							0.424										0.563

    unsigned short count[8] = {4500, 2500, 810, 700, 110, 45, 20, 15};	// 10Hz, 16.6Hz, 50Hz, 60Hz, 400Hz, 1200Hz, 3600Hz, 14400Hz
    if(AdcSSP>7)
        HPT200_SampleManage::WaitForSampleCounter = 4500;
    else
        HPT200_SampleManage::WaitForSampleCounter = count[AdcSSP];

    //	switch(AdcSSP)
    //	{
    //		case 0:
    //			 HPT200_SampleManage::WaitForSampleCounter=4500;  //10HZ
    //			break;
    //		case 1:
    //			 HPT200_SampleManage::WaitForSampleCounter=2500;  //16.6HZ
    //			break;
    //		case 2:
    //			 HPT200_SampleManage::WaitForSampleCounter=810;  //50HZ
    //			break;
    //		case 3:
    //			 HPT200_SampleManage::WaitForSampleCounter=700;  //60HZ
    //			break;
    //		case 4:
    //			 HPT200_SampleManage::WaitForSampleCounter=110;  //400HZ
    //			break;
    //		case 5:
    //			 HPT200_SampleManage::WaitForSampleCounter=45;  //1200HZ
    //		  break;
    //		case 6:
    //			 HPT200_SampleManage::WaitForSampleCounter=20;  //3600HZ
    //			break;
    //		case 7:
    //			 HPT200_SampleManage::WaitForSampleCounter=15;  //14400HZ
    //			break;
    //		default:
    //			 HPT200_SampleManage::WaitForSampleCounter=4500;
    //			break;
    //	}
    SSPIFunc::setPara(&HPT200_Base::HPT200_SSP1ADC,1);
    SSPIFunc::my_Init(1);              //初始化
    Adc24bit_ADS1259::Pin_Conver=HPT200_Base::PinGroup[HPT200_Pin_Cover];

    Adc24bit_ADS1259::Pin_CS= HPT200_Base::PinGroup[HPT200_Pin_CS_C];
    Adc24bit_ADS1259::Init(eCONVCODETYPE_BIPOLAR,AdcSSP);   //初始化电流
    Adc24bit_ADS1259::WriteReg(1);

    Adc24bit_ADS1259::Pin_CS= HPT200_Base::PinGroup[HPT200_Pin_CS_V];
    Adc24bit_ADS1259::Init(eCONVCODETYPE_BIPOLAR,AdcSSP);
    Adc24bit_ADS1259::WriteReg(1);      //初始化电压

}

void HPT200_ADC5DAC::Dac_Init(void)
{
    //AD5754
    DAC16bit_AD5754::Pin_DacBin = HPT200_Base::PinGroup[HPT200_Pin_DacBIN2COMP];
    DAC16bit_AD5754::Pin_DacCS = HPT200_Base::PinGroup[HPT200_Pin_DacCS];
    DAC16bit_AD5754::Pin_DacLoad = HPT200_Base::PinGroup[HPT200_Pin_DacLoad];
    DAC16bit_AD5754::Pin_DacClr = HPT200_Base::PinGroup[HPT200_Pin_DacCLR];
    DAC16bit_AD5754::SPI_Idx = 0;

    //初始化，power up AD5754
    SSPIFunc::ConfirmDevice(0, &HPT200_Base::HPT200_SSP0DAC);
    DAC16bit_AD5754::Init(eCONVCODETYPE_UNIPOLAR);    // 1.27 BIN     BIN COMP
}

void HPT200_ADC5DAC::Dac_out_Cur(char chn,unsigned long value)
{
    SSPIFunc::ConfirmDevice(0, &HPT200_Base::HPT200_SSP0DAC);
    DAC16bit_AD5754::Dac_Out(0,value);
}

//电压为ADC1
void HPT200_ADC5DAC::Dac_out_Vlt(char chn,unsigned long value)
{
    SSPIFunc::ConfirmDevice(0, &HPT200_Base::HPT200_SSP0DAC);
    DAC16bit_AD5754::Dac_Out(1,value);
}

//clamp voltage
void HPT200_ADC5DAC::Dac_out_Clamp_Vlt(char chn,unsigned long value)
{
    SSPIFunc::ConfirmDevice(0, &HPT200_Base::HPT200_SSP0DAC);
    DAC16bit_AD5754::Dac_Out(chn,value);    //AD5754

}

/*********************************************************************************
**                            End Of File
*********************************************************************************/
