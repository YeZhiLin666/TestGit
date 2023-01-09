
#include "../../COMMON/includes.h"

//各个模块的MYIO必须与GPIO_MODValue一致

////////user definition	MyIO////////

//class sdram_2468
unsigned char sdram::MyIOCount = 57;

unsigned char sdram::MyIO[57][4] =     //[Port],[pin],[mod],[initvalue]
{
	{2,14,PM_MOD,0},
	{2,15,PM_MOD,0},
	{2,16,PM_MOD,0},
	{2,17,PM_MOD,0},
	{2,18,PM_MOD,0},
	{2,19,PM_MOD,0},
	{2,20,PM_MOD,0},
	{2,21,PM_MOD,0},
	{2,24,PM_MOD,0},
	{2,25,PM_MOD,0},
	{2,28,PM_MOD,0},
	{2,29,PM_MOD,0},
	{3,0,PM_MOD,0},
	{3,1,PM_MOD,0},
	{3,2,PM_MOD,0},
	{3,3,PM_MOD,0},
	{3,4,PM_MOD,0},
	{3,5,PM_MOD,0},
	{3,6,PM_MOD,0},
	{3,7,PM_MOD,0},
	{3,8,PM_MOD,0},
	{3,9,PM_MOD,0},
	{3,10,PM_MOD,0},
	{3,11,PM_MOD,0},
	{3,12,PM_MOD,0},
	{3,13,PM_MOD,0},
	{3,14,PM_MOD,0},
	{3,15,PM_MOD,0},
	{4,0,PM_MOD,0},
	{4,1,PM_MOD,0},
	{4,2,PM_MOD,0},
	{4,3,PM_MOD,0},
	{4,4,PM_MOD,0},
	{4,5,PM_MOD,0},
	{4,6,PM_MOD,0},
	{4,7,PM_MOD,0},
	{4,8,PM_MOD,0},
	{4,9,PM_MOD,0},
	{4,10,PM_MOD,0},
	{4,11,PM_MOD,0},
	{4,12,PM_MOD,0},
	{4,13,PM_MOD,0},
	{4,14,PM_MOD,0},
	{4,15,PM_MOD,0},
	{4,16,PM_MOD,0},
	{4,17,PM_MOD,0},
	{4,18,PM_MOD,0},
	{4,19,PM_MOD,0},
	{4,20,PM_MOD,0},
	{4,21,PM_MOD,0},
	{4,22,PM_MOD,0},
	{4,23,PM_MOD,0},
	{4,24,PM_MOD,0},
	{4,25,PM_MOD,0},
	{4,26,PM_MOD,0},
	{4,27,PM_MOD,0},
	{4,30,PM_MOD,0}
	
};

//class eint_2468
unsigned char eint::MyIOCount =2;

unsigned char eint::MyIO[2][4] =     //[Port],[pin],[mod],
{
	{2,12,PM_MOD,0},
	{2,13,PM_MOD,0}
};

//class ethernet_2468    EMAC
//使用的是MII接口
//P1[15:0]
//P1[17:16]
//将来move to ethernet类中
unsigned char ethernet::MyIOCount =18;

unsigned char ethernet::MyIO[18][4] =     //[Port],[pin],[mod],
{
	{1,0,PM_MOD,0},
	{1,1,PM_MOD,0},
	{1,2,PM_MOD,0},
	{1,3,PM_MOD,0},
	{1,4,PM_MOD,0},
	{1,5,PM_MOD,0},
	{1,6,PM_MOD,0},
	{1,7,PM_MOD,0},
	{1,8,PM_MOD,0},
	{1,9,PM_MOD,0},
	{1,10,PM_MOD,0},
	{1,11,PM_MOD,0},
	{1,12,PM_MOD,0},
	{1,13,PM_MOD,0},
	{1,14,PM_MOD,0},
	{1,15,PM_MOD,0},
	{1,16,PM_MOD,0},
	{1,17,PM_MOD,0}
};

unsigned char cpld::MyIOCount =1;
unsigned char cpld::MyIO[1][4] =  
{
  {4,31,PM_MOD,0}
};//[Port],[pin],[mod],
////////end user////////


unsigned char GPIOFunc::GPIO_MOD[PORT_MAX][PIN_MAX];

