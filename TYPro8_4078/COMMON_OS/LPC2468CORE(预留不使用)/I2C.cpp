
#include "../../COMMON/includes.h"

/*
Slave Address
STA ( 1 0 1 0 A2 A1 A0 R/W )   ACK/NACK
A2 A1 A0 ���뿪��
1026
Ŀǰֻ�����SDA1 SCL1
*/

//private
unsigned char I2CFunc::I2CG_Address[I2C_MAX_CHANNEL];

unsigned char I2CFunc::I2CWrite_Length[I2C_MAX_CHANNEL];
unsigned char I2CFunc::I2CRead_Length[I2C_MAX_CHANNEL];

unsigned char I2CFunc::BlockFlag[I2C_MAX_CHANNEL];//�Ƿ��ȡ��

unsigned char I2CFunc::DataBuf[I2C_MAX_CHANNEL];

volatile unsigned char I2CFunc::I2CMasterState[I2C_MAX_CHANNEL];//ȫ��״̬
volatile unsigned char I2CFunc::I2CIsBusy[I2C_MAX_CHANNEL];//�Ƿ�busy
volatile  unsigned char I2CFunc::I2CIsBuffer[I2C_MAX_CHANNEL];//buffer�Ƿ�ռ��

//����Ĺ���
volatile unsigned char I2CFunc::FirstWriteFlag[I2C_MAX_CHANNEL];

//bufferָ��
unsigned char I2CFunc::pReadBuf[I2C_MAX_CHANNEL];
unsigned char I2CFunc::pWriteBuf[I2C_MAX_CHANNEL];

//public
////user stuff////
//��gpio�����

unsigned char I2CFunc::MyIOCount = 4;
unsigned char I2CFunc::MyIO[][4] = //11����SDA1 SCL1  19 20        SDA0 SCL0    01����        **ò��ֻ����SDA1 SCL1    24LC128
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



