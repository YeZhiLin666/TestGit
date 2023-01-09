/*****************************************************************************
// �����ļ�      :ADC24bit_CS5560.cpp     ��Ҫ����ģ��ת��,��Ϊģ��������ת��Ϊ16λ��������Ӳ��оƬCS5560������
// �ļ�����      :ΪMCU board����
// ��д����      :2012.07.13
*****************************************************************************/

#include "includes.h"
#if ADC_CS5560_ENABLE == 1

unsigned char   Adc24bit_CS5560::CodeType;  //Bipolar Code Type or Unipolar Code Type;

St_Port  Adc24bit_CS5560::Pin_Conver;
St_Port  Adc24bit_CS5560::Pin_Busy;
St_Port  Adc24bit_CS5560::Pin_CS;
St_Port  Adc24bit_CS5560::Pin_Rst;

unsigned char   Adc24bit_CS5560::SPI_Idx;        //CS5560оƬʹ�õ���SPI,SSP0,SSP1,�������оƬ����ʹ�õ����ĸ�����ģ��

//put cs5560 chip into reset mode,and init all used CP pins
void Adc24bit_CS5560::Init(unsigned char ConvCodeType)
{
    switch(ConvCodeType)
    {
    case eCONVCODETYPE_BIPOLAR:
        CodeType= eCONVCODETYPE_BIPOLAR;
        break;
        //case eCONVCODETYPE_UNIPOLAR:
    default:
        CodeType = eCONVCODETYPE_UNIPOLAR;
        break;
    }

    ControlPin::SetMeHigh(Pin_CS);
    ControlPin::SetMeHigh(Pin_Conver);

    Reset();
}

void Adc24bit_CS5560::Convert(void)
{
    ControlPin::SetMeLow(Pin_Conver);
    ARBUS_DELAY(10);
    ControlPin::SetMeHigh(Pin_Conver);
}

// ͬΪ��ȡ���ݣ���������Ҫ�ȴ��ʹ���оƬæʱ�źš�
unsigned long Adc24bit_CS5560::Read(void)    //�ڶ�������Ԥ�� ����������Ҫ
{
    unsigned long int  temp = 0;    //���ص�ֵ
    unsigned long uw32_timeout;

    unsigned short Tx[3],Rx[3];
    uw32_timeout = 0;

    while(ControlPin::ReadMe(Pin_Busy))
    {
        if ( uw32_timeout ++ >= ADC_CS5560_TIME_OUT )
            return 0;
    }

    Tx[0] = 0;
    Tx[1] = 0;
    Tx[2] = 0;
    ControlPin::SetMeLow(Pin_CS);
    SSPIFunc::RxTx_Frames(Tx,3,SPI_Idx);    //�����������Ҫ����CS�ź�
    ControlPin::SetMeHigh(Pin_CS);

    for(char i =0; i<3; i++)
        Rx[i] = SSPIFunc::SSPIRxBuff[1][i];    //��������

    //��Rx��ɳ�Ϊu32
    temp |= Rx[0]<<16;
    temp |= Rx[1]<<8;
    temp |= Rx[2];

    if(CodeType == eCONVCODETYPE_BIPOLAR)
        temp  = CommonFunc::Bipolar2Unipolar24BitCode(temp);
    //else ���ADC����������ľ���Unipolar�ģ���ô��ʲôҲ��������

    return temp;
}

void Adc24bit_CS5560::Trigger(void)
{
    unsigned short Tx[3];//,Rx[3];

    Tx[0] = 0;
    Tx[1] = 0;
    Tx[2] = 0;
    SSPIFunc::ClearFIFO(SPI_Idx);
    ControlPin::SetMeLow(Pin_CS);
    SSPIFunc::WriteTxFIFO(Tx,3,SPI_Idx);
}

// ͬΪ��ȡ���ݣ������򲻵ȴ��ʹ���оƬæʱ�źš�
unsigned long Adc24bit_CS5560::ReadData(void)
{
    unsigned long int  temp = 0;    //���ص�ֵ
    unsigned short Rx[3];

    SSPIFunc::ReadRxFIFO(3,SPI_Idx);
    ControlPin::SetMeHigh(Pin_CS);

    for(char i =0; i<3; i++)
        Rx[i] = SSPIFunc::SSPIRxBuff[1][i];    //��������

    //��Rx��ɳ�Ϊu32
    temp |= Rx[0]<<16;
    temp |= Rx[1]<<8;
    temp |= Rx[2];

    if(CodeType == eCONVCODETYPE_BIPOLAR)
        temp  = CommonFunc::Bipolar2Unipolar24BitCode(temp);
    //else ���ADC����������ľ���Unipolar�ģ���ô��ʲôҲ��������

    return temp;
}

void Adc24bit_CS5560::Reset(void)
{
    ControlPin::SetMeLow(Pin_Rst);
    TIMERSFunc::DELAY_SYS(200, 0);
    ControlPin::SetMeHigh(Pin_Rst);
}

#endif
