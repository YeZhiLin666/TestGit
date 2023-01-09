
//include
#include "includes.h"


////private data////
SSPI_Para    SSPIFunc::SSPI_Para_me[SSPI_MAX_CHANNEL];
// UWord32     SSPIFunc::TxCounter;
// unsigned char SSPIFunc::bBusy[SSPI_MAX_CHANNEL];

unsigned short SSPIFunc::SSPITxBuff[SSPI_MAX_CHANNEL][SSPI_MAX_BUFFER];
unsigned short SSPIFunc::SSPIRxBuff[SSPI_MAX_CHANNEL][SSPI_MAX_BUFFER];

// void SSPIFunc::Init(void)
// {
// 	for(int SspIdx = 0;SspIdx < SSPI_MAX_CHANNEL; SspIdx++)
// 	{
// 		SSPI_Para_me[SspIdx].uc_Module_Name = SSP_DEVICE_NULL;
// 		bBusy[SspIdx] = 0;
// 	}
// }
// void SSPIFunc::initSSPICLK(unsigned char SSPIChannel)
// {
// 	unsigned char uc_SelectSpeed;
// 	Reg_SSPI *pSSP = 0;
// 	uc_SelectSpeed =  SSPI_Para_me[SSPIChannel].uc_Speed;
// 	// para protection.
// 	/*if(SSPIChannel > 0 && uc_SelectSpeed > 9)
// 	uc_SelectSpeed = 9;*/
// 	//uc_temp=18/uc_SelectSpeed ;

// 	unsigned char uc_Temp = (60/uc_SelectSpeed);  //  CCLK/2 = PCLK = 60M
// 	if((uc_Temp%2) > 0)
// 		uc_Temp = uc_Temp + 1;  //  确保分频值为偶数

// 	if( uc_Temp >= 60)
// 	{
// 		uc_Temp = 60;   //SCK =(CCLK/VPB)/(CPSDVSR*[SCR+1])=PCLK/(CPSDVSR*[SCR+1])   (60M/CPSR)=1M
// 	}
// 	else if( uc_Temp <= 1)
// 	{
// 		uc_Temp = 2;    //SCK =(CCLK/VPB)/(CPSDVSR*[SCR+1])=PCLK/(CPSDVSR*[SCR+1])   (60M/CPSR)=30M
// 	}

// 	if(SSPIChannel == 0)    //use ssp0
// 	{
// 		pSSP = pSSPI0;
// 	}
// 	else if(SSPIChannel == 1)   //use ssp1
// 	{
// 		pSSP = pSSPI1;
// 	}

// 	pSSP->CPSR = uc_Temp;   //SCK =(CCLK/VPB)/(CPSDVSR*[SCR+1])=PCLK/(CPSDVSR*[SCR+1])

// }



void SSPIFunc::setPara(SSPI_Para * p_SSPI_Para,unsigned char SSPIChannel)
{
    SSPI_Para_me[SSPIChannel].uc_Module_Name = p_SSPI_Para->uc_Module_Name;

    SSPI_Para_me[SSPIChannel].uc_Speed = p_SSPI_Para ->uc_Speed;
    SSPI_Para_me[SSPIChannel].uc_CPOL = p_SSPI_Para->uc_CPOL;
    SSPI_Para_me[SSPIChannel].uc_CPHA = p_SSPI_Para->uc_CPHA;

    SSPI_Para_me[SSPIChannel].uc_MSTR = p_SSPI_Para->uc_MSTR;
    SSPI_Para_me[SSPIChannel].uc_BITS = p_SSPI_Para->uc_BITS;
    SSPI_Para_me[SSPIChannel].uc_LSBF = p_SSPI_Para->uc_LSBF;
    //setCS(p_SSPI_Para, SSPIChannel);

    //comment out chen 20130104
// 	SSPI_Para_me[SSPIChannel]. st_CS.Type= p_SSPI_Para-> st_CS.Type;
// 	SSPI_Para_me[SSPIChannel]. st_CS.GPIO_PIN.P1= p_SSPI_Para-> st_CS.GPIO_PIN.P1;
// 	SSPI_Para_me[SSPIChannel]. st_CS.GPIO_PIN.P2= p_SSPI_Para-> st_CS.GPIO_PIN.P2;
// 	SSPI_Para_me[SSPIChannel]. st_CS.Address= p_SSPI_Para-> st_CS.Address;
// 	SSPI_Para_me[SSPIChannel]. st_CS.Value= p_SSPI_Para-> st_CS.Value;

// 	for(unsigned char i = 0 ; i <3 ; i++)
// 	{
// 		SSPI_Para_me[SSPIChannel]. st_CS.st_CS38[i].P1= p_SSPI_Para->st_CS. st_CS38[i].P1;
// 		SSPI_Para_me[SSPIChannel]. st_CS.st_CS38[i].P2= p_SSPI_Para-> st_CS.st_CS38[i].P2;
// 	}
// 	SSPI_Para_me[SSPIChannel]. st_CS.st_CS38EN.P1= p_SSPI_Para-> st_CS.st_CS38EN.P1;
// 	SSPI_Para_me[SSPIChannel]. st_CS.st_CS38EN.P2= p_SSPI_Para-> st_CS.st_CS38EN.P2;
// 	SSPI_Para_me[SSPIChannel]. st_CS.CS_Value= p_SSPI_Para-> st_CS.CS_Value;

}


