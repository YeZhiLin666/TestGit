///////////////////////////////////////////////////////////////////////////////////
//File Name: AUX_SampleManage.cpp
//Version:		V1.0
//Date:				2013/11/18
//Description:
//						辅助采样相关处理
//Others:      dirui
//History：

///////////////////////////////////////////////////////////////////////////////////
#include "../../Entry/includes.h"

//ST_AUX_BOARD_INFO  	  Aux_DataManager::m_AllAuxBoards使用这个来代替
//st_aux_sample_setting   chen 20131203
// Aux_DataManager::m_AllAuxBoards 相当于实际使用的物理Bid单元
//data
//st_Aux_ADC_status AUX_SampleManage::st_Aux[8]; 
st_Aux_Chn_Info AUX_SampleManage::st_Aux_info[MAX_BOARD_PER_AUXMCU][17];    //类似于channle info
SSPI_Para AUX_SampleManage::st_Aux_Para[2];
st_Aux_Sample AUX_SampleManage::st_aux_sample_setting[MAX_BOARD_PER_AUXMCU];

//P0.15 P0.17 P0.18 ssp0      P1.20 P1.23 P1.24 ssp0
// const St_Port AUX_SampleManage::Pin_SCK0      = {1,        20,         eCPTYPE_MCUGPIO,   SSP0_SCK,              0};
// const St_Port AUX_SampleManage::Pin_MISO0     = {1,        23,         eCPTYPE_MCUGPIO,   SSP0_MISO,             0};
// const St_Port AUX_SampleManage::Pin_MOSI0     = {1,        24,         eCPTYPE_MCUGPIO,   SSP0_MOSI,             0};
//P0.7 P0.8 P0.9  ssp1
const St_Port AUX_SampleManage::Pin_SCK1      = {0,        7,         eCPTYPE_MCUGPIO,   SSP1_SCK,              0};
const St_Port AUX_SampleManage::Pin_MISO1     = {0,        8,         eCPTYPE_MCUGPIO,   SSP1_MISO,             0};
const St_Port AUX_SampleManage::Pin_MOSI1     = {0,        9,         eCPTYPE_MCUGPIO,   SSP1_MOSI,             0};

////将来使用数组    chen 20131127
//const St_Port AUX_SampleManage::Pin_AD_CS1    = {1,        6,        eCPTYPE_MCUGPIO,   GPIO_O,                0};
//const St_Port AUX_SampleManage::Pin_AD_CS2    = {1,        7,        eCPTYPE_MCUGPIO,   GPIO_O,                0};
//const St_Port AUX_SampleManage::Pin_AD_CS3    = {1,        11,        eCPTYPE_MCUGPIO,   GPIO_O,                0};
//const St_Port AUX_SampleManage::Pin_AD_CS4    = {1,        12,        eCPTYPE_MCUGPIO,   GPIO_O,                0};
//const St_Port AUX_SampleManage::Pin_AD_CS5    = {1,        13,        eCPTYPE_MCUGPIO,   GPIO_O,                0};
//const St_Port AUX_SampleManage::Pin_AD_CS6    = {1,        18,        eCPTYPE_MCUGPIO,   GPIO_O,                0};
//const St_Port AUX_SampleManage::Pin_AD_CS7    = {1,        19,        eCPTYPE_MCUGPIO,   GPIO_O,                0};
//const St_Port AUX_SampleManage::Pin_AD_CS8    = {1,        21,        eCPTYPE_MCUGPIO,   GPIO_O,                0};


const St_Port AUX_SampleManage::Pin_ECODE1		={1,        18,        eCPTYPE_MCUGPIO,   GPIO_O,                0};
const St_Port AUX_SampleManage::Pin_ECODE2		={1,        19,        eCPTYPE_MCUGPIO,   GPIO_O,                0};
const St_Port AUX_SampleManage::Pin_ECODE3		={1,        20,        eCPTYPE_MCUGPIO,   GPIO_O,                0};
const St_Port AUX_SampleManage::Pin_EOE			={1,        21,        eCPTYPE_MCUGPIO,   GPIO_O,                0};
const St_Port AUX_SampleManage::Pin_ADCBUSY		={2,        12,        eCPTYPE_MCUGPIO,   GPIO_I,                0};

#if(AUX_2ndV_FOR_HRNEW==1)		
const St_Port AUX_SampleManage::Pin_Cover		={1,        28,        eCPTYPE_MCUGPIO,   GPIO_O,                0};
const St_Port AUX_SampleManage::Pin_CS		    ={2,        8,         eCPTYPE_MCUGPIO,   GPIO_O,                0};
#endif


