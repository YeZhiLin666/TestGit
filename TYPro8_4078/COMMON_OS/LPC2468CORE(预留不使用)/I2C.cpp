
#include "../../COMMON/includes.h"

/*
Slave Address
STA ( 1 0 1 0 A2 A1 A0 R/W )   ACK/NACK
A2 A1 A0 拨码开关
1026
目前只完成了SDA1 SCL1
*/

//private
unsigned char I2CFunc::I2CG_Address[I2C_MAX_CHANNEL];

unsigned char I2CFunc::I2CWrite_Length[I2C_MAX_CHANNEL];
unsigned char I2CFunc::I2CRead_Length[I2C_MAX_CHANNEL];

unsigned char I2CFunc::BlockFlag[I2C_MAX_CHANNEL];//是否读取块

unsigned char I2CFunc::DataBuf[I2C_MAX_CHANNEL];

volatile unsigned char I2CFunc::I2CMasterState[I2C_MAX_CHANNEL];//全局状态
volatile unsigned char I2CFunc::I2CIsBusy[I2C_MAX_CHANNEL];//是否busy
volatile  unsigned char I2CFunc::I2CIsBuffer[I2C_MAX_CHANNEL];//buffer是否被占用

//特殊的功能
volatile unsigned char I2CFunc::FirstWriteFlag[I2C_MAX_CHANNEL];

//buffer指针
unsigned char I2CFunc::pReadBuf[I2C_MAX_CHANNEL];
unsigned char I2CFunc::pWriteBuf[I2C_MAX_CHANNEL];

//public
////user stuff////
//在gpio里添加

unsigned char I2CFunc::MyIOCount = 4;
unsigned char I2CFunc::MyIO[][4] = //11功能SDA1 SCL1  19 20        SDA0 SCL0    01功能        **貌似只适用SDA1 SCL1    24LC128
{
	0,19,PM_MOD,0,
	0,20,PM_MOD,0,
	0,27,PM_MOD,0,
	0,28,PM_MOD,0
};


////user stuff////

unsigned char I2CFunc::I2CReadBuffer[I2C_MAX_CHANNEL][MAX_I2C_BUFFER];
unsigned char I2CFunc::I2CWriteBuffer[I2C_MAX_CHANNEL][MAX_I2C_BUFFER];
unsigned char I2CFunc::ReadData[I2C_MAX_CHANNEL];

////////end data////////



