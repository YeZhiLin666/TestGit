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

const St_Port HighMode2ndV_Base::Pin_SCK1      = {0,        7,         eCPTYPE_MCUGPIO,   SSP1_SCK,              0};
const St_Port HighMode2ndV_Base::Pin_MISO1     = {0,        8,         eCPTYPE_MCUGPIO,   SSP1_MISO,             0};
const St_Port HighMode2ndV_Base::Pin_MOSI1     = {0,        9,         eCPTYPE_MCUGPIO,   SSP1_MOSI,             0};

const St_Port HighMode2ndV_Base::Pin_ECODE1		= {1,        18,        eCPTYPE_MCUGPIO,   GPIO_O,                0};
const St_Port HighMode2ndV_Base::Pin_ECODE2		= {1,        19,        eCPTYPE_MCUGPIO,   GPIO_O,                0};
const St_Port HighMode2ndV_Base::Pin_ECODE3		= {1,        20,        eCPTYPE_MCUGPIO,   GPIO_O,                0};
const St_Port HighMode2ndV_Base::Pin_EOE		= {1,        21,        eCPTYPE_MCUGPIO,   GPIO_O,                0};
const St_Port HighMode2ndV_Base::Pin_ADCBUSY	= {2,        12,        eCPTYPE_MCUGPIO,   GPIO_I,                0};
const St_Port HighMode2ndV_Base::Pin_Cover		= {1,        28,        eCPTYPE_MCUGPIO,   GPIO_O,                0};
const St_Port HighMode2ndV_Base::Pin_CS				= {2,        8,        eCPTYPE_MCUGPIO,   GPIO_O,                0};


SSPI_Para HighMode2ndV_Base::HighMode2ndV_SSP1ADC;

void HighMode2ndV_Base::IO_Init(unsigned char BoardID)
{
    ControlPin::SetIObyCP(Pin_MISO1);
    ControlPin::SetIObyCP(Pin_MOSI1);
    ControlPin::SetIObyCP(Pin_SCK1);
    ControlPin::SetIObyCP(Pin_EOE);
    ControlPin::SetIObyCP(Pin_ECODE1);
    ControlPin::SetIObyCP(Pin_ECODE2);
    ControlPin::SetIObyCP(Pin_ECODE3);
    ControlPin::SetIObyCP(Pin_ADCBUSY);
    ControlPin::SetIObyCP(Pin_CS);
    ControlPin::SetIObyCP(Pin_Cover);

}
void HighMode2ndV_Base::ADC_Init(unsigned char BoardID)
{
    HighMode2ndV_ADC5DAC::Adc_Init();


}
void HighMode2ndV_Base::Func_Init(unsigned long BoardID,unsigned long ADCID,unsigned long ChannelID)
{

    board_config::Func_AUX_ADC_rd[BoardID][ADCID][ChannelID]=&HighMode2ndV_Base::ADC_rd;
    board_config::Func_AUX_ADC_CS[BoardID][ADCID][ChannelID]=&HighMode2ndV_Base::ADC_Cs;
    board_config::Func_AUX_Sample[BoardID][ADCID][ChannelID]=&HighMode2ndV_SampleManage::SampleISR;
    board_config::Func_AUX_CaliChannel[BoardID][ADCID][ChannelID]=&LTC2449::CaliChannel_2ndV;
}
void HighMode2ndV_Base::Hardware_Init(unsigned char BoardID)
{
    IO_Init(BoardID);
    HighMode2ndV_SSP1ADC.uc_Module_Name = SSP1_DEVICE0;
    HighMode2ndV_SSP1ADC.uc_Speed = 2;
    HighMode2ndV_SSP1ADC.uc_CPOL = 0;
    HighMode2ndV_SSP1ADC.uc_CPHA = 1;
    HighMode2ndV_SSP1ADC.uc_MSTR = 1;    //master
    HighMode2ndV_SSP1ADC.uc_BITS = 8;
    HighMode2ndV_SSP1ADC.uc_LSBF = 0;

    ControlPin::SetMeLow(Pin_EOE,0);
    ControlPin::SetMeLow(Pin_ECODE1,0);
    ControlPin::SetMeLow(Pin_ECODE2,0);
    ControlPin::SetMeLow(Pin_ECODE3,0);
    HighMode2ndV_ADC5DAC::Adc_Init();

}
void HighMode2ndV_Base::Software_Init(unsigned char Board_ID)
{
    for(unsigned char uc_i=0; uc_i<MAX_AUXCH_PER_BOARD; uc_i++)
    {
        Func_Init(Board_ID,0,uc_i);
        Aux_DataManager::ADC_SampleProcessType[Board_ID][0][uc_i]=SAMPLEISR;   //注册为中断类型采样处理
    }

    HighMode2ndV_SampleManage::Init(Board_ID);
}
char HighMode2ndV_Base::ADC_rd(unsigned long BoardID,unsigned long ADCID,unsigned long ChannelID)
{
    return 0;
}

