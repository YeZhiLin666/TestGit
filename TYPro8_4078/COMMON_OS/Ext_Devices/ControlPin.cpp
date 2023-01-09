/*****************************************************************************
// 程序文件      :CP.cpp     主要用于将MCU的GPIO，CPLD的PIN口，38译码器的输出 统筹成一个统一接口
// 文件描述      :为MCU board服务
// 编写日期      :2012.07.13
*****************************************************************************/

#include "includes.h"
unsigned long RegOfC = 0x0fff;

void ControlPin::SetIObyCP(St_Port MyPort)
{
    switch(MyPort.PortType)
    {
    case eCPTYPE_MCUGPIO:
        GPIOFunc::InitPortFunc(MyPort.Port,
                               MyPort.Bits,
                               MyPort.Function,
                               0);
        break;
        //case eCPTYPE_CPLD:
        //case eCPTYPE_38CODE:
    default:
        break;
    }
}

void ControlPin::SetMeHigh(St_Port MyPort)//unsigned char Value)
{
    switch (MyPort.PortType)
    {
    case eCPTYPE_MCUGPIO:
        GPIOFunc::Set_G_Port(MyPort.Port,MyPort.Bits);
        break;
#if USE_AS_AUXMCU == 0
    case eCPTYPE_CPLD:
        if(MyPort.Address != NBUS_WRITE_DCS)
            *(volatile unsigned short *)MyPort.Address |=  (0x01<<MyPort.Bits);
        else
        {
            // For 2108HC, ACIM control bits definition:
            //
            // | bit15  bit14  bit13 | bit12 | bit11  bit10  bit9  bit8  bit7  bit6  bit5  bit4  bit3  bit2  bit1  bit0 |
            // |   ACIM Ch Select    | Enable|                 used for other function                                  |
            // |                                                                                                        |
            // |***********************************NBUS_WRITE_DCS(0x9000000C)*********************************************|
            //
            // When bit12 = 0, all ch will disconnect ACIM loop,
            // When bit9 = 0 && bit12 = 1, this ch will connect ACIM loop.
            //
            // Every time we write NBUS_WRITE_DCS, always check and maintain ACIM status

            if(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType == BOARDTYPE_LBT21084 && ChannelInfo_Manager::m_st_BoadType.m_PowerLevel==1)
            {
                if(Step_Manager_task::ACIM_CH_ON<8)	// at least one channel has ACIM on
                {
                    RegOfC = Step_Manager_task::ACIM_CH_ON<<13;
                    RegOfC |= 0x1000;	// Set bit12=1, bit9=0
                }
                else
                    RegOfC &= 0x0fff;
            }
            RegOfC |= (0x01<<MyPort.Bits);
            *(volatile unsigned short *)MyPort.Address =  RegOfC;
        }
        break;
#endif
        //case eCPTYPE_38CODE://调用38译码器的通用接口；
        //    break;
    default:
        break;
    }
}
void ControlPin::SetMeLow(St_Port MyPort)//unsigned char Value)
{
    switch (MyPort.PortType)
    {
    case eCPTYPE_MCUGPIO:
        GPIOFunc::Clr_G_Port(MyPort.Port,MyPort.Bits);
        break;
#if USE_AS_AUXMCU == 0
    case eCPTYPE_CPLD:
        if(MyPort.Address != NBUS_WRITE_DCS)
            *(volatile short *)MyPort.Address &=  ~(0x01<<MyPort.Bits);
        else
        {
            // For 2108HC, ACIM control bits definition:
            //
            // | bit15  bit14  bit13 | bit12 | bit11  bit10 | bit9 | bit8  bit7  bit6  bit5  bit4  bit3  bit2  bit1  bit0 |
            // |   ACIM Ch Select    | Enable|              | ~Set |             used for other function                  |
            // |                                                                                                          |
            // |***********************************NBUS_WRITE_DCS(0x9000000C)*********************************************|
            //
            // When bit12 = 0, all ch will disconnect ACIM loop,
            // When bit9 = 0 && bit12 = 1, this ch will connect ACIM loop.
            //
            // Every time we write NBUS_WRITE_DCS, always check and maintain ACIM status

            if(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType == BOARDTYPE_LBT21084 && ChannelInfo_Manager::m_st_BoadType.m_PowerLevel==1)
            {
                if(Step_Manager_task::ACIM_CH_ON<8)	// at least one channel has ACIM on
                {
                    RegOfC = Step_Manager_task::ACIM_CH_ON<<13;
                    RegOfC |= 0x1000;	// Set bit12=1, bit9=0
                }
                else
                    RegOfC &= 0x0fff;
            }
            RegOfC &=  ~(0x01<<MyPort.Bits);
            *(volatile short *)MyPort.Address = RegOfC;
        }
        break;
#endif
        //case eCPTYPE_38CODE:    //调用38译码器的通用接口；
        //    break;
    default:
        break;
    }
}

unsigned char ControlPin::ReadMe(St_Port MyPort)
{
    unsigned char result = 0;

    switch (MyPort.PortType)
    {
    case eCPTYPE_MCUGPIO:
        result = GPIOFunc::RD_G_Port(MyPort.Port,MyPort.Bits);
        //调用MCU的通用接口；
        break;
#if USE_AS_AUXMCU == 0
    case eCPTYPE_CPLD:
        result = CPLD_FUNC::Read(MyPort.Address);
        break;
#endif
        //case eCPTYPE_38CODE: //调用38译码器的通用接口；
        //    break;
    default:
        break;
    }

    return result;
}

void ControlPin::ReInitPullUP(void)
{
    unsigned char  uc_Bit;
    UWord32 * This_P = NULL;
    This_P = (UWord32 *) (&LPC_IOCON->P0_0);
    for(uc_Bit =0; uc_Bit < PIN_ALL_TOTAL; uc_Bit++) // P0.00 --P5.04 Address 5*32+5//
    {
        *This_P |= 0x10;   // PX[X] — General purpose digital input/output pin.
        This_P++;
    }
}

