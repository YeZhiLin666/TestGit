
//include
#include "../../COMMON/includes.h"


////private data////
SSPI_Para    SSPIFunc::SSPI_Para_me[SSPI_MAX_CHANNEL];
UWord32     SSPIFunc::TxCounter;
//static成员变量——类内部声明；只能在cpp中的各方法外部定义
//（且不能加static关键词），定义时可以不进行初始化，这时默认为0(也可以不定义，
//但若使用到了该成员变量，则必须定义，否则链接出错)；
////private function////


////public data////
//SPI  SPI0   Port 0.15 SPI SCK, port0.17 MISO, port0.18 MOSI //
//SSP0    P1.20 uses GPIO SSP0_SCK0,P1.23 MISO0,P1.24 MOSI0//
//SSP1    p0.07 SCK1,p0.08 MISO1,p0.09 MOSI1//
//准备加个使能端口    CS  PM_OUT	comment out chen
unsigned char SSPIFunc::MyIOCount1 =    //三个通道
3;
unsigned char SSPIFunc::MyIO1[3][4] =    //三个通道
{
	{0,15,PM_MOD,0},
	{0,17,PM_MOD,0},
	{0,18,PM_MOD,0}
};

//其他先不管了
unsigned char SSPIFunc::MyIOCount2 =    //三个通道
3;
unsigned char SSPIFunc::MyIO2[3][4] =    //三个通道
{
	{0,7,PM_MOD,0},
	{0,8,PM_MOD,0},
	{0,9,PM_MOD,0}
};
unsigned char SSPIFunc::MyIOCount3 =    //三个通道
3;
unsigned char SSPIFunc::MyIO3[3][4] =    //三个通道
{
	{1,20,PM_MOD,0},
	{1,23,PM_MOD,0},
	{1,24,PM_MOD,0}
};
unsigned short SSPIFunc::SSPITxBuff[SSPI_MAX_CHANNEL][SSPI_MAX_BUFFER];

unsigned short SSPIFunc::SSPIRxBuff[SSPI_MAX_CHANNEL][SSPI_MAX_BUFFER];
////end public data////


//copy自原先的
//in	 N/A
//out    N/A
//SPI CLK为PCLK/SPI_REG  
//PCLK由PCLKSEL0来决定的  CCLK/APB
void SSPIFunc::initSSPICLK(unsigned char SSPIChannel)
{
	unsigned char uc_SelectSpeed;
	Reg_SSPI *pSSP = 0;
	uc_SelectSpeed =  SSPI_Para_me[SSPIChannel].uc_Speed;
	// para protection.
	if(SSPIChannel == 0 && uc_SelectSpeed > 7)			  
		uc_SelectSpeed = 7;

	if(SSPIChannel > 0 && uc_SelectSpeed > 9)
		uc_SelectSpeed = 9;
	//uc_temp=18/uc_SelectSpeed ;

	if(SSPIChannel == 0)    //SPI
	{
		S0SPCCR = 20 - 2 * uc_SelectSpeed;	// Set SPI CLK : S0SPCCR = 18, 16, 14, 12, 10, 8 accordingly when u_SelectSpeed = 1, 2, 3, 4, 5, 6
	}
	else
	{
		if(SSPIChannel == 1)
		{
			pSSP=(Reg_SSPI *)0xE0068000;
		}
		else if(SSPIChannel == 2)
		{
			pSSP=( Reg_SSPI *)0xE0030000;
		}

		pSSP->CPSR = 20 - 2 * uc_SelectSpeed;   //SCK =(CCLK/VPB)/(CPSDVSR*[SCR+1])=PCLK/(CPSDVSR*[SCR+1])
	}
}