void SSPIFunc::my_Init(unsigned char SSPIChannel)
{
    unsigned char FreqDiv = 0;

    Arbin_LPC_SSP_TypeDef *pSSP  = 0;

    switch(SSPIChannel)
    {
    case 0:
        pSSP = Arbin_SSP0;
        Arbin_SC->PCONP.BitReg.bPCSSP0 = 1;
        break;
    case 1:
        pSSP =  Arbin_SSP1;
        Arbin_SC->PCONP.BitReg.bPCSSP1 = 1;
        break;
    default:
        return;
    }

    //initCLK
    FreqDiv = (60 / SSPI_Para_me[SSPIChannel].uc_Speed);  //  CCLK/2 = PCLK = 60M
    if((FreqDiv & 1) > 0)
        FreqDiv++;  //  确保分频值为偶数

    if( FreqDiv < 2)
        pSSP->CPSR.BitReg.CPSDVSR = 2;
    else if(FreqDiv >= 60)
        pSSP->CPSR.BitReg.CPSDVSR = 60;
    else
        pSSP->CPSR.BitReg.CPSDVSR = FreqDiv;

    if(SSPI_Para_me[SSPIChannel].uc_CPHA)
        pSSP->CR0.BitReg.CPHA = 1;
    else
        pSSP->CR0.BitReg.CPHA = 0;

    if(SSPI_Para_me[SSPIChannel].uc_CPOL)
        pSSP->CR0.BitReg.CPOL = 1;
    else
        pSSP->CR0.BitReg.CPOL = 0;

    if(SSPI_Para_me[SSPIChannel].uc_BITS <= 4)
        pSSP->CR0.BitReg.DDS = 0x04 - 1;   //表示为4BIT传输。
    else if(SSPI_Para_me[SSPIChannel].uc_BITS <= 16)
        pSSP->CR0.BitReg.DDS = SSPI_Para_me[SSPIChannel].uc_BITS - 1;
    else
        pSSP->CR0.BitReg.DDS = 0x10 - 1;   //表示为16BIT传输。

    pSSP->CR0.BitReg.FRF = 0;   //SPI模式;

    if(SSPI_Para_me[SSPIChannel].uc_MSTR)
        pSSP->CR1.BitReg.MS = 0;
    else
        pSSP->CR1.BitReg.MS = 1;

    pSSP->CR1.BitReg.SSE = 1;
}

// unsigned char SSPIFunc::Check(SSPI_Para * p_SSPI_Para,unsigned char SSPIChannel)   //用这个参数跟当前的比较来判断能否直接使用
// {
// 	//present
// 	unsigned char uc_Pre_Speed = SSPI_Para_me[SSPIChannel].uc_Speed ;
// 	//	unsigned char uc_Pre_Name = SSPI_Para_me[SSPIChannel].uc_Module_Name;
// 	unsigned char uc_Pre_CPOL = SSPI_Para_me[SSPIChannel].uc_CPOL;
// 	unsigned char uc_Pre_CPHA = SSPI_Para_me[SSPIChannel].uc_CPHA;
// 	//unsigned char uc_IfSingleCS = SSPI_Para_me[SSPIChannel].IfSingleCS;

