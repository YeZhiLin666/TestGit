/*!
LTC6804-2 Multicell Battery Monitor
@verbatim
The LTC6804 is a 3rd generation multicell battery stack
monitor that measures up to 12 series connected battery
cells with a total measurement error of less than 1.2mV. The
cell measurement range of 0V to 5V makes the LTC6804
suitable for most battery chemistries. All 12 cell voltages
can be captured in 290uS, and lower data acquisition rates
can be selected for high noise reduction.

Using the LTC6804-2, multiple devices are connected in
parallel to the host processor, with each device
individually addressed.
@endverbatim
REVISION HISTORY
$Revision: 1000 $
$Date: 2013-12-13

Copyright (c) 2013, Linear Technology Corp.(LTC)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of Linear Technology Corp.

The Linear Technology Linduino is not affiliated with the official Arduino team.
However, the Linduino is only possible because of the Arduino team's commitment
to the open-source community.  Please, visit http://www.arduino.cc and
http://store.arduino.cc , and consider a purchase that will help fund their
ongoing work.

Copyright 2013 Linear Technology Corp. (LTC)
***********************************************************/

//! @defgroup LTC68042 LTC6804-2: Multicell Battery Monitor

/*! @file
@ingroup LTC68042
Library for LTC6804-2 Multicell Battery Monitor
*/
#include "includes.h"

St_Port  ADC16bit_LTC68042::LTC6804_CS;
unsigned char   ADC16bit_LTC68042::CodeType;  //Bipolar Code Type or Unipolar Code Type;
/*
ADC control Variables for LTC6804
*/



/*!
6804 conversion command variables.
*/
unsigned char  ADC16bit_LTC68042::ADCV[2];     //!< Cell Voltage conversion command.
unsigned char  ADC16bit_LTC68042::ADAX[2];      //!< GPIO conversion command.
unsigned short ADC16bit_LTC68042::Data_send[50];


/*!
\brief This function will initialize all 6804 variables and the SPI port.

input:
------
IC: number of ICs being controlled. The address of the ICs in a LTC6804-2 network will start at 0 and continue in an ascending order.
*/
void ADC16bit_LTC68042::LTC6804_initialize(unsigned char ConvCodeType)
{
    // 初始化电池包采样芯片
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
    set_adc(MD_NORMAL,DCP_DISABLED,CELL_CH_ALL,AUX_CH_ALL);

}

/*!******************************************************************************************************************
\brief Maps  global ADC control variables to the appropriate control bytes for each of the different ADC commands

@param[in] uint8_t MD The adc conversion mode
@param[in] uint8_t DCP Controls if Discharge is permitted during cell conversions
@param[in] uint8_t CH Determines which cells are measured during an ADC conversion command
@param[in] uint8_t CHG Determines which GPIO channels are measured during Auxiliary conversion command

Command Code: \n
|command	|  10   |   9   |   8   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   |
|-----------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|
|ADCV:	    |   0   |   1   | MD[1] | MD[2] |   1   |   1   |  DCP  |   0   | CH[2] | CH[1] | CH[0] |
|ADAX:	    |   1   |   0   | MD[1] | MD[2] |   1   |   1   |  DCP  |   0   | CHG[2]| CHG[1]| CHG[0]|
******************************************************************************************************************/
void ADC16bit_LTC68042::set_adc(uint8_t MD, //ADC Mode
                                uint8_t DCP, //Discharge Permit
                                uint8_t CH, //Cell Channels to be measured
                                uint8_t CHG //GPIO Channels to be measured
                               )
{
    // 设置电池包采样芯片ADC寄存器
    uint8_t md_bits;

    md_bits = (MD & 0x02) >> 1;
    ADCV[0] = md_bits + 0x02;
    md_bits = (MD & 0x01) << 7;
    ADCV[1] =  md_bits + 0x60 + (DCP<<4) + CH;

    md_bits = (MD & 0x02) >> 1;
    ADAX[0] = md_bits + 0x04;
    md_bits = (MD & 0x01) << 7;
    ADAX[1] = md_bits + 0x60 + CHG ;

}


/*!*********************************************************************************************
\brief Starts cell voltage conversion

Starts ADC conversions of the LTC6804 Cpin inputs.
The type of ADC conversion done is set using the associated global variables:
|Variable|Function                                      |
|--------|----------------------------------------------|
| MD     | Determines the filter corner of the ADC      |
| CH     | Determines which cell channels are converted |
| DCP    | Determines if Discharge is Permitted	     |

***********************************************************************************************/
/*  //函数流程
LTC6804_adcv Function sequence:
1. Load adcv command into cmd array
2. Calculate adcv cmd PEC and load pec into cmd array
3. wakeup isoSPI port, this step can be removed if isoSPI status is previously guaranteed
4. send broadcast adcv command to LTC6804 stack
*/
void ADC16bit_LTC68042::LTC6804_adcv(uint8_t SPI_Idx)
{
    //发送开始转换命令

    uint16_t cmd[4];
    uint16_t temp_pec;

    //1
    cmd[0] = (uint16_t)ADCV[0];
    cmd[1] = (uint16_t)ADCV[1];

    //2
    temp_pec = pec15_calc(2, ADCV);
    cmd[2] = (temp_pec >> 8);
    cmd[3] = (temp_pec&0x00FF);

    //4
    ControlPin::SetMeLow(LTC6804_CS);
    SSPIFunc::WriteTxFIFO(&cmd[0],4,SPI_Idx);
    ARBUS_DELAY(800);  //延时大约35us
    SSPIFunc::ReadRxFIFO(4,SPI_Idx);
}