//直接作为主机使用
UWord32 I2CFunc::my_Init(unsigned char chn)
{
	if(chn > I2C_MAX_CHANNEL)
	{
		return FALSE;
	}

	switch(chn)
	{
	    case 0:
			//I2C0接口
			{
				//功率enable
				PCONP |= (1 << 7);//I2C0

				//GPIO
                //I2C1    P0.27 uses GPIO I2C0_SDA, P0.28 I2C0_SCL //

				/*--- Clear flags ---*/
				I20CONCLR = I2CONCLR_AAC | I2CONCLR_SIC | I2CONCLR_STAC | I2CONCLR_I2ENC; 

				/*--- Reset registers ---*/
				//速率
				//I20SCLL   = I2SCLL_SCLL;
				//I20SCLH   = I2SCLH_SCLH;
                unsigned char uc_Temp=300/I2S_SelectSpeed;     //[(120/2)*(1/2)/speed]=30*(10)/speed //  I2SCLL+I2SCLH values at PCLK (MHz) 60  
				if( I2S_SelectSpeed>=4)
				{
				    I20SCLL  = 75;  //I2Cbitfreq 400KHz; //  PCLK=  60(MHz), I2SCLL+I2SCLH values=150
				    I20SCLH   = 75;  //I2Cbitfreq=(120M/2)/(i2cll+i2sclh))=400khz;  CK =(CCLK/VPB)/(i2cll+i2sclh)
				}
				else if( I2S_SelectSpeed<=1)
				{
					I20SCLL   = 300;  //I2Cbitfreq 100Hz; //  PCLK=  60(MHz), I2SCLL+I2SCLH values=600
					I20SCLH   = 300;  //I2Cbitfreq=(120M/2)/(i2cll+i2sclh))=100khz;  CK =(CCLK/VPB)/(i2cll+i2sclh)
				}
				else 
				{
					I20SCLL   = uc_Temp;  //I2Cbitfreq 0Hz---400KHz;
					I20SCLH   = uc_Temp;  //
				}

				if( IRQFunc::install_irq( I2C0_INT, (void *)I2C0HandlerISR, HIGHEST_PRIORITY) == FALSE)
				{
					return( FALSE );
				}

				I20CONSET = I2CONSET_I2EN;//使能

				//全局变量初始化
				{
					I2CMasterState[chn] = I2C_IDLE;
					I2CIsBusy[chn] = 0;//不忙
					I2CIsBuffer[chn] = 0;
				}

			}
		break;
		case 1:
			//I2C1接口
			{
				//功率enable
				PCONP |= (1 << 19);//I2C1

				//GPIO
                //I2C1    P0.19 uses GPIO I2C1_SDA, P0.20 I2C1_SCL //

				/*--- Clear flags ---*/
				I21CONCLR = I2CONCLR_AAC | I2CONCLR_SIC | I2CONCLR_STAC | I2CONCLR_I2ENC; 

				/*--- Reset registers ---*/
				//速率
				//I21SCLL   = I2SCLL_SCLL;
				//I21SCLH   = I2SCLH_SCLH;
                
				//--- Reset registers ---//
				//速率
			    unsigned char uc_Temp=300/I2S_SelectSpeed;     //[(120/2)*(1/2)/speed]=30*(10)/speed //  I2SCLL+I2SCLH values at PCLK (MHz) 60  
				if( I2S_SelectSpeed>=4)
				{
				    I21SCLL  = 75;  //I2Cbitfreq 400KHz; //  PCLK=  60(MHz), I2SCLL+I2SCLH values=150
				    I21SCLH   = 75;  //I2Cbitfreq=(120M/2)/(i2cll+i2sclh))=400khz;  CK =(CCLK/VPB)/(i2cll+i2sclh)
				}
				else if( I2S_SelectSpeed<=1)
				{
					I21SCLL   = 300;  //I2Cbitfreq 100Hz; //  PCLK=  60(MHz), I2SCLL+I2SCLH values=600
					I21SCLH   = 300;  //I2Cbitfreq=(120M/2)/(i2cll+i2sclh))=100khz;  CK =(CCLK/VPB)/(i2cll+i2sclh)
				}
				else 
				{
					I21SCLL   = uc_Temp;  //I2Cbitfreq 0Hz---400KHz;
					I21SCLH   = uc_Temp;  //
				}
				if( IRQFunc::install_irq( I2C1_INT, (void *)I2C1HandlerISR, HIGHEST_PRIORITY) == FALSE)
				{
					return( FALSE );
				}

				I21CONSET = I2CONSET_I2EN;//使能

				//全局变量初始化
				{
					I2CMasterState[chn] = I2C_IDLE;
					I2CIsBusy[chn] = 0;//不忙
					I2CIsBuffer[chn] = 0;
				}

			}
			break;
		case 2:
			//I2C2接口
			{
				//功率enable
				PCONP |= (1 << 26);//I2C2

				//GPIO
                //I2C2    P0. uses GPIO I2C2_SDA, P0. I2C2_SCL //
				
				
				/*--- Clear flags ---*/
				I22CONCLR = I2CONCLR_AAC | I2CONCLR_SIC | I2CONCLR_STAC | I2CONCLR_I2ENC; 

				//I22SCLL   = I2SCLL_SCLL;
				//I22SCLH   = I2SCLH_SCLH;
				
                //--- Reset registers ---//
				//速率
			    unsigned char uc_Temp=300/I2S_SelectSpeed;     //[(120/2)*(1/2)/speed]=30*(10)/speed //  I2SCLL+I2SCLH values at PCLK (MHz) 60  
				if( I2S_SelectSpeed>=4)
				{
				    I21SCLL  = 75;  //I2Cbitfreq 400KHz; //  PCLK=  60(MHz), I2SCLL+I2SCLH values=150
				    I21SCLH   = 75;  //I2Cbitfreq=(120M/2)/(i2cll+i2sclh))=400khz;  CK =(CCLK/VPB)/(i2cll+i2sclh)
				}
				else if( I2S_SelectSpeed<=1)
				{
					I21SCLL   = 300;  //I2Cbitfreq 100Hz; //  PCLK=  60(MHz), I2SCLL+I2SCLH values=600
					I21SCLH   = 300;  //I2Cbitfreq=(120M/2)/(i2cll+i2sclh))=100khz;  CK =(CCLK/VPB)/(i2cll+i2sclh)
				}
				else 
				{
					I21SCLL   = uc_Temp;  //I2Cbitfreq 0Hz---400KHz;
					I21SCLH   = uc_Temp;  //
				}
				
				
				if( IRQFunc::install_irq( I2C2_INT, (void *)I2C2HandlerISR, HIGHEST_PRIORITY) == FALSE)
				{
					return( FALSE );
				}

				I22CONSET = I2CONSET_I2EN;//使能

				//全局变量初始化
				{
					I2CMasterState[chn] = I2C_IDLE;
					I2CIsBusy[chn] = 0;//不忙
					I2CIsBuffer[chn] = 0;
				}

			}
			break;
		default:
			return FALSE;
	}

	return TRUE;
}


	
////IRQ////
//1027    read  readblock    is done    comment out chen
//1028 all is done comment out chen