void SSPIFunc::CS_Enable(unsigned char SSPIChannel)
{
// 	int i;

// 	if(SPI_Para_me[SPIChannel].IfSingleCS)
// 	{
// 		gpio_2468::Clr_G_Port( &(SPI_Para_me[SPIChannel].st_CS) );
// 	}
// 	else
// 	{
// 		for(i = 0; i < 3; i ++)
// 		{
// 			if((SPI_Para_me[SPIChannel].CS_Value & (0x01 << i)) == 0)
// 			{
// 				gpio_2468::Clr_G_Port( &(SPI_Para_me[SPIChannel].st_CS38[i]) );
// 			}
// 			else
// 			{
// 				gpio_2468::Set_G_Port( &(SPI_Para_me[SPIChannel].st_CS38[i]) );
// 			}
// 		}
// 		gpio_2468::Clr_G_Port( &(SPI_Para_me[SPIChannel].st_CS38EN) );
// 	}
	unsigned char i;
    switch (SSPI_Para_me[SSPIChannel].st_CS.Type)
	{
		case GP_MOD:
				GPIOFunc::Clr_G_Port( &(SSPI_Para_me[SSPIChannel].st_CS.GPIO_PIN) );    //bug comment out chen 20130106
			break;
		
		case Address_MOD:
      {		
			//20130218  chen
			uint16 temp = *(volatile unsigned short int *)(SSPI_Para_me[SSPIChannel].st_CS.Address);
			temp &= ~(SSPI_Para_me[SSPIChannel].st_CS.Value);
			*(volatile unsigned short int *)(SSPI_Para_me[SSPIChannel].st_CS.Address) = temp;

			
			////可用    comment out chen 20130131
			//uint16 temp = IBT_Base::NBUS_WRITE_DCS_status;
		   // temp &= ~(SSPI_Para_me[SSPIChannel].st_CS.Value);
		   // *(volatile unsigned short int *)(SSPI_Para_me[SSPIChannel].st_CS.Address) = temp;
		   // IBT_Base::NBUS_WRITE_DCS_status = temp;    //保存
			
			}
		    break;
		
		case CS38_MOD:
				for(i = 0; i < 3; i ++)
			{
				if((SSPI_Para_me[SSPIChannel].st_CS.CS_Value & (0x01 << i)) == 0)
				{
					GPIOFunc::Clr_G_Port( &(SSPI_Para_me[SSPIChannel].st_CS.st_CS38[i]) );
				}
				else
				{
					GPIOFunc::Set_G_Port( &(SSPI_Para_me[SSPIChannel].st_CS.st_CS38[i]) );
				}
			}
				GPIOFunc::Clr_G_Port( &(SSPI_Para_me[SSPIChannel].st_CS.st_CS38EN) );
			
			break;
			
		default:
			
			break;
	}
}
void SSPIFunc::CS_Disable(unsigned char SSPIChannel)
{

// 	if(SPI_Para_me[SSPIChannel].IfSingleCS)
// 	{
// 		GPIOFunc::Set_G_Port( (&SSPI_Para_me[SPIChannel].st_CS) );
// 	}
// 	else
// 	{
// 		GPIOFunc::Set_G_Port( (&SSPI_Para_me[SPIChannel].st_CS38EN) );
// 	}
		switch (SSPI_Para_me[SSPIChannel].st_CS.Type)
	  {
			case  GP_MOD:
				GPIOFunc::Set_G_Port( &(SSPI_Para_me[SSPIChannel].st_CS.GPIO_PIN) );
			break;
			case  Address_MOD:
			  {    
				//最好不要在case里声明变量，因为可能会造成case k，k的值被改变，如果非要这样，建议加上{}隔离开
				//20130218  chen
				uint16 temp = *(volatile unsigned short *)(SSPI_Para_me[SSPIChannel].st_CS.Address);
				temp |= (SSPI_Para_me[SSPIChannel].st_CS.Value);
				*(volatile unsigned short int *)(SSPI_Para_me[SSPIChannel].st_CS.Address) = temp; 


				////可用    comment out chen 20130131
				//uint16 temp = IBT_Base::NBUS_WRITE_DCS_status;
				//temp |= (SSPI_Para_me[SSPIChannel].st_CS.Value);
				//(*(volatile unsigned short int *)(SSPI_Para_me[SSPIChannel].st_CS.Address)) = temp;
				//IBT_Base::NBUS_WRITE_DCS_status = temp;
				}
				break;
			case CS38_MOD:
				GPIOFunc::Set_G_Port( (&SSPI_Para_me[SSPIChannel].st_CS.st_CS38EN) );
			break;
			default:
				break;
	}
}

////end private////


////publice function////


//set para to private data
//in  SPI_Para *  ,chn
//out N/A
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
	SSPI_Para_me[SSPIChannel]. st_CS.Type= p_SSPI_Para-> st_CS.Type;
	SSPI_Para_me[SSPIChannel]. st_CS.GPIO_PIN.P1= p_SSPI_Para-> st_CS.GPIO_PIN.P1;
	SSPI_Para_me[SSPIChannel]. st_CS.GPIO_PIN.P2= p_SSPI_Para-> st_CS.GPIO_PIN.P2;
	SSPI_Para_me[SSPIChannel]. st_CS.Address= p_SSPI_Para-> st_CS.Address;
	SSPI_Para_me[SSPIChannel]. st_CS.Value= p_SSPI_Para-> st_CS.Value;
	for(unsigned char i = 0 ; i <3 ; i++)
	{
	    SSPI_Para_me[SSPIChannel]. st_CS.st_CS38[i].P1= p_SSPI_Para->st_CS. st_CS38[i].P1;
	    SSPI_Para_me[SSPIChannel]. st_CS.st_CS38[i].P2= p_SSPI_Para-> st_CS.st_CS38[i].P2;
	}
    SSPI_Para_me[SSPIChannel]. st_CS.st_CS38EN.P1= p_SSPI_Para-> st_CS.st_CS38EN.P1;
	SSPI_Para_me[SSPIChannel]. st_CS.st_CS38EN.P2= p_SSPI_Para-> st_CS.st_CS38EN.P2;
	SSPI_Para_me[SSPIChannel]. st_CS.CS_Value= p_SSPI_Para-> st_CS.CS_Value;
}

