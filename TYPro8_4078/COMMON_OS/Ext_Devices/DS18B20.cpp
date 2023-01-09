#include "includes.h"
//100 5us

UInt16        	Ds18b20::Priority=PRIORITYBG;
UInt16        	Ds18b20::TaskID=DS18B20_TASK;
unsigned short 		Ds18b20::Ds18b20_Used;
unsigned char 		Ds18b20::Ds18b20_Status[MAXDITITALTEMPNUM];
float 		 		Ds18b20::Temperature[MAXDITITALTEMPNUM];
unsigned long Ds18b20::Ds18b20_Delay[MAXDITITALTEMPNUM];

//                                   Port      Bits       PortType           Function               Address
St_Port    Ds18b20::Pin_SingleBus = {0,        31,        eCPTYPE_MCUGPIO,   GPIO_O,                0};
//                                   Port      Bits       PortType           Function               Address
St_Port    Ds18b20::Pin_IODIR =     {2,        9,         eCPTYPE_MCUGPIO,   GPIO_O,                0};

//                                   Port      Bits       PortType           Function               Address
St_Port    Ds18b20::Pin_MOSFET =    {0,        21,        eCPTYPE_MCUGPIO,   GPIO_O,                0};

unsigned char    Ds18b20::bIsBusy;

void Ds18b20::DoWork(void)
{
    for(unsigned char uc_i=0; uc_i<MAXDITITALTEMPNUM; uc_i++) //不能循环暂时测试用
        Process18B20(uc_i);
}

unsigned char Ds18b20::Process18B20(unsigned char uc_channelNo)//状态机，时序处理部分可能有问题，需与硬件联调。
{
    unsigned short Bit[MAXDITITALTEMPNUM];
    unsigned char ReadLowBit[MAXDITITALTEMPNUM];
    unsigned char ReadHighBit[MAXDITITALTEMPNUM];

    if(!(Ds18b20_Used>>uc_channelNo)&0x0001)
        return 0;
    switch (Ds18b20_Status[uc_channelNo])
    {
    case DS18b20IDLE:
    case DS18b20INITA:
        if(Init18b20())
            return 0;
        else
            Ds18b20_Status[uc_channelNo]=DS18b20WRITECFG;
        break;
    case DS18b20WRITECFG:
        Write_OneBit(0xCC);
        Write_OneBit(0x44);
        Ds18b20_Status[uc_channelNo]=DS18b20WAITFORTRANSTION;
        break;
    case DS18b20WAITFORTRANSTION:
        if(Ds18b20_Delay[uc_channelNo]>T1)
        {
            Ds18b20_Status[uc_channelNo]=DS18b20WAITFORPOWERON;
            Ds18b20_Delay[uc_channelNo]=0;
        }
        else
            Ds18b20_Delay[uc_channelNo]++;
        break;
    case DS18b20WAITFORPOWERON:
        ControlPin::SetMeHigh(Pin_MOSFET,1);
        if(Ds18b20_Delay[uc_channelNo]>T2)
        {
            Ds18b20_Status[uc_channelNo]=DS18b20WAITFORPOWERON;
            Ds18b20_Delay[uc_channelNo]=0;
        }
        else
            Ds18b20_Delay[uc_channelNo]++;
        break;
    case DS18b20INITB:
        ControlPin::SetMeLow(Pin_MOSFET,1);
        if(Init18b20())
            return 0;
        else
            Ds18b20_Status[uc_channelNo]=DS18b20READTEMP;
        break;
    case DS18b20READTEMP:
        Write_OneBit(0xCC);
        Write_OneBit(0xBE);
        Ds18b20_Status[uc_channelNo]=DS18b20WATIFORREAD;
        break;
    case DS18b20WATIFORREAD:
        if(Ds18b20_Delay[uc_channelNo]>T3)
        {
            Ds18b20_Status[uc_channelNo]=DS18b20GETTEMP;
            Ds18b20_Delay[uc_channelNo]=0;
        }
        else
            Ds18b20_Delay[uc_channelNo]++;
        break;
    case DS18b20GETTEMP:
        ReadLowBit[uc_channelNo]=Read_OneBit(); // 低8位
        ReadHighBit[uc_channelNo]=Read_OneBit();//高8位
        Bit[uc_channelNo]=ReadHighBit[uc_channelNo];
        Bit[uc_channelNo]<<=8;
        Bit[uc_channelNo]=Bit[uc_channelNo]|ReadLowBit[uc_channelNo];
        if(Bit[uc_channelNo]&0x8000)
        {
            Bit[uc_channelNo]&=0x7fff;
            Temperature[uc_channelNo]=Bit[uc_channelNo]*(-0.0625);
        }
        else
            Temperature[uc_channelNo]=Bit[uc_channelNo]*(0.0625);
        break;
    default:
        return 0;
    }
    return 0;
}