//I2C0
void I2CFunc::I2C0HandlerISR(void)__irq
{
	unsigned char StatValue;

	StatValue = I20STAT;

	switch ( StatValue )//状态代码
	{
		//尽量的简化
		//write 0x08  0x18  0x20  0x28  0x30
		//read 0x08  0x40  0x48  0x50  0x58  0x10
	case 0x08://STA is issued
			I20DAT = I2CG_Address[0];//从地址+R/W
			I20CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC);//清掉中断和开始条件
		break;

	case 0x10://special function    重发STA然后到达了0x10
		if(FirstWriteFlag[0] == 1)
		{
			I20DAT = (I2CG_Address[0] | 0x01);//置成read
			I20CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC);//清掉中断和开始条件
		}
		//转入read模式
		I20CONCLR = I2CONCLR_SIC;
		break;

	case 0x18://ACK
		if(BlockFlag[0] == 1)
		{
			I20DAT = I2CWriteBuffer[0][pWriteBuf[0]];
			pWriteBuf[0]++;
			I20CONSET = I2CONSET_AA;
		}
		else
		{
			I20DAT = DataBuf[0];
			I20CONSET = I2CONSET_AA;
		}
		I20CONCLR = I2CONCLR_SIC;
		break;
	case 0x20://NACK
		I2CMasterState[0] = I2C_SLAVE_NACK;
		I20CONSET = I2CONSET_STO|I2CONSET_AA;//发送STO和AA
		I2CIsBusy[0] = 0;
		I20CONCLR = I2CONCLR_SIC;
		break;
	case 0x28://数据已经传输并且ACK
		if (BlockFlag[0] == 1)
		{   
			if(pWriteBuf[0] >= I2CWrite_Length[0])
			{
				if(FirstWriteFlag[0] == 1)
				{
					//发送ACK 然后发送STA
					I20CONSET = I2CONSET_STA;
					I2CMasterState[0] = I2C_REPEATED_START;
				}
				else
				{
					I2CMasterState[0] = I2C_TRAN_DONE;//传输成功
					I20CONSET = I2CONSET_STO|I2CONSET_AA;
					I2CIsBusy[0] = 0;
				}
			}
			else
			{
				I20DAT = I2CWriteBuffer[0][pWriteBuf[0]];
				pWriteBuf[0]++;
				I20CONSET = I2CONSET_AA;
			}
		}
		else
		{	
			I2CMasterState[0] = I2C_TRAN_DONE;//传输成功
			I20CONSET = I2CONSET_STO|I2CONSET_AA;
			I2CIsBusy[0] = 0;
		}
		I20CONCLR = I2CONCLR_SIC;
		break;
	case 0x30:
		I2CMasterState[0] = I2C_SLAVE_NACK;
		I20CONSET = I2CONSET_STO|I2CONSET_AA;
		I2CIsBusy[0] = 0;
		I20CONCLR = I2CONCLR_SIC;
		break;
		//又返回从机使用
		//case 0x38:
		//    break;

		////////read////////
	case 0x40:/* Master Receive, SLA_R has been sent */
		I20CONSET = I2CONSET_AA;
		I20CONCLR = I2CONCLR_SIC;
		break;
		//	case 0x50://接收数据，
		//		if(BlockFlag == 1)
		//		{
		//			I2CReadBuffer[pReadBuf] = I21DAT;
		//			pReadBuf++;
		//			if(pReadBuf > g_Length)
		//			{
		//				I2CMasterState = I2C_DATA_ACK;
		//				I21CONCLR = I2CONCLR_AAC;//最后一个数据  返回的是NACK  clear ACK    
		//			}
		//			else
		//			{
		//				I2CMasterState = I2C_DATA_ACK;
		//				I21CONSET = I2CONSET_AA;	/* assert ACK after data is received */
		//			}
		//		}
		//		else
		//		{
		//			ReadData = I21DAT;
		//			I2CMasterState = I2C_TRAN_DONE;//传输成功
		//
		//		}
		//		I21CONCLR = I2CONCLR_SIC;
		//		break;
		//	case 0x58:		  //已经返回非应答
		//		I2CMasterState = I2C_TRAN_DONE;//传输成功
		//		I2CReadBuffer[pReadBuf] = I21DAT; //最后一个数据
		//		I21CONSET = I2CONSET_STO;
		//		I21CONCLR = I2CONCLR_SIC;
		//		break;

	//看来不能合并在一起
	case 0x50:
		if(BlockFlag[0] == 1)
		{
			I2CReadBuffer[0][pReadBuf[0]]= I20DAT;
			pReadBuf[0]++;
			if(pReadBuf[0] >= I2CRead_Length[0])
			{
				I2CMasterState[0] = I2C_TRAN_DONE;//传输成功
				I20CONCLR = I2CONCLR_AAC;
			}
			else
			{
				I2CMasterState[0] = I2C_DATA_ACK;
				I20CONSET = I2CONSET_AA;	/* assert ACK after data is received */
			}
		}
		else
		{
			ReadData[0] = I20DAT;
			I2CMasterState[0] = I2C_TRAN_DONE;//传输成功
			I20CONCLR = I2CONCLR_AAC;
			I2CIsBusy[0] = 0;
		}
		I20CONCLR = I2CONCLR_SIC;
		break;
	case 0x58:
		I2CIsBusy[0] = 0;
		I20CONSET = I2CONSET_STO;//置STO
		I20CONCLR = I2CONCLR_SIC;
		break;

	case 0x48://NACK
		I2CMasterState[0] = I2C_SLAVE_NACK;
		I20CONSET = I2CONSET_STO;
		I2CIsBusy[0] = 0;
		I20CONCLR = I2CONCLR_SIC;
		break;

	default:
		I2CMasterState[0] = I2C_ERROR;
		I20CONCLR = I2CONCLR_SIC;
		break;
	}

	VICVectAddr = 0;		// Acknowledge Interrupt //
}