void SSPIFunc::setCS(SSPI_Para * p_SSPI_Para,unsigned char SSPIChannel)
{
	SSPI_Para_me[SSPIChannel].uc_Module_Name = p_SSPI_Para->uc_Module_Name;

// 	SPI_Para_me[SPIChannel].IfSingleCS = p_SPI_Para->IfSingleCS;

// 	SPI_Para_me[SPIChannel].st_CS.P1 = p_SPI_Para->st_CS.P1;
// 	SPI_Para_me[SPIChannel].st_CS.P2 = p_SPI_Para->st_CS.P2;

// 	for(unsigned char i = 0 ; i <3 ; i++)
// 	{
// 		SPI_Para_me[SPIChannel].st_CS38[i].P1 = p_SPI_Para->st_CS38[i].P1;
// 		SPI_Para_me[SPIChannel].st_CS38[i].P2 = p_SPI_Para->st_CS38[i].P2;
// 	}

// 	SPI_Para_me[SPIChannel].st_CS38EN.P1 = p_SPI_Para->st_CS38EN.P1;
//     SPI_Para_me[SPIChannel].st_CS38EN.P2 = p_SPI_Para->st_CS38EN.P2;

// 	SPI_Para_me[SPIChannel].CS_Value = p_SPI_Para->CS_Value;

// 	SPI_Para_me[SPIChannel].st_SSEL.P1 = p_SPI_Para->st_SSEL.P1;
// 	SPI_Para_me[SPIChannel].st_SSEL.P2 = p_SPI_Para->st_SSEL.P2;
		SSPI_Para_me[SSPIChannel]. st_CS.Type= p_SSPI_Para-> st_CS.Type;
	  SSPI_Para_me[SSPIChannel]. st_CS.GPIO_PIN.P1= p_SSPI_Para-> st_CS.GPIO_PIN.P1;
	  SSPI_Para_me[SSPIChannel]. st_CS.GPIO_PIN.P2= p_SSPI_Para-> st_CS.GPIO_PIN.P2;
		SSPI_Para_me[SSPIChannel]. st_CS.Address= p_SSPI_Para-> st_CS.Address;
		SSPI_Para_me[SSPIChannel]. st_CS.Value= p_SSPI_Para-> st_CS.Value;
	  for(unsigned char i = 0 ; i <3 ; i++)
	{
	  SSPI_Para_me[SSPIChannel]. st_CS.st_CS38[i].P1= p_SSPI_Para->st_CS. st_CS38[i].P1;
	  SSPI_Para_me[SSPIChannel]. st_CS.st_CS38[i].P2= p_SSPI_Para-> st_CS.st_CS38[i].P2;
	}
    SSPI_Para_me[SSPIChannel]. st_CS.st_CS38EN.P1= p_SSPI_Para-> st_CS.st_CS38EN.P1;
		SSPI_Para_me[SSPIChannel]. st_CS.st_CS38EN.P2= p_SSPI_Para-> st_CS.st_CS38EN.P2;
	  SSPI_Para_me[SSPIChannel]. st_CS.CS_Value= p_SSPI_Para-> st_CS.CS_Value;
}