// 	//G_Port uc_Pre_CS = SSPI_Para_me[SSPIChannel].st_CS;

// 	//setting
// 	unsigned char uc_Set_Speed = p_SSPI_Para->uc_Speed;
// 	//	unsigned char uc_Set_Name = p_SSPI_Para->uc_Module_Name;
// 	unsigned char uc_Set_CPOL = p_SSPI_Para->uc_CPOL;
// 	unsigned char uc_Set_CPHA = p_SSPI_Para->uc_CPHA;
// 	//unsigned char uc_Set_IfSingleCS = p_SSPI_Para->IfSingleCS;

// 	//G_Port uc_Set_CS = p_SSPI_Para->st_CS;

// 	/*	if(uc_Pre_Name == uc_Set_Name)
// 	{
// 	return 0;
// 	}
// 	*/
// 	if( (uc_Pre_Speed == uc_Set_Speed)&&(uc_Pre_CPOL == uc_Set_CPOL)&&(uc_Pre_CPHA == uc_Set_CPHA) )
// 	{
// 		return 1;
// 	}
// 	else
// 	{
// 		return 2;
// 	}
// }


unsigned char SSPIFunc::getModuleName(unsigned char SSPIChannel)
{
    return SSPI_Para_me[SSPIChannel].uc_Module_Name;
}


unsigned short SSPIFunc::RxTx_Frame(unsigned short Txdata,unsigned char SSPIChannel)
{
    unsigned short uw16_i = 0;
    unsigned short uw16_feedback = 0;
    unsigned long  uw32_timeout = uw16_i;

    Reg_SSPI  * pSSP = 0;

#ifdef SSPI_POLL
    if(SSPIChannel == 0)    //use ssp0
    {
        pSSP = pSSPI0;
    }
    else if(SSPIChannel == 1)   //use ssp1
    {
        pSSP = pSSPI1;
    }



    uw32_timeout = 0;
    while(pSSP->SR & SSPSR_RNE)
    {
        uw16_i = pSSP->DR;											// uc_i is used as dummy to clear receiving buffer if necessary
        if(uw32_timeout++ >= SSPIMAX_TIMEOUT)						// if the time excceeded, then break.
        {
            return 0;
        }
    }

    uw32_timeout = 0;
    while((pSSP->SR & (SSPSR_TNF | SSPSR_BSY)) != SSPSR_TNF)		// if TNF != 1(Transfer buffer is full) and SSPSR !=0(Busy), wait till 'not full' and 'not busy'
    {
        if(uw32_timeout++ >= SSPIMAX_TIMEOUT)					// if the time excceeded, then break.
        {
            return 0;
        }
    }

    //	CS_Enable(SSPIChannel);                             //IOCLR1 |= SSP0_SEL;			// CS Enable

    pSSP->DR = Txdata;									     	// Send the byte immediately.

    uw32_timeout = 0;
    while((pSSP->SR & (SSPSR_RNE | SSPSR_BSY)) != SSPSR_RNE)		// if RNE != 1(Receiving buffer is empty) and SSPSR !=0(Busy), wait till 'not empty' and 'not busy'
    {
        if(uw32_timeout++ >= SSPIMAX_TIMEOUT)					// if the time excceeded, then break.
        {
            //CS_Disable(SSPIChannel);;                  //IOSET1 |= SSP0_SEL;	// CS Disable
            return 0;
        }
    }
    //CS_Disable(SSPIChannel);                        //IOSET1 |= SSP0_SEL;	// CS Disable
    uw16_feedback = pSSP->DR;
    // uc_Bytes = SSP0DR;										// Read the received byte and put it in the same buffer in the recent sent position.

    // is work but don't know why

    return uw16_feedback;

#endif

// #ifdef     SSPI_INTERRUPT

// #endif

}