//I2C1
void I2CFunc::I2C1HandlerISR(void) __irq
{
	unsigned char StatValue;

	StatValue = I21STAT;

	switch ( StatValue )//状态代码
	{
	//尽量的简化
	//write 0x08  0x18  0x20  0x28  0x30
	//read 0x08  0x40  0x48  0x50  0x58  0x10
	case 0x08://STA is issued
		I21DAT = I2CG_Address[1];//从地址+R/W
		I21CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC);//清掉中断和开始条件
		break;

	case 0x10://special function    重发STA然后到达了0x10
		if(FirstWriteFlag[1] == 1)
		{
			I21DAT = (I2CG_Address[1] | 0x01);//置成read
			I21CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC);//清掉中断和开始条件
		}
		//转入read模式
		I21CONCLR = I2CONCLR_SIC;
		break;

	case 0x18://ACK
		if(BlockFlag[1] == 1)
		{
			I21DAT = I2CWriteBuffer[1][pWriteBuf[1]];
			pWriteBuf[1]++;
			I21CONSET = I2CONSET_AA;
		}
		else
		{
			I21DAT = DataBuf[1];
			I21CONSET = I2CONSET_AA;
		}
		I21CONCLR = I2CONCLR_SIC;
		break;
	case 0x20://NACK
		I2CMasterState[1] = I2C_SLAVE_NACK;
		I21CONSET = I2CONSET_STO;//|I2CONSET_AA;//发送STO和AA
		I2CIsBusy[1] = 0;
		I21CONCLR = I2CONCLR_SIC;
		break;
	case 0x28://数据已经传输并且ACK
		if (BlockFlag[1] == 1)
		{   
			if(pWriteBuf[1] >= I2CWrite_Length[1])
			{
				if(FirstWriteFlag[1] == 1)
				{
					//发送ACK 然后发送STA
					I21CONSET = I2CONSET_STA;
					I2CMasterState[1] = I2C_REPEATED_START;
				}
				else
				{
					I2CMasterState[1] = I2C_TRAN_DONE;//传输成功
					I21CONSET = I2CONSET_STO|I2CONSET_AA;
					I2CIsBusy[1] = 0;
				}
			}
			else
			{
				I21DAT = I2CWriteBuffer[1][pWriteBuf[1]];
				pWriteBuf[1]++;
			 	I21CONSET = I2CONSET_AA;
			}
		}
		else
		{	
			I2CMasterState[1] = I2C_TRAN_DONE;//传输成功
			I21CONSET = I2CONSET_STO|I2CONSET_AA;
			I2CIsBusy[1] = 0;
		}
		I21CONCLR = I2CONCLR_SIC;
		break;
	case 0x30:
		I2CMasterState[1] = I2C_SLAVE_NACK;
        I21CONSET = I2CONSET_STO|I2CONSET_AA;
		I2CIsBusy[1] = 0;
		I21CONCLR = I2CONCLR_SIC;
		break;
	//又返回从机使用
	//case 0x38:
	//    break;

	////////read////////
	case 0x40:/* Master Receive, SLA_R has been sent */
		I21CONSET = I2CONSET_AA;
		I21CONCLR = I2CONCLR_SIC;
		break;