//initCLK    init CSport    init SPI control register and then enable spi interface  with the persent parameter
//init clk
//init cs port
//power control
//init register
//in    chn
//out  N/A
void SSPIFunc::my_Init(unsigned char SSPIChannel)
{
	
    unsigned char uc_SelectBits;
    unsigned char uc_SelectMSTR;
	unsigned char uc_SelectLSBF;
	unsigned char uc_SelectCPOL,uc_SelectCPHA;

	unsigned short u16_temp = 0;
	unsigned char uc_temp = 0;

	Reg_SSPI *pSSP;

	//initCLK
	SSPIFunc::initSSPICLK(SSPIChannel);

	//initCS port
	//如果使用高速GPIO寄存器的话 需要设置SCS状态控制寄存器
	//SSPIFunc::initSPICS(SPIChannel);

    uc_SelectBits = SSPI_Para_me[SSPIChannel].uc_BITS;

	uc_SelectCPOL = SSPI_Para_me[SSPIChannel].uc_CPOL;
    uc_SelectCPHA = SSPI_Para_me[SSPIChannel].uc_CPHA;
    uc_SelectMSTR = SSPI_Para_me[SSPIChannel].uc_MSTR;
    uc_SelectLSBF = SSPI_Para_me[SSPIChannel].uc_LSBF;

	if(SSPIChannel == 0)    //SPI
	{
			PCONP |= (1 << 8);    //power control SPI
	    S0SPCR = 0x0000;   //复位
	    //初始化传输BITS
	    if( (uc_SelectBits) >8 && (uc_SelectBits<17) )    //在9-16之间
	    {
	        S0SPCR |= 0x0004;    //bitEnable
	        u16_temp = uc_SelectBits;
	        u16_temp &= 0x0F;    //去掉0x10000的1
	        u16_temp = u16_temp<< 8;         //左移8位
	        S0SPCR |= u16_temp;
	    }
	    else    //传输8位
	    {
	        S0SPCR |= 0x0800;
	    }
	
	    if(uc_SelectCPOL == 1)
		{
	        S0SPCR |= DE_SPI_CPOL;
		}
	
		if(uc_SelectCPHA == 1)
		{
	        S0SPCR |= DE_SPI_CPHA;
		}
	    
		if(uc_SelectMSTR == 1)     //主模式
		{
			S0SPCR |= DE_SPI_MASTER_MODE; 
		}
	
		if(uc_SelectLSBF == 1)      //LSBF
		{
	        S0SPCR |= DE_SPI_LSBF;
		}
	
		#ifdef     SPI_INTERRUPT
			S0SPCR |= DE_SPIF;  //中断使能
		#endif
	
	}
	else
	{
		if(SSPIChannel == 1)    //SSP0
		{
		  pSSP =  (Reg_SSPI *)0xE0068000;
			PCONP |= (1 << 21);    //power control SSP0
		}
		if(SSPIChannel == 2)    //SSP1
		{
			pSSP =  (Reg_SSPI *)0xE0030000;
			PCONP |= (1 << 10);    //power control SSP1
		}

	    pSSP->CR0 = 0x0000;    //复位
		pSSP->CR1 = 0x0000;
		if( (uc_SelectBits) >8 && (uc_SelectBits<17) )    //在9-16之间
		{
			u16_temp = uc_SelectBits - 1;
			u16_temp &= 0x0F;    //去掉0x10000的1
			pSSP->CR0 |= u16_temp;
		}
		else    //传输8位
		{
			pSSP->CR0 |= 0x0007;
		}
	
		if(uc_SelectCPOL == 1)
		{
			pSSP->CR0 |= 0x0040;
		}
	
		if(uc_SelectCPHA == 1)
		{
			pSSP->CR0 |= 0x0080;
		}
	
		if(uc_SelectMSTR == 0)     //主模式
		{
			pSSP->CR1 |= 0x0004; 
		}
	
		for( uc_temp=0; uc_temp<8; uc_temp++)  //clear the FIFO;
		{
			pSSP->DR = 0;	
		}
	
		pSSP ->CR1 |= 0x0002;    //enable SSP0
	
	    #ifdef     SPI_INTERRUPT
		#endif
	}//end SSP0
}
//end registerSPI

//check para if fit
//in  SPI_Para * , chn
//check: 0 相同    1 CS不同，set CS    2 全都不同，setPara
unsigned char SSPIFunc::Check(SSPI_Para * p_SSPI_Para,unsigned char SSPIChannel)   //用这个参数跟当前的比较来判断能否直接使用
{
	//present
    unsigned char uc_Pre_Speed = SSPI_Para_me[SSPIChannel].uc_Speed ;
//	unsigned char uc_Pre_Name = SSPI_Para_me[SSPIChannel].uc_Module_Name;
	unsigned char uc_Pre_CPOL = SSPI_Para_me[SSPIChannel].uc_CPOL;
	unsigned char uc_Pre_CPHA = SSPI_Para_me[SSPIChannel].uc_CPHA;
	//unsigned char uc_IfSingleCS = SPI_Para_me[SSPIChannel].IfSingleCS;
    
	//G_Port uc_Pre_CS = SSPI_Para_me[SSPIChannel].st_CS;

    //setting
	unsigned char uc_Set_Speed = p_SSPI_Para->uc_Speed;
//	unsigned char uc_Set_Name = p_SSPI_Para->uc_Module_Name;
	unsigned char uc_Set_CPOL = p_SSPI_Para->uc_CPOL;
	unsigned char uc_Set_CPHA = p_SSPI_Para->uc_CPHA;
	//unsigned char uc_Set_IfSingleCS = p_SSPI_Para->IfSingleCS;

	//G_Port uc_Set_CS = p_SSPI_Para->st_CS;

/*	if(uc_Pre_Name == uc_Set_Name)
	{
		return 0;
	}
*/
	if( (uc_Pre_Speed == uc_Set_Speed)&&(uc_Pre_CPOL == uc_Set_CPOL)&&(uc_Pre_CPHA == uc_Set_CPHA) )
	{
		return 1;
	}
	else
	{
		return 2;
	}
}