// void SSPIFunc::SSPI0_Init(void)
// {
// 	SSPI_Para 	EEPROM_Spi = 		// will be replaced with setPara(&xxxxx::SPISettying, 0)
// 	{
// 		EEPROM_SSPI,    //name
// 		5,    //speed  1.8M
// 		0,    //CPOL
// 		0,    //CPHA
// 		1,    //master
// 		8,    //bits
// 		0,    //LSBF
// 		{
// 			GP_MOD,    //Single CS
// 			{1,21}, //cs pin 1.21
// 			0,
// 			0,
// 			{{1,21},{1,21},{1,21}}, //cs38 ignored
// 			{1,21},				// cs38 en ignored
// 			0,					// cs38 value ignored
// 		},

// 		{
// 			GP_MOD,    // ssel ignored
// 			{1,21}, //cs pin 1.21
// 			0,
// 			0,
// 			{{1,21},{1,21},{1,21}}, //cs38 ignored
// 			{1,21},				// cs38 en ignored
// 			0,					// cs38 value ignored
// 			},

// 	};
// 	//通道0即SPI通道用于读取EEPROM
// 	SSPIFunc::setPara(&EEPROM_Spi,0);    //设置参数
// 	SSPIFunc::my_Init(0);    //初始化
// }

/*****************************************************************************
** Function name:		SPI_SendData
**
** Descriptions:		Send a block of data to the SPI port,
**				        The firstparameter is the buffer pointer, The 2nd
**				        parameter is the block length.
**
** parameters:		uc_SelectModule: 0:SPI0, 1:SSP0  ,2:SSP1 选择模块
**                              uw32_Length： 发送长度
** Returned value:		uc_Result: true 操作成功；false操作失败
**
*****************************************************************************/
UChar8  SSPIFunc::SSPI_SendData(UChar8 SSPIChannel, UWord32 uw32_Length )
{
    UWord32 uw32_i=0,uw32_timeout = 0;
    Reg_SSPI *pSSP ;
    UChar8  uc_Result = true;
    if ( uw32_Length == 0 )
    {
        return  (false);
    }

    if(SSPIChannel == 0)    //use ssp0
    {
        pSSP = pSSPI0;
    }
    else if(SSPIChannel == 1)    //use ssp1
    {
        pSSP = pSSPI1;
    }

    for ( uw32_i = 0; uw32_i < uw32_Length; uw32_i++ )
    {
        uw32_timeout = 0;
        while ( !((pSSP->SR) & SSPSR_TNF) )
        {
            if ( uw32_timeout++ >= SSPIMAX_TIMEOUT )//if the time excceeded ,then break.
            {
                return  (false);
            }
        }
        pSSP->DR = SSPITxBuff[SSPIChannel][uw32_i];
        uw32_timeout = 0;
        while( pSSP->SR & SSPSR_BSY)             // WAIT FOR TRANSFER COMPLETE;
        {
            if ( uw32_timeout++>= SSPIMAX_TIMEOUT )//if the time excceeded ,then break.
            {
                return  (false);
            }
        }
        SSPIRxBuff[SSPIChannel][uw32_i] = pSSP->DR;		/* Flush the RxFIFO */
    }

    return  (uc_Result);
}