//需要手工填充
unsigned char GPIOFunc::GPIO_InitValue[PORT_MAX][PIN_MAX] =     //作为gpio out的高低电平    1是高    0是低
{
		{
			0, 0, 0, 0, 0, 0, 0, 0,    //0.0 - 0.7
			0, 0, 0, 0, 0, 0, 1, 0,    //0.8 - 0.15
			0, 0, 0, 0, 0, 0, 0, 0,    //0.16 - 0.23
			0, 0, 0, 0, 0, 0, 0, 0    //0.24 - 0.31
		},
		{
			0, 0, 0, 0, 0, 0, 0, 0,    //1.0 - 1.7
			0, 0, 0, 0, 0, 0, 0, 0,    //1.8 - 1.15
			0, 0, 0, 0, 0, 0, 0, 0,    //1.16 - 1.23
			0, 0, 0, 0, 0, 0, 0, 0    //1.24 - 1.31
		},
		{
			0, 0, 0, 0, 0, 0, 0, 0,    //2.0 - 2.7
			0, 0, 0, 0, 0, 0, 0, 0,    //2.8 - 2.15
			0, 0, 0, 0, 0, 0, 0, 0,    //2.16 - 2.23
			0, 0, 0, 0, 0, 0, 0, 0    //2.24 - 2.31
		},
		{
			0, 0, 0, 0, 0, 0, 0, 0,    //3.0 - 3.7
			0, 0, 0, 0, 0, 0, 0, 0,    //3.8 - 3.15
			0, 0, 0, 0, 0, 0, 0, 0,    //3.16 - 3.23
			0, 0, 0, 0, 0, 0, 0, 0    //3.24 - 3.31
		},
		{
			0, 0, 0, 0, 0, 0, 0, 0,    //4.0 - 4.7
			0, 0, 0, 0, 0, 0, 0, 0,    //4.8 - 4.15
			0, 0, 0, 0, 0, 0, 0, 0,    //4.16 - 4.23
			0, 0, 0, 0, 0, 0, 0, 0    //4.24 - 4.31
		}
};