bool ADC16bit_LTC68042::LTC6804_poll(unsigned char chip_address, uint8_t SPI_Idx)
{
    // 查询是否有数据返回
    uint16_t cmd[4];
    uint16_t temp_pec;
    uint8_t  temp[2];

    //1
    cmd[0] = 0x87+(chip_address<<3);
    cmd[1] = 0x14;
    temp[0] = (uint8_t) cmd[0];
    temp[1] = (uint8_t) cmd [1];
    //2
    temp_pec = pec15_calc(2, temp);
    cmd[2] = (temp_pec >> 8);
    cmd[3] = (temp_pec&0x00FF);

    //4
    SSPIFunc::WriteTxFIFO(&cmd[0],4,SPI_Idx);
    ARBUS_DELAY(800);  //延时大约35us
    SSPIFunc::ReadRxFIFO(4,SPI_Idx);

    for (int i=0; i<4; i++)
    {
        if(SSPIFunc::SSPIRxBuff[SPI_Idx][i] !=0x0 )
        {
            return true;
        }
    }
    return false;
}

/*!******************************************************************************************************
\brief Start an GPIO Conversion

Starts an ADC conversions of the LTC6804 GPIO inputs.
The type of ADC conversion done is set using the associated global variables:
|Variable|Function                                      |
|--------|----------------------------------------------|
| MD     | Determines the filter corner of the ADC      |
| CHG    | Determines which GPIO channels are converted |

*********************************************************************************************************/
/*  //函数流程
LTC6804_adax Function sequence:
1. Load adax command into cmd array
2. Calculate adax cmd PEC and load pec into cmd array
3. wakeup isoSPI port, this step can be removed if isoSPI status is previously guaranteed
4. send broadcast adax command to LTC6804 stack
*/
void ADC16bit_LTC68042::LTC6804_adax(uint8_t SPI_Idx)
{
    // 发送ADAX命令滤波
    uint16_t cmd[4];
    uint16_t temp_pec;

    cmd[0] = (uint16_t)ADAX[0];
    cmd[1] = (uint16_t)ADAX[1];
    temp_pec = pec15_calc(2, ADAX);
    cmd[2] = (temp_pec >> 8);
    cmd[3] = (temp_pec&0x00FF);

    ControlPin::SetMeLow(LTC6804_CS);
    SSPIFunc::WriteTxFIFO(cmd,4,SPI_Idx);
    ARBUS_DELAY(650);  //延时大约35us

}


/***********************************************//**
												 \brief Reads and parses the LTC6804 cell voltage registers.

												 The function is used to read the cell codes of the LTC6804.
												 This function will send the requested read commands parse the data
												 and store the cell voltages in cell_codes variable.


												 @param[in] uint8_t reg; This controls which cell voltage register is read back.

												 0: Read back all Cell registers

												 1: Read back cell group A

												 2: Read back cell group B

												 3: Read back cell group C

												 4: Read back cell group D

												 @param[in] uint8_t total_ic; This is the number of ICs in the network


												 @param[out] uint16_t cell_codes[]; An array of the parsed cell codes from lowest to highest. The cell codes will
												 be stored in the cell_codes[] array in the following format:
												 |  cell_codes[0]| cell_codes[1] |  cell_codes[2]|    .....     |  cell_codes[11]|  cell_codes[12]| cell_codes[13] |  .....   |
												 |---------------|----------------|--------------|--------------|----------------|----------------|----------------|----------|
												 |IC1 Cell 1     |IC1 Cell 2      |IC1 Cell 3    |    .....     |  IC1 Cell 12   |IC2 Cell 1      |IC2 Cell 2      | .....    |

												 @return int8_t, PEC Status.

												 0: No PEC error detected

												 -1: PEC error detected, retry read
												 *************************************************/
/*  //函数流程
LTC6804_rdcv Sequence
1. Switch Statement:
a. Reg = 0
i. Read cell voltage registers A-D for every IC in the stack
ii. Parse raw cell voltage data in cell_codes array
iii. Check the PEC of the data read back vs the calculated PEC for each read register command
b. Reg != 0
i.Read single cell voltage register for all ICs in stack
ii. Parse raw cell voltage data in cell_codes array
iii. Check the PEC of the data read back vs the calculated PEC for each read register command
2. Return pec_error flag
*/
uint8_t ADC16bit_LTC68042::LTC6804_rdcv(uint8_t reg,uint8_t total_ic,uint16_t cell_codes[][12],uint8_t SPI_Idx)
{
    // 读取单元转换电压
    const uint8_t NUM_RX_BYT = 8;
    const uint8_t BYT_IN_REG = 6;
    const uint8_t CELL_IN_REG = 3;

    int8_t pec_error = 0;
    uint16_t parsed_cell;
    uint16_t received_pec;
    uint16_t data_pec;
    uint8_t data_counter=0; //data counter
    uint8_t cell_data[NUM_RX_BYT*TOTAL_IC];


    //1.a
    if (reg == 0)
    {
        //a.i
        for(uint8_t cell_reg = 1; cell_reg<5; cell_reg++)         			 //executes once for each of the LTC6804 cell voltage registers
        {
            data_counter = 0;
            LTC6804_rdcv_reg(cell_reg, total_ic,cell_data,SPI_Idx);
            for (uint8_t current_ic = 0 ; current_ic < total_ic; current_ic++) // executes for every LTC6804 in the stack
            {   // current_ic is used as an IC counter
                //a.ii
                for(uint8_t current_cell = 0; current_cell<CELL_IN_REG; current_cell++)	 								  // This loop parses the read back data. Loops
                {   // once for each cell voltages in the register
                    parsed_cell = cell_data[data_counter] + (cell_data[data_counter + 1] << 8);
                    cell_codes[current_ic][current_cell  + ((cell_reg - 1) * CELL_IN_REG)] = parsed_cell;
                    data_counter = data_counter + 2;
                }
                //a.iii
                received_pec = (cell_data[data_counter] << 8) + cell_data[data_counter+1];
                data_pec = pec15_calc(BYT_IN_REG, &cell_data[current_ic * NUM_RX_BYT ]);
                if(received_pec != data_pec)
                {
                    pec_error--;
                }
                else
                {
                    pec_error = pec_error;
                }
                data_counter=data_counter+2;
            }
        }
    }
    //1.b
    else
    {
        //b.i
        LTC6804_rdcv_reg(reg, total_ic,cell_data,SPI_Idx);
        for (uint8_t current_ic = 0 ; current_ic < total_ic; current_ic++) // executes for every LTC6804 in the stack
        {   // current_ic is used as an IC counter
            //b.ii
            for(uint8_t current_cell = 0; current_cell < CELL_IN_REG; current_cell++)   									// This loop parses the read back data. Loops
            {   // once for each cell voltage in the register
                parsed_cell = cell_data[data_counter] + (cell_data[data_counter+1]<<8);
                cell_codes[current_ic][current_cell + ((reg - 1) * CELL_IN_REG)] = 0x0000FFFF & parsed_cell;
                data_counter= data_counter + 2;
            }
            //b.iii
            received_pec = (cell_data[data_counter] << 8 )+ cell_data[data_counter + 1];
            data_pec = pec15_calc(BYT_IN_REG, &cell_data[current_ic * NUM_RX_BYT * (reg-1)]);
            if(received_pec != data_pec)
            {
                pec_error--;
            }
            else
            {
                pec_error = pec_error;
            }
        }
    }
    //2
    return(pec_error);
}