unsigned char Ds18b20::SetPin(unsigned char dir)
{
    if(dir>1)
        return 0;
    if(dir==BUS_INPUT)
    {
        ControlPin::SetMeLow(Pin_IODIR,1);  //245input
        Pin_SingleBus.Function = GPIO_I;
    }
    else
    {
        ControlPin::SetMeHigh(Pin_IODIR,1);   //245output
        Pin_SingleBus.Function = GPIO_O;
    }
    ControlPin::SetIObyCP(Pin_SingleBus);
    return 1;
}

unsigned char Ds18b20::Init18b20(void)
{
    unsigned char m_return=0x01;

    Pin_MOSFET.Function=GPIO_O;
    ControlPin::SetIObyCP(Pin_MOSFET);
    ControlPin::SetMeLow(Pin_MOSFET,1);


    Pin_IODIR.Function=GPIO_O;
    ControlPin::SetIObyCP(Pin_IODIR);


    SetPin(BUS_OUTPUT);
    ControlPin::SetMeHigh(Pin_SingleBus,1);
    ARBUS_DELAY(1000);

    ControlPin::SetMeLow(Pin_SingleBus,1);
    ARBUS_DELAY(10000);
    ControlPin::SetMeHigh(Pin_SingleBus,1);


    SetPin(BUS_INPUT);
    ARBUS_DELAY(2000);
    m_return=ControlPin::ReadMe(Pin_SingleBus);
    ARBUS_DELAY(3000);

    return m_return;
}

unsigned char Ds18b20::MyInit(void)
{
    Ds18b20_Used=0;
    for(unsigned char uc_i=0; uc_i<MAXDITITALTEMPNUM; uc_i++)
    {
        Ds18b20_Status[uc_i]=DS18b20IDLE;
        Ds18b20_Delay[uc_i]=0;
        Temperature[uc_i]=0;
    }

    return 1;
}

unsigned char Ds18b20::Read_OneBit(void)
{
    unsigned char dat=0;

    for(unsigned char uc_i=8; uc_i>0; uc_i--)
    {

        SetPin(BUS_OUTPUT);
        ControlPin::SetMeLow(Pin_SingleBus,1);

        dat>>=1;
        ARBUS_DELAY(100);
        SetPin(BUS_OUTPUT);
        ControlPin::SetMeHigh(Pin_SingleBus,1);
        SetPin(BUS_INPUT);
        ARBUS_DELAY(100);

        if(ControlPin::ReadMe(Pin_SingleBus))
            dat|=0x80;
        ARBUS_DELAY(1000);
    }
    SetPin(BUS_OUTPUT);
    ControlPin::SetMeHigh(Pin_SingleBus,1);
    ARBUS_DELAY(100);
    return dat;
}

unsigned char Ds18b20::Write_OneBit(unsigned char dat)
{
    SetPin(BUS_OUTPUT);
    ControlPin::SetMeHigh(Pin_SingleBus,1);
    for(unsigned char uc_i=8; uc_i>0; uc_i--)
    {
        ControlPin::SetMeLow(Pin_SingleBus,1);
        ARBUS_DELAY(100);
        if(dat&0x01)
            ControlPin::SetMeHigh(Pin_SingleBus,1);
        else
            ControlPin::SetMeLow(Pin_SingleBus,1);
        ARBUS_DELAY(800);
        ControlPin::SetMeHigh(Pin_SingleBus,1);
        dat>>=1;
        ARBUS_DELAY(100);
    }
    return 1;
}

unsigned char Ds18b20::checkDs18b20Busy(void)
{
    ARBUS_DELAY(11000);  //10000 530us
    return 1;
}

float Ds18b20::ReadTemperature(void)//测试代码已通过
{
    unsigned short Bit;
    unsigned char ReadLowBit;
    unsigned char ReadHighBit;
    float temperature;
    if(Init18b20())
        return 0;
    Write_OneBit(0xCC);
    Write_OneBit(0x44);
    for(unsigned long uc_i=0; uc_i<200000; uc_i++)
        ARBUS_DELAY(100);

    ControlPin::SetMeHigh(Pin_MOSFET,1);
    for(unsigned long uc_i=0; uc_i<100000; uc_i++)
        ARBUS_DELAY(100);
    checkDs18b20Busy();
    ControlPin::SetMeLow(Pin_MOSFET,1);
    if(Init18b20())
        return 0;
    Write_OneBit(0xCC);
    Write_OneBit(0xBE);

    checkDs18b20Busy();
    ReadLowBit=Read_OneBit(); // 低8位
    ReadHighBit=Read_OneBit();//高8位
    Bit=ReadHighBit;
    Bit<<=8;
    Bit=Bit|ReadLowBit;
    if(Bit&0x8000)
    {
        Bit&=0x7fff;
        temperature=Bit*(-0.0625);
    }
    else
        temperature=Bit*(0.0625);
    return temperature;
}


