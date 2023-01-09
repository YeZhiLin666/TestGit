#include "includes.h"

unsigned char DAC16bit_AD5686::CodeType;  //Offset Code Type or 2's Comp Code Type;
unsigned char DAC16bit_AD5686::SPI_Idx;

St_Port   DAC16bit_AD5686::Pin_DacCS;
St_Port   DAC16bit_AD5686::Pin_DacLoad;
St_Port   DAC16bit_AD5686::Pin_DacRst;

void DAC16bit_AD5686::Init(unsigned char ConvCodeType)
{
    switch(ConvCodeType)
    {
    case eCONVCODETYPE_BIPOLAR:
        CodeType= eCONVCODETYPE_BIPOLAR;
        break;
    case eCONVCODETYPE_UNIPOLAR:
    default:
        CodeType = eCONVCODETYPE_UNIPOLAR;
        break;
    }

    ControlPin::SetMeLow(Pin_DacRst);
    TIMERSFunc::DELAY_SYS( 1000, 0 );    //20ns
    ControlPin::SetMeHigh(Pin_DacRst);

    //��ʼ����power up AD5686
    UWord16 comand;
    unsigned short SPI_Send_Buffer[3];

    TIMERSFunc::DELAY_SYS( 30000, 0 );	//delay 5ms

    comand = Power_up_and_down_AD5686;
    SPI_Send_Buffer[0] = comand;      // ��λ���У���λ���3*8
    SPI_Send_Buffer[1] = 0x00;       //  ����
    SPI_Send_Buffer[2] = 0x00;       //  �ض�ģʽ0Ϊ����ģʽ
    ControlPin::SetMeLow(Pin_DacCS);
    SSPIFunc::RxTx_Frames(SPI_Send_Buffer,3,SPI_Idx);    //send data
    ControlPin::SetMeHigh(Pin_DacCS);

    TIMERSFunc::DELAY_SYS( 30000, 0 );	//delay 5ms

    comand = Internal_Ref_setup_register_AD5686;
    SPI_Send_Buffer[0] = comand;      //  ��λ���У���λ���
    SPI_Send_Buffer[1] = 0x00;        //  ����
    SPI_Send_Buffer[2] = 0x00;        //  �ڲ��ο�Դ
    ControlPin::SetMeLow(Pin_DacCS);
    SSPIFunc::RxTx_Frames(SPI_Send_Buffer,3,SPI_Idx);    //send data
    ControlPin::SetMeHigh(Pin_DacCS);

    TIMERSFunc::DELAY_SYS( 30000, 0 );	//delay 5ms

    ControlPin::SetMeLow(Pin_DacLoad);    //ʹ�õڶ���ģʽͬ��ģʽ
}

///////////////////////////////////////////////////////////////DAC
/*****************************************************************************
** Function name:		Dac_Out
**
** Descriptions:		Set the DAC value, the first
**				        parameter is DAC channel No., the 2nd
**				        parameter is DAC value.
**
** parameters:		uc_Channel��ͨ����
**                              uw16_Value����ֵ
** Returned value:	  uc_Result: true �����ɹ���false����ʧ��
**
*****************************************************************************/
// ͬΪ������ݣ������������Ҫ�ر�оƬƬѡ�źš�
UChar8 DAC16bit_AD5686::Dac_Out(unsigned char chn,unsigned long value)
{
    unsigned long DacCode = 0,TimeOut = 0;
    UWord16 comand;
    unsigned short SPI_Send_Buffer[3];

    if(CodeType == eCONVCODETYPE_BIPOLAR)
    {
        signed long sValue = CommonFunc::Unipolar2Bipolar24BitCode(value);
        DacCode = (unsigned long)(sValue >> 8);     //PRO7��λ�������ı�׼24BIT����ת��Ϊ
    }
    else
        DacCode = value >> 8;                       //PRO7��λ�������ı�׼24BIT����ת��Ϊ

    comand = Write_and_Updata_DAC_n_AD5686 + chn;
    SPI_Send_Buffer[0] = comand;
    SPI_Send_Buffer[1] = (DacCode>>8);    //����ȥ
    SPI_Send_Buffer[2] = (UChar8) (0x00FF & DacCode);

    while(SSPIFunc::bBusy[SPI_Idx])    //block here to wait for SSP idle
    {
        TimeOut++;
        if(TimeOut > 0x0FFFFF)
            return 0;
    }
    ControlPin::SetMeLow(Pin_DacCS);
    SSPIFunc::RxTx_Frames(SPI_Send_Buffer,3,SPI_Idx);    //0��ͨ��
    ControlPin::SetMeHigh(Pin_DacCS);

    return 1;
}

// ͬΪ������ݣ������򲻶�оƬƬѡ�źŲ�����
void DAC16bit_AD5686::Dac_SendData(unsigned char chn,unsigned long value)
{
    unsigned long DacCode = 0,TimeOut = 0;
    UWord16 comand;
    unsigned short SPI_Send_Buffer[3];

    if(CodeType == eCONVCODETYPE_BIPOLAR)
    {
        signed long sValue = CommonFunc::Unipolar2Bipolar24BitCode(value);
        DacCode = (unsigned long)(sValue >> 8);     //PRO7��λ�������ı�׼24BIT����ת��Ϊ
    }
    else
        DacCode = value >> 8;                       //PRO7��λ�������ı�׼24BIT����ת��Ϊ

    comand = Write_and_Updata_DAC_n_AD5686 + chn;
    SPI_Send_Buffer[0] = comand;
    SPI_Send_Buffer[1] = (DacCode>>8);    //����ȥ
    SPI_Send_Buffer[2] = (UChar8) (0x00FF & DacCode);

    while(SSPIFunc::bBusy[SPI_Idx])    //block here to wait for SSP idle
    {
        TimeOut++;
        if(TimeOut > 0x0FFFFF)
            return;
    }
    ControlPin::SetMeLow(Pin_DacCS);
    SSPIFunc::WriteTxFIFO(SPI_Send_Buffer,3,SPI_Idx);
    //ControlPin::SetMeHigh(Pin_DacCS);
}
void DAC16bit_AD5686::Dac_SetLoad(void)
{
    ControlPin::SetMeHigh(Pin_DacCS);
}

/*********************************************************************************
**                            End Of File
*********************************************************************************/

