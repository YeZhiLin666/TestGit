
#include "../../COMMON/includes.h"

unsigned char Adc_AD7190::diff;

void Adc_AD7190::Adc_Init(unsigned char Device_Number)
{
    Adc_AD7190::diff = 8;    //24bit

    //AUTO_SSP0_MODULE_NAME

    Reset();    //上电开始直接复位    写入40个1

    TIMERSFunc::DELAY_SYS(30000);    //500us

    //设置工作方式及连续转换模式    MR寄存器
    //设置配置寄存器并使能相应通道    CON寄存器
    //使用外部时钟
    AD7190_SetRegisterValue(AD7190_REG_MODE,0x100060,3,1);

    AD7190_RangeSetup(1,0);    //单极，gain为1
    //AD7190_ChannelSelect(0);    //AN1 AN2    差分输入
    //AD7190_ChannelSelect(1);    //AN3 AN4    差分输入
    AD7190_SetRegisterValue(AD7190_REG_CONF,0x100318,3,1);    //使能2个通道
}

//全差分输入对
//1，2一对   3，4一对组成I和V的测量
//不管是此口作为GPIO还是其他所选择的，都可以从IOPIN读入
//只有作为内部ADC的时候，才会无效
unsigned long int  Adc_AD7190::AD7190_Read(char chn,char IVtype)
{
    //wait RDY    WaitRdyGoLow();
    //read
    //好像只能通过判断状态寄存器来判断数据寄存器内通道

    unsigned char bOK = SSPIFunc::getModuleName(0);
    if(bOK  != AUTO_SSP0_MODULE_NAME)    //AD7190    SSP0
    {
        //重新初始化
        //SPI

        pSSPI0->CR0 |= 0x0000;       //复位SPI
        pSSPI0->DR = 0x0000;
        SSPI_Para Auto_SPP0;
        Auto_SPP0.uc_Module_Name = AUTO_SSP0_MODULE_NAME;    //name
        Auto_SPP0.uc_Speed = 5;    //5;
        Auto_SPP0.uc_CPOL = 1;
        Auto_SPP0.uc_CPHA = 1;
        Auto_SPP0.uc_MSTR = 1;    //master
        Auto_SPP0.uc_BITS = 8;
        Auto_SPP0.uc_LSBF = 0;
        Auto_SPP0.st_CS.Type = GP_MOD;     //gpio mod
        Auto_SPP0.st_CS.GPIO_PIN.P1 = 1;
        Auto_SPP0.st_CS.GPIO_PIN.P2 = 21;    //AD5754使用DCS3  P0.16

        SSPIFunc::setPara(&Auto_SPP0,1);    //设置参数
        SSPIFunc::my_Init(1);              //初始化
    }

    //conv?
    //WaitRdyGoLow();
    return AD7190_SingleConversion();
}