/***********************************************//**
												 \brief Reads and parses the LTC6804 cell voltage registers.

												 The function is used to read the cell codes of the LTC6804.
												 This function will send the requested read commands parse the data
												 and store the cell voltages in cell_codes variable.


												 @param[in] uint8_t reg; This controls which cell voltage register is read back.

												 0: Read back all Cell registers

												 1: Read back cell group A

												 2: Read back cell group B

												 3: Read back cell group C

												 4: Read back cell group D

												 @param[in] uint8_t total_ic; This is the number of ICs in the network


												 @param[out] uint16_t cell_codes[]; An array of the parsed cell codes from lowest to highest. The cell codes will
												 be stored in the cell_codes[] array in the following format:
												 |  cell_codes[0]| cell_codes[1] |  cell_codes[2]|    .....     |  cell_codes[11]|  cell_codes[12]| cell_codes[13] |  .....   |
												 |---------------|----------------|--------------|--------------|----------------|----------------|----------------|----------|
												 |IC1 Cell 1     |IC1 Cell 2      |IC1 Cell 3    |    .....     |  IC1 Cell 12   |IC2 Cell 1      |IC2 Cell 2      | .....    |

												 @return int8_t, PEC Status.

												 0: No PEC error detected

												 -1: PEC error detected, retry read
												 *************************************************/
/*  //函数流程
LTC6804_rd_single_cv Sequence
1. Switch Statement:
a. Reg = 0
i. Read cell voltage registers A-D for every IC in the stack
ii. Parse raw cell voltage data in cell_codes array
iii. Check the PEC of the data read back vs the calculated PEC for each read register command
b. Reg != 0
i.Read single cell voltage register for all ICs in stack
ii. Parse raw cell voltage data in cell_codes array
iii. Check the PEC of the data read back vs the calculated PEC for each read register command
2. Return pec_error flag
*/
uint8_t ADC16bit_LTC68042::LTC6804_rd_single_cv(uint8_t reg,uint8_t chip_address,unsigned long data_codes[][12],uint8_t SPI_Idx)
{
    // 读取独立单元转换电压
    const uint8_t NUM_RX_BYT = 8;
    const uint8_t BYT_IN_REG = 6;
    const uint8_t CELL_IN_REG = 3;
    uint16_t cell_codes[12];
    long temp_code;

    int8_t pec_error = 0;
    uint16_t parsed_cell;
    uint16_t received_pec;
    uint16_t data_pec;
    uint8_t data_counter=0; //data counter
    uint8_t cell_data[NUM_RX_BYT*TOTAL_IC];
    memset(cell_data,0, sizeof(cell_data));

    //1.a
    if (reg == 0)
    {
        //a.i
        for(uint8_t cell_reg = 1; cell_reg<5; cell_reg++)         			 //executes once for each of the LTC6804 cell voltage registers
        {
            data_counter = chip_address*8;
            LTC6804_rd_single_cv_reg(cell_reg, chip_address,cell_data,SPI_Idx);
            // current_ic is used as an IC counter
            //a.ii
            for(uint8_t current_cell = 0; current_cell<CELL_IN_REG; current_cell++)	 								  // This loop parses the read back data. Loops
            {   // once for each cell voltages in the register
                parsed_cell = cell_data[data_counter] + (cell_data[data_counter + 1] << 8);
                cell_codes[current_cell  + ((cell_reg - 1) * CELL_IN_REG)] = parsed_cell;
                data_counter = data_counter + 2;
            }
            //a.iii
            received_pec = (cell_data[data_counter] << 8) + cell_data[data_counter+1];
            data_pec = pec15_calc(BYT_IN_REG, &cell_data[chip_address * NUM_RX_BYT ]);
            if(received_pec != data_pec)
            {
                pec_error--;//pec_error = -1;
            }
            else
            {
                pec_error = pec_error;
            }
            data_counter=data_counter+2;
        }
    }
    //1.b
    else
    {
        //b.i
        LTC6804_rd_single_cv_reg(reg, chip_address,cell_data,SPI_Idx);
        // current_ic is used as an IC counter
        //b.ii
        for(uint8_t current_cell = 0; current_cell < CELL_IN_REG; current_cell++)   									// This loop parses the read back data. Loops
        {   // once for each cell voltage in the register
            parsed_cell = cell_data[data_counter] + (cell_data[data_counter+1]<<8);
            cell_codes[current_cell + ((reg - 1) * CELL_IN_REG)] = 0x0000FFFF & parsed_cell;
            data_counter= data_counter + 2;
        }
        //b.iii
        received_pec = (cell_data[data_counter] << 8 )+ cell_data[data_counter + 1];
        data_pec = pec15_calc(BYT_IN_REG, &cell_data[chip_address * NUM_RX_BYT * (reg-1)]);
        if(received_pec != data_pec)
        {
            pec_error--;
        }
        else
        {
            pec_error = pec_error;
        }
    }
    //2转换成24bit
    if(pec_error==0)
    {
        for(unsigned char i=0; i<12; i++)
        {
            temp_code = cell_codes[i];
            data_codes[chip_address][i]= temp_code;
        }
    }
    return(pec_error);
}