char HighMode2ndV_Base::ADC_Cs(unsigned long BoardID,unsigned long ADCID,unsigned long ChannelID)
{
    if(ChannelID>=MAX_AUXCH_PER_BOARD)
        return 0;

    switch(ChannelID)
    {
    case 0:
        ControlPin::SetMeLow(HighMode2ndV_Base::Pin_EOE,0);
        ControlPin::SetMeLow(HighMode2ndV_Base::Pin_ECODE1,0);
        ControlPin::SetMeLow(HighMode2ndV_Base::Pin_ECODE2,0);
        ControlPin::SetMeLow(HighMode2ndV_Base::Pin_ECODE3,0);
        break;
    case 1:
        ControlPin::SetMeLow(HighMode2ndV_Base::Pin_EOE,0);
        ControlPin::SetMeHigh(HighMode2ndV_Base::Pin_ECODE1,1);
        ControlPin::SetMeLow(HighMode2ndV_Base::Pin_ECODE2,0);
        ControlPin::SetMeLow(HighMode2ndV_Base::Pin_ECODE3,0);
        break;
    case 2:
        ControlPin::SetMeLow(HighMode2ndV_Base::Pin_EOE,0);
        ControlPin::SetMeLow(HighMode2ndV_Base::Pin_ECODE1,0);
        ControlPin::SetMeHigh(HighMode2ndV_Base::Pin_ECODE2,1);
        ControlPin::SetMeLow(HighMode2ndV_Base::Pin_ECODE3,0);
        break;
    case 3:
        ControlPin::SetMeLow(HighMode2ndV_Base::Pin_EOE,0);
        ControlPin::SetMeHigh(HighMode2ndV_Base::Pin_ECODE1,1);
        ControlPin::SetMeHigh(HighMode2ndV_Base::Pin_ECODE2,1);
        ControlPin::SetMeLow(HighMode2ndV_Base::Pin_ECODE3,0);
        break;
    case 4:
        ControlPin::SetMeLow(HighMode2ndV_Base::Pin_EOE,0);
        ControlPin::SetMeLow(HighMode2ndV_Base::Pin_ECODE1,0);
        ControlPin::SetMeLow(HighMode2ndV_Base::Pin_ECODE2,0);
        ControlPin::SetMeHigh(HighMode2ndV_Base::Pin_ECODE3,1);
        break;
    case 5:
        ControlPin::SetMeLow(HighMode2ndV_Base::Pin_EOE,0);
        ControlPin::SetMeHigh(HighMode2ndV_Base::Pin_ECODE1,1);
        ControlPin::SetMeLow(HighMode2ndV_Base::Pin_ECODE2,0);
        ControlPin::SetMeHigh(HighMode2ndV_Base::Pin_ECODE3,1);
        break;
    case 6:
        ControlPin::SetMeLow(HighMode2ndV_Base::Pin_EOE,0);
        ControlPin::SetMeLow(HighMode2ndV_Base::Pin_ECODE1,0);
        ControlPin::SetMeHigh(HighMode2ndV_Base::Pin_ECODE2,1);
        ControlPin::SetMeHigh(HighMode2ndV_Base::Pin_ECODE3,1);
        break;
    case 7:
        ControlPin::SetMeLow(HighMode2ndV_Base::Pin_EOE,0);
        ControlPin::SetMeHigh(HighMode2ndV_Base::Pin_ECODE1,1);
        ControlPin::SetMeHigh(HighMode2ndV_Base::Pin_ECODE2,1);
        ControlPin::SetMeHigh(HighMode2ndV_Base::Pin_ECODE3,1);
        break;
    case 8:
        ControlPin::SetMeHigh(HighMode2ndV_Base::Pin_EOE,1);
        ControlPin::SetMeLow(HighMode2ndV_Base::Pin_ECODE1,0);
        ControlPin::SetMeLow(HighMode2ndV_Base::Pin_ECODE2,0);
        ControlPin::SetMeLow(HighMode2ndV_Base::Pin_ECODE3,0);
        break;
    case 9:
        ControlPin::SetMeHigh(HighMode2ndV_Base::Pin_EOE,1);
        ControlPin::SetMeHigh(HighMode2ndV_Base::Pin_ECODE1,1);
        ControlPin::SetMeLow(HighMode2ndV_Base::Pin_ECODE2,0);
        ControlPin::SetMeLow(HighMode2ndV_Base::Pin_ECODE3,0);
        break;
    case 10:
        ControlPin::SetMeHigh(HighMode2ndV_Base::Pin_EOE,1);
        ControlPin::SetMeLow(HighMode2ndV_Base::Pin_ECODE1,0);
        ControlPin::SetMeHigh(HighMode2ndV_Base::Pin_ECODE2,1);
        ControlPin::SetMeLow(HighMode2ndV_Base::Pin_ECODE3,0);
        break;
    case 11:
        ControlPin::SetMeHigh(HighMode2ndV_Base::Pin_EOE,1);
        ControlPin::SetMeHigh(HighMode2ndV_Base::Pin_ECODE1,1);
        ControlPin::SetMeHigh(HighMode2ndV_Base::Pin_ECODE2,1);
        ControlPin::SetMeLow(HighMode2ndV_Base::Pin_ECODE3,0);
        break;
    case 12:
        ControlPin::SetMeHigh(HighMode2ndV_Base::Pin_EOE,1);
        ControlPin::SetMeLow(HighMode2ndV_Base::Pin_ECODE1,0);
        ControlPin::SetMeLow(HighMode2ndV_Base::Pin_ECODE2,0);
        ControlPin::SetMeHigh(HighMode2ndV_Base::Pin_ECODE3,1);
        break;
    case 13:
        ControlPin::SetMeHigh(HighMode2ndV_Base::Pin_EOE,1);
        ControlPin::SetMeHigh(HighMode2ndV_Base::Pin_ECODE1,1);
        ControlPin::SetMeLow(HighMode2ndV_Base::Pin_ECODE2,0);
        ControlPin::SetMeHigh(HighMode2ndV_Base::Pin_ECODE3,1);
        break;
    case 14:
        ControlPin::SetMeHigh(HighMode2ndV_Base::Pin_EOE,1);
        ControlPin::SetMeLow(HighMode2ndV_Base::Pin_ECODE1,0);
        ControlPin::SetMeHigh(HighMode2ndV_Base::Pin_ECODE2,1);
        ControlPin::SetMeHigh(HighMode2ndV_Base::Pin_ECODE3,1);
        break;
    case 15:
        ControlPin::SetMeHigh(HighMode2ndV_Base::Pin_EOE,1);
        ControlPin::SetMeHigh(HighMode2ndV_Base::Pin_ECODE1,1);
        ControlPin::SetMeHigh(HighMode2ndV_Base::Pin_ECODE2,1);
        ControlPin::SetMeHigh(HighMode2ndV_Base::Pin_ECODE3,1);
        break;
    default:
        break;
    }

    return 0;
}