//getModuleName
//in   chn
//out  uc_moduleName
unsigned char SSPIFunc::getModuleName(unsigned char SSPIChannel)
{
	return SSPI_Para_me[SSPIChannel].uc_Module_Name;
}

//duplex transfer
//in    u16 data , chn
//out  receive data
//SSP和SPI在硬件接口上是有差异的，SSP具有FIFP，而SPI不具备
//所以说感觉SSP双工操作还是问题很大
unsigned short SSPIFunc::RxTx_Frame(unsigned short Txdata,unsigned char SSPIChannel)
{
	unsigned short uw16_i;
	unsigned short uw16_feedback = 0;
	unsigned long  uw32_timeout;
    
	Reg_SSPI  * pSSP = 0;

#ifdef SSPI_POLL
	if(SSPIChannel == 0)    //use spi
	{
	    uw32_timeout = 0;

		CS_Enable(SSPIChannel);

		S0SPDR = Txdata;
		while ( !(S0SPSR & S0SPSR_SPIF) )
		{
			if ( uw32_timeout ++>= SSPIMAX_TIMEOUT )//if the time excceeded ,then break.
			{
				return  (0);
			}
		}
		uw16_feedback = S0SPDR;

		CS_Disable(SSPIChannel);

		return uw16_feedback;
	}
	else 
	{
	    if(SSPIChannel == 1)    //use ssp0
		{
		    pSSP = (Reg_SSPI *)0xE0068000;
		}
		else if(SSPIChannel == 2)    //use ssp1
		{
			pSSP = (Reg_SSPI *)0xE0030000;
		}



		uw32_timeout = 0;
		while(SSP0SR & SSPSR_RNE)
		{
			uw16_i = SSP0DR;											// uc_i is used as dummy to clear receiving buffer if necessary
			if(uw32_timeout++ >= SSPIMAX_TIMEOUT)						// if the time excceeded, then break.
			{
			    return 0; 
			}
		}

		uw32_timeout = 0;
		while((SSP0SR & (SSPSR_TNF | SSPSR_BSY)) != SSPSR_TNF)		// if TNF != 1(Transfer buffer is full) and SSPSR !=0(Busy), wait till 'not full' and 'not busy'
		{
			if(uw32_timeout++ >= SSPIMAX_TIMEOUT)					// if the time excceeded, then break.
			{
			    return 0; 
			}
		}

		CS_Enable(SSPIChannel);                             //IOCLR1 |= SSP0_SEL;			// CS Enable

		pSSP->DR = Txdata;									     	// Send the byte immediately.

		uw32_timeout = 0;
		while((SSP0SR & (SSPSR_RNE | SSPSR_BSY)) != SSPSR_RNE)		// if RNE != 1(Receiving buffer is empty) and SSPSR !=0(Busy), wait till 'not empty' and 'not busy'
		{
			if(uw32_timeout++ >= SSPIMAX_TIMEOUT)					// if the time excceeded, then break.
			{
				CS_Disable(SSPIChannel);;                  //IOSET1 |= SSP0_SEL;	// CS Disable
			    return 0; 
			}
		}
		CS_Disable(SSPIChannel);                        //IOSET1 |= SSP0_SEL;	// CS Disable
		uw16_feedback = pSSP->DR;									
		// uc_Bytes = SSP0DR;										// Read the received byte and put it in the same buffer in the recent sent position.

// is work but don't know why		

		return uw16_feedback;
	}

#endif

#ifdef     SPI_INTERRUPT

#endif
}
//SPI0 Init in order to Read IP from Fram
//in N/A
//out N/A
void SSPIFunc::SSPI0_Init(void)
{
		
	SSPI_Para 	EEPROM_Spi = 		// will be replaced with setPara(&xxxxx::SPISettying, 0)
			{
				EEPROM_SSPI,    //name
				5,    //speed  1.8M
				0,    //CPOL
				0,    //CPHA
				1,    //master
				8,    //bits
				0,    //LSBF
				{
				GP_MOD,    //Single CS
				{1,21}, //cs pin 1.21
				0,
				0,
				{{1,21},{1,21},{1,21}}, //cs38 ignored
				{1,21},				// cs38 en ignored
				0,					// cs38 value ignored
			  },
				
				{
				GP_MOD,    // ssel ignored
				{1,21}, //cs pin 1.21
				0,
				0,
				{{1,21},{1,21},{1,21}}, //cs38 ignored
				{1,21},				// cs38 en ignored
				0,					// cs38 value ignored
			  },	
				
			};
			//通道0即SPI通道用于读取EEPROM
			SSPIFunc::setPara(&EEPROM_Spi,0);    //设置参数
			SSPIFunc::my_Init(0);    //初始化
}

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
		
    if(SSPIChannel == 0)
	{
	   	for ( uw32_i = 0; uw32_i < uw32_Length; uw32_i++ )
		{
			S0SPDR = SSPITxBuff[SSPIChannel][uw32_i];
			while ( !(S0SPSR & S0SPSR_SPIF) )
			{
			   if ( uw32_timeout ++>= SSPIMAX_TIMEOUT )//if the time excceeded ,then break.
			       {
			               return  (false);
				}
			}
			SSPIRxBuff[SSPIChannel][uw32_i]  = S0SPDR;		/* Flush the RxFIFO */
		}
 
	}
	else
	{
	  
	  if(SSPIChannel == 1) 
	  pSSP = (Reg_SSPI *)0xE0068000;
	  if(SSPIChannel == 2) 
	  pSSP = (Reg_SSPI *)0xE0030000;
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
			while( pSSP->SR & SSPSR_BSY);             // WAIT FOR TRANSFER COMPLETE;
			{
				if ( uw32_timeout++>= SSPIMAX_TIMEOUT )//if the time excceeded ,then break.
				{
					return  (false);
				}
			}
			SSPIRxBuff[SSPIChannel][uw32_i] = pSSP->DR;		/* Flush the RxFIFO */
	  }

	}

      return  (uc_Result);
}