//完全按照LPC2468 datasheet 填充
//0就是空而已
const unsigned char GPIOFunc::GPIO_MODValue[PORT_MAX][PIN_MAX][MOD_MAX] =
{
	{//Port 0
		{CAN_ENABLE,UART_ENABLE,I2C_ENABLE},//0
		{CAN_ENABLE,UART_ENABLE,I2C_ENABLE},//1
		{UART_ENABLE,0,0},//2
		{UART_ENABLE,0,0},//3
		{I2S_ENABLE,CAN_ENABLE,TIMER_ENABLE},//4
		{I2S_ENABLE,CAN_ENABLE,TIMER_ENABLE},//5
		{I2S_ENABLE,SSP_ENABLE,TIMER_ENABLE},//6
		{I2S_ENABLE,SSP_ENABLE,TIMER_ENABLE},//7
		{I2S_ENABLE,SSP_ENABLE,TIMER_ENABLE},//8
		{I2S_ENABLE,SSP_ENABLE,TIMER_ENABLE},//9
		{UART_ENABLE,I2C_ENABLE,TIMER_ENABLE},//10
		{UART_ENABLE,I2C_ENABLE,TIMER_ENABLE},//11
		{USB_ENABLE,SSP_ENABLE,AD_ENABLE},//12
		{USB_ENABLE,SSP_ENABLE,AD_ENABLE},//13
		{USB_ENABLE,USB_ENABLE,SSP_ENABLE},//14
		{UART_ENABLE,SSP_ENABLE,SPI_ENABLE},//15
		{UART_ENABLE,SSP_ENABLE,SPI_ENABLE},//16
		{UART_ENABLE,SSP_ENABLE,SPI_ENABLE},//17
		{UART_ENABLE,SSP_ENABLE,SPI_ENABLE},//18
		{UART_ENABLE,SD_ENABLE,I2C_ENABLE},//19
		{UART_ENABLE,SD_ENABLE,I2C_ENABLE},//20
		{UART_ENABLE,SD_ENABLE,CAN_ENABLE},//21
		{UART_ENABLE,SD_ENABLE,CAN_ENABLE},//22
		{AD_ENABLE,I2S_ENABLE,TIMER_ENABLE},//23
		{AD_ENABLE,I2S_ENABLE,TIMER_ENABLE},//24
		{AD_ENABLE,I2S_ENABLE,UART_ENABLE},//25
		{AD_ENABLE,DA_ENABLE,UART_ENABLE},//26
		{I2C_ENABLE,0,0},//27
		{I2C_ENABLE,0,0},//28
		{USB_ENABLE,0,0},//29
		{USB_ENABLE,0,0},//30
		{USB_ENABLE,0,0},//31
	},
	{//Port 1
		{ENET_ENABLE,0,0},//0
		{ENET_ENABLE,0,0},//1
		{ENET_ENABLE,SD_ENABLE,PWM_ENABLE},//2
		{ENET_ENABLE,SD_ENABLE,PWM_ENABLE},//3
		{ENET_ENABLE,0,0},//4
		{ENET_ENABLE,SD_ENABLE,PWM_ENABLE},//5
		{ENET_ENABLE,SD_ENABLE,PWM_ENABLE},//6
		{ENET_ENABLE,SD_ENABLE,PWM_ENABLE},//7
		{ENET_ENABLE,0,0},//8
		{ENET_ENABLE,0,0},//9
		{ENET_ENABLE,0,0},//10
		{ENET_ENABLE,SD_ENABLE,PWM_ENABLE},//11
		{ENET_ENABLE,SD_ENABLE,PWM_ENABLE},//12
		{ENET_ENABLE,0,0},//13
		{ENET_ENABLE,0,0},//14
		{ENET_ENABLE,0,0},//15
		{ENET_ENABLE,0,0},//16
		{ENET_ENABLE,0,0},//17
		{USB_ENABLE,PWM_ENABLE,TIMER_ENABLE},//18
		{USB_ENABLE,USB_ENABLE,TIMER_ENABLE},//19    ////////////USB重复////////////
		{USB_ENABLE,PWM_ENABLE,SSP_ENABLE},//20
		{USB_ENABLE,PWM_ENABLE,SSP_ENABLE},//21
		{USB_ENABLE,USB_ENABLE,TIMER_ENABLE},//22	////////////USB重复////////////
		{USB_ENABLE,PWM_ENABLE,SSP_ENABLE},//23
		{USB_ENABLE,PWM_ENABLE,SSP_ENABLE},//24
		{USB_ENABLE,USB_ENABLE,TIMER_ENABLE},//25	////////////USB重复////////////
		{USB_ENABLE,PWM_ENABLE,TIMER_ENABLE},//26
		{USB_ENABLE,USB_ENABLE,TIMER_ENABLE},//27	////////////USB重复////////////
		{USB_ENABLE,PWM_ENABLE,TIMER_ENABLE},//28
		{USB_ENABLE,PWM_ENABLE,TIMER_ENABLE},//29
		{USB_ENABLE,USB_ENABLE,AD_ENABLE},//30		////////////USB重复////////////
		{USB_ENABLE,SSP_ENABLE,AD_ENABLE},//31
		},
		{// Port 2
			{PWM_ENABLE,UART_ENABLE,TIMER_ENABLE},//0
			{PWM_ENABLE,UART_ENABLE,0},//1      ////////////流水线状态?////////////
			{PWM_ENABLE,UART_ENABLE,0},//2		////////////流水线状态?////////////
			{PWM_ENABLE,UART_ENABLE,0},//3		////////////流水线状态?////////////
			{PWM_ENABLE,UART_ENABLE,0},//4		////////////跟踪同步?////////////
			{PWM_ENABLE,UART_ENABLE,0},//5		////////////跟踪同步?////////////
			{PWM_ENABLE,UART_ENABLE,0},//6		////////////跟踪同步?////////////
			{CAN_ENABLE,UART_ENABLE,0},//7		////////////跟踪同步?////////////
			{CAN_ENABLE,UART_ENABLE,0},//8		////////////跟踪同步?////////////
			{USB_ENABLE,UART_ENABLE,EXTIN_ENABLE},//9
			{EXTIN_ENABLE,0,0},//10
			{EXTIN_ENABLE,SD_ENABLE,I2S_ENABLE},//11
			{EXTIN_ENABLE,SD_ENABLE,I2S_ENABLE},//12
			{EXTIN_ENABLE,SD_ENABLE,I2S_ENABLE},//13
			{SDRAM_ENABLE,TIMER_ENABLE,I2C_ENABLE},//14
			{SDRAM_ENABLE,TIMER_ENABLE,I2C_ENABLE},//15
			{SDRAM_ENABLE,0,0},//16
			{SDRAM_ENABLE,0,0},//17
			{SDRAM_ENABLE,0,0},//18
			{SDRAM_ENABLE,0,0},//19
			{SDRAM_ENABLE,0,0},//20
			{SDRAM_ENABLE,0,0},//21
			{SDRAM_ENABLE,TIMER_ENABLE,SSP_ENABLE},//22
			{SDRAM_ENABLE,TIMER_ENABLE,SSP_ENABLE},//23
			{SDRAM_ENABLE,0,0},//24
			{SDRAM_ENABLE,0,0},//25
			{SDRAM_ENABLE,TIMER_ENABLE,SSP_ENABLE},//26
			{SDRAM_ENABLE,TIMER_ENABLE,SSP_ENABLE},//27
			{SDRAM_ENABLE,0,0},//28
			{SDRAM_ENABLE,0,0},//29
			{SDRAM_ENABLE,TIMER_ENABLE,I2C_ENABLE},//30
			{SDRAM_ENABLE,TIMER_ENABLE,I2C_ENABLE},//31
		},
		{// Port 3
			{SDRAM_ENABLE,0,0},//0
			{SDRAM_ENABLE,0,0},//1
			{SDRAM_ENABLE,0,0},//2
			{SDRAM_ENABLE,0,0},//3
			{SDRAM_ENABLE,0,0},//4
			{SDRAM_ENABLE,0,0},//5
			{SDRAM_ENABLE,0,0},//6
			{SDRAM_ENABLE,0,0},//7
			{SDRAM_ENABLE,0,0},//8
			{SDRAM_ENABLE,0,0},//9
			{SDRAM_ENABLE,0,0},//10
			{SDRAM_ENABLE,0,0},//11
			{SDRAM_ENABLE,0,0},//12
			{SDRAM_ENABLE,0,0},//13
			{SDRAM_ENABLE,0,0},//14
			{SDRAM_ENABLE,0,0},//15
			{SDRAM_ENABLE,PWM_ENABLE,UART_ENABLE},//16
			{SDRAM_ENABLE,PWM_ENABLE,UART_ENABLE},//17
			{SDRAM_ENABLE,PWM_ENABLE,UART_ENABLE},//18
			{SDRAM_ENABLE,PWM_ENABLE,UART_ENABLE},//19
			{SDRAM_ENABLE,PWM_ENABLE,UART_ENABLE},//20
			{SDRAM_ENABLE,PWM_ENABLE,UART_ENABLE},//21
			{SDRAM_ENABLE,PWM_ENABLE,UART_ENABLE},//22
			{SDRAM_ENABLE,TIMER_ENABLE,PWM_ENABLE},//23
			{SDRAM_ENABLE,TIMER_ENABLE,PWM_ENABLE},//24
			{SDRAM_ENABLE,TIMER_ENABLE,PWM_ENABLE},//25
			{SDRAM_ENABLE,TIMER_ENABLE,PWM_ENABLE},//26
			{SDRAM_ENABLE,TIMER_ENABLE,PWM_ENABLE},//27
			{SDRAM_ENABLE,TIMER_ENABLE,PWM_ENABLE},//28
			{SDRAM_ENABLE,TIMER_ENABLE,PWM_ENABLE},//29
			{SDRAM_ENABLE,TIMER_ENABLE,UART_ENABLE},//30
			{SDRAM_ENABLE,TIMER_ENABLE,0},//31
			},
			{// Port 4
				{SDRAM_ENABLE,0,0},//0
				{SDRAM_ENABLE,0,0},//1
				{SDRAM_ENABLE,0,0},//2
				{SDRAM_ENABLE,0,0},//3
				{SDRAM_ENABLE,0,0},//4
				{SDRAM_ENABLE,0,0},//5
				{SDRAM_ENABLE,0,0},//6
				{SDRAM_ENABLE,0,0},//7
				{SDRAM_ENABLE,0,0},//8
				{SDRAM_ENABLE,0,0},//9
				{SDRAM_ENABLE,0,0},//10
				{SDRAM_ENABLE,0,0},//11
				{SDRAM_ENABLE,0,0},//12
				{SDRAM_ENABLE,0,0},//13
				{SDRAM_ENABLE,0,0},//14
				{SDRAM_ENABLE,0,0},//15
				{SDRAM_ENABLE,0,0},//16
				{SDRAM_ENABLE,0,0},//17
				{SDRAM_ENABLE,0,0},//18
				{SDRAM_ENABLE,0,0},//19
				{SDRAM_ENABLE,I2C_ENABLE,SSP_ENABLE},//20
				{SDRAM_ENABLE,I2C_ENABLE,SSP_ENABLE},//21
				{SDRAM_ENABLE,UART_ENABLE,SSP_ENABLE},//22
				{SDRAM_ENABLE,UART_ENABLE,SSP_ENABLE},//23
				{SDRAM_ENABLE,0,0},//24
				{SDRAM_ENABLE,0,0},//25
				{SDRAM_ENABLE,0,0},//26
				{SDRAM_ENABLE,0,0},//27
				{SDRAM_ENABLE,TIMER_ENABLE,UART_ENABLE},//28
				{SDRAM_ENABLE,TIMER_ENABLE,UART_ENABLE},//29
				{SDRAM_ENABLE,0,0},//30
				{CPLD_ENABLE,0,0},//31
			},
};
//end GPIO_MODValue

