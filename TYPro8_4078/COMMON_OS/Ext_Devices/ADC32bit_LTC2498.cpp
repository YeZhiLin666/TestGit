/*****************************************************************************
// �����ļ�      :ADC32bit_LTC2498.cpp     ��Ҫ����ģ��ת��,��Ϊģ��������ת��Ϊ32λ��������Ӳ��оƬLTC2498������
// �ļ�����      :ΪMCU board����
// ��д����      :2012.07.13
*****************************************************************************/

#include "includes.h"

St_Port  Adc32bit_LTC2498::Pin_CS;
St_Port  Adc32bit_LTC2498::Pin_Busy;
unsigned char Adc32bit_LTC2498::IsExCrystalsFlag=0;

//unsigned char Adc32bit_LTC2498::CodeType;    //Bipolar Code Type or Unipolar Code Type;    Ԥ������

//���ʹ��internal serial clockģʽ��SSP SCK�������������״̬�������ǰ�sck������ltc2498������
//���ܻᵼ��LPC4078 sck���������
//��������ֻ��ʹ��external�ⲿ����ʱ��ģʽ������ʹ��ʱ����Ҫ���ò���Ϊ

unsigned long Adc32bit_LTC2498::Trigger(unsigned char sspId,unsigned char ADCchn)    //first time no return value
{
    unsigned short datasent[4];

    // 1st 4 bits: 1011: 1,0,1-enable setting, 1-Single end
    // 2nd 4 bits: set ODD,A2,A1,A0 bits. channel # is actually ODD,A2,A1,A0
    if(ADCchn < 16)	// if not ref temp
    {
        datasent[0] = 0xB0 | (((ADCchn >> 1) + (ADCchn << 3)) & 0x0F);
        datasent[1] = 0x80;		// 1000: 1-En2, 0-IM clear, 00-50/60hz rejection. 0-Spd = 1x. rest: ignore.
    }
    else // if ref temp.  >=16chn
    {
        datasent[0] = 0xB0;
        datasent[1] = 0xC0;		// 1100: 1-En2, 1-IM set, 00-50/60hz rejection. 0-Spd = 1x. rest: ignore.
    }
    datasent[2] = 0x00;
    datasent[3] = 0x00;

    SSPIFunc::WriteTxFIFO(datasent,4,sspId);    //ʹ��sspIdxȥ��ȡ����ռ�ô�����
    return 1;
}

unsigned long Adc32bit_LTC2498::Read(unsigned char sspId)
{
    unsigned long datareceived;
    unsigned short Rx[4];

    SSPIFunc::ReadRxFIFO(4,sspId);

    for(char i =0; i<4; i++)
        Rx[i] = SSPIFunc::SSPIRxBuff[sspId][i];    //��������

    //��Rx��ɳ�Ϊu32
    //������Ҫ��ϸ������֤
    datareceived |= Rx[0]<<24;
    datareceived |= Rx[1]<<16;
    datareceived |= Rx[2]<<8;
    datareceived |= Rx[3];

    if(datareceived)
    {
        if((datareceived & 0x80000000) != 0)		// bit 31 != 0, means not a valid data
        {
            return 0;
        }
        else
        {
            if((datareceived & 0xF0000000) == 0x30000000)  // higher beyond range value ( > 0.5 Vref )is set to uttermost high value -> 0.5 Vref
            {
                datareceived = 0x2FFFFFFF;
            }
            if((datareceived & 0xF0000000) == 0x00000000)	// lower beyond range value( < 0.5 Vref ) is set to uttermost low value -> -0.5 Vref
            {
                datareceived = 0x10000000;
            }
        }
        datareceived -= 0x10000000;
        datareceived >>= 5;
    }
    else    //no data
    {
        datareceived = 0;    //0xFFFFFFFF;
    }

    return datareceived;
}





