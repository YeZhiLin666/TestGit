
#include "includes.h"
#include "DDS_AD9834.h"

// St_Port   DDS_AD9834::Pin_SCLK;
// St_Port   DDS_AD9834::Pin_SDATA;
St_Port   DDS_AD9834::FSYNC;


void DDS_AD9834::Init(void)
{
// 	ControlPin::SetMeLow(FSYNC);
// // 	ControlPin::SetMeHigh(FSYNC);
// 	TIMERSFunc::DELAY_SYS(20, 0);
//RESET: RESET=1,PIN/SW=0
    WriteToAD9834Word(0X0100,1);
// 	ControlPin::SetMeHigh(FSYNC);
// 	TIMERSFunc::DELAY_SYS(150, 0);

// 	ControlPin::SetMeLow(FSYNC);
//Write a full 28-Bit Word to Frequency Register: B28=1, RESET=1
    WriteToAD9834Word(0X2100,1);//0X2100
//Frequency0
    WriteToAD9834Word(0X5893,1);	//0X68DB//0x9893;	//1kHz Output, 1MHz MCLK
    WriteToAD9834Word(0X4010,1);	//0X4001//0x8010 // (fMCLK / 2^28 ) * FREQREG = 1kHz  , FREQREG = 268435 = 1000001100010010011
    // 0100 0000 0001 0000    0101 1000 1001 0011
// 	ControlPin::SetMeHigh(FSYNC);
// 	TIMERSFunc::DELAY_SYS(150, 0);

//Frequency1
// 	WriteToAD9834Word(0XB5C3,1);	  //
// 	WriteToAD9834Word(0X80A3,1);
    //Phase
    WriteToAD9834Word(0XC000,1);	  //初始相位为0 0XC000
// 	WriteToAD9834Word(0XE000,1);
// 	TIMERSFunc::DELAY_SYS(150, 0);

    WriteToAD9834Word(0X2000,1);	  //

// 	WriteToAD9834Word(0X0C00,1);	  //

// 	TIMERSFunc::DELAY_SYS(150, 0);
// 	ControlPin::SetMeHigh(FSYNC);
}

void DDS_AD9834::SetFreq(float freq)
{
    if(freq<=0)
        return;

    //RESET: RESET=1,PIN/SW=0
    WriteToAD9834Word(0X0100,1);

    //Write a full 28-Bit Word to Frequency Register: B28=1, RESET=1
    WriteToAD9834Word(0X2100,1);//0X2100

    // 28 bit FREQREG, FREQREG = 2^28 * freq / 10^6, two consecutive write(14 bits each)

    unsigned int FReg_bits = freq * 268.435456f;	// = freq * 2^28 * freq / 10^6

    unsigned short LSB = (FReg_bits & 0x3FFF) | 0x4000;

    unsigned short MSB = ((FReg_bits>>14) & 0x3FFF) | 0x4000;



    WriteToAD9834Word(LSB,1);	//0X68DB//0x9893;	//976.56Hz Output, 1MHz MCLK
    WriteToAD9834Word(MSB,1);	//0X4001//0x8010 // (fMCLK / 2^28 ) * FREQREG = 0.97656kHz  , FREQREG = 262143 = 0011 1111 1111 1111 1111


    //Phase
    WriteToAD9834Word(0XC000,1);	  //初始相位为0 0XC000

    WriteToAD9834Word(0X2000,1);	  //

}

//Write AD9834 by 16 bit
//data is written at the clock's falling edge
void DDS_AD9834::WriteToAD9834Word(unsigned short Buf,unsigned char sspId)
{
    unsigned short SPI_Send_Buffer[1];
    ControlPin::SetMeLow(FSYNC);  //FSYNC become low, indicate the beginning of the write
    TIMERSFunc::DELAY_SYS(20, 0);
    SPI_Send_Buffer[0]=Buf;
    SSPIFunc::RxTx_Frames(&SPI_Send_Buffer[0],1,sspId);

    TIMERSFunc::DELAY_SYS(20, 0);
    ControlPin::SetMeHigh(FSYNC);		  	//FSYNC become high, indicate the end of the write

}
void DDS_AD9834::DDS_Output(void)
{

//Select the Frequency Source and CLEAR RESET: RESET BIT=0, FSEL=0,PSEL=0
// 	  WriteToAD9834Word(0X0000,1);
    WriteToAD9834Word(0X2000,1);
}