unsigned char AUX_SampleManage::sspIsUsedFlag[3];
unsigned char AUX_SampleManage::TemperChannel;
unsigned long temp[8];
#pragma arm section rwdata = "SRAM",zidata = "SRAM"
unsigned long AUX_SampleManage::m_SampleTempData2ndV[MAX_BOARD_PER_AUXMCU][MAX_AUXCH_PER_BOARD][3];
unsigned long AUX_SampleManage::m_SampleRecord[MAX_BOARD_PER_AUXMCU][MAX_AUXCH_PER_BOARD];
#pragma arm section
//unsigned char AUX_SampleManage::Aux_V_valid;
//unsigned char AUX_SampleManage::Aux_T_valid;

//在获得Fram配置后运行
//仅需告诉我  这个mcu有几个Bid和类型和通道即可
//FRAM 慢ADC2498 DAC为ssp0    快速ADC2449 ssp1
void AUX_SampleManage::Aux_Sample_init(void)
{
	////获得MCU类型
	////获得Bid和chn 数量，类型
	////进行初始化的valid
	//for(unsigned char i = 0;i<8;i++)
	//{
	//	st_Aux[i].stateMachine = AUX_SAMPLE_IDLE_ENUM;    //idle
	//	st_Aux[i].ADCchn = 0;    //从0 开始
	//	st_Aux[i].ADCid = 99;    //认为都没使用
	//}

	for(unsigned char i = 0;i<3;i++)
	{
		sspIsUsedFlag[i] = 0;    //no used
	}

	////先暂时写死
	////chen 20131120
	//st_Aux[0].ADCid = 0;
	//st_Aux[0].ADCtype = AUX_T_SAMPLE_TYPE;//AUX_V_SAMPLE_TYPE    AUX_T_SAMPLE_TYPE
	//st_Aux[0].sspChn = 0;    //T用ssp0，V用ssp1
	//st_Aux[0].cs.PortType = eCPTYPE_MCUGPIO;
	//st_Aux[0].cs.Port = 1;
	//st_Aux[0].cs.Bits = 6;    //cs1 P1.6

	//Aux_V_valid = 1;
	//Aux_T_valid = 1;


	//ssp 参数 init
	st_Aux_Para[0].uc_Module_Name = SSP0_DEVICE1;
	st_Aux_Para[0].uc_Speed = 2;
	st_Aux_Para[0].uc_CPOL = 0;
	st_Aux_Para[0].uc_CPHA = 0;
	st_Aux_Para[0].uc_MSTR = 1;    //master
	st_Aux_Para[0].uc_BITS = 8;
	st_Aux_Para[0].uc_LSBF = 0;

#if(AUX_2ndV_FOR_HRNEW==1)
	{
		st_Aux_Para[1].uc_Module_Name = SSP1_DEVICE2;
		st_Aux_Para[1].uc_Speed = 2;
		st_Aux_Para[1].uc_CPOL = 0;
		st_Aux_Para[1].uc_CPHA = 1;
		st_Aux_Para[1].uc_MSTR = 1;    //master
		st_Aux_Para[1].uc_BITS = 8;
		st_Aux_Para[1].uc_LSBF = 0;
	}
#else
	{
		st_Aux_Para[1].uc_Module_Name = SSP1_DEVICE1;
		st_Aux_Para[1].uc_Speed = 2;
		st_Aux_Para[1].uc_CPOL = 0;
		st_Aux_Para[1].uc_CPHA = 0;
		st_Aux_Para[1].uc_MSTR = 1;    //master
		st_Aux_Para[1].uc_BITS = 8;
		st_Aux_Para[1].uc_LSBF = 0;
	}
#endif

	//ssp0
	ControlPin::SetIObyCP(AUX_Base::Pin_MISO0);
	ControlPin::SetIObyCP(AUX_Base::Pin_MOSI0);
	ControlPin::SetIObyCP(AUX_Base::Pin_SCK0);



	//ssp1
	ControlPin::SetIObyCP(Pin_MISO1);
	ControlPin::SetIObyCP(Pin_MOSI1);
	ControlPin::SetIObyCP(Pin_SCK1);
#if( AUX_2ndV_FOR_HRNEW==1)
	ControlPin::SetIObyCP(Pin_Cover);  
#endif
	//cs
	//ControlPin::SetIObyCP(Pin_AD_CS1);
	//ControlPin::SetIObyCP(Pin_AD_CS2);
	//ControlPin::SetIObyCP(Pin_AD_CS3);
	//ControlPin::SetIObyCP(Pin_AD_CS4);
	//ControlPin::SetIObyCP(Pin_AD_CS5);
	//ControlPin::SetIObyCP(Pin_AD_CS6);
	//ControlPin::SetIObyCP(Pin_AD_CS7);
	//ControlPin::SetIObyCP(Pin_AD_CS8);

	ControlPin::SetIObyCP(Pin_EOE);
	ControlPin::SetIObyCP(Pin_ECODE1);
	ControlPin::SetIObyCP(Pin_ECODE2);
	ControlPin::SetIObyCP(Pin_ECODE3);
	ControlPin::SetIObyCP(Pin_ADCBUSY);

#if(AUX_2ndV_FOR_HRNEW==1)
	ControlPin::SetIObyCP(Pin_CS);
	ControlPin::SetIObyCP(Pin_Cover);
#endif	

	//ssp0
	SSPIFunc::setPara(&st_Aux_Para[0],0);
	SSPIFunc::my_Init(0);              //初始化  T  
	//ssp1
	SSPIFunc::setPara(&st_Aux_Para[1],1);
	SSPIFunc::my_Init(1);              //初始化  2VD


	//Timer0
	//board_config::Func_Sample = &(Aux_Sample_ISR);   


	////new 20131128
	//Sample状态机的内容初始化填充
	for (unsigned char i = 0;i<MAX_BOARD_PER_AUXMCU;i++)
	{
		st_aux_sample_setting[i].ADCid = 99;    //all is no used
		st_aux_sample_setting[i].ADC_chn = 0;
		st_aux_sample_setting[i].stateMachine = AUX_SAMPLE_IDLE_ENUM;    //all idle
		for (unsigned char j = 0;j<17;j++)    //可以被更改
		{
			st_aux_sample_setting[i].sampleSetting[j].exist = 0;
			st_aux_sample_setting[i].sampleSetting[j].inUsed = 0;
		}
	}


	//手工配置 测试时使用
	//根据ST_AUX_BOARD_INFO  	m_AllAuxBoards在这里进行填充  m_AllAuxBoards来自FRAM读取内容
	//这样就把m_AllAuxBoards和这里的st_aux_sample_setting脱离开了，这样使得st_aux_sample_setting私有化了  有一定好处
	//20131209  与sf_hwf.h同步
	for (unsigned char i = 0;i<MAX_BOARD_PER_AUXMCU;i++)
	{
		if(Aux_DataManager::m_AllAuxBoards[i].m_BoardInfo.m_AliveFlg == 1)    //如果存在
		{
			st_aux_sample_setting[i].ADCid = Aux_DataManager::m_AllAuxBoards[i].m_BoardInfo.m_BType.m_AuxType;    //1级分类
			//st_aux_sample_setting[i].type = Aux_DataManager::m_AllAuxBoards[i].m_BoardInfo.m_AuxSensor[0];    //2级分类

			//根据类型来赋值ssp通道
			//AUX_TEMPERATURE和AUX_VOLTAGE  from SF_Hwf.h
			if(Aux_DataManager::m_AllAuxBoards[i].m_BoardInfo.m_BType.m_AuxType == AUX_VOLTAGE)
			{
				st_aux_sample_setting[i].sspChn = AUX_V_SSP_CHN;
			}
			else if(Aux_DataManager::m_AllAuxBoards[i].m_BoardInfo.m_BType.m_AuxType == AUX_TEMPERATURE)
			{
				st_aux_sample_setting[i].sspChn = AUX_T_SSP_CHN;    //改线后用AUX_T_SSP_CHN，我使用的是未改线的为1

				//ref temp 假通道  当温度通道时使能
				st_aux_sample_setting[i].sampleSetting[16].exist = 1;
				st_aux_sample_setting[i].sampleSetting[16].inUsed = 1;
			}

			//根据m_ChannelCount来标记sampleSetting的exist和inUsed 
			//comment out chen 20131209    
			for (unsigned char k = 0;k<Aux_DataManager::m_AllAuxBoards[i].m_BoardInfo.m_ChannelCount;k++)
			{
				st_aux_sample_setting[i].sampleSetting[k].exist = 1;
				st_aux_sample_setting[i].sampleSetting[k].inUsed = 1;
				if((Aux_DataManager::m_AllAuxBoards[i].m_BoardInfo.m_BType.m_AuxType == AUX_TEMPERATURE) && \
					(Aux_DataManager::m_AllAuxBoards[i].m_AuxSensor_ChInfo[k].m_AuxTemp.TempChType == AUX_TEMP_SENSOR_THERMOCOUPLE))  //暂时这么写吧，为了能编译通过

					//                if(Aux_DataManager::m_AllAuxBoards[i].m_AuxType == AUX_TEMPERATURE)
					//给温度通道加传感器类型
				{ 
					st_aux_sample_setting[i].type[k] = Aux_DataManager::m_AllAuxBoards[i].m_AuxSensor_ChInfo[k].m_AuxTemp.m_AuxThermoCouple.SensorType; //  03.19.2014 DKQ  //2级分类  
					//st_aux_sample_setting[i].type[k] = Aux_DataManager::m_AllAuxBoards[i].m_AuxSensor_ChInfo[k].m_AuxTemp.m_AuxThermoCouple.TempChType;    //2级分类
				} 			
			}

		}
#if(AUX_2ndV_FOR_HRNEW==1)	
		Adc24bit_ADS1259::Init(eCONVCODETYPE_BIPOLAR);
		ControlPin::SetMeLow(Pin_EOE,0);
		ControlPin::SetMeLow(Pin_ECODE1,0);
		ControlPin::SetMeLow(Pin_ECODE2,0);
		ControlPin::SetMeLow(Pin_ECODE3,0);
		Adc24bit_ADS1259::Pin_Conver=AUX_SampleManage::Pin_Cover;
		Adc24bit_ADS1259::Pin_CS= AUX_SampleManage::Pin_CS;
		for(unsigned char i=0;i<Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_ChannelCount;i++)
		{
			Adc24bit_ADS1259::SetChannel(i);
			Adc24bit_ADS1259::WriteReg(1,i);
			// 			Adc24bit_ADS1259::ReadReg(1,i);
			// 			Adc24bit_ADS1259::Reset();
			// 			Adc24bit_ADS1259::ReadReg(1,i);
		}
#endif
	}


	//使用新改写的中断处理，之前的可以保留
	//2013 11 29 chen

	board_config::Func_Sample = &(testSample);  

}



