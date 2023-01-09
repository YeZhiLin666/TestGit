#pragma once
#define MAXDITITALTEMPNUM 16	//最大支持16个18b20
#define T1       	200000
#define T2		100000
#define T3		11000
enum
{
    BUS_INPUT,
    BUS_OUTPUT
};

enum
{
    DS18b20IDLE=0,
    DS18b20INITA,
    DS18b20WRITECFG,
    DS18b20WAITFORTRANSTION,
    DS18b20WAITFORPOWERON,
    DS18b20INITB,
    DS18b20READTEMP,
    DS18b20WATIFORREAD,
    DS18b20GETTEMP
};

class Ds18b20
{
private:
    static UInt16        Priority;
    static UInt16        TaskID;
private:
    static St_Port    	Pin_SingleBus;
    static St_Port    	Pin_IODIR;
    static St_Port    	Pin_MOSFET;
    static unsigned char    bIsBusy;
private:
    static unsigned short Ds18b20_Used;
    static unsigned char Ds18b20_Status[MAXDITITALTEMPNUM];
    static unsigned long Ds18b20_Delay[MAXDITITALTEMPNUM];
    static unsigned char SetPin(unsigned char dir);
    static unsigned char Read_OneBit(void);
    static unsigned char Write_OneBit(unsigned char dat);
public:
    static unsigned char MyInit(void);
    static unsigned char Init18b20(void);
    static void 		 DoWork(void);
    static float Temperature[MAXDITITALTEMPNUM];
public:
    static unsigned char Process18B20(unsigned char uc_channelNo);
    static unsigned char checkDs18b20Busy(void);
    static float 		ReadTemperature(void);

};