/***********************************************//**
												 \brief Read the raw data from the LTC6804 cell voltage register

												 The function reads a single cell voltage register and stores the read data
												 in the *data point as a byte array. This function is rarely used outside of
												 the LTC6804_rdcv() command.

												 @param[in] uint8_t reg; This controls which cell voltage register is read back.

												 1: Read back cell group A

												 2: Read back cell group B

												 3: Read back cell group C

												 4: Read back cell group D

												 @param[in] uint8_t total_ic; This is the number of ICs in the network

												 @param[out] uint8_t *data; An array of the unparsed cell codes
												 *************************************************/
/*  //函数流程
LTC6804_rdcv_reg Function Process:
1. Determine Command and initialize command array
2. Calculate Command PEC
3. Wake up isoSPI, this step is optional
4. Send Global Command to LTC6804 stack
*/
void ADC16bit_LTC68042::LTC6804_rdcv_reg(uint8_t reg,uint8_t total_ic, uint8_t *data,uint8_t SPI_Idx)
{
    // 读取单元转换电压寄存器
    uint8_t cmd[8];
    uint16_t datasent[12];
    uint16_t temp_pec;

    //1
    if (reg == 1)
    {
        cmd[1] = 0x04;
        cmd[0] = 0x00;
    }
    else if(reg == 2)
    {
        cmd[1] = 0x06;
        cmd[0] = 0x00;
    }
    else if(reg == 3)
    {
        cmd[1] = 0x08;
        cmd[0] = 0x00;
    }
    else if(reg == 4)
    {
        cmd[1] = 0x0A;
        cmd[0] = 0x00;
    }

    //4
    for(int current_ic = 0; current_ic<total_ic; current_ic++)
    {
        cmd[0] = 0x80 + (current_ic<<3); //Setting address
        temp_pec = pec15_calc(2, cmd);
        cmd[2] = (uint8_t)(temp_pec >> 8);
        cmd[3] = (uint8_t)(temp_pec);
        for(char i=0; i<4; i++)
            datasent[i]=(uint16_t)(cmd[i]);
        wakeup_idle();
        ControlPin::SetMeLow(LTC6804_CS);
        SSPIFunc::WriteTxFIFO(datasent,4,SPI_Idx);
        ARBUS_DELAY(800);
        SSPIFunc::ReadRxFIFO(4,SPI_Idx);

        memset(&datasent,0,sizeof(datasent));
        SSPIFunc::WriteTxFIFO(datasent,8,SPI_Idx);
        ARBUS_DELAY(2000);
        SSPIFunc::ReadRxFIFO(8,SPI_Idx);
        ControlPin::SetMeHigh(LTC6804_CS);

        for(char i =0; i<8; i++)
            data[current_ic*8+i] = (uint8_t)(SSPIFunc::SSPIRxBuff[SPI_Idx][i]);    //接收数据

    }
}


/***********************************************//**
												 \brief Read the raw data from the LTC6804 cell voltage register

												 The function reads a single cell voltage register and stores the read data
												 in the *data point as a byte array. This function is rarely used outside of
												 the LTC6804_rdcv() command.

												 @param[in] uint8_t reg; This controls which cell voltage register is read back.

												 1: Read back cell group A

												 2: Read back cell group B

												 3: Read back cell group C

												 4: Read back cell group D

												 @param[in] uint8_t chip_address

												 @param[out] uint8_t *data; An array of the unparsed cell codes
												 *************************************************/
/* //函数流程
LTC6804_rdcv_reg Function Process:
1. Determine Command and initialize command array
2. Calculate Command PEC
3. Wake up isoSPI, this step is optional
4. Send Global Command to LTC6804 stack
*/