unsigned char Adc_AD7190::WaitRdyGoLow(void)
{
    // 	unsigned long uw32_timeout;
    // 	//p1.23
    // 	while(FIO1PIN & AD7190_RDY_STATE)
    // 	{
    // 		if ( uw32_timeout ++ >= ADC_AD7190_TIME_OUT )
    // 		{
    // 			return 0;
    // 		}
    // 	}
    //上面的不work  下面的读取AD7190_REG_STAT的work

    unsigned char temp =
        AD7190_GetRegisterValue(AD7190_REG_STAT,1,1);
    //当temp 第一位是0的时候判断最后3位来确定当前寄存器内通道
    //如果返回值是0，那么认为没有转换成功，可以舍弃此数据
    //comment out chen 20130514
    //这样也不用花时间去等待RDY

    if((temp >>7) == 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
    //   unsigned char temp = AD7190_GetRegisterValue(AD7190_REG_STAT,1,1);
    // 	unsigned long uw32_timeout = 0;
    //  	while(FIO1PIN & AD7190_RDY_STATE)
    //  	{
    //  		if ( uw32_timeout ++ >= ADC_AD7190_TIME_OUT )
    //  		{
    //  			return 0;
    //  		}
    //  	}

    // 	return 1;
}

void Adc_AD7190::Reset(void)
{
    unsigned short registerWord[7];

    registerWord[0] = 0x01;
    registerWord[1] = 0xFF;
    registerWord[2] = 0xFF;
    registerWord[3] = 0xFF;
    registerWord[4] = 0xFF;
    registerWord[5] = 0xFF;
    registerWord[6] = 0xFF;

    //SPI write
    SSPIFunc::RxTx_Frames_1CS(registerWord,7,1);
}

void Adc_AD7190::AD7190_RangeSetup(unsigned char polarity, unsigned char range)
{
    unsigned long oldRegValue = 0x0;
    unsigned long newRegValue = 0x0;

    oldRegValue = AD7190_GetRegisterValue(AD7190_REG_CONF,3, 1);
    oldRegValue &= ~(AD7190_CONF_UNIPOLAR |
                     AD7190_CONF_GAIN(0x7));
    newRegValue = oldRegValue |
                  (polarity * AD7190_CONF_UNIPOLAR) |
                  AD7190_CONF_GAIN(range);
    AD7190_SetRegisterValue(AD7190_REG_CONF, newRegValue, 3, 1);
}

void Adc_AD7190::AD7190_SetRegisterValue(unsigned char registerAddress,
        unsigned long registerValue,
        unsigned char bytesNumber,
        unsigned char modifyCS)
{
    unsigned short writeCommand[5] = {0, 0, 0, 0, 0};
    unsigned char* dataPointer    = (unsigned char*)&registerValue;
    unsigned char bytesNr         = bytesNumber;

    //writeCommand[0] = 0x01 * modifyCS;
    writeCommand[0] = AD7190_COMM_WRITE |
                      AD7190_COMM_ADDR(registerAddress);
    while(bytesNr > 0)
    {
        writeCommand[bytesNr] = *dataPointer;
        dataPointer ++;
        bytesNr --;
    }

    //SPI_Write(writeCommand, bytesNumber + 1);
    SSPIFunc::RxTx_Frames_1CS(writeCommand,bytesNumber+1,1);     //类似于AD7190_GetRegisterValue操作
}

unsigned long Adc_AD7190::AD7190_GetRegisterValue(unsigned char registerAddress,
        unsigned char bytesNumber,
        unsigned char modifyCS)
{
    unsigned short registerWord[5] = {0, 0, 0, 0, 0};
    unsigned long buffer          = 0x0;
    unsigned char i               = 0;

    //registerWord[0] = 0x01 * modifyCS;    是官方驱动用于SPI的一个chip select，直接去掉不用即可
    registerWord[0] = AD7190_COMM_READ |
                      AD7190_COMM_ADDR(registerAddress);    //通信寄存器内容
    registerWord[1] = 0x00;
    registerWord[2] = 0x00;
    //SPI_Read(registerWord, bytesNumber + 1);
    SSPIFunc::RxTx_Frames_1CS(registerWord,bytesNumber+1,1);    //+1是指通信寄存器
    //Rx[i] = SSPIFunc::SSPIRxBuff[1][i];    //接收数据
    for(i = 1; i < bytesNumber + 1; i ++)
    {
        buffer = (buffer << 8) + SSPIFunc::SSPIRxBuff[1][i];    //从spi接口获取数据
        //第一个是发送通信寄存器，读回值没意义
    }

    return(buffer);
}

//通道选择
void Adc_AD7190::AD7190_ChannelSelect(unsigned short channel)
{
    unsigned long oldRegValue = 0x0;
    unsigned long newRegValue = 0x0;

    oldRegValue = AD7190_GetRegisterValue(AD7190_REG_CONF, 3, 1);
    oldRegValue &= ~(AD7190_CONF_CHAN(0x3F));
    newRegValue = oldRegValue | AD7190_CONF_CHAN(1 << channel);
    AD7190_SetRegisterValue(AD7190_REG_CONF, newRegValue, 3, 1);
}

unsigned long Adc_AD7190::AD7190_SingleConversion(void)
{
    //	unsigned long command = 0x0;
    unsigned long regData = 0x0;

    //command = AD7190_MODE_SEL(AD7190_MODE_SINGLE) |
    //	AD7190_MODE_CLKSRC(AD7190_CLK_INT) |
    //	AD7190_MODE_RATE(0x060);
    //AD7190_SetRegisterValue(AD7190_REG_MODE, command, 3, 0); // CS is not modified.

    //conv();    //作为转换开始
    if(WaitRdyGoLow() == 1)
    {
        //WaitRdyGoLow();
        regData = AD7190_GetRegisterValue(AD7190_REG_DATA, 3, 0);
        return regData;
    }
    else
    {
        return 0;    //not ready
    }

    //超时处理？
    //加入GPIO切换模式P1.23
    //PINSEL3 |= 0x0000C000;    //切换成MISO0
    //regData = AD7190_GetRegisterValue(AD7190_REG_DATA, 3, 0);
    //PINSEL3 &= 0xFFFF3FFF;    //切换成gpio
    //FIO1DIR &= 0xFF7FFFFF;    //设置成输入

    //return(regData);
}

//多器件使用，单器件可以不用
void Adc_AD7190::conv(void)
{
    G_Port con = {1,19};
    GPIOFunc::Set_G_Port(&con);    //拉高
    ARBUS_DELAY(5);
    GPIOFunc::Clr_G_Port(&con);
    ARBUS_DELAY(5);
    GPIOFunc::Set_G_Port(&con);
}


///***************************************************************************//**
// *   @file   AD7190.c
// *   @brief  Implementation of AD7190 Driver.
// *   @author DNechita
//********************************************************************************
// * Copyright 2012(c) Analog Devices, Inc.
// *
// * All rights reserved.
// *
// * Redistribution and use in source and binary forms, with or without
// * modification, are permitted provided that the following conditions are met:
// *  - Redistributions of source code must retain the above copyright
// *    notice, this list of conditions and the following disclaimer.
// *  - Redistributions in binary form must reproduce the above copyright
// *    notice, this list of conditions and the following disclaimer in
// *    the documentation and/or other materials provided with the
// *    distribution.
// *  - Neither the name of Analog Devices, Inc. nor the names of its
// *    contributors may be used to endorse or promote products derived
// *    from this software without specific prior written permission.
// *  - The use of this software may or may not infringe the patent rights
// *    of one or more patent holders.  This license does not release you
// *    from the requirement that you obtain separate licenses from these
// *    patent holders to use this software.
// *  - Use of the software either in source or binary form, must be run
// *    on or directly connected to an Analog Devices Inc. component.
// *
// * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
// * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
// * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
// * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
// * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// *
//********************************************************************************
// *   SVN Revision: 526
//*******************************************************************************/
//
///******************************************************************************/
///* Include Files                                                              */
///******************************************************************************/
//#include "AD7190.h"		// AD7190 definitions.
//
///***************************************************************************//**
// * @brief Writes data into a register.
// *
// * @param registerAddress - Address of the register.
// * @param registerValue - Data value to write.
// * @param bytesNumber - Number of bytes to be written.
// * @param modifyCS - Allows Chip Select to be modified.
// *
// * @return none.
//*******************************************************************************/
//void AD7190_SetRegisterValue(unsigned char registerAddress,
//                             unsigned long registerValue,
//                             unsigned char bytesNumber,
//                             unsigned char modifyCS)
//{
//    unsigned char writeCommand[5] = {0, 0, 0, 0, 0};
//    unsigned char* dataPointer    = (unsigned char*)&registerValue;
//    unsigned char bytesNr         = bytesNumber + 1;
//
//    writeCommand[0] = 0x01 * modifyCS;
//    writeCommand[1] = AD7190_COMM_WRITE |
//                      AD7190_COMM_ADDR(registerAddress);
//    while(bytesNr > 1)
//    {
//        writeCommand[bytesNr] = *dataPointer;
//        dataPointer ++;
//        bytesNr --;
//    }
//    SPI_Write(writeCommand, bytesNumber + 1);
//}
//
///***************************************************************************//**
// * @brief Reads the value of a register.
// *
// * @param registerAddress - Address of the register.
// * @param bytesNumber - Number of bytes that will be read.
// * @param modifyCS    - Allows Chip Select to be modified.
// *
// * @return buffer - Value of the register.
//*******************************************************************************/
//unsigned long AD7190_GetRegisterValue(unsigned char registerAddress,
//                                      unsigned char bytesNumber,
//                                      unsigned char modifyCS)
//{
//    unsigned char registerWord[5] = {0, 0, 0, 0, 0};
//    unsigned long buffer          = 0x0;
//    unsigned char i               = 0;
//
//    registerWord[0] = 0x01 * modifyCS;
//    registerWord[1] = AD7190_COMM_READ |
//                      AD7190_COMM_ADDR(registerAddress);
//    registerWord[2] = 0x00;
//    registerWord[3] = 0x00;
//    SPI_Read(registerWord, bytesNumber + 1);
//    for(i = 1;i < bytesNumber + 1;i ++)
//    {
//        buffer = (buffer << 8) + registerWord[i];
//    }
//
//    return(buffer);
//}
//
///***************************************************************************//**
// * @brief Checks if the AD7190 part is present.
// *
// * @return status - Indicates if the part is present or not.
//*******************************************************************************/
//unsigned char AD7190_Init(void)
//{
//    unsigned char status = 1;
//    unsigned char regVal = 0;
//
//    SPI_Init(0, 1000000, 1, 1);
//    AD7190_Reset();
//    regVal = AD7190_GetRegisterValue(AD7190_REG_ID, 1, 1);
//    if( (regVal & AD7190_ID_MASK)  != ID_AD7190)
//    {
//        status = 0;
//    }
//    return(status);
//}
//
///***************************************************************************//**
// * @brief Resets the device.
// *
// * @return none.
//*******************************************************************************/
//void AD7190_Reset(void)
//{
//    unsigned char registerWord[7] = {0};
//
//    registerWord[0] = 0x01;
//    registerWord[1] = 0xFF;
//    registerWord[2] = 0xFF;
//    registerWord[3] = 0xFF;
//    registerWord[4] = 0xFF;
//    registerWord[5] = 0xFF;
//    registerWord[6] = 0xFF;
//    SPI_Write(registerWord, 6);
//}
//
///***************************************************************************//**
// * @brief Set device to idle or power-down.
// *
// * @param pwrMode - Selects idle mode or power-down mode.
// *                  Example: 0 - power-down
// *                           1 - idle
// *
// * @return none.
//*******************************************************************************/
//void AD7190_SetPower(unsigned char pwrMode)
//{
//     unsigned long oldPwrMode = 0x0;
//     unsigned long newPwrMode = 0x0;
//
//     oldPwrMode = AD7190_GetRegisterValue(AD7190_REG_MODE, 3, 1);
//     oldPwrMode &= ~(AD7190_MODE_SEL(0x7));
//     newPwrMode = oldPwrMode |
//                  AD7190_MODE_SEL((pwrMode * (AD7190_MODE_IDLE)) |
//                                  (!pwrMode * (AD7190_MODE_PWRDN)));
//     AD7190_SetRegisterValue(AD7190_REG_MODE, newPwrMode, 3, 1);
//}
//
///***************************************************************************//**
// * @brief Waits for RDY pin to go low.
// *
// * @return none.
//*******************************************************************************/
//void AD7190_WaitRdyGoLow(void)
//{
//    while( AD7190_RDY_STATE )
//    {
//        ;
//    }
//}
//
///***************************************************************************//**
// * @brief Selects the channel to be enabled.
// *
// * @param channel - Selects a channel.
// *
// * @return none.
//*******************************************************************************/
//void AD7190_ChannelSelect(unsigned short channel)
//{
//     unsigned long oldRegValue = 0x0;
//     unsigned long newRegValue = 0x0;
//
//    oldRegValue = AD7190_GetRegisterValue(AD7190_REG_CONF, 3, 1);
//    oldRegValue &= ~(AD7190_CONF_CHAN(0x3F));
//    newRegValue = oldRegValue | AD7190_CONF_CHAN(1 << channel);
//    AD7190_SetRegisterValue(AD7190_REG_CONF, newRegValue, 3, 1);
//}
//
///***************************************************************************//**
// * @brief Performs the given calibration to the specified channel.
// *
// * @param mode - Calibration type.
// * @param channel - Channel to be calibrated.
// *
// * @return none.
//*******************************************************************************/
//void AD7190_Calibrate(unsigned char mode, unsigned char channel)
//{
//    unsigned long oldRegValue = 0x0;
//    unsigned long newRegValue = 0x0;
//
//    AD7190_ChannelSelect(channel);
//    oldRegValue = AD7190_GetRegisterValue(AD7190_REG_MODE, 3, 1);
//    oldRegValue &= ~AD7190_MODE_SEL(0x7);
//    newRegValue = oldRegValue | AD7190_MODE_SEL(mode);
//    ADI_PART_CS_LOW;
//    AD7190_SetRegisterValue(AD7190_REG_MODE, newRegValue, 3, 0); // CS is not modified.
//    AD7190_WaitRdyGoLow();
//    ADI_PART_CS_HIGH;
//}
//
///***************************************************************************//**
// * @brief Selects the polarity of the conversion and the ADC input range.
// *
// * @param polarity - Polarity select bit.
//                     Example: 0 - bipolar operation is selected.
//                              1 - unipolar operation is selected.
//* @param range - Gain select bits. These bits are written by the user to select
//                 the ADC input range.
// *
// * @return none.
//*******************************************************************************/
//void AD7190_RangeSetup(unsigned char polarity, unsigned char range)
//{
//    unsigned long oldRegValue = 0x0;
//    unsigned long newRegValue = 0x0;
//
//    oldRegValue = AD7190_GetRegisterValue(AD7190_REG_CONF,3, 1);
//    oldRegValue &= ~(AD7190_CONF_UNIPOLAR |
//                     AD7190_CONF_GAIN(0x7));
//    newRegValue = oldRegValue |
//                  (polarity * AD7190_CONF_UNIPOLAR) |
//                  AD7190_CONF_GAIN(range);
//    AD7190_SetRegisterValue(AD7190_REG_CONF, newRegValue, 3, 1);
//}
//
///***************************************************************************//**
// * @brief Returns the result of a single conversion.
// *
// * @return regData - Result of a single analog-to-digital conversion.
//*******************************************************************************/
//unsigned long AD7190_SingleConversion(void)
//{
//    unsigned long command = 0x0;
//    unsigned long regData = 0x0;
//
//    command = AD7190_MODE_SEL(AD7190_MODE_SINGLE) |
//              AD7190_MODE_CLKSRC(AD7190_CLK_INT) |
//              AD7190_MODE_RATE(0x060);
//    ADI_PART_CS_LOW;
//    AD7190_SetRegisterValue(AD7190_REG_MODE, command, 3, 0); // CS is not modified.
//    AD7190_WaitRdyGoLow();
//    regData = AD7190_GetRegisterValue(AD7190_REG_DATA, 3, 0);
//    ADI_PART_CS_HIGH;
//
//    return(regData);
//}
//
///***************************************************************************//**
// * @brief Returns the average of several conversion results.
// *
// * @return samplesAverage - The average of the conversion results.
//*******************************************************************************/
//unsigned long AD7190_ContinuousReadAvg(unsigned char sampleNumber)
//{
//    unsigned long samplesAverage = 0x0;
//    unsigned char count = 0x0;
//    unsigned long command = 0x0;
//
//    command = AD7190_MODE_SEL(AD7190_MODE_CONT) |
//              AD7190_MODE_CLKSRC(AD7190_CLK_INT) |
//              AD7190_MODE_RATE(0x060);
//    ADI_PART_CS_LOW;
//    AD7190_SetRegisterValue(AD7190_REG_MODE, command, 3, 0); // CS is not modified.
//    for(count = 0;count < sampleNumber;count ++)
//    {
//        AD7190_WaitRdyGoLow();
//        samplesAverage += AD7190_GetRegisterValue(AD7190_REG_DATA, 3, 0); // CS is not modified.
//    }
//    ADI_PART_CS_HIGH;
//    samplesAverage = samplesAverage / sampleNumber;
//
//    return(samplesAverage);
//}
//
///***************************************************************************//**
// * @brief Read data from temperature sensor and converts it to Celsius degrees.
// *
// * @return temperature - Celsius degrees.
//*******************************************************************************/
//unsigned long AD7190_TemperatureRead(void)
//{
//    unsigned char temperature = 0x0;
//    unsigned long dataReg = 0x0;
//
//    AD7190_RangeSetup(0, AD7190_CONF_GAIN_1);
//    AD7190_ChannelSelect(AD7190_CH_TEMP_SENSOR);
//    dataReg = AD7190_SingleConversion();
//    dataReg -= 0x800000;
//    dataReg /= 2815;   // Kelvin Temperature
//    dataReg -= 273;    //Celsius Temperature
//    temperature = (unsigned long) dataReg;
//
//    return(temperature);
//}