void SSPIFunc::RxTx_Frames(unsigned short *pTx,unsigned short len,unsigned char SSPIChannel)
{
    unsigned short uw16_i = 0;
    unsigned short i;
    unsigned short * pRx;
    unsigned long uw32_timeout = uw16_i;
    Reg_SSPI *pSSP = 0;

    //pTx = & SSPITxBuff[SSPIChannel][0];
    //pRx = & SSPIRxBuff[SSPIChannel][0];

    pRx = pTx;


    if(len>SSPI_MAX_BUFFER)
        return ;

    for(i = 0; i<len; i++)
    {
        SSPITxBuff[SSPIChannel][i] = *pTx;
        pTx++;
    }

#ifdef    SSPI_POLL
    if(SSPIChannel == 0)    //use ssp0
        pSSP = pSSPI0;
    else if(SSPIChannel == 1)   //use ssp1
        pSSP = pSSPI1;
    else
        return;

    for( i=0; i<len; i++ )
    {
        uw32_timeout = 0;
        while(pSSP->SR & SSPSR_RNE)
        {
            uw16_i = pSSP->DR;											// uc_i is used as dummy to clear receiving buffer if necessary
            if(uw32_timeout++ >= SSPIMAX_TIMEOUT)						// if the time excceeded, then break.
            {
                return;
            }
        }

        uw32_timeout = 0;
        while((pSSP->SR & (SSPSR_TNF | SSPSR_BSY)) != SSPSR_TNF)		// if TNF != 1(Transfer buffer is full) and SSPSR !=0(Busy), wait till 'not full' and 'not busy'
        {
            if(uw32_timeout++ >= SSPIMAX_TIMEOUT)					// if the time excceeded, then break.
            {
                return;
            }
        }

        pSSP->DR = SSPITxBuff[SSPIChannel][i];

        uw32_timeout = 0;
        while((pSSP->SR & (SSPSR_RNE | SSPSR_BSY)) != SSPSR_RNE)		// if RNE != 1(Receiving buffer is empty) and SSPSR !=0(Busy), wait till 'not empty' and 'not busy'
        {
            if(uw32_timeout++ >= SSPIMAX_TIMEOUT)					// if the time excceeded, then break.
            {
                //CS_Disable(SSPIChannel);                 //IOSET1 |= SSP0_SEL;	// CS Disable
                return;
            }
        }
        //CS_Disable(SSPIChannel);	                        //IOSET1 |= SSP0_SEL;	// CS Disable
        //* pRx = pSSP->DR;
        SSPIRxBuff[SSPIChannel][i] = pSSP->DR;

        pTx++,pRx++;
    }
#endif

// #ifdef     SSPI_INTERRUPT

// #endif
}
void SSPIFunc::WriteTxFIFO(unsigned short *pTx,unsigned short len,unsigned char SSPIChannel)
{
    //unsigned short uw16_i = 0;
    unsigned short i;
    //unsigned short * pRx;
    //unsigned long uw32_timeout = uw16_i;
    Reg_SSPI *pSSP = 0;

    //	pRx = pTx;

    if(len>SSPI_MAX_BUFFER)
        return ;

    for(i = 0; i<len; i++)
    {
        SSPITxBuff[SSPIChannel][i] = *pTx;
        pTx++;
    }

    if(SSPIChannel == 0)    //use ssp0
        pSSP = pSSPI0;
    else if(SSPIChannel == 1)   //use ssp1
        pSSP = pSSPI1;
    else
        return;

    for( i=0; i<len; i++ )
        pSSP->DR = SSPITxBuff[SSPIChannel][i];
}
void SSPIFunc::ReadRxFIFO(unsigned short len,unsigned char SSPIChannel)
{
    Reg_SSPI *pSSP = 0;

    if(len>SSPI_MAX_BUFFER)
        return ;

    if(SSPIChannel == 0)    //use ssp0
        pSSP = pSSPI0;
    else if(SSPIChannel == 1)   //use ssp1
        pSSP = pSSPI1;
    else
        return;

    for(int i = 0 ; i < len; i++)
        SSPIRxBuff[SSPIChannel][i] = pSSP->DR;
}
// unsigned char SSPIFunc::SSPI_Write_Byte(unsigned char SSPIChannel,unsigned char uc_Byte)
// {
// 	//  向SSPI接口发送一个字数据
// 	//  Send a block of data to the SSPI port.
// 	//  Returned value:	 true 操作成功；false操作失败

// 	UWord32 uw32_timeout = 0;
// 	Reg_SSPI *pSSP = 0;

// 	if(SSPIChannel == 0)    //use ssp0
// 	{
// 		pSSP = pSSPI0;
// 	}
// 	else if(SSPIChannel == 1 )   //use ssp1
// 	{
// 		pSSP = pSSPI1;
// 	}


// 	while ( !(pSSP->SR & SSPSR_TNF) )
// 	{
// 		if ( uw32_timeout++ >= SSPIMAX_TIMEOUT )//if the time excceeded ,then break.
// 		{
// 			return  (false);
// 		}
// 	}
// 	pSSP->DR=uc_Byte;
// 	uw32_timeout = 0;
// 	while( pSSP->SR & SSPSR_BSY)
// 	{
// 		if ( uw32_timeout++ >= SSPIMAX_TIMEOUT )//if the time excceeded ,then break.
// 		{
// 			return  (false);
// 		}
// 	}
// 	return  (true);
// }