unsigned char AUX_SampleManage::IssspUsed(unsigned char sspIdx)
{
	return sspIsUsedFlag[sspIdx];
}

void AUX_SampleManage::sspUsed(unsigned char sspIdx)
{
	sspIsUsedFlag[sspIdx] = 1;
}

void AUX_SampleManage::clearsspUsed(unsigned char sspIdx)
{
	sspIsUsedFlag[sspIdx] = 0;
}

//st_aux_sample_setting 与 st_Aux
//V电压
unsigned long AUX_SampleManage::Aux_V_Trigger(unsigned char Bid,unsigned char Bchn)
{
	//直接调用LTC2449
#if(AUX_2ndV_FOR_HRNEW==1)
	return 1;
#endif
	Adc32bit_LTC2449::Trigger(st_aux_sample_setting[Bid].sspChn,Bchn);

	return 1;
}

unsigned long AUX_SampleManage::Aux_V_Read(unsigned char Bid)
{
#if(AUX_2ndV_FOR_HRNEW==1)
	return 0;
#endif
	return Adc32bit_LTC2449::Read(st_aux_sample_setting[Bid].sspChn);
}

//T温度，在这里温度可能是T型，也可能是K型
unsigned long AUX_SampleManage::Aux_T_Trigger(unsigned char Bid,unsigned char Bchn)
{
	//直接调用LTC2498

	Adc32bit_LTC2498::Trigger(st_aux_sample_setting[Bid].sspChn,Bchn);
	return 1;
}