void ADC16bit_LTC68042::LTC6804_rd_single_cv_reg(uint8_t reg,uint8_t chip_address, uint8_t *data,uint8_t SPI_Idx)
{
    // 读取独立单元转换电压寄存器
    uint8_t cmd[8];
    uint16_t datasent[12];
    uint16_t temp_pec;

    //1
    if (reg == 1)
    {
        cmd[1] = 0x04;
        cmd[0] = 0x00;
    }
    else if(reg == 2)
    {
        cmd[1] = 0x06;
        cmd[0] = 0x00;
    }
    else if(reg == 3)
    {
        cmd[1] = 0x08;
        cmd[0] = 0x00;
    }
    else if(reg == 4)
    {
        cmd[1] = 0x0A;
        cmd[0] = 0x00;
    }

    //4
    cmd[0] = 0x80 + (chip_address<<3); //Setting address
    temp_pec = pec15_calc(2, cmd);
    cmd[2] = (uint8_t)(temp_pec >> 8);
    cmd[3] = (uint8_t)(temp_pec);

    for(char i=0; i<4; i++)
        datasent[i]=(uint16_t)(cmd[i]);

    ControlPin::SetMeLow(LTC6804_CS);

    SSPIFunc::WriteTxFIFO(datasent,4,SPI_Idx);
    ARBUS_DELAY(800);
    SSPIFunc::ReadRxFIFO(4,SPI_Idx);

    memset(&datasent,0,sizeof(datasent));
    SSPIFunc::WriteTxFIFO(datasent,8,SPI_Idx);
    ARBUS_DELAY(2000);
    SSPIFunc::ReadRxFIFO(8,SPI_Idx);

    ControlPin::SetMeHigh(LTC6804_CS);

    for(char i =0; i<8; i++)
        data[chip_address*8+i] = (uint8_t)(SSPIFunc::SSPIRxBuff[SPI_Idx][i]);    //接收数据
}





/***********************************************************************************//**
																					 \brief Reads and parses the LTC6804 auxiliary registers.

																					 The function is used
																					 to read the  parsed GPIO codes of the LTC6804. This function will send the requested
																					 read commands parse the data and store the gpio voltages in aux_codes variable

																					 @param[in] uint8_t reg; This controls which GPIO voltage register is read back.

																					 0: Read back all auxiliary registers

																					 1: Read back auxiliary group A

																					 2: Read back auxiliary group B


																					 @param[in] uint8_t total_ic; This is the number of ICs in the network

																					 @param[out] uint8_t aux_codes[]; An array of the aux codes from lowest to highest. The GPIO codes will
																					 be stored in the aux_codes[] array in the following format:
																					 |  aux_codes[0]| aux_codes[1] |  aux_codes[2]|  aux_codes[3]|  aux_codes[4]|  aux_codes[5]| aux_codes[6] |aux_codes[7]|  .....    |
																					 |--------------|--------------|--------------|--------------|--------------|--------------|--------------|------------|-----------|
																					 |IC1 GPIO1     |IC1 GPIO2     |IC1 GPIO3     |IC1 GPIO4     |IC1 GPIO5     |IC1 Vref2     |IC2 GPIO1     |IC2 GPIO2   |  .....    |


																					 @return int8_t, PEC Status.

																					 0: No PEC error detected

																					 -1: PEC error detected, retry read
																					 *************************************************/
/*  //函数流程
LTC6804_rdaux Sequence
1. Switch Statement:
a. Reg = 0
i. Read GPIO voltage registers A-D for every IC in the stack
ii. Parse raw GPIO voltage data in cell_codes array
iii. Check the PEC of the data read back vs the calculated PEC for each read register command
b. Reg != 0
i.Read single GPIO voltage register for all ICs in stack
ii. Parse raw GPIO voltage data in cell_codes array
iii. Check the PEC of the data read back vs the calculated PEC for each read register command
2. Return pec_error flag
*/
int8_t ADC16bit_LTC68042::LTC6804_rdaux(uint8_t reg,uint8_t total_ic, uint16_t aux_codes[][6],uint8_t SPI_Idx)
{
    // 读取辅助
    const uint8_t NUM_RX_BYT = 8;
    const uint8_t BYT_IN_REG = 6;
    const uint8_t GPIO_IN_REG = 3;

    uint8_t *data;
    uint8_t data_counter = 0;
    int8_t pec_error = 0;
    uint16_t received_pec;
    uint16_t data_pec;
    data = (uint8_t *) malloc((NUM_RX_BYT*total_ic)*sizeof(uint8_t));
    //1.a
    if (reg == 0)
    {
        //a.i
        for(uint8_t gpio_reg = 1; gpio_reg<3; gpio_reg++)		 	   		 //executes once for each of the LTC6804 aux voltage registers
        {
            data_counter = 0;
            LTC6804_rdaux_reg(gpio_reg, total_ic,data,SPI_Idx);
            for (uint8_t current_ic = 0 ; current_ic < total_ic; current_ic++) // This loop executes once for each LTC6804
            {   // current_ic is used as an IC counter
                //a.ii
                for(uint8_t current_gpio = 0; current_gpio< GPIO_IN_REG; current_gpio++)	// This loop parses GPIO voltages stored in the register
                {
                    aux_codes[current_ic][current_gpio +((gpio_reg-1)*GPIO_IN_REG)] = data[data_counter] + (data[data_counter+1]<<8);
                    data_counter=data_counter+2;
                }
                //a.iii
                received_pec = (data[data_counter]<<8)+ data[data_counter+1];
                data_pec = pec15_calc(BYT_IN_REG, &data[current_ic*NUM_RX_BYT*(gpio_reg-1)]);
                if(received_pec != data_pec)
                {
                    pec_error = -1;
                }
                data_counter=data_counter+2;
            }
        }
    }
    else
    {
        //b.i
        LTC6804_rdaux_reg(reg, total_ic, data,SPI_Idx);
        for (int current_ic = 0 ; current_ic < total_ic; current_ic++) // executes for every LTC6804 in the stack
        {   // current_ic is used as an IC counter
            //b.ii
            for(int current_gpio = 0; current_gpio<GPIO_IN_REG; current_gpio++)   // This loop parses the read back data. Loops
            {   // once for each aux voltage in the register
                aux_codes[current_ic][current_gpio +((reg-1)*GPIO_IN_REG)] = 0x0000FFFF & (data[data_counter] + (data[data_counter+1]<<8));
                data_counter=data_counter+2;
            }
            //b.iii
            received_pec = (data[data_counter]<<8) + data[data_counter+1];
            data_pec = pec15_calc(6, &data[current_ic*8*(reg-1)]);
            if(received_pec != data_pec)
            {
                pec_error = -1;
            }
        }
    }
    free(data);
    return (pec_error);
}