//	//看来不能合并在一起
//	case 0x50:
//		if(BlockFlag[1] == 1)
//		{
//			I2CReadBuffer[1][pReadBuf[1]]= I21DAT;
//			pReadBuf[1]++;
//			if(pReadBuf[1] >= I2CRead_Length[1])
//			{
//				I2CMasterState[1] = I2C_TRAN_DONE;//传输成功
//				I21CONCLR = I2CONCLR_AAC;
//			}
//			else
//			{
//				I2CMasterState[1] = I2C_DATA_ACK;
//				I21CONSET = I2CONSET_AA;	/* assert ACK after data is received */
//			}
//		}
//		else
//		{
//			ReadData[1] = I21DAT;
//			I2CMasterState[1] = I2C_TRAN_DONE;//传输成功
//			I21CONCLR = I2CONCLR_AAC;
//			I2CIsBusy[1] = 0;
//		}
//		I21CONCLR = I2CONCLR_SIC;
//		break;
//	case 0x58:
//		I2CIsBusy[1] = 0;
//		I21CONSET = I2CONSET_STO;//置STO
//		I21CONCLR = I2CONCLR_SIC;
//		break;

	//看来不能合并在一起
	case 0x50:
		if(BlockFlag[1] == 1)
		{
			I2CReadBuffer[1][pReadBuf[1]]= I21DAT;
			pReadBuf[1]++;
			if(pReadBuf[1] >= I2CRead_Length[1])
			{
				I2CMasterState[1] = I2C_TRAN_DONE;//传输成功
				I21CONCLR = I2CONCLR_AAC;
			}
			else
			{
				I2CMasterState[1] = I2C_DATA_ACK;
				I21CONSET = I2CONSET_AA;	/* assert ACK after data is received */
			}
		}
		else
		{
			ReadData[1] = I21DAT;
			I2CMasterState[1] = I2C_TRAN_DONE;//传输成功
			I21CONCLR = I2CONCLR_AAC;
			I2CIsBusy[1] = 0;
		}
		I21CONCLR = I2CONCLR_SIC;
		break;
	case 0x58:
		I2CIsBusy[1] = 0;
		I21CONSET = I2CONSET_STO;//置STO
		I21CONCLR = I2CONCLR_SIC;
		break;

	case 0x48://NACK
		I2CMasterState[1] = I2C_SLAVE_NACK;
		I21CONSET = I2CONSET_STO;
		I2CIsBusy[1] = 0;
		I21CONCLR = I2CONCLR_SIC;
		break;

	default:
		I2CMasterState[1] = I2C_ERROR;
		I21CONCLR = I2CONCLR_SIC;
		break;
	}

	VICVectAddr = 0;		// Acknowledge Interrupt //
}

