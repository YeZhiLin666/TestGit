#include "includes.h"

unsigned char CPLD_FUNC::Valid = 1;

void CPLD_FUNC::Init(void)
{
    unsigned long IRQ1,IRQ2;
    IRQFunc::DisableIRQ(&IRQ1,&IRQ2);

    cpu_iowr_16(NBUS_WIRTE_ARBIN,0x01);
    cpu_iowr_16(NBUS_WRITE_ARBINDIR,0x01);
    cpu_iowr_16(NBUS_ARBINWR_RD,0);
    cpu_iowr_16(NBUS_WRITE_CHANNEL,0);
    cpu_iowr_16(NBUS_WRITE_DOWRITE,0x03);    //Ĭ��Ϊ��    ADWRITE DOWIRTE��Ϊ��  ���Ӷ���LELOAD֧�֣���ȥ��0x03
    cpu_iowr_16(NBUS_WRITE_DCS,0x1F);    //0x1FĬ��Ϊ��
    Write(NBUS_WRITE_PGA_GAIN,0x10);


    IRQFunc::EnableIRQ(IRQ1,IRQ2);
}

void CPLD_FUNC::WriteDOBus(unsigned char ChannelIdx,unsigned short SetValue)
{
    unsigned long IRQ1,IRQ2;
    IRQFunc::DisableIRQ(&IRQ1,&IRQ2);

    ARBUS_DELAY(5);
    cpu_iowr_16(NBUS_WRITE_ARBINDIR,0);             //DIR����  ��סXD����,�Ȳ����
    ARBUS_DELAY(5);

    //��Ҫд��������������
    cpu_iowr_16(NBUS_ARBINWR_RD, SetValue );        //���SetValue,��������CPLD��
    ARBUS_DELAY(5);


    cpu_iowr_16(NBUS_WRITE_CHANNEL, ChannelIdx);    //ѡ��ͨ��

    //������ѡ��ͨ���������
    cpu_iowr_16(NBUS_WIRTE_ARBIN, 0&NBUS_ARBIN);    //ARBIN����ʹ��Yi
    ARBUS_DELAY(5);

    cpu_iowr_16(NBUS_WRITE_DOWRITE, 0x02);          //DOWRITE����    ADWRITEΪ��
    ARBUS_DELAY(5);
    cpu_iowr_16(NBUS_WRITE_DOWRITE, 0x03);          //DOWRITE����    ADWRITEΪ��
    ARBUS_DELAY(5);

    cpu_iowr_16(NBUS_WIRTE_ARBIN, 0|NBUS_ARBIN);    //����ARBIN

    IRQFunc::EnableIRQ(IRQ1,IRQ2);
}

void CPLD_FUNC::WriteSpecialACIM(unsigned short ChannelIdx,unsigned char SetValue)
{
// 	if(ChannelIdx >= MAXCHANNELNO)
// 		return;

    // For 2108HC, ACIM control bits definition:
    //
    // | bit15  bit14  bit13 | bit12 | bit11  bit10 | bit9 | bit8  bit7  bit6  bit5  bit4  bit3  bit2  bit1  bit0 |
    // |   ACIM Ch Select    | Enable|              | ~Set |             used for other function                  |
    // |                                                                                                          |
    // |***********************************NBUS_WRITE_DCS(0x9000000C)*********************************************|
    //
    // When bit12 = 0, all ch will disconnect ACIM loop,
    // When bit9 = 0 && bit12 = 1, this ch will connect ACIM loop.


#if DEMO_MODE == 1
    return;    //��Demoģʽ�£���ֹû��ADC���������¹��ȵȴ���ʱ��
#endif

    unsigned long IRQ1,IRQ2;
    unsigned short temp = 0x0000;
    IRQFunc::DisableIRQ(&IRQ1,&IRQ2);
    /*
    ARBUS_DELAY(5);
    cpu_iowr_16(NBUS_WRITE_ARBINDIR,0);             //DIR����  ��סXD����,�Ȳ����
    ARBUS_DELAY(5);

    //��Ҫд��������������
    //    cpu_iowr_16(NBUS_ARBINWR_RD, SetValue );        //���SetValue,��������CPLD��
    //    ARBUS_DELAY(5);

    //
    //	cpu_iowr_16(NBUS_WRITE_DCS, ChannelIdx);    //ѡ���ַ
    */
    ARBUS_DELAY(5);
    if(SetValue)
        temp = 0x1000;
    else
        temp = 0x0000;
    temp |= ChannelIdx<<13;

    cpu_iowr_16(NBUS_WRITE_DCS, temp );
    ARBUS_DELAY(5);
    /*
    //������ѡ��ͨ���������
    cpu_iowr_16(NBUS_WIRTE_ARBIN, 0&NBUS_ARBIN);    //ARBIN����ʹ��Yi
    ARBUS_DELAY(5);

    cpu_iowr_16(NBUS_WRITE_DOWRITE, 0x02);          //DOWRITE����    ADWRITEΪ��
    ARBUS_DELAY(5);
    cpu_iowr_16(NBUS_WRITE_DOWRITE, 0x03);          //DOWRITE����    ADWRITEΪ��
    ARBUS_DELAY(5);

    cpu_iowr_16(NBUS_WIRTE_ARBIN, 0|NBUS_ARBIN);    //����ARBIN
    //	cpu_iowr_16(NBUS_WRITE_ARBINDIR, 0|NBUS_ARBINDIR);    //DIR����     //2012 11 29  comment out chen  20140324��ʱ�޸����ڵ���ע�͵�
    */

    IRQFunc::EnableIRQ(IRQ1,IRQ2);
}