/***********************************************//**
												 \brief Read the raw data from the LTC6804 auxiliary register

												 The function reads a single GPIO voltage register and stores thre read data
												 in the *data point as a byte array. This function is rarely used outside of
												 the LTC6804_rdaux() command.

												 @param[in] uint8_t reg; This controls which GPIO voltage register is read back.

												 1: Read back auxiliary group A

												 2: Read back auxiliary group B


												 @param[in] uint8_t total_ic; This is the number of ICs in the stack

												 @param[out] uint8_t *data; An array of the unparsed aux codes
												 *************************************************/
/*  //函数流程
LTC6804_rdaux_reg Function Process:
1. Determine Command and initialize command array
2. Calculate Command PEC
3. Wake up isoSPI, this step is optional
4. Send Global Command to LTC6804 stack
*/
void ADC16bit_LTC68042::LTC6804_rdaux_reg(uint8_t reg, uint8_t total_ic,uint8_t *data,uint8_t SPI_Idx)
{
    // 读取辅助寄存器
    uint8_t cmd[4];
    uint16_t datasent[4];
    uint16_t cmd_pec;

    //1
    if (reg == 1)
    {
        cmd[1] = 0x0C;
        cmd[0] = 0x00;
    }
    else if(reg == 2)
    {
        cmd[1] = 0x0e;
        cmd[0] = 0x00;
    }
    else
    {
        cmd[1] = 0x0C;
        cmd[0] = 0x00;
    }
    //2
    cmd_pec = pec15_calc(2, cmd);
    cmd[2] = (uint8_t)(cmd_pec >> 8);
    cmd[3] = (uint8_t)(cmd_pec);

    //4
    for(int current_ic = 0; current_ic<total_ic; current_ic++)
    {
        cmd[0] = 0x80 + (current_ic<<3); //Setting address
        cmd_pec = pec15_calc(2, cmd);
        cmd[2] = (uint8_t)(cmd_pec >> 8);
        cmd[3] = (uint8_t)(cmd_pec);
        for(char i =0; i<4; i++)
            datasent[i]= (uint16_t)(cmd[i]);
        ControlPin::SetMeLow(LTC6804_CS);
        SSPIFunc::WriteTxFIFO(datasent,4,SPI_Idx);
        SSPIFunc::ReadRxFIFO(8,SPI_Idx);
        ARBUS_DELAY(2000);  //延时大约95us
        ControlPin::SetMeHigh(LTC6804_CS);
        for(char i =0; i<8; i++)
            data[current_ic*8+i] =(uint8_t)(SSPIFunc::SSPIRxBuff[SPI_Idx][i]);    //接收数据
    }
}


/********************************************************//**
														  \brief Clears the LTC6804 cell voltage registers

														  The command clears the cell voltage registers and intiallizes
														  all values to 1. The register will read back hexadecimal 0xFF
														  after the command is sent.
														  ************************************************************/
/*  //函数流程
LTC6804_clrcell Function sequence:
1. Load clrcell command into cmd array
2. Calculate clrcell cmd PEC and load pec into cmd array
3. wakeup isoSPI port, this step can be removed if isoSPI status is previously guaranteed
4. send broadcast clrcell command to LTC6804 stack
*/
void ADC16bit_LTC68042::LTC6804_clrcell(uint8_t SPI_Idx)
{
    // 清除单元电压寄存器
    uint8_t cmd[4];
    uint16_t datasent[4];
    uint16_t cmd_pec;

    //1
    cmd[0] = 0x07;
    cmd[1] = 0x11;

    //2
    cmd_pec = pec15_calc(2, cmd);
    cmd[2] = (uint8_t)(cmd_pec >> 8);
    cmd[3] = (uint8_t)(cmd_pec );

    //4
    for(char i =0; i<4; i++)
        datasent[i]= cmd[i];
    ControlPin::SetMeLow(LTC6804_CS);
    SSPIFunc::WriteTxFIFO(datasent,4,SPI_Idx);
    ARBUS_DELAY(650);  //延时大约35us
    ControlPin::SetMeHigh(LTC6804_CS);
}



/***********************************************************//**
															 \brief Clears the LTC6804 Auxiliary registers

															 The command clears the Auxiliary registers and intiallizes
															 all values to 1. The register will read back hexadecimal 0xFF
															 after the command is sent.
															 ***************************************************************/
/*   //函数流程
LTC6804_clraux Function sequence:
1. Load clraux command into cmd array
2. Calculate clraux cmd PEC and load pec into cmd array
3. wakeup isoSPI port, this step can be removed if isoSPI status is previously guaranteed
4. send broadcast clraux command to LTC6804 stack
*/
void ADC16bit_LTC68042::LTC6804_clraux(uint8_t SPI_Idx)
{
    // 清除辅助寄存器
    uint8_t cmd[4];
    uint16_t datasent[4];
    uint16_t cmd_pec;

    //1
    cmd[0] = 0x07;
    cmd[1] = 0x12;

    //2
    cmd_pec = pec15_calc(2, cmd);
    cmd[2] = (uint8_t)(cmd_pec >> 8);
    cmd[3] = (uint8_t)(cmd_pec);

    //4
    for(char i =0; i<4; i++)
        datasent[i]= cmd[i];
    ControlPin::SetMeLow(LTC6804_CS);
    SSPIFunc::WriteTxFIFO(datasent,4,SPI_Idx);
    ARBUS_DELAY(650);  //延时大约35us
    ControlPin::SetMeHigh(LTC6804_CS);
}



