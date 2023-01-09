
#include "includes.h"


St_Port  Adc32bit_LTC2449::Pin_CS;
St_Port  Adc32bit_LTC2449::Pin_Busy;

unsigned char Adc32bit_LTC2449::CodeType;    //Bipolar Code Type or Unipolar Code Type;    预留而已
//unsigned char sspIdx;         //ssp口


//做类似于IV的
unsigned long Adc32bit_LTC2449::Trigger(unsigned char sspId,unsigned char ADCchn)    //first time no return value
{
    unsigned short datasent[4];

    // 1st 4 bits: 1011: 1,0,1-enable setting, 1-Single end
    // 2nd 4 bits: set ODD,A2,A1,A0 bits. channel # is actually ODD,A2,A1,A0
    datasent[0] = 0xB0 | (((ADCchn >> 1) + (ADCchn << 3)) & 0x0F);
    datasent[1] = 0x10;		// 0001 0000: 3.52Khz [bits: OS3,OS2,OS1,OS0,TWOX].
    datasent[2] = 0x00;
    datasent[3] = 0x00;

    SSPIFunc::WriteTxFIFO(datasent,4,sspId);    //使用sspIdx去读取，并占用此总线
    return 1;
}

//0xFFFFFF    0.5Vref
//0x000000  -0.5Vref
//0x800000   0
unsigned long Adc32bit_LTC2449::Read(unsigned char sspId)
{
    UInt32 datareceived;
    unsigned short Rx[4];

    SSPIFunc::ReadRxFIFO(4,sspId);

    for(char i =0; i<4; i++)
        Rx[i] = SSPIFunc::SSPIRxBuff[sspId][i];    //接收数据

    //把Rx组成成为u32
    //这里需要仔细测试验证
    datareceived |= Rx[0]<<24;
    datareceived |= Rx[1]<<16;
    datareceived |= Rx[2]<<8;
    datareceived |= Rx[3];

    if(datareceived)
    {
        if((datareceived & 0x80000000) != 0)		// bit 31 != 0, means not a valid data
        {
            datareceived = 0;//0xFFFFFFFF;
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

void Adc32bit_LTC2449::setCS(const St_Port * cs)
{
    Pin_CS = *cs;
}

St_Port Adc32bit_LTC2449::getCS(void)
{
    return Pin_CS;
}