//start RxTxBuffer Transfer in many CS
//in	len, chn
//out N/A
void SSPIFunc::RxTx_Frames(unsigned short *SSPI_Tx,unsigned short len,unsigned char SSPIChannel)
{
	unsigned char i;
	unsigned short * pRx;
	unsigned long uw32_timeout;
	unsigned short uw16_i=0;
	Reg_SSPI *pSSP = 0;

//	pTx = & SSPITxBuff[SSPIChannel][0];
	pRx = & SSPIRxBuff[SSPIChannel][0];
  

	if(len>SSPI_MAX_BUFFER)
		return ;

//  	for(i = 0;i<len;i++) 	{
//  		SPITxBuff[SPIChannel][i] =*SPI_Tx;
//  		*SPI_Tx++;
//  	}


#ifdef SSPI_POLL
	if(SSPIChannel == 0)    //user spi
	{
		CS_Enable(SSPIChannel);
    	for( i=0;i<len;i++ )
		{
			uw32_timeout = 0;

			

			S0SPDR =*SSPI_Tx;
			while ( !(S0SPSR & S0SPSR_SPIF) )
			{
				if ( uw32_timeout ++>= SSPIMAX_TIMEOUT )//if the time excceeded ,then break.
				{
					return ;
				}
			}
			* pRx = S0SPDR;

			SSPI_Tx++,pRx++;

			
		}
		CS_Disable(SSPIChannel);
	}
	else
	{
	    if(SSPIChannel == 1)    //use ssp0
		{
		    pSSP = (Reg_SSPI *)0xE0068000;
		}
		else if(SSPIChannel == 2)    //use ssp1
		{
		    pSSP = (Reg_SSPI *)0xE0030000;
		}
			CS_Enable(SSPIChannel);
		for( i=0;i<len;i++ )
		{



		uw32_timeout = 0;
		while(SSP0SR & SSPSR_RNE)
		{
			uw16_i = SSP0DR;											// uc_i is used as dummy to clear receiving buffer if necessary
			if(uw32_timeout++ >= SSPIMAX_TIMEOUT)						// if the time excceeded, then break.
			{
			    return; 
			}
		}

		uw32_timeout = 0;
		while((SSP0SR & (SSPSR_TNF | SSPSR_BSY)) != SSPSR_TNF)		// if TNF != 1(Transfer buffer is full) and SSPSR !=0(Busy), wait till 'not full' and 'not busy'
		{
			if(uw32_timeout++ >= SSPIMAX_TIMEOUT)					// if the time excceeded, then break.
			{
			    return; 
			}
		}

	

		pSSP->DR = *SSPI_Tx;									     	// Send the byte immediately.

		uw32_timeout = 0;
		while((SSP0SR & (SSPSR_RNE | SSPSR_BSY)) != SSPSR_RNE)		// if RNE != 1(Receiving buffer is empty) and SSPSR !=0(Busy), wait till 'not empty' and 'not busy'
		{
			if(uw32_timeout++ >= SSPIMAX_TIMEOUT)					// if the time excceeded, then break.
			{
				CS_Disable(SSPIChannel);                 //IOSET1 |= SSP0_SEL;	// CS Disable
			    return; 
			}
		}
		
		* pRx = pSSP->DR;
		
		SSPI_Tx++,pRx++;
			
		}
		
			CS_Disable(SSPIChannel);	                        //IOSET1 |= SSP0_SEL;	// CS Disable
	}
#endif

#ifdef     SSPI_INTERRUPT

#endif
}