// unsigned char SSPIFunc::SSPI_Read_Byte(unsigned char SSPIChannel)
// {
// 	//  从SSP 接口读取一个字数据
// 	//  Read from SSP port.
// 	UChar8  uc_Result=0;
// 	UWord32 uw32_timeout = 0;
// 	Reg_SSPI *pSSP = 0;

// 	if(SSPIChannel == 0)    //use ssp0
// 	{
// 		pSSP = pSSPI0;
// 	}
// 	else if(SSPIChannel ==1)   //use ssp1
// 	{
// 		pSSP = pSSPI1;
// 	}

// 	pSSP->DR = 0x00;
// 	while( LPC_SSP0->SR & SSPSR_BSY)    // (uw32_timeout < 6)  8字节发送时最少得延时6次,否则CLK没发送完CS信号就变了。
// 	{
// 		if ( uw32_timeout++ >= SSPIMAX_TIMEOUT )//if the time excceeded ,then break.
// 		{
// 			return  (false);
// 		}
// 	}

// 	//uc_Result = pSSP->DR;
// 	/*uw32_timeout = 0;
// 	while( !(pSSP->SR & SSPSR_RNE))   // (uw32_timeout < 6)  8字节发送时最少得延时6次,否则CLK没发送完CS信号就变了。
// 	{
// 	if ( uw32_timeout++ >= SSPIMAX_TIMEOUT )//if the time excceeded ,then break.
// 	{
// 	return  (false);
// 	}
// 	}*/

// 	// Hardware_Timer::DELAY_SYS(15);    // 8字节发送时最少得延时6次,否则CLK没发送完CS信号就变了。

// 	uw32_timeout = 0;
// 	while( !(pSSP->SR & SSPSR_RNE))   // (uw32_timeout < 6)  8字节发送时最少得延时6次,否则CLK没发送完CS信号就变了。
// 	{
// 		if ( uw32_timeout++ >= SSPIMAX_TIMEOUT )//if the time excceeded ,then break.
// 		{
// 			return  (false);
// 		}
// 	}
// 	uc_Result = pSSP->DR;
// 	return (uc_Result);
// }

void SSPIFunc::ClearFIFO(unsigned char SSPIChannel)
{
    unsigned short uw16_i;
    unsigned short uw16_j = 0;
    uw16_i = uw16_j;
    Reg_SSPI *pSSP = 0;

    if(SSPIChannel == 0 )    //use ssp0
    {
        pSSP = pSSPI0;
    }
    else if(SSPIChannel == 1)   //use ssp1
    {
        pSSP = pSSPI1;
    }

    for( uw16_i = 0; uw16_i < SSPI_FIFOSIZE; uw16_i ++) //clear the FIFO;
    {
        uw16_j = pSSP->DR;
    }
}

void SSPIFunc::ConfirmDevice ( unsigned char SSPIChannel,SSPI_Para * p_SSPI_Para )
{
    if( getModuleName(SSPIChannel) != p_SSPI_Para->uc_Module_Name)
    {
        setPara(p_SSPI_Para,SSPIChannel);
        memcpy(&SSPI_Para_me[SSPIChannel], p_SSPI_Para, sizeof(SSPI_Para));
        my_Init(SSPIChannel);              //3?ê??ˉ
        ARBUS_DELAY(10);
    }

}


// #ifdef SSPI_INTERRUPT
// void SSP0_IRQHandler(void)
// {
// 	DWORD regValue;
// 	Reg_SSPI *pSSP = pSSPI0;

// 	//clear interrupt flag
// 	pSSP->ICR |= SSPI_INT_RORIC;

// 	regValue = pSSP->MIS;
// 	if (( regValue & SSPI_RORMIS ) != SSPI_RORMIS)
// 	{
// 		pSSP->IMSC |= SSPI_INT_RORIC;
// 	}

// 	if ( regValue & SSPI_RORMIS )//传输完成
// 	{
// 		SSPIFunc::TxCounter++;
// 	}
// }
// #endif

/*********************************************************************************
**                            End Of File
*********************************************************************************/