/*****************************************************//**
													   \brief Write the LTC6804 configuration register

													   This command will write the configuration registers of the stacks
													   connected in a stack stack. The configuration is written in descending
													   order so the last device's configuration is written first.


													   @param[in] uint8_t total_ic; The number of ICs being written.

													   @param[in] uint8_t *config an array of the configuration data that will be written, the array should contain the 6 bytes for each
													   IC in the stack. The lowest IC in the stack should be the first 6 byte block in the array. The array should
													   have the following format:
													   |  config[0]| config[1] |  config[2]|  config[3]|  config[4]|  config[5]| config[6] |  config[7]|  config[8]|  .....    |
													   |-----------|-----------|-----------|-----------|-----------|-----------|-----------|-----------|-----------|-----------|
													   |IC1 CFGR0  |IC1 CFGR1  |IC1 CFGR2  |IC1 CFGR3  |IC1 CFGR4  |IC1 CFGR5  |IC2 CFGR0  |IC2 CFGR1  | IC2 CFGR2 |  .....    |

													   The function will calculate the needed PEC codes for the write data
													   and then transmit data to the ICs on a stack.
													   ********************************************************/
/*   //函数流程
LTC6804_wrcfg Function sequence:
1. Load cmd array with the write configuration command and PEC
2. Load the cmd with LTC6804 configuration data
3. Calculate the pec for the LTC6804 configuration data being transmitted
4. wakeup isoSPI port, this step can be removed if isoSPI status is previously guaranteed
5. Write configuration of each LTC6804 on the stack

*/
void ADC16bit_LTC68042::LTC6804_wrcfg(uint8_t total_ic,uint8_t config[][6],uint8_t SPI_Idx)
{
    // 写配置寄存器
    const uint8_t BYTES_IN_REG = 6;
//	const uint8_t CMD_LEN = 4+(8*total_ic);

    uint16_t temp_pec;
    uint8_t cmd_index; //command counter

    uint8_t cmd [36];
    uint16_t datasent [36];
    //1
    cmd[0] = 0x00;
    cmd[1] = 0x01;
    cmd[2] = 0x3d;
    cmd[3] = 0x6e;

    //2
    cmd_index = 4;
    for (uint8_t current_ic = total_ic; current_ic > 0; current_ic--) 			// executes for each LTC6804 in stack,
    {
        for (uint8_t current_byte = 0; current_byte < BYTES_IN_REG; current_byte++) // executes for each byte in the CFGR register
        {   // i is the byte counter
            cmd[cmd_index] = config[current_ic-1][current_byte]; 		//adding the config data to the array to be sent
            cmd_index = cmd_index + 1;
        }
        //3
        temp_pec = (uint16_t)pec15_calc(BYTES_IN_REG, &config[current_ic-1][0]);// calculating the PEC for each board
        cmd[cmd_index] = (uint8_t)(temp_pec >> 8);
        cmd[cmd_index + 1] = (uint8_t)temp_pec;
        cmd_index = cmd_index + 2;
    }

    //5
    for(int current_ic = 0; current_ic<total_ic; current_ic++)
    {
        cmd[0] = 0x80 + (current_ic<<3); //Setting address
        temp_pec = pec15_calc(2, cmd);
        cmd[2] = (uint8_t)(temp_pec >> 8);
        cmd[3] = (uint8_t)(temp_pec);
        for(char i=0; i<4; i++)
        {
            Data_send[i]= cmd[i];
            datasent[i]= Data_send[i];
        }
        for(char i=0; i<8; i++)
        {
            Data_send[4+(8*current_ic)+i]= cmd[4+(8*current_ic)+i];
            datasent[4+(8*current_ic)+i]=Data_send[4+(8*current_ic)+i];
        }

        ControlPin::SetMeLow(LTC6804_CS);
        SSPIFunc::WriteTxFIFO(datasent,4,SPI_Idx);
        ARBUS_DELAY(100);  //延时大约100us
        SSPIFunc::WriteTxFIFO(&datasent[4+(8*current_ic)],8,SPI_Idx);
        ARBUS_DELAY(2000);  //延时大约100us
        ControlPin::SetMeHigh(LTC6804_CS);
    }
}