////start RxTxBuffer Transfer in 1CS
////in	len , chn
////out N/A
//int SSPIFunc::RxTx_Frames_1CS(unsigned short *arry,unsigned short len,unsigned char SSPIChannel)
//{
//	unsigned short i = 0;
//	unsigned short * pRx = 0;
//	unsigned short * pTx = 0;
//	unsigned long uw32_timeout;
//	Reg_SSPI *pSSP = 0;
//
//	pTx = & SSPITxBuff[SSPIChannel][0];
//	pRx = & SSPIRxBuff[SSPIChannel][0];
//
//	if(len > 8)		// using Fifo for SSP0 and SSP1, can not exceed Fifo count : 8
//		return 0;
//
//	for(i = 0;i<len;i++)
//	{
//		SSPITxBuff[SSPIChannel][i] = *arry;
//		arry++;
//	}
//
//	
//	
////#ifdef SSPI_POLL
//	if(SSPIChannel == 0)    //user spi
//	{
//		CS_Enable(SSPIChannel);
//    	for(i=0; i < len; i++)
//		{
//			uw32_timeout = 0;
//
//			S0SPDR = *pTx;
//			while (!(S0SPSR & S0SPSR_SPIF))
//			{
//				if ( uw32_timeout ++ >= SSPIMAX_TIMEOUT )//if the time excceeded ,then break.
//				{
//					CS_Disable(SSPIChannel);
//					return 0;
//				}
//			}
//			* pRx = S0SPDR;
//
//			pTx++;
//			pRx++;
//		}
//	}
//	else
//	{
//	    if(SSPIChannel == 1)         //use ssp0
//		{
//		    pSSP = (Reg_SSPI *)0xE0068000;
//		}
//		else if(SSPIChannel == 2)    //use ssp1
//		{
//		    pSSP = (Reg_SSPI *)0xE0030000;
//		}
//
////  		// Wait till my Xsfr session is ready.(Tx Fifo empty)
////  		uw32_timeout = 0;
////  		while((pSSP->SR & SSPSR_TFE) == 0)
////  		{
////  			if(uw32_timeout++ >= SSPIMAX_TIMEOUT)					// if the time excceeded, then break.
////  			{
////  			    return 0; 
////  			}
////  		}
////  		// Flushing Data FIFO
////  		uw32_timeout = 0;
////  		while((pSSP->SR & SSPSR_RNE) != 0)
////  		{
////  			i = pSSP->DR;
////  			if(uw32_timeout++ >= SSPIMAX_TIMEOUT)					// if the time excceeded, then break.
////  			{
////  			    return 0; 
////  			}
////  		}
//
//		// Sending the data
//		CS_Enable(SSPIChannel);
//		for(i = 0; i < len; i++)
//		{
//			pSSP->DR = *pTx;
//			pTx ++;
//		}
//		// Retrieving the data		
//		for(i = 0; i < len; i++)
//		{									     	// Send the byte immediately.
//			uw32_timeout = 0;
//			while((pSSP->SR & SSPSR_RNE) != SSPSR_RNE)		// if RNE != 1(Receiving buffer is empty) , wait till 'not empty'
//			{
//				if(uw32_timeout++ >= SSPIMAX_TIMEOUT)					// if the time excceeded, then break.
//				{
//					CS_Disable(SSPIChannel);                  //IOSET1 |= SSP0_SEL;	// CS Disable
//				    return 0; 
//				}
//			}								
//			* pRx = pSSP->DR;											// Read the received byte and put it in the same buffer in the recent sent position.
// 			pRx++;
//		}
//	}
// 
//	CS_Disable(SSPIChannel);
//
//	return 1;
////#endif
////
////#ifdef     SSPI_INTERRUPT
////	{
////
////	}
////#endif
//}
//SSP 貌似有bug    comment out chen 20130106