//I2C2
void I2CFunc::I2C2HandlerISR(void)__irq
{
	unsigned char StatValue;

	StatValue = I22STAT;

	switch ( StatValue )//状态代码
	{
		//尽量的简化
		//write 0x08  0x18  0x20  0x28  0x30
		//read 0x08  0x40  0x48  0x50  0x58  0x10
	case 0x08://STA is issued
		I22DAT = I2CG_Address[2];//从地址+R/W
		I22CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC);//清掉中断和开始条件
		break;

	case 0x10://special function    重发STA然后到达了0x10
		if(FirstWriteFlag[2] == 1)
		{
			I22DAT = (I2CG_Address[2] | 0x01);//置成read
			I22CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC);//清掉中断和开始条件
		}
		//转入read模式
		I22CONCLR = I2CONCLR_SIC;
		break;

	case 0x18://ACK
		if(BlockFlag[2] == 1)
		{
			I22DAT = I2CWriteBuffer[2][pWriteBuf[2]];
			pWriteBuf[2]++;
			I22CONSET = I2CONSET_AA;
		}
		else
		{
			I22DAT = DataBuf[2];
			I22CONSET = I2CONSET_AA;
		}
		I22CONCLR = I2CONCLR_SIC;
		break;
	case 0x20://NACK
		I2CMasterState[2] = I2C_SLAVE_NACK;
		I22CONSET = I2CONSET_STO|I2CONSET_AA;//发送STO和AA
		I2CIsBusy[2] = 0;
		I22CONCLR = I2CONCLR_SIC;
		break;
	case 0x28://数据已经传输并且ACK
		if (BlockFlag[2] == 1)
		{   
			if(pWriteBuf[2] >= I2CWrite_Length[2])
			{
				if(FirstWriteFlag[2] == 1)
				{
					//发送ACK 然后发送STA
					I22CONSET = I2CONSET_STA;
					I2CMasterState[2] = I2C_REPEATED_START;
				}
				else
				{
					I2CMasterState[2] = I2C_TRAN_DONE;//传输成功
					I22CONSET = I2CONSET_STO|I2CONSET_AA;
					I2CIsBusy[2] = 0;
				}
			}
			else
			{
				I22DAT = I2CWriteBuffer[2][pWriteBuf[2]];
				pWriteBuf[2]++;
				I22CONSET = I2CONSET_AA;
			}
		}
		else
		{	
			I2CMasterState[2] = I2C_TRAN_DONE;//传输成功
			I22CONSET = I2CONSET_STO|I2CONSET_AA;
			I2CIsBusy[2] = 0;
		}
		I22CONCLR = I2CONCLR_SIC;
		break;
	case 0x30:
		I2CMasterState[2] = I2C_SLAVE_NACK;
		I22CONSET = I2CONSET_STO|I2CONSET_AA;
		I2CIsBusy[2] = 0;
		I22CONCLR = I2CONCLR_SIC;
		break;
		//又返回从机使用
		//case 0x38:
		//    break;

		////////read////////
	case 0x40:/* Master Receive, SLA_R has been sent */
		I22CONSET = I2CONSET_AA;
		I22CONCLR = I2CONCLR_SIC;
		break;
		//	case 0x50://接收数据，
		//		if(BlockFlag == 1)
		//		{
		//			I2CReadBuffer[pReadBuf] = I21DAT;
		//			pReadBuf++;
		//			if(pReadBuf > g_Length)
		//			{
		//				I2CMasterState = I2C_DATA_ACK;
		//				I21CONCLR = I2CONCLR_AAC;//最后一个数据  返回的是NACK  clear ACK    
		//			}
		//			else
		//			{
		//				I2CMasterState = I2C_DATA_ACK;
		//				I21CONSET = I2CONSET_AA;	/* assert ACK after data is received */
		//			}
		//		}
		//		else
		//		{
		//			ReadData = I21DAT;
		//			I2CMasterState = I2C_TRAN_DONE;//传输成功
		//
		//		}
		//		I21CONCLR = I2CONCLR_SIC;
		//		break;
		//	case 0x58:		  //已经返回非应答
		//		I2CMasterState = I2C_TRAN_DONE;//传输成功
		//		I2CReadBuffer[pReadBuf] = I21DAT; //最后一个数据
		//		I21CONSET = I2CONSET_STO;
		//		I21CONCLR = I2CONCLR_SIC;
		//		break;

	//看来不能合并在一起
	case 0x50:
		if(BlockFlag[2] == 1)
		{
			I2CReadBuffer[2][pReadBuf[2]]= I22DAT;
			pReadBuf[2]++;
			if(pReadBuf[2] >= I2CRead_Length[2])
			{
				I2CMasterState[2] = I2C_TRAN_DONE;//传输成功
				I22CONCLR = I2CONCLR_AAC;
			}
			else
			{
				I2CMasterState[2] = I2C_DATA_ACK;
				I22CONSET = I2CONSET_AA;	/* assert ACK after data is received */
			}
		}
		else
		{
			ReadData[2] = I21DAT;
			I2CMasterState[2] = I2C_TRAN_DONE;//传输成功
			I22CONCLR = I2CONCLR_AAC;
			I2CIsBusy[2] = 0;
		}
		I22CONCLR = I2CONCLR_SIC;
		break;
	case 0x58:
		I2CIsBusy[2] = 0;
		I22CONSET = I2CONSET_STO;//置STO
		I22CONCLR = I2CONCLR_SIC;
		break;

	case 0x48://NACK
		I2CMasterState[2] = I2C_SLAVE_NACK;
		I22CONSET = I2CONSET_STO;
		I2CIsBusy[2] = 0;
		I22CONCLR = I2CONCLR_SIC;
		break;

	default:
		I2CMasterState[2] = I2C_ERROR;
		I22CONCLR = I2CONCLR_SIC;
		break;
	}

	VICVectAddr = 0;		// Acknowledge Interrupt //
}
////end IRQ////