/*!******************************************************
\brief Reads configuration registers of a LTC6804 stack




@param[in] uint8_t total_ic: number of ICs in the stack

@param[out] uint8_t *r_config: array that the function will write configuration data to. The configuration data for each IC
is stored in blocks of 8 bytes with the configuration data of the lowest IC on the stack in the first 8 bytes
of the array, the second IC in the second 8 byte etc. Below is an table illustrating the array organization:

|r_config[0]|r_config[1]|r_config[2]|r_config[3]|r_config[4]|r_config[5]|r_config[6]  |r_config[7] |r_config[8]|r_config[9]|  .....    |
|-----------|-----------|-----------|-----------|-----------|-----------|-------------|------------|-----------|-----------|-----------|
|IC1 CFGR0  |IC1 CFGR1  |IC1 CFGR2  |IC1 CFGR3  |IC1 CFGR4  |IC1 CFGR5  |IC1 PEC High |IC1 PEC Low |IC2 CFGR0  |IC2 CFGR1  |  .....    |


@return int8_t PEC Status.
0: Data read back has matching PEC

-1: Data read back has incorrect PEC
********************************************************/
/*   //函数流程
LTC6804_rdcfg Function sequence:
1. Load cmd array with the write configuration command and PEC
2. wakeup isoSPI port, this step can be removed if isoSPI status is previously guaranteed
3. read configuration of each LTC6804 on the stack
4. For each LTC6804 in the stack
a. load configuration data into r_config array
b. calculate PEC of received data and compare against calculated PEC
5. Return PEC Error

*/
int8_t ADC16bit_LTC68042::LTC6804_rdcfg(uint8_t total_ic, uint8_t r_config[][8],uint8_t SPI_Idx)
{
    // 读配置寄存器
    const uint8_t BYTES_IN_REG = 8;
    uint8_t cmd[4];
    uint16_t datasent[4];
    uint8_t *rx_data;
    int8_t pec_error = 0;
    uint16_t data_pec;
    uint16_t received_pec;
    rx_data = (uint8_t *) malloc((8*total_ic)*sizeof(uint8_t));
    //1
    cmd[0] = 0x00;
    cmd[1] = 0x02;
    cmd[2] = 0x2b;
    cmd[3] = 0x0A;

    //3
    for(int current_ic = 0; current_ic<total_ic; current_ic++)
    {
        cmd[0] = 0x80 + (current_ic<<3); //Setting address
        data_pec = pec15_calc(2, cmd);
        cmd[2] = (uint8_t)(data_pec >> 8);
        cmd[3] = (uint8_t)(data_pec);
        for(char i=0; i<4; i++)
            datasent[i]= (uint16_t)(cmd[i]);
        ControlPin::SetMeLow(LTC6804_CS);
        SSPIFunc::WriteTxFIFO(datasent,4,SPI_Idx);
        SSPIFunc::ReadRxFIFO(4,SPI_Idx);
        SSPIFunc::ReadRxFIFO(8,SPI_Idx);
        ARBUS_DELAY(2000);  //延时大约95us
        ControlPin::SetMeHigh(LTC6804_CS);
        for(char i =0; i<8; i++)
            cmd[4+(8*current_ic)+i] = (uint8_t)(SSPIFunc::SSPIRxBuff[SPI_Idx][i]);    //接收数据
    }

    for (uint8_t current_ic = 0; current_ic < total_ic; current_ic++) //executes for each LTC6804 in the stack
    {
        //4.a
        for (uint8_t current_byte = 0; current_byte < BYTES_IN_REG; current_byte++)
        {
            r_config[current_ic][current_byte] = rx_data[current_byte + (current_ic*BYTES_IN_REG)];
        }
        //4.b
        received_pec = (r_config[current_ic][6]<<8) + r_config[current_ic][7];
        data_pec = pec15_calc(6, &r_config[current_ic][0]);
        if(received_pec != data_pec)
        {
            pec_error = -1;
        }
    }
    free(rx_data);
    //5
    return(pec_error);
}


/*!****************************************************
\brief Wake isoSPI up from idle state
Generic wakeup commannd to wake isoSPI up out of idle
*****************************************************/
void ADC16bit_LTC68042::wakeup_idle()
{
    // 从空闲唤醒芯片
    unsigned short dumi[1];
    dumi[0] = 0;
    ControlPin::SetMeLow(LTC6804_CS);
    SSPIFunc::WriteTxFIFO(dumi,1,1);
    ARBUS_DELAY(1000);
    ARBUS_DELAY(1000);
    SSPIFunc::ReadRxFIFO(1,1);
    ControlPin::SetMeHigh(LTC6804_CS);
}

/*!****************************************************
\brief Wake the LTC6804 from the sleep state

Generic wakeup commannd to wake the LTC6804 from sleep
*****************************************************/
bool ADC16bit_LTC68042::wakeup_sleep(MsTime *start_time)
{
    // 从睡眠唤醒芯片
    unsigned short dumi[1];
    unsigned char past_ticks;
    dumi[0] = 0;
    ControlPin::SetMeLow(LTC6804_CS);
    past_ticks =CommonFunc::GetTimeTicks(start_time);
    if (past_ticks>=5)
    {
        SSPIFunc::WriteTxFIFO(dumi,1,1);
        ARBUS_DELAY(0x2000);
        SSPIFunc::ReadRxFIFO(1,1);
        ControlPin::SetMeHigh(LTC6804_CS);
        return true;
    }
    return false;
}
/*!**********************************************************
\brief calaculates  and returns the CRC15


@param[in]  uint8_t len: the length of the data array being passed to the function

@param[in]  uint8_t data[] : the array of data that the PEC will be generated from


@return  The calculated pec15 as an unsigned int16_t
***********************************************************/
uint16_t ADC16bit_LTC68042::pec15_calc(uint8_t len, uint8_t *data)
{
    // 校验码CRC15
    uint16_t remainder,addr;

    remainder = 16;//initialize the PEC
    for(uint8_t i = 0; i<len; i++) // loops for each byte in data array
    {
        addr = ((remainder>>7)^data[i])&0xff;//calculate PEC table address
        remainder = (remainder<<8)^crc15Table[addr];
    }
    return(remainder*2);//The CRC15 has a 0 in the LSB so the remainder must be multiplied by 2
}


void ADC16bit_LTC68042::setCS(const St_Port * cs)
{
    // 片选信号
    LTC6804_CS = *cs;
}

St_Port ADC16bit_LTC68042::getCS(void)
{
    // 取得片选信号
    return LTC6804_CS;
}