void CPLD_FUNC::WriteADGBus(unsigned char ChannelIdx,unsigned short SetValue)
{
    //#if ((_BOARD_TYPE_  == BOARDTYPE_LBT21024 )||(_BOARD_TYPE_  == BOARDTYPE_LBT2108)||(_BOARD_TYPE_  == BOARDTYPE_LBT21084VB)||(_BOARD_TYPE_  == BOARDTYPE_LBT21024FR ))
    //if((board_config::IVBoardType == BOARDTYPE_LBT21024 )||(board_config::IVBoardType == BOARDTYPE_LBT2108)||(board_config::IVBoardType == BOARDTYPE_LBT21084VB))
    //judge channel number
//#else
#if USE_AS_AUXMCU==0
    if((ChannelInfo_Manager::m_st_BoadType.m_u16BoardType != BOARDTYPE_LBT21024 )
            &&(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType != BOARDTYPE_LBT21084)
            &&(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType != BOARDTYPE_LBT21044)
//         &&(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType != BOARDTYPE_LBT21084VB)
// 		&&(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType != BOARDTYPE_LBT21024FR)
            &&(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType != BOARDTYPE_LBT21014)
// 		&&(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType != BOARDTYPE_LBT21024FRVB)
// 		&&(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType != BOARDTYPE_IBT31162)
            &&(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType != BOARDTYPE_LBT20084)
            &&(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType != BOARDTYPE_LBT21162))
    {
        if(ChannelIdx >= MAXCHANNELNO)
            return;
    }
#endif

//#endif

    unsigned long IRQ1,IRQ2;
    IRQFunc::DisableIRQ(&IRQ1,&IRQ2);

    cpu_iowr_16(NBUS_WRITE_CHANNEL, ChannelIdx );    //cpu_iowr_16(NBUS_WRITE_E_ADD, 0&chn );    //����CPLD����ʱADD�����
    cpu_iowr_16(NBUS_WIRTE_ARBIN, 0&NBUS_ARBIN);    //ARBIN����ʹ��Yi
    ARBUS_DELAY(5);    //��delay

    cpu_iowr_16(NBUS_WRITE_DOWRITE, 0x01);    //DOWRITEΪ��    ADWRITE����
    ARBUS_DELAY(5);    //��delay

    cpu_iowr_16(NBUS_WRITE_DOWRITE, 0x03);    //DOWRITEΪ��    ADWRITE����
    ARBUS_DELAY(5);

    cpu_iowr_16(NBUS_WIRTE_ARBIN, 0|NBUS_ARBIN);

    IRQFunc::EnableIRQ(IRQ1,IRQ2);
}

// unsigned short CPLD_FUNC::ReadDOBus(void)
// {
// 	unsigned short temp = 0;