#ifdef _DEBUG
    unsigned char  GPIOFunc::GPIO_CheckResult[PORT_MAX][PIN_MAX];
	unsigned char  GPIOFunc::GPIO_MODValueError = 0;    //用来判断错误数的	
#endif

//<<<End>>> Private Members Sys2468 Class

// Private Method, Sys2468 Class


	////////
	//SetModuleIO(): Set the settings of GPIO configs for a single "hardware modules"
	//in: ioCount-the GPIO numbers; (ioSetting *)[4]-setting of these IO, from [0]~[3]
	//    are port, pin, MOD, value for PM_OUT mode.
	//out: 0
	////////
	//unsigned char GPIOFunc::SetModuleIO(int ioCount, UInt16 (ioSetting *)[3], int moduleName)
	void GPIOFunc::SetModuleIO(unsigned char ioCount, unsigned char ioSetting[][4], unsigned char moduleName)//合法的
	{
		// MOD value information stored in GPIO_MODValue[][][4], e.g. if GPIO_MODValue[a][b][c] == CAN_ENABLE
		// then port a, pin b's value should be set to c+1 in order to set this pin for CAN moudle in PM_MOD mode.
		// correspondingly, CAN_ENABLE, SPI_ENABLE should have a distinct value each.
		unsigned int i, j, myport, mypin;

		for(i = 0; i < ioCount; i ++)						// for each IO needs to be set.
		{
			myport = ioSetting[i][0];						// get port
			mypin = ioSetting[i][1];						// get pin
			GPIO_MOD[myport][mypin] = ioSetting[i][2];		// set corresponding setting's mode
			if(ioSetting[i][2] == PM_OUT)					// set value for PM_OUT mode.
			{
				GPIO_InitValue[myport][mypin] = ioSetting[i][3];//设置为高或者低
			}
			else if(ioSetting[i][2] == PM_MOD)					// set value for PM_MOD mode.
			{
				for(j = 0; j < MOD_MAX; j ++)
				{
					if(GPIO_MODValue[myport][mypin][j] == moduleName)
					{
						GPIO_InitValue[myport][mypin] = j + 1;
						break;						
					}				
				}
				#ifdef _DEBUG
				GPIO_CheckResult[myport][mypin]++;    //自加1 判断重复定义
				#endif
			}
		}

	}


	////////
	//GPIO_Set_n_Check(): Set the settings of GPIO configs for all "hardware modules"
	//    and if _DEBUG defined, also check violations.
	//    This function should be run under _DEBUG mode at least once before releasing to
	//    spot any possible violation, and should be released without _DEBUG setting.
	//in: N/A
	//out: 0    一旦重复定义 return 1
	////////
	void GPIOFunc::GPIO_Set_n_Check(void)
	{

/*
//SPI bus
//EINT
//外部扩展存储器    可用作sdram
//read boardID    ??
//Ethernet
//CAN
//DAC
//ADC
//RTC
//UART
//I2C
//Timer捕获

//预留，尚未用到
//USB
//PWM
//SD card
//
*/

		// and so on, set other moudles GPIO, this part need to be modified manually when
		// a new module is added.
		// #ifdef xxxx
		//   init xxxx
		// #endif (xxxx)

		#ifdef SDRAM_ENABLE
		SetModuleIO(SDRAMFunc::MyIOCount,SDRAMFunc::MyIO,SDRAM_ENABLE);
		#endif

		//spi部分需要修改
		#ifdef SPI_ENABLE		// set different modules seperately according to modules' parameters
		SetModuleIO(SSPIFunc::MyIOCount1, SSPIFunc::MyIO1, SPI_ENABLE);
		//SetModuleIO(SSPIFunc::MyIOCount2, SSPIFunc::MyIO2, SSP_ENABLE);
		//SetModuleIO(SSPIFunc::MyIOCount3, SSPIFunc::MyIO3, SSP_ENABLE);
		//建议放到IBT中进行    comment out chen
		#endif

		
		
		#ifdef _SYNCSTART_
			//EINTInit();    //不支持同步不用外中断，屏蔽处理
		#endif

		#ifdef ENET_ENABLE		// set different modules seperately according to modules' parameters
		// this IO definition has problems.
		//SetModuleIO(EthernetFunc::MyIOCount, EthernetFunc::MyIO, ENET_ENABLE);
		#endif

		// this part should include a special "module": Channel modules, and the IO of them are relay and switchs.
		//特定位的输出值0(PI接口)

		//comment out chen 1121
		#ifdef I2C_ENABLE
		//SetModuleIO(I2CFunc::MyIOCount,I2CFunc::MyIO,I2C_ENABLE);
		#endif

        #ifdef CPLD_ENABLE
		SetModuleIO(CPLDFunc::MyIOCount,CPLDFunc::MyIO,CPLD_ENABLE);
		#endif

		SetModuleIO(LEDFunc::MyIOCount,LEDFunc::MyIO,0);
		
		//start debug
		#ifdef _DEBUG
		unsigned char i, j;
		unsigned char result = 0;			// init the checkresult arrays for violation checking
		result = 0;				// spot the violations on request, return 1 to indicate violation happening.
		for(i = 0; i < PORT_MAX; i++)
		{
			for(j = 0; j < PIN_MAX; j ++)
			{
				if(GPIO_CheckResult[i][j] > 1)    //重复定义是解决了，但是GPIO_MODValue还是无法解决
				{
					result = 1;
				}
			}
		}
		#endif
		//end debug

	}

	////////
	//GPIO_Init(): Set the GPIO according to GPIO setting config.
	//in: N/A
	//out: M/A
	////////
	void GPIOFunc::GPIO_Init(void)
	{
		unsigned char uc_Port, uc_Bit;

	    for(uc_Port = 0; uc_Port < PORT_MAX; uc_Port ++)
	    {
	        for(uc_Bit = 0; uc_Bit < PIN_MAX; uc_Bit ++)
	            InitPort(uc_Port, uc_Bit, GPIO_MOD[uc_Port][uc_Bit], GPIO_InitValue[uc_Port][uc_Bit],0);
	    }	

		//判断是否有错误
		#ifdef  _DEBUG
		unsigned char debug = 0;
		debug = GPIO_MODValueError;
		#endif
	}