unsigned long AUX_SampleManage::Aux_T_Read(unsigned char Bid)
{
	return Adc32bit_LTC2498::Read(st_aux_sample_setting[Bid].sspChn);
}


//////20131128  chen test
//把上面的融合在一起，用快速和慢速来判别


char AUX_SampleManage::testSample(unsigned long  Value_A,unsigned long Value_B)
{
	static unsigned short Sample_conter[8] = {0,0,0,0,0,0,0,0};
	//static signed char adc_chn = 0;    //之前trigger的chn  用st_Aux_Sample内ADC_chn代替
	static signed char next_chn[8] = {0,0,0,0,0,0,0,0};   //下一个trigger的chn
	unsigned char bOK;

	for(unsigned char i = 0;i<MAX_BOARD_PER_AUXMCU;i++)    //遍历
	{
		if(st_aux_sample_setting[i].ADCid == 99)    //99认为没有使用该板
		{
			continue;    //继续循环
		}

		//进入轮询
		switch(st_aux_sample_setting[i].stateMachine)    //状态机
		{
		case AUX_SAMPLE_IDLE_ENUM:
			if(IssspUsed(st_aux_sample_setting[i].sspChn) == 0x01)
			{
				//donothing
			}
			else
			{
				sspUsed(st_aux_sample_setting[i].sspChn);

				//第一次只是简单的进行采样存在的板子的第一个物理通道即可
				next_chn[i] = 0;
				st_aux_sample_setting[i].ADC_chn = next_chn[i];
				if(st_aux_sample_setting[i].ADCid == AUX_TEMPERATURE)
				{
					bOK = SSPIFunc::getModuleName(0);
					if(bOK  != SSP0_DEVICE1)
					{
						SSPIFunc::setPara(&st_Aux_Para[0],0);
						SSPIFunc::my_Init(0);              //初始化
						ARBUS_DELAY(10);
					}
					Aux_T_Trigger(i,next_chn[i]);    //first just trigger and no return
				}
				else
				{
					Aux_V_Trigger(i,next_chn[i]);    //first just trigger and no return
				}
				st_aux_sample_setting[i].stateMachine = AUX_SAMPLE_WAITTING_TRIGGER;
			}
			break;
		case AUX_SAMPLE_WAITTING_TRIGGER:
			if(st_aux_sample_setting[i].ADCid == AUX_TEMPERATURE)
			{
				bOK = SSPIFunc::getModuleName(0);
				if(bOK  != SSP0_DEVICE1)
				{
					SSPIFunc::setPara(&st_Aux_Para[0],0);
					SSPIFunc::my_Init(0);              //初始化
					ARBUS_DELAY(10);
				}
				Aux_T_Read(i);
			}
			else
			{
				Aux_V_Read(i);    //此时read信息 无用
			}    
			clearsspUsed(st_aux_sample_setting[i].sspChn);

			st_aux_sample_setting[i].stateMachine = AUX_SAMPLE_WAITTING_SAMPLE;
			break;
		case AUX_SAMPLE_WAITTING_SAMPLE:
			Sample_conter[i]++;
			if(st_aux_sample_setting[i].ADCid == AUX_VOLTAGE)    //快速ADC conv时间短500us
			{
				if(Sample_conter[i] >= AUX_LTC_2449_SAMPLE_TIME)    //500us
				{
					Sample_conter[i] = 0;
					st_aux_sample_setting[i].stateMachine = AUX_SAMPLE_TRIGGER_NEXT;
				}
			}
			else if(st_aux_sample_setting[i].ADCid == AUX_TEMPERATURE)    //慢速ADC conv时间长200ms
			{
				if(Sample_conter[i] >= AUX_LTC_2498_SAMPLE_TIME)    //200ms
				{
					Sample_conter[i] = 0;
					st_aux_sample_setting[i].stateMachine = AUX_SAMPLE_TRIGGER_NEXT;
				}
			}

			break;
		case AUX_SAMPLE_TRIGGER_NEXT:
			bOK = 0;
			for(unsigned char uc_k = 0; uc_k < MAX_BOARD_PER_AUXMCU; uc_k ++)
				bOK |= (IssspUsed(st_aux_sample_setting[i].sspChn) << uc_k);  // 对三条SSPI总线共用的CS信号加锁  20140822 yy
			if(bOK != 0x00)
				//if(IssspUsed(st_aux_sample_setting[i].sspChn) == 0x01)   // 判断锁定,同一SSPI总线仅能允许一块板在使用
			{
				//donothing
				break;  // 同一SSPI总线仅能允许一块板在使用
			}
			else
			{
				sspUsed(st_aux_sample_setting[i].sspChn);   // 上锁,同一SSPI总线仅能允许一块板在使用。同时相当于CS信号也上了锁。
				sampleSettingSet_CS(i);   // 片选信号输出进行过滤处理

				if (st_aux_sample_setting[i].ADCid == AUX_TEMPERATURE)
				{
					Adc32bit_LTC2498::setCS(&Pin_EOE);     //T
				} 
				else
				{
#if(AUX_2ndV_FOR_HRNEW!=1)
					Adc32bit_LTC2449::setCS(&Pin_EOE);     //V
#endif
				}

				//按照快速慢速通道来
				//如果是应用中的快速通道，每次进入，如果是没有应用中的快速通道4/8/16一次
				//如果是慢速通道  128次
				next_chn[i] = findUsedChn(i);
				//test1 = next_chn[i];
				//获得数据，并trigger next
				if(st_aux_sample_setting[i].ADCid == AUX_TEMPERATURE)
				{
					bOK = SSPIFunc::getModuleName(0);
					if(bOK  != SSP0_DEVICE1)
					{
						SSPIFunc::setPara(&st_Aux_Para[0],0);
						SSPIFunc::my_Init(0);              //初始化
						ARBUS_DELAY(10);
					}
					Aux_T_Trigger(i,next_chn[i]);  
#if DEBUG_TASK_STATISTIC == 1
					Aux_DataManager::Count_SampleADC ++;     
#endif                    
				}
				else
				{
					Aux_V_Trigger(i,next_chn[i]);    
				}

				st_aux_sample_setting[i].stateMachine = AUX_SAMPLE_WAITTING_TRIGGER_NEXT;
			}
			break;
		case AUX_SAMPLE_WAITTING_TRIGGER_NEXT:
			//供采样任务后续处理，填到大数据体内
			unsigned long adc_code_temp;
			//首先判断adc_code_temp是否正确，如果不正确，则放弃这次数据，正确则记录同时NewAdcCode = 1
			//chen 20131227
			if(st_aux_sample_setting[i].ADCid == AUX_TEMPERATURE)
			{
				bOK = SSPIFunc::getModuleName(0);
				if(bOK  != SSP0_DEVICE1)
				{
					SSPIFunc::setPara(&st_Aux_Para[0],0);
					SSPIFunc::my_Init(0);              //初始化
					ARBUS_DELAY(10);
				}
				adc_code_temp  = Aux_T_Read(i);
				if (adc_code_temp!=0)
				{
					st_Aux_info[i][st_aux_sample_setting[i].ADC_chn].AdcCode = adc_code_temp;
					st_Aux_info[i][st_aux_sample_setting[i].ADC_chn].NewAdcCode = 1;
				}
				else
				{
					st_Aux_info[i][st_aux_sample_setting[i].ADC_chn].NewAdcCode = 0;
				}
				//st_Aux_info[i][st_aux_sample_setting[i].ADC_chn].AdcCode = Aux_T_Read(i);
			}
			else
			{
#if( AUX_2ndV_FOR_HRNEW==1)
				Adc24bit_ADS1259::SetChannel(st_aux_sample_setting[i].ADC_chn);
				Adc24bit_ADS1259::Convert();
				temp[st_aux_sample_setting[i].ADC_chn]=Adc24bit_ADS1259::Read(1,st_aux_sample_setting[i].ADC_chn);//采样数据源码

				adc_code_temp =temp[st_aux_sample_setting[i].ADC_chn];//Adc24bit_ADS1259::Read(1,i);//st_aux_sample_setting[i].ADC_chn);
#else
				adc_code_temp = Aux_V_Read(i);
#endif
				if (adc_code_temp!=0)
				{
					AUX_SampleManage::m_SampleTempData2ndV[i][st_aux_sample_setting[i].ADC_chn][AUX_SampleManage::m_SampleRecord[i][st_aux_sample_setting[i].ADC_chn]]=adc_code_temp;
					if(AUX_SampleManage::m_SampleRecord[i][st_aux_sample_setting[i].ADC_chn]>2)
					{
						unsigned long Max,Min;
						if(AUX_SampleManage::m_SampleTempData2ndV[i][st_aux_sample_setting[i].ADC_chn][0] \
							>=AUX_SampleManage::m_SampleTempData2ndV[i][st_aux_sample_setting[i].ADC_chn][1])
							Max=AUX_SampleManage::m_SampleTempData2ndV[i][st_aux_sample_setting[i].ADC_chn][0];
						else
							Max=AUX_SampleManage::m_SampleTempData2ndV[i][st_aux_sample_setting[i].ADC_chn][1];
						if(AUX_SampleManage::m_SampleTempData2ndV[i][st_aux_sample_setting[i].ADC_chn][2]>Max)
							Max=AUX_SampleManage::m_SampleTempData2ndV[i][st_aux_sample_setting[i].ADC_chn][2];
						if(AUX_SampleManage::m_SampleTempData2ndV[i][st_aux_sample_setting[i].ADC_chn][0] \
							<=AUX_SampleManage::m_SampleTempData2ndV[i][st_aux_sample_setting[i].ADC_chn][1])
							Min=AUX_SampleManage::m_SampleTempData2ndV[i][st_aux_sample_setting[i].ADC_chn][0];
						else
							Min=AUX_SampleManage::m_SampleTempData2ndV[i][st_aux_sample_setting[i].ADC_chn][1];
						if(AUX_SampleManage::m_SampleTempData2ndV[i][st_aux_sample_setting[i].ADC_chn][2]<Min)
							Min=AUX_SampleManage::m_SampleTempData2ndV[i][st_aux_sample_setting[i].ADC_chn][2];

						st_Aux_info[i][st_aux_sample_setting[i].ADC_chn].AdcCode=AUX_SampleManage::m_SampleTempData2ndV[i][st_aux_sample_setting[i].ADC_chn][0] \
							+AUX_SampleManage::m_SampleTempData2ndV[i][st_aux_sample_setting[i].ADC_chn][1] \
							+AUX_SampleManage::m_SampleTempData2ndV[i][st_aux_sample_setting[i].ADC_chn][2]-Max-Min;
						AUX_SampleManage::m_SampleRecord[i][st_aux_sample_setting[i].ADC_chn]=0;
						st_Aux_info[i][st_aux_sample_setting[i].ADC_chn].NewAdcCode = 1;
					}
					else
						AUX_SampleManage::m_SampleRecord[i][st_aux_sample_setting[i].ADC_chn]++;
					//st_Aux_info[i][st_aux_sample_setting[i].ADC_chn].AdcCode = adc_code_temp;

				}
				else
				{
					st_Aux_info[i][st_aux_sample_setting[i].ADC_chn].NewAdcCode = 0;
				}
				//st_Aux_info[i][st_aux_sample_setting[i].ADC_chn].AdcCode = Aux_V_Read(i);
			}
			//st_Aux_info[i][st_aux_sample_setting[i].ADC_chn].NewAdcCode = 1;
			clearsspUsed(st_aux_sample_setting[i].sspChn);    // 解锁,同一SSPI总线仅能允许一块板在使用

			st_aux_sample_setting[i].ADC_chn = next_chn[i];    //为下一次数据准备

			st_aux_sample_setting[i].stateMachine = AUX_SAMPLE_WAITTING_SAMPLE;    //继续等待采样

			break;
		default:
			st_aux_sample_setting[i].stateMachine = AUX_SAMPLE_IDLE_ENUM;
			break;

		}

	}//end for

	return 1;
}