// 	unsigned long IRQ1,IRQ2;
// 	IRQFunc::DisableIRQ(&IRQ1,&IRQ2);

// 	cpu_iowr_16(NBUS_WRITE_ARBINDIR,0&NBUS_ARBINDIR);    //����DIR
// 	ARBUS_DELAY(5);
// 	cpu_iowr_16(NBUS_WRITE_ARBINDIR, NBUS_ARBINDIR);    //����DIR
// 	ARBUS_DELAY(5);
// 	cpu_iowr_16(NBUS_ARBINWR_RD,0x00);  //����
// 	ARBUS_DELAY(5);
// 	cpu_iowr_16(NBUS_ARBINWR_RD,0x01);  //����
// 	//��סBD

// 	temp = cpu_iord_16(NBUS_ARBINWR_RD);
// 	ARBUS_DELAY(5);
// 	cpu_iowr_16(NBUS_WRITE_ARBINDIR, 0&NBUS_ARBINDIR);    //����

// 	ARBUS_DELAY(5);
// 	cpu_iowr_16(NBUS_WRITE_ARBINDIR, NBUS_ARBINDIR);    //������DIR

// 	IRQFunc::EnableIRQ(IRQ1,IRQ2);

// 	return temp;
// }

// unsigned short CPLD_FUNC::ReadDOBus(unsigned char ChannelIdx)
// {
// 	unsigned short temp = 0;

// 	unsigned long IRQ1,IRQ2;
// 	IRQFunc::DisableIRQ(&IRQ1,&IRQ2);

// 	ARBUS_DELAY(5);
// 	cpu_iowr_16(NBUS_WRITE_ARBINDIR,0);             //DIR����  ��סXD����,�Ȳ����
// 	ARBUS_DELAY(5);

// 	cpu_iowr_16(NBUS_WRITE_CHANNEL, ChannelIdx);    //ѡ��ͨ��

// 	//������ѡ��ͨ���������
// 	cpu_iowr_16(NBUS_WIRTE_ARBIN, 0&NBUS_ARBIN);    //ARBIN����ʹ��Yi
// 	ARBUS_DELAY(5);

// 	cpu_iowr_16(NBUS_WRITE_DOWRITE, 0x02);          //DOWRITE����    ADWRITEΪ��
// 	ARBUS_DELAY(5);
// 	cpu_iowr_16(NBUS_WRITE_DOWRITE, 0x03);          //DOWRITE����    ADWRITEΪ��
// 	ARBUS_DELAY(5);

// 	cpu_iowr_16(NBUS_WIRTE_ARBIN, 0|NBUS_ARBIN);    //����ARBIN

// 	//--------------------------
// 	cpu_iowr_16(NBUS_WRITE_ARBINDIR,0&NBUS_ARBINDIR);    //����DIR
// 	ARBUS_DELAY(5);
// 	cpu_iowr_16(NBUS_WRITE_ARBINDIR, NBUS_ARBINDIR);    //����DIR
// 	ARBUS_DELAY(5);
// 	cpu_iowr_16(NBUS_ARBINWR_RD,0x00);  //����
// 	ARBUS_DELAY(5);
// 	cpu_iowr_16(NBUS_ARBINWR_RD,0x01);  //����
// 	//��סBD

// 	temp = cpu_iord_16(NBUS_ARBINWR_RD);
// 	ARBUS_DELAY(5);
// 	cpu_iowr_16(NBUS_WRITE_ARBINDIR, 0&NBUS_ARBINDIR);    //����

// 	ARBUS_DELAY(5);
// 	cpu_iowr_16(NBUS_WRITE_ARBINDIR, NBUS_ARBINDIR);    //������DIR

// 	IRQFunc::EnableIRQ(IRQ1,IRQ2);
// 	return temp;
// }

void CPLD_FUNC::Write(unsigned long Address,unsigned short SetValue)
{
    if(Valid)
        cpu_iowr_16(Address,SetValue);
}
unsigned short CPLD_FUNC::Read(unsigned long Address)
{
    unsigned short Value = cpu_iord_16(Address);
    return Value;
}

/*********************************************************************************
**                            End Of File
*********************************************************************************/