//ֱ����Ϊ����ʹ��
UWord32 I2CFunc::my_Init(unsigned char chn)
{
	if(chn > I2C_MAX_CHANNEL)
	{
		return FALSE;
	}

	switch(chn)
	{
	    case 0:
			//I2C0�ӿ�
			{
				//����enable
				PCONP |= (1 << 7);//I2C0

				//GPIO
                //I2C1    P0.27 uses GPIO I2C0_SDA, P0.28 I2C0_SCL //

				/*--- Clear flags ---*/
				I20CONCLR = I2CONCLR_AAC | I2CONCLR_SIC | I2CONCLR_STAC | I2CONCLR_I2ENC; 

				/*--- Reset registers ---*/
				//����
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

				I20CONSET = I2CONSET_I2EN;//ʹ��

				//ȫ�ֱ�����ʼ��
				{
					I2CMasterState[chn] = I2C_IDLE;
					I2CIsBusy[chn] = 0;//��æ
					I2CIsBuffer[chn] = 0;
				}

			}
		break;
		case 1:
			//I2C1�ӿ�
			{
				//����enable
				PCONP |= (1 << 19);//I2C1

				//GPIO
                //I2C1    P0.19 uses GPIO I2C1_SDA, P0.20 I2C1_SCL //

				/*--- Clear flags ---*/
				I21CONCLR = I2CONCLR_AAC | I2CONCLR_SIC | I2CONCLR_STAC | I2CONCLR_I2ENC; 

				/*--- Reset registers ---*/
				//����
				//I21SCLL   = I2SCLL_SCLL;
				//I21SCLH   = I2SCLH_SCLH;
                
				//--- Reset registers ---//
				//����
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

				I21CONSET = I2CONSET_I2EN;//ʹ��

				//ȫ�ֱ�����ʼ��
				{
					I2CMasterState[chn] = I2C_IDLE;
					I2CIsBusy[chn] = 0;//��æ
					I2CIsBuffer[chn] = 0;
				}

			}
			break;
		case 2:
			//I2C2�ӿ�
			{
				//����enable
				PCONP |= (1 << 26);//I2C2

				//GPIO
                //I2C2    P0. uses GPIO I2C2_SDA, P0. I2C2_SCL //
				
				
				/*--- Clear flags ---*/
				I22CONCLR = I2CONCLR_AAC | I2CONCLR_SIC | I2CONCLR_STAC | I2CONCLR_I2ENC; 

				//I22SCLL   = I2SCLL_SCLL;
				//I22SCLH   = I2SCLH_SCLH;
				
                //--- Reset registers ---//
				//����
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

				I22CONSET = I2CONSET_I2EN;//ʹ��

				//ȫ�ֱ�����ʼ��
				{
					I2CMasterState[chn] = I2C_IDLE;
					I2CIsBusy[chn] = 0;//��æ
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

	switch ( StatValue )//״̬����
	{
		//�����ļ�
		//write 0x08  0x18  0x20  0x28  0x30
		//read 0x08  0x40  0x48  0x50  0x58  0x10
	case 0x08://STA is issued
			I20DAT = I2CG_Address[0];//�ӵ�ַ+R/W
			I20CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC);//����жϺͿ�ʼ����
		break;

	case 0x10://special function    �ط�STAȻ�󵽴���0x10
		if(FirstWriteFlag[0] == 1)
		{
			I20DAT = (I2CG_Address[0] | 0x01);//�ó�read
			I20CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC);//����жϺͿ�ʼ����
		}
		//ת��readģʽ
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
		I20CONSET = I2CONSET_STO|I2CONSET_AA;//����STO��AA
		I2CIsBusy[0] = 0;
		I20CONCLR = I2CONCLR_SIC;
		break;
	case 0x28://�����Ѿ����䲢��ACK
		if (BlockFlag[0] == 1)
		{   
			if(pWriteBuf[0] >= I2CWrite_Length[0])
			{
				if(FirstWriteFlag[0] == 1)
				{
					//����ACK Ȼ����STA
					I20CONSET = I2CONSET_STA;
					I2CMasterState[0] = I2C_REPEATED_START;
				}
				else
				{
					I2CMasterState[0] = I2C_TRAN_DONE;//����ɹ�
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
			I2CMasterState[0] = I2C_TRAN_DONE;//����ɹ�
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
		//�ַ��شӻ�ʹ��
		//case 0x38:
		//    break;

		////////read////////
	case 0x40:/* Master Receive, SLA_R has been sent */
		I20CONSET = I2CONSET_AA;
		I20CONCLR = I2CONCLR_SIC;
		break;
		//	case 0x50://�������ݣ�
		//		if(BlockFlag == 1)
		//		{
		//			I2CReadBuffer[pReadBuf] = I21DAT;
		//			pReadBuf++;
		//			if(pReadBuf > g_Length)
		//			{
		//				I2CMasterState = I2C_DATA_ACK;
		//				I21CONCLR = I2CONCLR_AAC;//���һ������  ���ص���NACK  clear ACK    
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
		//			I2CMasterState = I2C_TRAN_DONE;//����ɹ�
		//
		//		}
		//		I21CONCLR = I2CONCLR_SIC;
		//		break;
		//	case 0x58:		  //�Ѿ����ط�Ӧ��
		//		I2CMasterState = I2C_TRAN_DONE;//����ɹ�
		//		I2CReadBuffer[pReadBuf] = I21DAT; //���һ������
		//		I21CONSET = I2CONSET_STO;
		//		I21CONCLR = I2CONCLR_SIC;
		//		break;

	//�������ܺϲ���һ��
	case 0x50:
		if(BlockFlag[0] == 1)
		{
			I2CReadBuffer[0][pReadBuf[0]]= I20DAT;
			pReadBuf[0]++;
			if(pReadBuf[0] >= I2CRead_Length[0])
			{
				I2CMasterState[0] = I2C_TRAN_DONE;//����ɹ�
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
			I2CMasterState[0] = I2C_TRAN_DONE;//����ɹ�
			I20CONCLR = I2CONCLR_AAC;
			I2CIsBusy[0] = 0;
		}
		I20CONCLR = I2CONCLR_SIC;
		break;
	case 0x58:
		I2CIsBusy[0] = 0;
		I20CONSET = I2CONSET_STO;//��STO
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

	switch ( StatValue )//״̬����
	{
	//�����ļ�
	//write 0x08  0x18  0x20  0x28  0x30
	//read 0x08  0x40  0x48  0x50  0x58  0x10
	case 0x08://STA is issued
		I21DAT = I2CG_Address[1];//�ӵ�ַ+R/W
		I21CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC);//����жϺͿ�ʼ����
		break;

	case 0x10://special function    �ط�STAȻ�󵽴���0x10
		if(FirstWriteFlag[1] == 1)
		{
			I21DAT = (I2CG_Address[1] | 0x01);//�ó�read
			I21CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC);//����жϺͿ�ʼ����
		}
		//ת��readģʽ
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
		I21CONSET = I2CONSET_STO;//|I2CONSET_AA;//����STO��AA
		I2CIsBusy[1] = 0;
		I21CONCLR = I2CONCLR_SIC;
		break;
	case 0x28://�����Ѿ����䲢��ACK
		if (BlockFlag[1] == 1)
		{   
			if(pWriteBuf[1] >= I2CWrite_Length[1])
			{
				if(FirstWriteFlag[1] == 1)
				{
					//����ACK Ȼ����STA
					I21CONSET = I2CONSET_STA;
					I2CMasterState[1] = I2C_REPEATED_START;
				}
				else
				{
					I2CMasterState[1] = I2C_TRAN_DONE;//����ɹ�
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
			I2CMasterState[1] = I2C_TRAN_DONE;//����ɹ�
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
	//�ַ��شӻ�ʹ��
	//case 0x38:
	//    break;

	////////read////////
	case 0x40:/* Master Receive, SLA_R has been sent */
		I21CONSET = I2CONSET_AA;
		I21CONCLR = I2CONCLR_SIC;
		break;

//	//�������ܺϲ���һ��
//	case 0x50:
//		if(BlockFlag[1] == 1)
//		{
//			I2CReadBuffer[1][pReadBuf[1]]= I21DAT;
//			pReadBuf[1]++;
//			if(pReadBuf[1] >= I2CRead_Length[1])
//			{
//				I2CMasterState[1] = I2C_TRAN_DONE;//����ɹ�
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
//			I2CMasterState[1] = I2C_TRAN_DONE;//����ɹ�
//			I21CONCLR = I2CONCLR_AAC;
//			I2CIsBusy[1] = 0;
//		}
//		I21CONCLR = I2CONCLR_SIC;
//		break;
//	case 0x58:
//		I2CIsBusy[1] = 0;
//		I21CONSET = I2CONSET_STO;//��STO
//		I21CONCLR = I2CONCLR_SIC;
//		break;

	//�������ܺϲ���һ��
	case 0x50:
		if(BlockFlag[1] == 1)
		{
			I2CReadBuffer[1][pReadBuf[1]]= I21DAT;
			pReadBuf[1]++;
			if(pReadBuf[1] >= I2CRead_Length[1])
			{
				I2CMasterState[1] = I2C_TRAN_DONE;//����ɹ�
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
			I2CMasterState[1] = I2C_TRAN_DONE;//����ɹ�
			I21CONCLR = I2CONCLR_AAC;
			I2CIsBusy[1] = 0;
		}
		I21CONCLR = I2CONCLR_SIC;
		break;
	case 0x58:
		I2CIsBusy[1] = 0;
		I21CONSET = I2CONSET_STO;//��STO
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

	switch ( StatValue )//״̬����
	{
		//�����ļ�
		//write 0x08  0x18  0x20  0x28  0x30
		//read 0x08  0x40  0x48  0x50  0x58  0x10
	case 0x08://STA is issued
		I22DAT = I2CG_Address[2];//�ӵ�ַ+R/W
		I22CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC);//����жϺͿ�ʼ����
		break;

	case 0x10://special function    �ط�STAȻ�󵽴���0x10
		if(FirstWriteFlag[2] == 1)
		{
			I22DAT = (I2CG_Address[2] | 0x01);//�ó�read
			I22CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC);//����жϺͿ�ʼ����
		}
		//ת��readģʽ
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
		I22CONSET = I2CONSET_STO|I2CONSET_AA;//����STO��AA
		I2CIsBusy[2] = 0;
		I22CONCLR = I2CONCLR_SIC;
		break;
	case 0x28://�����Ѿ����䲢��ACK
		if (BlockFlag[2] == 1)
		{   
			if(pWriteBuf[2] >= I2CWrite_Length[2])
			{
				if(FirstWriteFlag[2] == 1)
				{
					//����ACK Ȼ����STA
					I22CONSET = I2CONSET_STA;
					I2CMasterState[2] = I2C_REPEATED_START;
				}
				else
				{
					I2CMasterState[2] = I2C_TRAN_DONE;//����ɹ�
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
			I2CMasterState[2] = I2C_TRAN_DONE;//����ɹ�
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
		//�ַ��شӻ�ʹ��
		//case 0x38:
		//    break;

		////////read////////
	case 0x40:/* Master Receive, SLA_R has been sent */
		I22CONSET = I2CONSET_AA;
		I22CONCLR = I2CONCLR_SIC;
		break;
		//	case 0x50://�������ݣ�
		//		if(BlockFlag == 1)
		//		{
		//			I2CReadBuffer[pReadBuf] = I21DAT;
		//			pReadBuf++;
		//			if(pReadBuf > g_Length)
		//			{
		//				I2CMasterState = I2C_DATA_ACK;
		//				I21CONCLR = I2CONCLR_AAC;//���һ������  ���ص���NACK  clear ACK    
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
		//			I2CMasterState = I2C_TRAN_DONE;//����ɹ�
		//
		//		}
		//		I21CONCLR = I2CONCLR_SIC;
		//		break;
		//	case 0x58:		  //�Ѿ����ط�Ӧ��
		//		I2CMasterState = I2C_TRAN_DONE;//����ɹ�
		//		I2CReadBuffer[pReadBuf] = I21DAT; //���һ������
		//		I21CONSET = I2CONSET_STO;
		//		I21CONCLR = I2CONCLR_SIC;
		//		break;

	//�������ܺϲ���һ��
	case 0x50:
		if(BlockFlag[2] == 1)
		{
			I2CReadBuffer[2][pReadBuf[2]]= I22DAT;
			pReadBuf[2]++;
			if(pReadBuf[2] >= I2CRead_Length[2])
			{
				I2CMasterState[2] = I2C_TRAN_DONE;//����ɹ�
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
			I2CMasterState[2] = I2C_TRAN_DONE;//����ɹ�
			I22CONCLR = I2CONCLR_AAC;
			I2CIsBusy[2] = 0;
		}
		I22CONCLR = I2CONCLR_SIC;
		break;
	case 0x58:
		I2CIsBusy[2] = 0;
		I22CONSET = I2CONSET_STO;//��STO
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
//���еĲ��������ж���ɣ���Ӧ����ֻ����׼������
//������checkStatus��ѯI2C����״̬

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

//����I2CReadBuffer
//read ĩβ��1
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

//��д��I2CWriteBuffer��������
//write ĩβ��0
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

//�����жϵ�ǰ��I2C�ӿ�״̬
UWord32 I2CFunc::checkStatus(unsigned char chn)
{
	return I2CMasterState[chn];
}

//�����ж�I2C�Ƿ����ڱ�ʹ��    1ʹ����    0����
UWord32 I2CFunc::isBusy(unsigned char chn)
{
	return I2CIsBusy[chn];
}

// 1:ʹ����  0:idle
UWord32 I2CFunc::isUseBuf(unsigned char chn)
{
	return I2CIsBuffer[chn];
}

//clear��
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
														  unsigned char readLen,unsigned char chn)//addr ĩβ write 0 read 1
{
	if( (writeLen > MAX_I2C_BUFFER)||(readLen > MAX_I2C_BUFFER)||(chn > I2C_MAX_CHANNEL) )//����
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
24LC128 EEPROMоƬ��
������������write��SPOȻ��������START��ȡ����radnom read
*/



////////////////////////////////24LC128EEPROM////////////////////////////////

//readme
/*
24LC128��ַ��������
�ڲ���ַ
0x0000  0x0001  0x0002  0x0003

��νд���ֽھ����ڸõ�ַ��д�붫��
����д������ڸõ�ַ�ͣ�������ַ��д��������

��ȡ��̫���
*/

////////////////////////////////end 24LC128EEPROM////////////////////////////////
/*********************************************************************************
**                            End Of File
*********************************************************************************/