//原文：如果是应用中的快速通道，那么每次必采样，如果是没有在应用中的快速通道，则每4（8/16）次经过此通道采样一次。如
//果是慢速通道，每128次经过此通道采样一次。
signed char AUX_SampleManage::findUsedChn(unsigned char Bid)
{
	//从st_aux_sample_setting[Bid].ADC_chn开始判断此通道是否为快速
	//根据内部counter来自加维护
	for(unsigned char i = st_aux_sample_setting[Bid].ADC_chn+1;i<17;i++)
	{
		if(st_aux_sample_setting[Bid].sampleSetting[i].exist == 0)    //通道不存在
		{
			continue;
		}

		if(st_aux_sample_setting[Bid].sampleSetting[i].inUsed == 1)    //应用中，我理解为此通道map到sch并有实验在跑
		{
			return i;
		}
		else
		{
			if (st_aux_sample_setting[Bid].sampleSetting[i].fastChn == 1)    //快速通道
			{

				if(st_aux_sample_setting[Bid].sampleSetting[i].counter >= 4)
				{
					st_aux_sample_setting[Bid].sampleSetting[i].counter = 0;
					return i;
				}
			}
			else    //慢速
			{
				st_aux_sample_setting[Bid].sampleSetting[i].counter++;
				if(st_aux_sample_setting[Bid].sampleSetting[i].counter >= 128)
				{
					st_aux_sample_setting[Bid].sampleSetting[i].counter = 0;
					return i;
				}
			}
		}
	}

	//翻转了
	for(unsigned char i = 0;i<st_aux_sample_setting[Bid].ADC_chn;i++)
	{
		if(st_aux_sample_setting[Bid].sampleSetting[i].exist == 0)    //通道不存在
		{
			continue;
		}

		if(st_aux_sample_setting[Bid].sampleSetting[i].inUsed == 1)    //应用中，我理解为此通道map到sch并有实验在跑
		{
			return i;
		}
		else
		{
			if (st_aux_sample_setting[Bid].sampleSetting[i].fastChn == 1)    //快速通道
			{
				st_aux_sample_setting[Bid].sampleSetting[i].counter++;
				if(st_aux_sample_setting[Bid].sampleSetting[i].counter >= 4)
				{
					st_aux_sample_setting[Bid].sampleSetting[i].counter = 0;
					return i;
				}
			}
			else    //慢速
			{
				st_aux_sample_setting[Bid].sampleSetting[i].counter++;
				if(st_aux_sample_setting[Bid].sampleSetting[i].counter >= 128)
				{
					st_aux_sample_setting[Bid].sampleSetting[i].counter = 0;
					return i;
				}
			}
		}
	}

	return st_aux_sample_setting[Bid].ADC_chn;    //no found so trigger present channel



	//20131128 old
	//从st_aux_sample_setting[Bid].ADC_chn开始判断此通道是否为快速
	//根据内部counter来自加维护
	//for(unsigned char i = st_aux_sample_setting[Bid].ADC_chn;i<17;i++)
	//{
	//	if(st_aux_sample_setting[Bid].sampleSetting[i].inUsed == 1)    //应用中，我理解为此通道map到sch并有实验在跑
	//	{
	//		return i;
	//	}
	//	else
	//	{
	//		if (st_aux_sample_setting[Bid].sampleSetting[i].fastChn == 1)    //快速通道
	//		{
	//			
	//			if(st_aux_sample_setting[Bid].sampleSetting[i].counter >= 4)
	//			{
	//				st_aux_sample_setting[Bid].sampleSetting[i].counter = 0;
	//				return i;
	//			}
	//		}
	//		else    //慢速
	//		{
	//			st_aux_sample_setting[Bid].sampleSetting[i].counter++;
	//			if(st_aux_sample_setting[Bid].sampleSetting[i].counter >= 128)
	//			{
	//				st_aux_sample_setting[Bid].sampleSetting[i].counter = 0;
	//				return i;
	//			}
	//		}
	//	}
	//}

	//for(unsigned char i = 0;i<st_aux_sample_setting[Bid].ADC_chn;i++)
	//{
	//	if(st_aux_sample_setting[Bid].sampleSetting[i].inUsed == 1)    //应用中，我理解为此通道map到sch并有实验在跑
	//	{
	//		return i;
	//	}
	//	else
	//	{
	//		if (st_aux_sample_setting[Bid].sampleSetting[i].fastChn == 1)    //快速通道
	//		{
	//			st_aux_sample_setting[Bid].sampleSetting[i].counter++;
	//			if(st_aux_sample_setting[Bid].sampleSetting[i].counter >= 4)
	//			{
	//				st_aux_sample_setting[Bid].sampleSetting[i].counter = 0;
	//				return i;
	//			}
	//		}
	//		else    //慢速
	//		{
	//			st_aux_sample_setting[Bid].sampleSetting[i].counter++;
	//			if(st_aux_sample_setting[Bid].sampleSetting[i].counter >= 128)
	//			{
	//				st_aux_sample_setting[Bid].sampleSetting[i].counter = 0;
	//				return i;
	//			}
	//		}
	//	}
	//}

	//return st_aux_sample_setting[Bid].ADC_chn;    //no found so trigger present channel
}