//comment out chen 20130106
//start RxTxBuffer Transfer in 1CS
//in	len , chn
//out N/A
int SSPIFunc::RxTx_Frames_1CS(unsigned short *arry,unsigned short len,unsigned char SSPIChannel)
{
	unsigned short i = 0;
	unsigned short * pRx = 0;
	unsigned short * pTx = 0;
	unsigned long uw32_timeout;
	unsigned short uw16_i;
	Reg_SSPI *pSSP = 0;

	pTx = & SSPITxBuff[SSPIChannel][0];
	pRx = & SSPIRxBuff[SSPIChannel][0];

	if(len > 8)		// using Fifo for SSP0 and SSP1, can not exceed Fifo count : 8
		return 0;

	for(i = 0;i<len;i++)
	{
		SSPITxBuff[SSPIChannel][i] = *arry;
		arry++;
	}


	if(SSPIChannel == 0)    //user spi
	{
		CS_Enable(SSPIChannel);
		for(i=0; i < len; i++)
		{
			uw32_timeout = 0;

			S0SPDR = *pTx;
			while (!(S0SPSR & S0SPSR_SPIF))
			{
				if ( uw32_timeout ++ >= SSPIMAX_TIMEOUT )//if the time excceeded ,then break.
				{
					CS_Disable(SSPIChannel);
					return 0;
				}
			}
			* pRx = S0SPDR;

			pTx++;
			pRx++;
		}
	}
	else
	{
		if(SSPIChannel == 1)         //use ssp0
		{
			pSSP = (Reg_SSPI *)0xE0068000;
		}
		else if(SSPIChannel == 2)    //use ssp1
		{
			pSSP = (Reg_SSPI *)0xE0030000;
		}

	//  		// Wait till my Xsfr session is ready.(Tx Fifo empty)
	//  		uw32_timeout = 0;
	//  		while((pSSP->SR & SSPSR_TFE) == 0)
	//  		{
	//  			if(uw32_timeout++ >= SSPIMAX_TIMEOUT)					// if the time excceeded, then break.
	//  			{
	//  			    return 0; 
	//  			}
	//  		}
	//  		// Flushing Data FIFO
	//  		uw32_timeout = 0;
	//  		while((pSSP->SR & SSPSR_RNE) != 0)
	//  		{
	//  			i = pSSP->DR;
	//  			if(uw32_timeout++ >= SSPIMAX_TIMEOUT)					// if the time excceeded, then break.
	//  			{
	//  			    return 0; 
	//  			}
	//  		}

		// Sending the data
		CS_Enable(SSPIChannel);
		// Retrieving the data		
		for(i = 0; i < len; i++)
		{					
			uw32_timeout = 0;
			while(pSSP->SR & SSPSR_RNE)
			{
				uw16_i = pSSP->DR;											// uc_i is used as dummy to clear receiving buffer if necessary
				if(uw32_timeout++ >= SSPIMAX_TIMEOUT)						// if the time excceeded, then break.
				{
					CS_Disable(SSPIChannel);   
					return 0; 
				}
			}

			uw32_timeout = 0;
			while((pSSP->SR & (SSPSR_TNF | SSPSR_BSY)) != SSPSR_TNF)		// if TNF != 1(Transfer buffer is full) and SSPSR !=0(Busy), wait till 'not full' and 'not busy'
			{
				if(uw32_timeout++ >= SSPIMAX_TIMEOUT)					// if the time excceeded, then break.
				{
					CS_Disable(SSPIChannel);   
					return 0; 
				}
			}
			pSSP->DR = *pTx;									     	// Send the byte immediately.

			uw32_timeout = 0;
			while((pSSP->SR & (SSPSR_RNE | SSPSR_BSY)) != SSPSR_RNE)		// if RNE != 1(Receiving buffer is empty) and SSPSR !=0(Busy), wait till 'not empty' and 'not busy'
			{
				if(uw32_timeout++ >= SSPIMAX_TIMEOUT)					// if the time excceeded, then break.
				{
					CS_Disable(SSPIChannel);   
					return 0; 
				}
			}
			* pRx = pSSP->DR;

			pTx++,pRx++;
		}
	}

	CS_Disable(SSPIChannel);

	return 1;
}



//0811 SPI done chen
//0815 add ssp transfer
//0815 ssp is no work    need set pclk
//看例程说明SSP是8位FIFO帧操作，如果单独操作需要DMA
//SSP初始化，FIFO方式，硬件接线是否有问题 等等	需要解决
//all is ok

#ifdef SSPI_INTERRUPT
void SPI0HandlerISR(void) __irq
{
	DWORD regValue;

	S0SPINT = SPI0_INT_FLAG;		//clear interrupt flag
	//IENABLE;				/* handles nested interrupt */

	regValue = S0SPSR;
	if ( regValue & WCOL )
	{
		SPI0Status |= SPI0_COL;
	}
	if ( regValue & SPIF )//传输完成
	{
		SPI0Status |= SPI0_TX_DONE;
		TxCounter++;
	}
	//IDISABLE;
	VICVectAddr = 0;		//Acknowledge Interrupt
}
#endif

/*********************************************************************************
**                            End Of File
*********************************************************************************/