////I2C function////
//所有的操作都在中断完成，相应函数只是做准备工作
//结束后checkStatus查询I2C工作状态

//
UWord32 I2CFunc::I2CReadWord(unsigned char addr,unsigned char chn)
{
	if(chn > I2C_MAX_CHANNEL)
	{
		return FALSE;
	}

	I2CG_Address[chn] = (addr | 0x01);
	BlockFlag[chn] = 0;
	I2CRead_Length[chn] = 0;

	I2CMasterState[chn] = I2C_IDLE;

	I2CIsBusy[chn] = 1;

	switch(chn)
	{
	case 0:
		I20CONSET = I2CONSET_STA;	/* Set Start flag */
		break;
	case 1:
		I21CONSET = I2CONSET_STA;	/* Set Start flag */
		break;
	case 2:
		I22CONSET = I2CONSET_STA;	/* Set Start flag */
		break;
	default:
		return FALSE;
	}

	return TRUE;
}

//读出I2CReadBuffer
//read 末尾是1
UWord32 I2CFunc::I2CReadBlock(unsigned char addr,unsigned char len,unsigned char chn)
{
	if(len > MAX_I2C_BUFFER)
	{
		return FALSE;
	}

	if(chn > I2C_MAX_CHANNEL)
	{
		return FALSE;
	}

	I2CG_Address[chn] = (addr | 0x01);
	BlockFlag[chn] = 1;
	I2CRead_Length[chn] = len;
	pReadBuf[chn] = 0;

	I2CMasterState[chn] = I2C_IDLE;

	I2CIsBusy[chn] = 1;
	I2CIsBuffer[chn] = 1;

	switch(chn)
	{
	case 0:
		I20CONSET = I2CONSET_STA;	/* Set Start flag */
		break;
	case 1:
		I21CONSET = I2CONSET_STA;	/* Set Start flag */
		break;
	case 2:
		I22CONSET = I2CONSET_STA;	/* Set Start flag */
		break;
	default:
		return FALSE;
	}

	return TRUE;
}

//
UWord32 I2CFunc::I2CWriteWord(unsigned char addr,unsigned char value,unsigned char chn)
{
	if(chn > I2C_MAX_CHANNEL)
	{
		return FALSE;
	}

	I2CG_Address[chn] = (addr & 0xFE);
	BlockFlag[chn] = 0;
	I2CWrite_Length[chn] = 0;
	pWriteBuf[chn] = 0;

	DataBuf[chn] = value;

	I2CMasterState[chn] = I2C_IDLE;

	I2CIsBusy[chn] = 1;
	
	switch(chn)
	{
	case 0:
		I20CONSET = I2CONSET_STA;	/* Set Start flag */
		break;
	case  1:
		I21CONSET = I2CONSET_STA;	/* Set Start flag */
		break;
	case 2:
		I22CONSET = I2CONSET_STA;	/* Set Start flag */
		break;
	default:
		return FALSE;
	}

	return TRUE;
}