//change sample_setting
void AUX_SampleManage::sampleSettingSet(unsigned char Bid,unsigned char chn,st_Aux_SampleSetting * ss)
{
	st_aux_sample_setting[Bid].sampleSetting[chn] = *ss;
}

//change sample_setting_CS
void AUX_SampleManage::sampleSettingSet_CS(unsigned char Bid)
{
	//   片选信号输出进行过滤处理

#if(AUX_2ndV_FOR_HRNEW==1)
	return;
#endif
	switch(Bid)
	{
	case 7:
		ControlPin::SetMeLow(Pin_ECODE1,0); // 7号板
		ControlPin::SetMeLow(Pin_ECODE2,0);
		ControlPin::SetMeLow(Pin_ECODE3,0);
		break;
	case 6:
		ControlPin::SetMeHigh(Pin_ECODE1,1);
		ControlPin::SetMeLow(Pin_ECODE2,0);
		ControlPin::SetMeLow(Pin_ECODE3,0);
		break;
	case 5:
		ControlPin::SetMeLow(Pin_ECODE1,0);
		ControlPin::SetMeHigh(Pin_ECODE2,1);
		ControlPin::SetMeLow(Pin_ECODE3,0);
		break;
	case 4:
		ControlPin::SetMeHigh(Pin_ECODE1,1);
		ControlPin::SetMeHigh(Pin_ECODE2,1);
		ControlPin::SetMeLow(Pin_ECODE3,0);
		break;
	case 3:
		ControlPin::SetMeLow(Pin_ECODE1,0);
		ControlPin::SetMeLow(Pin_ECODE2,0);
		ControlPin::SetMeHigh(Pin_ECODE3,1);
		break;
	case 2:
		ControlPin::SetMeHigh(Pin_ECODE1,1);
		ControlPin::SetMeLow(Pin_ECODE2,0);
		ControlPin::SetMeHigh(Pin_ECODE3,1);
		break;
	case 1:
		ControlPin::SetMeLow(Pin_ECODE1,0);
		ControlPin::SetMeHigh(Pin_ECODE2,1);
		ControlPin::SetMeHigh(Pin_ECODE3,1);
		break;
	case 0:
		ControlPin::SetMeHigh(Pin_ECODE1,1);   // 0号板
		ControlPin::SetMeHigh(Pin_ECODE2,1);
		ControlPin::SetMeHigh(Pin_ECODE3,1);
		break;
	default:
		break;
	}
}
