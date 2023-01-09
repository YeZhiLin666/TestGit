#include "includes.h"

//public:


//private:
//                                             Port      Bits       PortType           Function               Address
const St_Port   FM25L256Func::Pin_FramCS    = {0,        14,        eCPTYPE_MCUGPIO,   GPIO_O,                0};

// unsigned char   FM25L256Func::bIsBusy = 0;
SSPI_Para       FM25L256Func::FRAM_SSPI0;

//public:
void FM25L256Func::MyInit(void)
{

// Initial SSPI0 to read FRAM later, Yifei, 2016.04.29
    const St_Port SSPI0_PinGroup[3] =
    {
        {0,        15,        eCPTYPE_MCUGPIO,   SSP0_SCK,              0},
        {0,        17,        eCPTYPE_MCUGPIO,   SSP0_MISO,             0},
        {0,        18,        eCPTYPE_MCUGPIO,   SSP0_MOSI,             0},
    };
    for(int i=2; i>=0; i--)
        ControlPin::SetIObyCP(SSPI0_PinGroup[i]);

// 	bIsBusy = 0;
    ControlPin::SetIObyCP(Pin_FramCS);

    FRAM_SSPI0.uc_Module_Name = SSP0_DEVICE2;    //name
    FRAM_SSPI0.uc_Speed = 4;   // BUS Baord input 8 AUX Baord, None terminal Resistance.
    FRAM_SSPI0.uc_CPOL = 0;
    FRAM_SSPI0.uc_CPHA = 0;
    FRAM_SSPI0.uc_MSTR = 1;    //master
    FRAM_SSPI0.uc_BITS = 8;
    FRAM_SSPI0.uc_LSBF = 0;
// 	FRAM_SSPI0.st_CS.Type = GP_MOD;
// 	FRAM_SSPI0.st_CS.GPIO_PIN.P1 = 0;    //CS for for ssp0 FRAM
// 	FRAM_SSPI0.st_CS.GPIO_PIN.P2 = 14;
}

// unsigned char FM25L256Func::checkEEPROMBusy(void)
// {
// 	return bIsBusy;
// }

// unsigned char FM25L256Func::clear(unsigned char chn)
// {
// 	return 1;
// }

//private:
unsigned char FM25L256Func::writePage(UWord16 wordAddr,unsigned char *array,unsigned char length)
{
    unsigned char uc_hi, uc_lo;
    unsigned int  uw16_i;
    unsigned char *uc_array = array;
    UChar8  uc_Result = true;

    unsigned char bOK = SSPIFunc::getModuleName(0);
    if(bOK  != SSP0_DEVICE2)
    {
        SSPIFunc::setPara(&FRAM_SSPI0,0);
        SSPIFunc::my_Init(0);              //初始化
        ARBUS_DELAY(10);
    }

    uc_hi = (wordAddr & 0xff00) >> 8;    //get the high address
    uc_lo = wordAddr & 0x00ff;      //get the low  address

    EEPROM_EnableWrite();                //write enable

    ControlPin::SetMeLow(Pin_FramCS);
    SSPIFunc::SSPITxBuff[SSPIChannel_0][0] = EEPROM_WRITE;
    SSPIFunc::SSPITxBuff[SSPIChannel_0][1] = uc_hi;
    SSPIFunc::SSPITxBuff[SSPIChannel_0][2] = uc_lo;
    SSPIFunc::SSPI_SendData(SSPIChannel_0, 3 );
    for(uw16_i = 0; uw16_i < length; uw16_i ++)
    {
        SSPIFunc::SSPITxBuff[SSPIChannel_0][uw16_i] = *uc_array;
        uc_array++;
    }
    SSPIFunc::SSPI_SendData(SSPIChannel_0, length );
    ControlPin::SetMeHigh(Pin_FramCS);

    EEPROM_DisableWrite();
    return  (uc_Result);
}

unsigned char FM25L256Func::readPage(UWord16 wordAddr,unsigned char *array,unsigned char length)
{
    unsigned int uw16_i;
    unsigned char uc_lo, uc_hi;
    unsigned char *uc_array = array;
    UChar8  uc_Result = true;

    if(length > SSPI_MAX_BUFFER)
        return false;

    unsigned char bOK = SSPIFunc::getModuleName(0);
    if(bOK  != SSP0_DEVICE2)
    {
        SSPIFunc::setPara(&FRAM_SSPI0,0);
        SSPIFunc::my_Init(0);              //初始化
        ARBUS_DELAY(10);
    }

    unsigned long IRQ1,IRQ2;
    IRQFunc::DisableIRQ(&IRQ1,&IRQ2);
    uc_hi = (wordAddr & 0xff00) >> 8;    //get high address
    uc_lo = wordAddr & 0x00ff;      //get low address

    ControlPin::SetMeLow(Pin_FramCS);
    SSPIFunc::SSPITxBuff[SSPIChannel_0][0] = EEPROM_READ;
    SSPIFunc::SSPITxBuff[SSPIChannel_0][1] = uc_hi;
    SSPIFunc::SSPITxBuff[SSPIChannel_0][2] = uc_lo;
    SSPIFunc::SSPI_SendData(SSPIChannel_0, 3 );

    SSPIFunc::SSPI_SendData(SSPIChannel_0,length);
    for(uw16_i = 0; uw16_i < length; uw16_i ++)
    {
        *uc_array = SSPIFunc::SSPIRxBuff[SSPIChannel_0][uw16_i];
        uc_array ++;
    }

    ControlPin::SetMeHigh(Pin_FramCS);
    IRQFunc::EnableIRQ(IRQ1,IRQ2);
    return  (uc_Result);
}

unsigned char FM25L256Func::EEPROM_EnableWrite(void)
{
    //  铁电使能可写
    //  Returned value:    true 操作成功；false操作失败
    UChar8  uc_Result = true;

    ControlPin::SetMeLow(Pin_FramCS);
    uc_Result= SSPIFunc::RxTx_Frame(EEPROM_WREN,SSPIChannel_0);      //write enable
    ControlPin::SetMeHigh(Pin_FramCS);
    return  (uc_Result);
}

unsigned char FM25L256Func::EEPROM_DisableWrite(void)
{
    UChar8  uc_Result = true;

    ControlPin::SetMeLow(Pin_FramCS);
    uc_Result= SSPIFunc::RxTx_Frame(EEPROM_WRDI,SSPIChannel_0);      //write enable
    ControlPin::SetMeHigh(Pin_FramCS);
    return  (uc_Result);
}