//先写到I2CWriteBuffer缓冲区里
//write 末尾是0
UWord32 I2CFunc::I2CWriteBlock(unsigned char addr,unsigned char array[],unsigned char len,unsigned char chn)
{
	if(chn > I2C_MAX_CHANNEL)
	{
		return FALSE;
	}

	if(len > MAX_I2C_BUFFER)
	{
		return FALSE;
	}

	unsigned char * pValue = 0;
	pValue = &array[0];

	for(unsigned char i = 0; i<len;i++)
	{
		I2CWriteBuffer[chn][i]  = * (pValue);//stuff buffer
		pValue++;
	}

	I2CG_Address[chn] = (addr & 0xFE);
	BlockFlag[chn] = 1;
	I2CWrite_Length[chn] = len;
	pWriteBuf[chn] = 0;

	I2CMasterState[chn] = I2C_IDLE;

	I2CIsBusy[chn] = 1;
	I2CIsBuffer[chn] = 1;

	switch(chn)
	{
	case 0:
		I20CONSET = I2CONSET_STA;	/* Set Start flag */
		break;
	case 1:
		I21CONSET = I2CONSET_STA;	/* Set Start flag */
		break;
	case 2:
		I22CONSET = I2CONSET_STA;	/* Set Start flag */
		break;
	default:
		return FALSE;
	}

	return TRUE;
}

//用来判断当前的I2C接口状态
UWord32 I2CFunc::checkStatus(unsigned char chn)
{
	return I2CMasterState[chn];
}

//用来判断I2C是否正在被使用    1使用中    0闲置
UWord32 I2CFunc::isBusy(unsigned char chn)
{
	return I2CIsBusy[chn];
}

// 1:使用中  0:idle
UWord32 I2CFunc::isUseBuf(unsigned char chn)
{
	return I2CIsBuffer[chn];
}

//clear掉
void I2CFunc::clearBuf(unsigned char chn)
{
	I2CIsBuffer[chn] = 0;
}

void I2CFunc::clearI2C(unsigned char chn)
{
	I2CG_Address[chn] = 0;

	I2CWrite_Length[chn] = 0;
	I2CRead_Length[chn] = 0;

	FirstWriteFlag[chn] = 0;
	BlockFlag[chn] = 0;

	I2CIsBuffer[chn] = 0;

	for(unsigned char i=0;i<MAX_I2C_BUFFER;i++)
	{
		I2CReadBuffer[chn][i] = 0;    
	}

}

UWord32 I2CFunc::FirstWriteThenRead(unsigned char addr,unsigned char WriteBuf[],unsigned char writeLen,
														  unsigned char readLen,unsigned char chn)//addr 末尾 write 0 read 1
{
	if( (writeLen > MAX_I2C_BUFFER)||(readLen > MAX_I2C_BUFFER)||(chn > I2C_MAX_CHANNEL) )//出错
	{
		return FALSE;
	}

	I2CG_Address[chn] = (addr & 0xFE);

	I2CWrite_Length[chn] = writeLen;
	I2CRead_Length[chn] = readLen;

	FirstWriteFlag[chn] = 1;

	unsigned char * pValue = 0;
	pValue = &WriteBuf[0];

	for(unsigned char i = 0; i<writeLen;i++)
	{
		I2CWriteBuffer[chn][i]  = * (pValue);//stuff buffer
		pValue++;
	}

	BlockFlag[chn] = 1;
	pWriteBuf[chn] = 0;
	pReadBuf[chn] = 0;

	I2CMasterState[chn] = I2C_IDLE;

	I2CIsBusy[chn] = 1;
	I2CIsBuffer[chn] = 1;

	switch(chn)
	{
	case 0:
		I20CONSET = I2CONSET_STA;	/* Set Start flag */
		break;
	case 1:
		I21CONSET = I2CONSET_STA;	/* Set Start flag */
		break;
	case 2:
		I22CONSET = I2CONSET_STA;	/* Set Start flag */
		break;
	default:
		return FALSE;
	}

	return TRUE;
}


////end I2C function////


//readme
/*
24LC128 EEPROM芯片有
特殊的情况，先write后不SPO然后重新你START读取。叫radnom read
*/



////////////////////////////////24LC128EEPROM////////////////////////////////

//readme
/*
24LC128地址是连续的
内部地址
0x0000  0x0001  0x0002  0x0003

所谓写单字节就是在该地址上写入东西
连续写入就是在该地址和（后续地址）写入连续串

读取不太清楚
*/

////////////////////////////////end 24LC128EEPROM////////////////////////////////
/*********************************************************************************
**                            End Of File
*********************************************************************************/