//<<<End>>> Private Method, Sys2468 Class

// Public Method, GPIO Class

	//modify comment out chen
	void GPIOFunc::InitPort(unsigned char uc_Port, unsigned char uc_Pin, unsigned char uc_Mode, unsigned char uc_Value,unsigned char uc_Base)
	{
		if (uc_Base == 1)
		{
        return;
		}

		UWord32 uw32_temp = 0;
		unsigned char i = 0, j = 0;

		if(uc_Mode == PM_MOD)    // if define as Module
		{
			uw32_temp = UWord32(GPIO_InitValue[uc_Port][uc_Pin]);	// Get Mod value
			// Get offset from PINSEL_BASE_ADDR.
			if(uc_Pin < 16)
			{
				i = uc_Port << 3;	//  = *2 *4
				j = uc_Pin;
			}
			else
			{
				i = (uc_Port * 2 + 1) << 2;	// = (*2 +1)*4
				j = uc_Pin - 16;			
			}
			uw32_temp = uw32_temp << (j << 1);
			(*(volatile unsigned long *)(PINSEL_BASE_ADDR + i)) |= uw32_temp;
		}
		else    // FGPIO,需要 SCS系统控制和状态寄存器 GPIOM = 1
		{
			SCS |= 0x01;    //使能FGPIO

			i = uc_Port << 5;	//  = * 0x20	Offset from FIOxDIR
			uw32_temp = (0x00000001) << uc_Pin;	// offset of Pin.

			if(uc_Mode == PM_IN)
			{
				(*(volatile unsigned long *)(FIO_BASE_ADDR + i)) &= ~(uw32_temp);    // FIOxDIR corresponding bit clr
			}
			
			if(uc_Mode == PM_OUT)
			{
				(*(volatile unsigned long *)(FIO_BASE_ADDR + i)) |= uw32_temp;    // FIOxDIR corresponding bit set

				// Now set Output Init Value
				//if(GPIO_InitValue[uc_Port][uc_Pin] == 1)		// output high
				if(uc_Value == 1)     //修改    comment out chen 20130206
				{
					(*(volatile unsigned long *)(FIO_BASE_ADDR + i + 0x18)) |= uw32_temp; // FIOxSET corresponding bit set
				}
				else											// output low
				{
					(*(volatile unsigned long *)(FIO_BASE_ADDR + i + 0x1C)) &= ~uw32_temp;    // FIOxCLR corresponding bit clr
				}
			}
		}
		return;
	}





	//GPIO操作 setGPort 设置高低电平    clrGPort低电平
	void GPIOFunc::Set_G_Port(G_Port * g_port)
	{
		unsigned char uc_Port = g_port->P1;
		unsigned char uc_Bits = g_port->P2;
		unsigned char uc_temp = 0;

		unsigned short uw16_temp = 0;

		if(uc_Bits < 16)
		{
			uw16_temp = (0x0001) << uc_Bits;
			(*(volatile unsigned short *)(FIO_BASE_ADDR + 0x18+(uc_Port*32))) |= uw16_temp;
		}
		else
		{
			uc_temp = uc_Bits-16;
			uw16_temp = (0x0001) << uc_temp;
			(*(volatile unsigned short *)(FIO_BASE_ADDR + 0x1A+(uc_Port*32))) |= uw16_temp;
		}

		
	}

	void GPIOFunc::Clr_G_Port(G_Port *g_port)
	{
		unsigned char uc_Port = g_port->P1;
		unsigned char uc_Bits = g_port->P2;
		unsigned char uc_temp = 0;
		unsigned short uw16_temp = 0;

		if(uc_Bits < 16)
		{
			uw16_temp = (0x0001) << uc_Bits;
			(*(volatile unsigned short *)(FIO_BASE_ADDR + 0x1C+(uc_Port*(0x20)))) |= uw16_temp;
		}
		else
		{
			uc_temp = uc_Bits-16;
			uw16_temp = (0x0001) << uc_temp;
			(*(volatile unsigned short *)(FIO_BASE_ADDR + 0x1E+(uc_Port*(0x20)))) |= uw16_temp;
		}

		
	}

    unsigned char GPIOFunc::RD_G_Port(G_Port *g_port)
	{
		unsigned char uc_Port = g_port->P1;
		unsigned char uc_Bits = g_port->P2;
		//unsigned char uc_temp = 0;
		unsigned short uw16_temp = 0;

		if(uc_Bits < 16)
		{
			
			uw16_temp=(*(volatile unsigned short *)(FIO_BASE_ADDR + 0x1C+(uc_Port*(0x20)))) ;
			uw16_temp >>= uc_Bits;
		}
		else
		{
			//uc_temp = uc_Bits-16;
			//uw16_temp = (0x0001) << uc_temp;
			uw16_temp =(*(volatile unsigned short *)(FIO_BASE_ADDR + 0x1E+(uc_Port*(0x20)))) ;
			uw16_temp >>= uc_Bits;
		}

		return (unsigned char)uw16_temp;
	}
/*********************************************************************************
**                            End Of File
*********************************************************************************/

