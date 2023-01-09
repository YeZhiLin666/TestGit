/*****************************************************************************
// 程序文件      :Aux_MTC_Base.cpp     MTC控制程序
// 文件描述      :LPC4078 MCU board
// 编写人: Chang Zhong
// 编写日期(MM.DD.YYYY)      :04.05.2018
*****************************************************************************/
#include "includes.h"

#define MTC_MAX_CHANNELNO 8
int 							Aux_MTC_Base::index;
bool              Aux_MTC_Base::Aux_MTC_Board = false;
int								Aux_MTC_Base::Aux_MTC_Version = 1;  //Add for New 2nd version MZTC by ZC 12/013/2019

UInt16            Aux_MTC_Base::Priority = PRIORITY4;
UInt16            Aux_MTC_Base::TaskID = 	AUX_MTC_TASK;
int								Aux_MTC_Base::PinLevel;							//FanFault check
unsigned short Aux_MTC_Base::TEC_Phase;
float Aux_MTC_Base::Temperature[MTC_MAX_CHANNELNO];//Current temperature for each channel
float Aux_MTC_Base::Temperature1[MTC_MAX_CHANNELNO];
unsigned char Aux_MTC_Base::TC_count;
CHAMBER_TEC_PID_INFO Aux_MTC_Base::MTC_Pid_Info[MTC_MAX_CHANNELNO];
unsigned char Aux_MTC_Base::T_Alert_Flag[MTC_MAX_CHANNELNO]; //T_Alert for 8 channels
unsigned short Aux_MTC_Base::Aux_MTC_TemperatureSafetyCheckFlag[MTC_MAX_CHANNELNO];
unsigned short Aux_MTC_Base::Aux_MTC_TemperatureSafetyReportFlag[MTC_MAX_CHANNELNO];
unsigned short Aux_MTC_Base::MTCSafetyCheckTimeValue[MTC_MAX_CHANNELNO] = {60,60,60,60,60,60,60,60};
unsigned short Aux_MTC_Base::Aux_MTC_TemperatureHighLimitCheckFlag[MTC_MAX_CHANNELNO];
unsigned short Aux_MTC_Base::Aux_MTC_TemperatureLowLimitCheckFlag[MTC_MAX_CHANNELNO];
unsigned short Aux_MTC_Base::ALLTemperatureSafetyCheckFlag;
unsigned short Aux_MTC_Base::ALLTemperatureHighLimitCheckFlag;
unsigned short Aux_MTC_Base::ALLTemperatureLowLimitCheckFlag;
long long Aux_MTC_Base::Aux_MTC_TemperatureHighLimit[MTC_MAX_CHANNELNO] = {65,65,65,65,65,65,65,65};
long long Aux_MTC_Base::Aux_MTC_TemperatureLowLimit[MTC_MAX_CHANNELNO] = {5,5,5,5,5,5,5,5};
MsTime Aux_MTC_Base::MTCSafetyCheckTimeBuffer[MTC_MAX_CHANNELNO];
unsigned short Aux_MTC_Base::MTCTempSafetyGoto[MTC_MAX_CHANNELNO];
int Aux_MTC_Base::MTC_CH_Status[MTC_MAX_CHANNELNO]; //开机时所有MTC都应该处于停止状态
int Aux_MTC_Base::MTCMode[MTC_MAX_CHANNELNO]; //MTC是否有与MITS PRO联系
int Aux_MTC_Base::MTC_Fan_Fault_Counter[MTC_MAX_CHANNELNO];  //Add for New 2nd version MZTC by ZC 12/013/2019

unsigned char Aux_MTC_Base::T_Alert_Flag2 = 0;
float Aux_MTC_Base::PW_INC_LMT = 0.8;
float Aux_MTC_Base::PW_DEC_LMT = 1.1236;
unsigned char Aux_MTC_Base::TempUp = 0;
unsigned char Aux_MTC_Base::COOLRANGE[MTC_MAX_CHANNELNO] = {5,5,5,5,5,5,5,5};
unsigned char Aux_MTC_Base::HEATRANGE[MTC_MAX_CHANNELNO] = {5,5,5,5,5,5,5,5};
unsigned char Aux_MTC_Base::temp[2];
unsigned char Aux_MTC_Base::ErrorReportCount[MTC_MAX_CHANNELNO];
MTC_GROUP_CTRL Aux_MTC_Base::st_MTC_Group_Ctrl[MTC_MAX_CHANNELNO];
unsigned char Aux_MTC_Base::groupCount;
//unsigned char Aux_MTC_Base::TouchScreenRefreshCount;
//unsigned short Aux_MTC_Base::TestInput[MTC_MAX_CHANNELNO] = {0,0,0,0,0,0,0,0};
//unsigned short Aux_MTC_Base::Fan_Fault_All_Flag = 0;
//bool Aux_MTC_Base::testflag = 1;

#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM",zidata = "SRAM"		   // ???????????????	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // ???????????????	   dynamic memory  0XA0000000  0x03E80000
#endif

unsigned char Aux_MTC_Base::Fan_Err_Screen_Iter[MTC_MAX_CHANNELNO];

#pragma arm section //????????

const St_Port PinGroup_PWM[Pin_MTC_Total_Pins] =
{
    {3, 16, eCPTYPE_MCUGPIO, PWM0, 0},   //P3.19 TEC PWM0.1(1) Signal //Chamber1
    {3, 20, eCPTYPE_MCUGPIO, PWM0, 0},   //P3.20 TEC PWM0.5(5) Signal //Chamber2
    {3, 17, eCPTYPE_MCUGPIO, PWM0, 0},   //P3.18 TEC PWM0.2(2) Signal //Chamber3
    {3, 21, eCPTYPE_MCUGPIO, PWM0, 0},   //P3.21 TEC PWM0.6(6) Signal //Chamber4
    {3, 18, eCPTYPE_MCUGPIO, PWM0, 0},   //P3.17 TEC PWM0.3(3) Signal //Chamber5
    {1, 18, eCPTYPE_MCUGPIO, PWM1, 0},   //P1.18 TEC PWM1.1(7) Signal //Chamber6
    {3, 19, eCPTYPE_MCUGPIO, PWM0, 0},   //P3.16 TEC PWM0.4(4) Signal //Chamber7
    {1, 20, eCPTYPE_MCUGPIO, PWM1, 0},   //P1.20 TEC PWM1.2(8) Signal //Chamber8
    {2, 2, eCPTYPE_MCUGPIO, GPIO_O, 0}, 	//P2.2 PWM1 Enable //Chamber1
    {2, 6, eCPTYPE_MCUGPIO, GPIO_O, 0},	  //P2.6 PWM5 Enable //Chamber2
    {2, 3, eCPTYPE_MCUGPIO, GPIO_O, 0}, 	//P2.3 PWM2 Enable //Chamber3
    {2, 7, eCPTYPE_MCUGPIO, GPIO_O, 0}, 	//P2.7 PWM6 Enable //Chamber4
    {2, 4, eCPTYPE_MCUGPIO, GPIO_O, 0},		//P2.4 PWM3 Enable //Chamber5
    {2, 8, eCPTYPE_MCUGPIO, GPIO_O, 0},	  //P2.8 PWM7 Enable //Chamber6
    {2, 5, eCPTYPE_MCUGPIO, GPIO_O, 0},		//P2.5 PWM4 Enable //Chamber7
    {2, 9, eCPTYPE_MCUGPIO, GPIO_O, 0}, 	//P2.9 PWM8 Enable //Chamber8
    {1, 21, eCPTYPE_MCUGPIO, GPIO_O, 1}, 	//P1.21 PWM1 Direction //Chamber1
    {1, 25, eCPTYPE_MCUGPIO, GPIO_O, 1}, 	//P1.25 PWM5 Direction //Chamber2
    {1, 22, eCPTYPE_MCUGPIO, GPIO_O, 1}, 	//P1.22 PWM2 Direction //Chamber3
    {1, 26, eCPTYPE_MCUGPIO, GPIO_O, 1}, 	//P1.26 PWM6 Direction //Chamber4
    {1, 23, eCPTYPE_MCUGPIO, GPIO_O, 1}, 	//P1.23 PWM3 Direction //Chamber5
    {1, 27, eCPTYPE_MCUGPIO, GPIO_O, 1}, 	//P1.27 PWM7 Direction //Chamber6
    {1, 24, eCPTYPE_MCUGPIO, GPIO_O, 1}, 	//P1.24 PWM4 Direction //Chamber7
    {1, 28, eCPTYPE_MCUGPIO, GPIO_O, 1}, 	//P1.28 PWM8 Direction //Chamber8
    {1, 7, eCPTYPE_MCUGPIO, GPIO_I, 0},  //P2.0 T_ALERT
    {1, 2, eCPTYPE_MCUGPIO, GPIO_I, 0}, 	//P1.2 FAN LOCK CHECK 1  //Chamber1
    {1, 7, eCPTYPE_MCUGPIO, GPIO_I, 0}, 	//P1.7 FAN LOCK CHECK 5  //Chamber2
    {1, 3, eCPTYPE_MCUGPIO, GPIO_I, 0}, 	//P1.3 FAN LOCK CHECK 2  //Chamber3
    {1, 11, eCPTYPE_MCUGPIO, GPIO_I, 0}, 	//P1.11 FAN LOCK CHECK 6 //Chamber4
    {1, 5, eCPTYPE_MCUGPIO, GPIO_I, 0}, 	//P1.5 FAN LOCK CHECK 3  //Chamber5
    {1, 12, eCPTYPE_MCUGPIO, GPIO_I, 0}, 	//P1.12 FAN LOCK CHECK 7 //Chamber6
    {1, 6, eCPTYPE_MCUGPIO, GPIO_I, 0}, 	//P1.6 FAN LOCK CHECK 4  //Chamber7
    {1, 13, eCPTYPE_MCUGPIO, GPIO_I, 0}, 	//P1.13 FAN LOCK CHECK 8 //Chamber8
};

const St_Port PinGroup_PWM_417118[Pin_MTC_Total_Pins] =
{
    {3, 16, eCPTYPE_MCUGPIO, PWM0, 0},   //P3.19 TEC PWM0.1(1) Signal //Chamber1
    {3, 17, eCPTYPE_MCUGPIO, PWM0, 0},   //P3.18 TEC PWM0.2(2) Signal //Chamber3
    {3, 18, eCPTYPE_MCUGPIO, PWM0, 0},   //P3.17 TEC PWM0.3(3) Signal //Chamber5
    {3, 19, eCPTYPE_MCUGPIO, PWM0, 0},   //P3.16 TEC PWM0.4(4) Signal //Chamber7
    {3, 20, eCPTYPE_MCUGPIO, PWM0, 0},   //P3.20 TEC PWM0.5(5) Signal //Chamber2
    {3, 21, eCPTYPE_MCUGPIO, PWM0, 0},   //P3.21 TEC PWM0.6(6) Signal //Chamber4
    {1, 18, eCPTYPE_MCUGPIO, PWM1, 0},   //P1.18 TEC PWM1.1(7) Signal //Chamber6
    {1, 20, eCPTYPE_MCUGPIO, PWM1, 0},   //P1.20 TEC PWM1.2(8) Signal //Chamber8
    {2, 2, eCPTYPE_MCUGPIO, GPIO_O, 0}, 	//P2.2 PWM1 Enable 
    {2, 3, eCPTYPE_MCUGPIO, GPIO_O, 0}, 	//P2.3 PWM2 Enable
    {2, 4, eCPTYPE_MCUGPIO, GPIO_O, 0},		//P2.4 PWM3 Enable 
    {2, 5, eCPTYPE_MCUGPIO, GPIO_O, 0},		//P2.5 PWM4 Enable 
    {2, 6, eCPTYPE_MCUGPIO, GPIO_O, 0},	  //P2.6 PWM5 Enable 
    {2, 7, eCPTYPE_MCUGPIO, GPIO_O, 0}, 	//P2.7 PWM6 Enable 
    {2, 8, eCPTYPE_MCUGPIO, GPIO_O, 0},	  //P2.8 PWM7 Enable 
    {2, 9, eCPTYPE_MCUGPIO, GPIO_O, 0}, 	//P2.9 PWM8 Enable 
    {1, 21, eCPTYPE_MCUGPIO, GPIO_O, 1}, 	//P1.21 PWM1 Direction
    {1, 22, eCPTYPE_MCUGPIO, GPIO_O, 1}, 	//P1.22 PWM2 Direction
    {1, 23, eCPTYPE_MCUGPIO, GPIO_O, 1}, 	//P1.23 PWM3 Direction
    {1, 24, eCPTYPE_MCUGPIO, GPIO_O, 1}, 	//P1.24 PWM4 Direction
    {1, 25, eCPTYPE_MCUGPIO, GPIO_O, 1}, 	//P1.25 PWM5 Direction
    {1, 26, eCPTYPE_MCUGPIO, GPIO_O, 1}, 	//P1.26 PWM6 Direction
    {1, 27, eCPTYPE_MCUGPIO, GPIO_O, 1}, 	//P1.27 PWM7 Direction
    {1, 28, eCPTYPE_MCUGPIO, GPIO_O, 1}, 	//P1.28 PWM8 Direction
    {1, 7, eCPTYPE_MCUGPIO, GPIO_I, 0},  //P2.0 T_ALERT
    {1, 2, eCPTYPE_MCUGPIO, GPIO_I, 0}, 	//P1.2 FAN LOCK CHECK 1  
    {1, 3, eCPTYPE_MCUGPIO, GPIO_I, 0}, 	//P1.3 FAN LOCK CHECK 2
    {1, 5, eCPTYPE_MCUGPIO, GPIO_I, 0}, 	//P1.5 FAN LOCK CHECK 3
    {1, 6, eCPTYPE_MCUGPIO, GPIO_I, 0}, 	//P1.6 FAN LOCK CHECK 4 
    {1, 7, eCPTYPE_MCUGPIO, GPIO_I, 0}, 	//P1.7 FAN LOCK CHECK 5    
    {1, 11, eCPTYPE_MCUGPIO, GPIO_I, 0}, 	//P1.11 FAN LOCK CHECK 6   
    {1, 12, eCPTYPE_MCUGPIO, GPIO_I, 0}, 	//P1.12 FAN LOCK CHECK 7  
    {1, 13, eCPTYPE_MCUGPIO, GPIO_I, 0}, 	//P1.13 FAN LOCK CHECK 8 
    {2, 14, eCPTYPE_MCUGPIO, GPIO_O, 1}, 	//P2.14 FAN CONTROL 1 
    {2, 15, eCPTYPE_MCUGPIO, GPIO_O, 1}, 	//P2.15 FAN CONTROL 2
    {2, 22, eCPTYPE_MCUGPIO, GPIO_O, 1}, 	//P2.22 FAN CONTROL 3 
    {2, 23, eCPTYPE_MCUGPIO, GPIO_O, 1}, 	//P2.23 FAN CONTROL 4  
    {2, 26, eCPTYPE_MCUGPIO, GPIO_O, 1}, 	//P2.26 FAN CONTROL 5 
    {2, 27, eCPTYPE_MCUGPIO, GPIO_O, 1}, 	//P2.27 FAN CONTROL 6 
    {2, 30, eCPTYPE_MCUGPIO, GPIO_O, 1}, 	//P2.30 FAN CONTROL 7 
    {2, 31, eCPTYPE_MCUGPIO, GPIO_O, 1}, 	//P2.31 FAN CONTROL 8 
//	{3, 0, eCPTYPE_MCUGPIO, GPIO_O, 0}, 	//P3.0 FCTLA1
//	{3, 1, eCPTYPE_MCUGPIO, GPIO_O, 0}, 	//P3.1 FCTLB1
//	{3, 2, eCPTYPE_MCUGPIO, GPIO_O, 0}, 	//P3.2 FCTLA2
//	{3, 3, eCPTYPE_MCUGPIO, GPIO_O, 0}, 	//P3.3 FCTLB2
//	{3, 4, eCPTYPE_MCUGPIO, GPIO_O, 0}, 	//P3.4 FCTLA3
//	{3, 5, eCPTYPE_MCUGPIO, GPIO_O, 0}, 	//P3.5 FCTLB3
//	{3, 6, eCPTYPE_MCUGPIO, GPIO_O, 0}, 	//P3.6 FCTLA4
//	{3, 7, eCPTYPE_MCUGPIO, GPIO_O, 0}, 	//P3.7 FCTLB4
//	{3, 8, eCPTYPE_MCUGPIO, GPIO_O, 0}, 	//P3.8 FCTLA5
//	{3, 9, eCPTYPE_MCUGPIO, GPIO_O, 0}, 	//P3.9 FCTLB5
//	{3, 10, eCPTYPE_MCUGPIO, GPIO_O, 0}, 	//P3.10 FCTLA6
//	{3, 11, eCPTYPE_MCUGPIO, GPIO_O, 0}, 	//P3.11 FCTLB6
//	{3, 12, eCPTYPE_MCUGPIO, GPIO_O, 0}, 	//P3.12 FCTLA7
//	{3, 13, eCPTYPE_MCUGPIO, GPIO_O, 0}, 	//P3.13 FCTLB7
//	{3, 14, eCPTYPE_MCUGPIO, GPIO_O, 0}, 	//P3.14 FCTLA8
//	{3, 15, eCPTYPE_MCUGPIO, GPIO_O, 0} 	//P3.15 FCTLB8

};

/******************************************************************************
                               MTC 8CH
******************************************************************************/
void Aux_MTC_Base::Hardware_Init(unsigned char BoardID)
{
    //分配 data_group 开始号
    Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start = BoardID;
    //标记data_group 有效
    Aux_DataManager::m_DataGroupMgr[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start] = true;
    for(int i = 0; i< MTC_MAX_CHANNELNO; i++) //该板型最多8个I2C通道
    {
        Aux_DataManager::m_AuxDataBank[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].m_Chn_Info =Aux_DataManager::m_AllAuxBoards[BoardID].m_BoardInfo.m_abi_uart_chn[i];   //copy  通道信息到 AuxDataBank
        Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].e_data_group_id = Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start;
        Aux_DataManager::m_PChn2EChn[Aux_DataManager::m_AllAuxBoards[BoardID].m_data_group_start][i].e_chan_id = i; //面板通道与电气通道同号
    }
}

void Aux_MTC_Base::Software_Init(unsigned char Board_ID)
{
    Aux_MTC_Board = true;
    for(int i=0; i<=7; i++) {
        DWIN_UartParse::MTCReportErr(0x0FC0+i*2,0x0000);
    }
		
		memset(Fan_Err_Screen_Iter,0,sizeof(Fan_Err_Screen_Iter));
}
/******************************************************************************
** Function name:        Init_PWM
** Descriptions:         Initialize PWM for TEC
                       - Frequency: 1kHz
** parameters:           None
** Returned value:       None
** Created Date:         2018/04/04
** Revision Dates:
******************************************************************************/
void Aux_MTC_Base::Init_PWM(void)
{
    PWM_MATCHCFG_Type  pwm_cofig;
//** Open interruption service
    IRQFunc::install_irq(PWM0_IRQn, HIGHEST_PRIORITY + 15);
    IRQFunc::install_irq(PWM1_IRQn, HIGHEST_PRIORITY + 16);
//** IO Setting
	if(Aux_MTC_Base::Aux_MTC_Version == 1)
    for( int i=0; i < Pin_MTC_Total_Pins; i++)
        ControlPin::SetIObyCP(PinGroup_PWM[i]);
	else if(Aux_MTC_Base::Aux_MTC_Version == 2)
    for( int i=0; i < Pin_MTC_Total_Pins; i++)
        ControlPin::SetIObyCP(PinGroup_PWM_417118[i]);		
//** Power on
    LPC4078PWM::CLKPWR_ConfigPPWR(0, ENABLE);
    LPC4078PWM::CLKPWR_ConfigPPWR(1, ENABLE);

//	for(int i=0; i < 16; i++){
//		ControlPin::SetMeHigh(PinGroup_PWM[Pin_FCTL_A1+i]);  //一上电就开启所有风扇
//	}

//** Clear all interrupts pending
    LPC_PWM0->IR = 0xFFF & ((uint32_t)(0x0000073F));
    LPC_PWM0->TCR = 0x00;
    LPC_PWM0->CTCR = 0x00;
    LPC_PWM0->MCR = 0x00;
    LPC_PWM0->CCR = 0x00;
    LPC_PWM0->PCR = 0x00; //PWM Control Register
    LPC_PWM0->LER = 0x00;

    LPC_PWM1->IR = 0xFFF & ((uint32_t)(0x0000073F));
    LPC_PWM1->TCR = 0x00;
    LPC_PWM1->CTCR = 0x00;
    LPC_PWM1->MCR = 0x00;
    LPC_PWM1->CCR = 0x00;
    LPC_PWM1->PCR = 0x00; //PWM Control Register
    LPC_PWM1->LER = 0x00;
//** Configure PCR (PWM CONTROL REGISTER)
    LPC4078PWM::PWM_ChannelCmd(0,1,ENABLE); //pPwm->PCR |= 0x1000; //PWM[1] output is enabled. //Chamber1
    LPC4078PWM::PWM_ChannelCmd(0,2,ENABLE); //pPwm->PCR |= 0x1000; //PWM[2] output is enabled. //Chamber5
    LPC4078PWM::PWM_ChannelCmd(0,3,ENABLE); //pPwm->PCR |= 0x1000; //PWM[3] output is enabled. //Chamber2
    LPC4078PWM::PWM_ChannelCmd(0,4,ENABLE); //pPwm->PCR |= 0x1000; //PWM[4] output is enabled. //Chamber6
    LPC4078PWM::PWM_ChannelCmd(0,5,ENABLE); //pPwm->PCR |= 0x1000; //PWM[5] output is enabled. //Chamber3
    LPC4078PWM::PWM_ChannelCmd(0,6,ENABLE); //pPwm->PCR |= 0x1000; //PWM[6] output is enabled. //Chamber7
    LPC4078PWM::PWM_ChannelCmd(1,1,ENABLE); //pPwm->PCR |= 0x1000; //PWM[7] output is enabled. //Chamber4
    LPC4078PWM::PWM_ChannelCmd(1,2,ENABLE); //pPwm->PCR |= 0x1000; //PWM[8] output is enabled. //Chamber8
    LPC4078PWM::PWM_ChannelConfig(0,1,PWM_CHANNEL_SINGLE_EDGE);//pPwm->PCR &= (~ 0x0010); // PWM[1] is single edge controlled //Chamber1
    LPC4078PWM::PWM_ChannelConfig(0,2,PWM_CHANNEL_SINGLE_EDGE);//pPwm->PCR &= (~ 0x0010); // PWM[2] is single edge controlled //Chamber5
    LPC4078PWM::PWM_ChannelConfig(0,3,PWM_CHANNEL_SINGLE_EDGE);//pPwm->PCR &= (~ 0x0010); // PWM[3] is single edge controlled //Chamber2
    LPC4078PWM::PWM_ChannelConfig(0,4,PWM_CHANNEL_SINGLE_EDGE);//pPwm->PCR &= (~ 0x0010); // PWM[4] is single edge controlled //Chamber6
    LPC4078PWM::PWM_ChannelConfig(0,5,PWM_CHANNEL_SINGLE_EDGE);//pPwm->PCR &= (~ 0x0010); // PWM[5] is single edge controlled //Chamber3
    LPC4078PWM::PWM_ChannelConfig(0,6,PWM_CHANNEL_SINGLE_EDGE);//pPwm->PCR &= (~ 0x0010); // PWM[6] is single edge controlled //Chamber7
    LPC4078PWM::PWM_ChannelConfig(1,1,PWM_CHANNEL_SINGLE_EDGE);//pPwm->PCR &= (~ 0x0010); // PWM[7] is single edge controlled //Chamber4
    LPC4078PWM::PWM_ChannelConfig(1,2,PWM_CHANNEL_SINGLE_EDGE);//pPwm->PCR &= (~ 0x0010); // PWM[8] is single edge controlled //Chamber8
//** Configure MR0 and LER
    LPC4078PWM::PWM_MatchUpdate(0,0,6e4,PWM_MATCH_UPDATE_NOW);//pPwm->MR0 = 0xEA60; pPwm->LER = 0x01;
    LPC4078PWM::PWM_MatchUpdate(1,0,6e4,PWM_MATCH_UPDATE_NOW);//pPwm->MR0 = 0xEA60; pPwm->LER = 0x01;
//** Configure PWM Match Control Register
    pwm_cofig.MatchChannel = 0;
    pwm_cofig.IntOnMatch = DISABLE;
    pwm_cofig.ResetOnMatch = ENABLE;
    pwm_cofig.StopOnMatch = DISABLE;
    LPC4078PWM::PWM_ConfigMatch(0, &pwm_cofig); //pPwm->MCR = 0x03;
    LPC4078PWM::PWM_ConfigMatch(1, &pwm_cofig); //pPwm->MCR = 0x03;
//** Configure MR and LER
    LPC4078PWM::PWM_MatchUpdate(0,1,0,PWM_MATCH_UPDATE_NOW);//pPwm->MR1 = 0x7530; pPwm->LER = 0x01; //UPDATE NOW, 50% DUTY CYCLE
    LPC4078PWM::PWM_MatchUpdate(0,2,0,PWM_MATCH_UPDATE_NOW);//pPwm->MR2 = 0x7530; pPwm->LER = 0x01; //UPDATE NOW, 50% DUTY CYCLE
    LPC4078PWM::PWM_MatchUpdate(0,3,0,PWM_MATCH_UPDATE_NOW);//pPwm->MR3 = 0x7530; pPwm->LER = 0x01; //UPDATE NOW, 50% DUTY CYCLE
    LPC4078PWM::PWM_MatchUpdate(0,4,0,PWM_MATCH_UPDATE_NOW);//pPwm->MR4 = 0x7530; pPwm->LER = 0x01; //UPDATE NOW, 50% DUTY CYCLE
    LPC4078PWM::PWM_MatchUpdate(0,5,0,PWM_MATCH_UPDATE_NOW);//pPwm->MR5 = 0xEA60; pPwm->LER = 0x01;
    LPC4078PWM::PWM_MatchUpdate(0,6,0,PWM_MATCH_UPDATE_NOW);//pPwm->MR6 = 0xEA60; pPwm->LER = 0x01;
    LPC4078PWM::PWM_MatchUpdate(1,1,0,PWM_MATCH_UPDATE_NOW);//pPwm->MR1 = 0xEA60; pPwm->LER = 0x01;
    LPC4078PWM::PWM_MatchUpdate(1,2,0,PWM_MATCH_UPDATE_NOW);//pPwm->MR2 = 0xEA60; pPwm->LER = 0x01;

    pwm_cofig.MatchChannel = 1;
    pwm_cofig.IntOnMatch = DISABLE;
    pwm_cofig.ResetOnMatch = DISABLE;
    pwm_cofig.StopOnMatch = DISABLE;
    LPC4078PWM::PWM_ConfigMatch(0, &pwm_cofig);
    LPC4078PWM::PWM_ConfigMatch(1, &pwm_cofig);
    pwm_cofig.MatchChannel = 2;
    pwm_cofig.IntOnMatch = DISABLE;
    pwm_cofig.ResetOnMatch = DISABLE;
    pwm_cofig.StopOnMatch = DISABLE;
    LPC4078PWM::PWM_ConfigMatch(0, &pwm_cofig);
    LPC4078PWM::PWM_ConfigMatch(1, &pwm_cofig);

    pwm_cofig.MatchChannel = 3;
    pwm_cofig.IntOnMatch = DISABLE;
    pwm_cofig.ResetOnMatch = DISABLE;
    pwm_cofig.StopOnMatch = DISABLE;
    LPC4078PWM::PWM_ConfigMatch(0, &pwm_cofig);
    pwm_cofig.MatchChannel = 4;
    pwm_cofig.IntOnMatch = DISABLE;
    pwm_cofig.ResetOnMatch = DISABLE;
    pwm_cofig.StopOnMatch = DISABLE;
    LPC4078PWM::PWM_ConfigMatch(0, &pwm_cofig);
    pwm_cofig.MatchChannel = 5;
    pwm_cofig.IntOnMatch = DISABLE;
    pwm_cofig.ResetOnMatch = DISABLE;
    pwm_cofig.StopOnMatch = DISABLE;
    LPC4078PWM::PWM_ConfigMatch(0, &pwm_cofig);
    pwm_cofig.MatchChannel = 6;
    pwm_cofig.IntOnMatch = DISABLE;
    pwm_cofig.ResetOnMatch = DISABLE;
    pwm_cofig.StopOnMatch = DISABLE;
    LPC4078PWM::PWM_ConfigMatch(0, &pwm_cofig);

    //** Reset Counter
    LPC4078PWM::PWM_ResetCounter(0);
    LPC4078PWM::PWM_ResetCounter(1);
    //** Counter Enable
    LPC4078PWM::PWM_CounterCmd(0,ENABLE);
    LPC4078PWM::PWM_CounterCmd(1,ENABLE);
    //** PWM Enable
    LPC4078PWM::PWM_Cmd(0,ENABLE);
    LPC4078PWM::PWM_Cmd(1,ENABLE);
		if(Aux_MTC_Base::Aux_MTC_Version == 2)
		{
			for( int i = Pin_FAN_CTRL_1; i <= Pin_FAN_CTRL_8; i++)
				ControlPin::SetMeLow(PinGroup_PWM_417118[i]);
		}
}

/******************************************************************************
** Function name:        Init_PIDPara
** Descriptions:         Initialize PID Parameter
** parameters:           None
** Returned value:       None
** Created Date:         2018/04/04
** Revision Dates:
******************************************************************************/
void Aux_MTC_Base::Init_PIDPara()
{
    for(int i=0; i<MTC_MAX_CHANNELNO; i++)
    {
        CHAMBER_TEC_PID_INFO * Pid_Info = &Aux_MTC_Base::MTC_Pid_Info[i];
        Pid_Info->LastErr = 0;
        Pid_Info->LastOutput = 0;
        Pid_Info->Kp = 0;
        Pid_Info->Ki = 0;
        Pid_Info->Kd = 0;
        Pid_Info->Ui = 0;
        Pid_Info->Up1 = 0;
        Pid_Info->Ud = 0;
        Pid_Info->Up = 0;
        Pid_Info->SatErr = 0;
    }
}

/******************************************************************************
** Function name:        lltostr
** Descriptions:         Convert long long to a string
** parameters:           long long TempSetPoint
** Returned value:       None
** Created Date:         2018/04/04
** Revision Dates:
******************************************************************************/
void Aux_MTC_Base::lltostr(long long TempSetPoit)
{
    if(TempSetPoit<0)
    {
        TempSetPoit = -TempSetPoit;
        temp[0]=(TempSetPoit>>8) | 0x80;
        temp[1]=TempSetPoit & 0xff;
    }
    else
    {
        temp[0]=(TempSetPoit>>8) & 0xff;
        temp[1]=TempSetPoit & 0xff;
    }
}



/******************************************************************************
** Function name:        Set_TempPoint
** Descriptions:         Set desired value for temperature
** parameters:           TECChannelNo
** Returned value:       None
** Created Date:         2018/04/04
** Revision Dates:
******************************************************************************/
void Aux_MTC_Base::Set_TempPoint(unsigned int TECChannelNo) //------------------------------------------------PID Calibration
{

    CHAMBER_TEC_PID_INFO * Pid_Info = &Aux_MTC_Base::MTC_Pid_Info[TECChannelNo];
    Pid_Info->SetTemp = (float)(Aux_MTC_Base::Temperature_set[TECChannelNo])/10.0f;
    lltostr(Temperature_set[TECChannelNo]);
    FM25L256Func::writePage(FRAM_TEMP_REC - 2 * TECChannelNo, &temp[0], 2);
    //-----------------------------------------------------------------------------------------------
    if(Aux_MTC_Base::Temperature_set[TECChannelNo]/10 > Aux_MTC_Base::Temperature[TECChannelNo])
        Aux_MTC_Base::TempUp = 1;
    else
        Aux_MTC_Base::TempUp = 0;
		if(Aux_MTC_Base::Aux_MTC_Version == 1)
		{
//** 0~18 degree
    if(Aux_MTC_Base::Temperature_set[TECChannelNo]/10 < 18.05)
    {
        COOLRANGE[TECChannelNo] = 1;
        HEATRANGE[TECChannelNo] = 5;
        if(TempUp == 1) // 升温 passed
        {
            Pid_Info->Kp = 1.1;
            Pid_Info->Ki = 3e-4;
            Pid_Info->Kd = 5e-6;
        }
        else //降温 passed
        {
            Pid_Info->Kp = 1.0;
            Pid_Info->Ki = 2e-4;
            Pid_Info->Kd = 5e-6;
        }
    }

//** 18~32 degree
    else if(Aux_MTC_Base::Temperature_set[TECChannelNo]/10 < 32.05)
    {
        COOLRANGE[TECChannelNo] = 5;
        HEATRANGE[TECChannelNo] = 5;
        if(TempUp == 1) //升温 passed
        {
            Pid_Info->Kp = 0.7;//0.59
            Pid_Info->Ki = 3e-4;//2.5e-5
            Pid_Info->Kd = 1e-6;
        }
        else //降温 passed
        {
            Pid_Info->Kp = 0.7;
            Pid_Info->Ki = 3e-4;
            Pid_Info->Kd = 1e-6;
        }
    }
//** 32~48 degree
    else if(Aux_MTC_Base::Temperature_set[TECChannelNo]/10 < 48.05)//
    {
        COOLRANGE[TECChannelNo] = 5;
        HEATRANGE[TECChannelNo] = 5;
        if(TempUp == 1) //升温 passed
        {
            Pid_Info->Kp = 0.9; //
            Pid_Info->Ki = 3e-4; //
            Pid_Info->Kd = 5e-5; //
        }
        else //降温 passed
        {
            Pid_Info->Kp = 0.9;//0.57
            Pid_Info->Ki = 3e-4;//2.0e-5
            Pid_Info->Kd = 5e-5;
        }
    }
//** 48~60 degree
    else
    {
        COOLRANGE[TECChannelNo] = 5;
        HEATRANGE[TECChannelNo] = 1;
        if(TempUp == 1) //升温
        {
            Pid_Info->Kp = 1;//0.57
            Pid_Info->Ki = 2e-4;//2.9e-5
            Pid_Info->Kd = 5e-6;
        }
        else //降温
        {
            Pid_Info->Kp = 1;
            Pid_Info->Ki = 2e-4;
            Pid_Info->Kd = 5e-6;
        }
    }
	}
		else if(Aux_MTC_Base::Aux_MTC_Version == 2)
		{
//** 0~18 degree
    if(Aux_MTC_Base::Temperature_set[TECChannelNo]/10 < 18.05)
    {
        COOLRANGE[TECChannelNo] = 1;
        HEATRANGE[TECChannelNo] = 5;
        if(TempUp == 1) // 升温 passed
        {
            Pid_Info->Kp = 0.8;//1.1
            Pid_Info->Ki = 1.0e-4;//3e-4
            Pid_Info->Kd = 5e-6;//5e-6
        }
        else //降温 passed
        {
            Pid_Info->Kp = 0.8; //1.0
            Pid_Info->Ki = 3e-4;	//2e-4
            Pid_Info->Kd = 6e-6;	//5e-6
        }
    }

//** 18~32 degree
    else if(Aux_MTC_Base::Temperature_set[TECChannelNo]/10 < 32.05)
    {
        COOLRANGE[TECChannelNo] = 5;
        HEATRANGE[TECChannelNo] = 5;
        if(TempUp == 1) //升温 passed
        {
            Pid_Info->Kp = 0.78;		//0.7
            Pid_Info->Ki = 8e-5;	//3e-4
            Pid_Info->Kd = 1e-6;	//1e-6
        }
        else //降温 passed
        {
            Pid_Info->Kp = 0.78;	//0.7
            Pid_Info->Ki = 8e-5;	//3e-4
            Pid_Info->Kd = 1e-6;	//1e-6
        }
    }
//** 32~48 degree
    else if(Aux_MTC_Base::Temperature_set[TECChannelNo]/10 < 48.05)//
    {
        COOLRANGE[TECChannelNo] = 5;
        HEATRANGE[TECChannelNo] = 5;
        if(TempUp == 1) //升温 passed
        {
            Pid_Info->Kp = 0.8; //0.9
            Pid_Info->Ki = 7e-5; //3e-4
            Pid_Info->Kd = 5e-5; //5e-5
        }
        else //降温 passed
        {
            Pid_Info->Kp = 0.8;//0.9
            Pid_Info->Ki = 7e-5;//3e-4
            Pid_Info->Kd = 5e-5;//5e-5
        }
    }
//** 48~60 degree
    else
    {
        COOLRANGE[TECChannelNo] = 5;
        HEATRANGE[TECChannelNo] = 5;
        if(TempUp == 1) //升温
        {
            Pid_Info->Kp = 0.65;//1
            Pid_Info->Ki = 9.0e-5;//2e-4
            Pid_Info->Kd = 5e-6;
        }
        else //降温
        {
            Pid_Info->Kp = 0.65;//1
            Pid_Info->Ki = 1.0e-4;//2e-4
            Pid_Info->Kd = 5e-6;
        }
    }
	}

}

/******************************************************************************
** Function name:        Init_MTC
** Descriptions:         Init PWM, PID parameters and enable H bridge
** parameters:           Pid_Info
** Returned value:       None
** Created Date:         2018/04/04
** Revision Dates:
******************************************************************************/

void Aux_MTC_Base::Init_MTC()
{
    long long Temp;
    unsigned char msb;
    for(int i=0; i<MTC_MAX_CHANNELNO; i++)
    {
        CHAMBER_TEC_PID_INFO * Pid_Info = &Aux_MTC_Base::MTC_Pid_Info[i];
        Aux_MTC_Base::TEC_Phase = GetTemp;
        FM25L256Func::readPage(FRAM_TEMP_REC-(2*i),&temp[0],2);
        msb=temp[0]>>7;
        Temp=temp[0]<<8;
        Temp=Temp+temp[1];
        Temp=Temp&(0x7fff);
        if(msb)
            Temp = -Temp;
        Aux_MTC_Base::Temperature_set[i] = Temp;
        Pid_Info->SetTemp = Temp;
        DWIN_UartParse::UpdateValue(0xf0+2*i, Temp);
//-------------------------------------------------------------------------from LCD
        if((Pid_Info->SetTemp > 60.0f) || (Pid_Info->SetTemp < 10.0f))
            Pid_Info->SetTemp = 24.0f;
        Aux_MTC_Base::Set_TempPoint(i);
        Pid_Info->OutMax = 0.99;
        Pid_Info->OutMin = -0.99;
    }
    for(int i = Pin_TEC_ENB_1; i <= Pin_TEC_ENB_8; i++)
		{
				if(Aux_MTC_Base::Aux_MTC_Version == 1)
					ControlPin::SetMeLow(PinGroup_PWM[i]);
				else
					ControlPin::SetMeLow(PinGroup_PWM_417118[i]);					
		}
}
void Aux_MTC_Base::Init(void)
{
    Aux_MTC_Base::Init_PWM(); //初始化PWM
    UARTFunc::UARTInit(1,TC_BR_115200,TC_DATA_BIT_8,TC_PARITY_NONE,TC_STOP_BIT_ONE);
    Aux_MTC_Base::Init_PIDPara();
    Aux_MTC_Base::Init_MTC();
    I2CBusNew::I2CInit(I2C0,100); //Initialize I2C0
    index = 0;
//Channel CH 1--8
    for( int i = 0; i < MTC_MAX_CHANNELNO; i++)
    {
        TMP75C_TEC::Init_TMP75C(i);
        Aux_MTC_Base::T_Alert_Flag[i]=0;
        Aux_MTC_Base::Temperature[i] = TMP75C_TEC::Get_MTC_Temp(i);
    }
}
/******************************************************************************
** Function name:        PID_Regulator
** Descriptions:         Traditional PID
** parameters:           TECChannelNo
** Returned value:       Pid_Info->Output
** Created Date:         2018/04/04
** Revision Dates:
******************************************************************************/
float Aux_MTC_Base::PID_Regulator(unsigned int TECChannelNo)
{

    CHAMBER_TEC_PID_INFO * Pid_Info = &Aux_MTC_Base::MTC_Pid_Info[TECChannelNo];
    Pid_Info->NewErr = Pid_Info->SetTemp - Pid_Info->SampleTemp; // e(k)
    if((Pid_Info->NewErr < 0.05) && (Pid_Info->NewErr > -0.05))
        Pid_Info->NewErr = 0;
    Pid_Info->Up = Pid_Info->NewErr - Pid_Info->LastErr; // Delta_e(k) = e(k) - e(k-1)
//** Delta_u(k) = Kp * Delta_e(k) + Ki * e(k) + Kd * (Delta_e(k) - Delta_e(k-1))
    Pid_Info->OutPreSat = Pid_Info->Kp * Pid_Info->Up + Pid_Info->Ki * Pid_Info->NewErr + Pid_Info->Kd * (Pid_Info->Up - Pid_Info->Up1);
//** If Kd = 0
//	  Pid_Info->OutPreSat = Pid_Info->Kp * Pid_Info->Up + Pid_Info->Ki * Pid_Info->NewErr;

    if(Pid_Info->OutPreSat > 0.1f)
        Pid_Info->OutPreSat = 0.1;
    else if(Pid_Info->OutPreSat < -0.1f)
        Pid_Info->OutPreSat = -0.1;

    //** u(k) = u(k-1) + Delta_u(k)
    Pid_Info->Output = Pid_Info->OutPreSat+Pid_Info->LastOutput;

    if(Pid_Info->Output > Pid_Info->OutMax)
        Pid_Info->Output = Pid_Info->OutMax;
    else if(Pid_Info->Output < Pid_Info->OutMin)
        Pid_Info->Output = Pid_Info->OutMin;

    Pid_Info->Up1 = Pid_Info->Up; //Delta_e(k-1). If Kd = 0, comment this.
    Pid_Info->LastOutput = Pid_Info->Output; //u(k-1)
    Pid_Info->LastErr = Pid_Info->NewErr; //e(k-1)
    return Pid_Info->Output;


}

/******************************************************************************
** Function name:        PWM_Calculation
** Descriptions:         Update the Macth4 Register and output PWM direction
** parameters:           pwm_output, TECChannelNo
** Returned value:       None
** Created Date:         2018/04/05
** Revision Dates:
******************************************************************************/
void Aux_MTC_Base::PWM_Calculation(float pwm_output,unsigned int TECChannelNo)
{
    unsigned int Pin_TEC_DIR;
    int md,ch;
		if(Aux_MTC_Base::Aux_MTC_Version == 1)
		{
			switch(TECChannelNo) {
				case 0:
						Pin_TEC_DIR = Pin_TEC_DIR_1;
						md=0;
						ch=1;
						break;
				case 1:
						Pin_TEC_DIR = Pin_TEC_DIR_2;
						md=0;
						ch=5;
						break;
				case 2:
						Pin_TEC_DIR = Pin_TEC_DIR_3;
						md=0;
						ch=2;
						break;
				case 3:
						Pin_TEC_DIR = Pin_TEC_DIR_4;
						md=0;
						ch=6;
						break;
				case 4:
						Pin_TEC_DIR = Pin_TEC_DIR_5;
						md=0;
						ch=3;
						break;
				case 5:
						Pin_TEC_DIR = Pin_TEC_DIR_6;
						md=1;
						ch=1;
						break;
				case 6:
						Pin_TEC_DIR = Pin_TEC_DIR_7;
						md=0;
						ch=4;
						break;
				case 7:
						Pin_TEC_DIR = Pin_TEC_DIR_8;
						md=1;
						ch=2;
						break;
				default:
						break;
			}
   }
		else if(Aux_MTC_Base::Aux_MTC_Version == 2)
		{
			switch(TECChannelNo) {
				case 0:
						Pin_TEC_DIR = Pin_TEC_DIR_1;
						md=0;
						ch=1;
						break;
				case 1:
						Pin_TEC_DIR = Pin_TEC_DIR_2;
						md=0;
						ch=2;
						break;
				case 2:
						Pin_TEC_DIR = Pin_TEC_DIR_3;
						md=0;
						ch=3;
						break;
				case 3:
						Pin_TEC_DIR = Pin_TEC_DIR_4;
						md=0;
						ch=4;
						break;
				case 4:
						Pin_TEC_DIR = Pin_TEC_DIR_5;
						md=0;
						ch=5;
						break;
				case 5:
						Pin_TEC_DIR = Pin_TEC_DIR_6;
						md=0;
						ch=6;
						break;
				case 6:
						Pin_TEC_DIR = Pin_TEC_DIR_7;
						md=1;
						ch=1;
						break;
				case 7:
						Pin_TEC_DIR = Pin_TEC_DIR_8;
						md=1;
						ch=2;
						break;
				default:
						break;
				}		
		}

    if(pwm_output >= -0.1f)
    {
				if(Aux_MTC_Base::Aux_MTC_Version == 1)
					ControlPin::SetMeHigh(PinGroup_PWM[Pin_TEC_DIR]);
				else if(Aux_MTC_Base::Aux_MTC_Version == 2)
					ControlPin::SetMeHigh(PinGroup_PWM_417118[Pin_TEC_DIR]);
        if(pwm_output <= 0)
            pwm_output = 0;
        LPC4078PWM::PWM_MatchUpdate(md,ch,PW_INC_LMT*pwm_output*6e4,PWM_MATCH_UPDATE_NEXT_RST);
    }
    else
    {
				if(Aux_MTC_Base::Aux_MTC_Version == 1)
					ControlPin::SetMeLow(PinGroup_PWM[Pin_TEC_DIR]);
				else if(Aux_MTC_Base::Aux_MTC_Version == 2)
					ControlPin::SetMeLow(PinGroup_PWM_417118[Pin_TEC_DIR]);
        LPC4078PWM::PWM_MatchUpdate(md,ch,-PW_DEC_LMT*(pwm_output+0.1)*6e4,PWM_MATCH_UPDATE_NEXT_RST);
    }

}


float round_up(float a)
{
    float res,t1;
    t1=a/0.001;
    while(t1>=10) {
        t1=t1-10;
    }
    if(t1>4) {
        res=a+0.01;
    }
    else res=a;
    return res;
}
/******************************************************************************
** Function name:        DoWork
** Descriptions:         Control TEC
** parameters:           None
** Returned value:       None
** Created Date:         2018/04/04
** Revision Dates:
******************************************************************************/
void Aux_MTC_Base::DoWork()
{
#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count[COUNT_MTC] ++;
    CommonFunc::GetTimeTicksUpdate(&OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
    Time_Count::thread_time_init(COUNT_MTC);
#endif
    static float pwm_output;
    int i = index;
    int md,ch;
    CHAMBER_TEC_PID_INFO * Pid_Info = &Aux_MTC_Base::MTC_Pid_Info[i];
    unsigned int Pin_TEC_DIR;
		if(Aux_MTC_Base::Aux_MTC_Version == 1)
		{
			switch(i) {
				case 0:
						Pin_TEC_DIR = Pin_TEC_DIR_1;
						md=0;
						ch=1;
						break;
				case 1:
						Pin_TEC_DIR = Pin_TEC_DIR_2;
						md=0;
						ch=5;
						break;
				case 2:
						Pin_TEC_DIR = Pin_TEC_DIR_3;
						md=0;
						ch=2;
						break;
				case 3:
						Pin_TEC_DIR = Pin_TEC_DIR_4;
						md=0;
						ch=6;
						break;
				case 4:
						Pin_TEC_DIR = Pin_TEC_DIR_5;
						md=0;
						ch=3;
						break;
				case 5:
						Pin_TEC_DIR = Pin_TEC_DIR_6;
						md=1;
						ch=1;
						break;
				case 6:
						Pin_TEC_DIR = Pin_TEC_DIR_7;
						md=0;
						ch=4;
						break;
				case 7:
						Pin_TEC_DIR = Pin_TEC_DIR_8;
						md=1;
						ch=2;
						break;
				default:
						break;
			}
		}
		else if(Aux_MTC_Base::Aux_MTC_Version == 2)
		{
			switch(i) {
				case 0:
						Pin_TEC_DIR = Pin_TEC_DIR_1;
						md=0;
						ch=1;
						break;
				case 1:
						Pin_TEC_DIR = Pin_TEC_DIR_2;
						md=0;
						ch=2;
						break;
				case 2:
						Pin_TEC_DIR = Pin_TEC_DIR_3;
						md=0;
						ch=3;
						break;
				case 3:
						Pin_TEC_DIR = Pin_TEC_DIR_4;
						md=0;
						ch=4;
						break;
				case 4:
						Pin_TEC_DIR = Pin_TEC_DIR_5;
						md=0;
						ch=5;
						break;
				case 5:
						Pin_TEC_DIR = Pin_TEC_DIR_6;
						md=0;
						ch=6;
						break;
				case 6:
						Pin_TEC_DIR = Pin_TEC_DIR_7;
						md=1;
						ch=1;
						break;
				case 7:
						Pin_TEC_DIR = Pin_TEC_DIR_8;
						md=1;
						ch=2;
						break;
				default:
						break;
			}
		}
		
    if((MTCMode[i] >= 1) && (MTCMode[i] <= 31)) {
        DWIN_UartParse::MTCSwitchToRemote(); //只允许Remote模式
    }
    else if (MTCMode[i] == 32) {
        DWIN_UartParse::MTCSwitchToRemote(); //只允许Remote模式
        for(unsigned char k=0; k<=7; k++) {
            if(MTCMode[k] == 0)
                DWIN_UartParse::SwitchModeReset(k);
        }
    }
    else if(MTCMode[i] == 33) {
        DWIN_UartParse::MTCKeepRemote(); //只允许Remote模式
    }

    switch(TEC_Phase)
    {
    case GetTemp:
    {
//			Aux_MTC_Base::Check_T_Alert();
        Aux_MTC_Base::Temperature[i] = TMP75C_TEC::Get_MTC_Temp(i);
			if(Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_abi_globe.m_BoardType == Aux_MZTC_Control_Board_444136)		
			{
				Aux_MTC_Base::Temperature[i] = Aux_MTC_Base::Temperature[i]+2- 0.08*Aux_MTC_Base::Temperature[i];
			}
//====将温度进行上传======
        Aux_DataManager::m_AuxDataBank[0][i].aux_instant_code = (int) (Aux_MTC_Base::Temperature[i] * 1000); //将数据放到Aux_DataBank 中
        Aux_DataManager::m_AuxDataBank[0][i].updated = true; //标记已经更新
//=======================
        TemperatureLimitCheck(i);
        //----------if over 65 degrees, begin to cool down-------------------------------------------
        if((Aux_MTC_Base::Temperature[i]>=65)&&(!Aux_MTC_Base::T_Alert_Flag[i])) {
            Aux_MTC_Base::T_Alert_Flag[i]=1;
        }
        //----------if back to 50 degrees, resume to normal settings---------------------------------
        if((Aux_MTC_Base::T_Alert_Flag[i])&&(Aux_MTC_Base::Temperature[i]<=50)) {
            Aux_MTC_Base::T_Alert_Flag[i]=0;
        }
        //-------------------------------------------------------------------------------------------
        //Update current measurement
//			if((TouchScreenRefreshCount < 255) && ( i == 7))
//			{
//				TouchScreenRefreshCount++;
//			}
//			else if((TouchScreenRefreshCount >= 255)&&(i == 7))
//			{
//				TouchScreenRefreshCount = 0;
//			}
//			if(TouchScreenRefreshCount >= 255)
//			{
//================与触摸屏更新各变量状态==========================================================
        DWIN_UartParse::UpdateValue(0x70+i*2,round_up(Temperature[i])*100);//11302018
        //Update current chamber status
        DWIN_UartParse::UpdateValue(0x80+i*2,MTC_CH_Status[i]); //温室开关状态
        DWIN_UartParse::MTCReportErr(0x0100+i*2, Aux_MTC_TemperatureSafetyCheckFlag[i]); //温度超时保护
        DWIN_UartParse::UpdateValue(0xB0+i*2, MTCSafetyCheckTimeValue[i]*100); //温度超时保护时间
        DWIN_UartParse::MTCReportErr(0x0110+i*2, Aux_MTC_TemperatureHighLimitCheckFlag[i]); //温度保护上限
        DWIN_UartParse::MTCReportErr(0x0120+i*2, Aux_MTC_TemperatureLowLimitCheckFlag[i]); //温度保护下限
        DWIN_UartParse::UpdateValue(0xA0+i*2, Aux_MTC_TemperatureHighLimit[i]*100); //温度保护上限值
        DWIN_UartParse::UpdateValue(0xC0+i*2, Aux_MTC_TemperatureLowLimit[i]*100); //温度保护下限值
        if(i == 0)
        {
            DWIN_UartParse::MTCReportErr(0xE0, ALLTemperatureSafetyCheckFlag); //ALL SET 温度超时保护
            DWIN_UartParse::MTCReportErr(0xE2, ALLTemperatureHighLimitCheckFlag); //ALL SET 温度保护上限
            DWIN_UartParse::MTCReportErr(0xE4, ALLTemperatureLowLimitCheckFlag); //ALL SET 温度保护下限
        }
        if(Aux_MTC_TemperatureSafetyReportFlag[i] == 1)
        {
            if(ErrorReportCount[i] > 0)
            {
                DWIN_UartParse::MTCReportErr(0x0FC0+i*2,0x0002);
                ErrorReportCount[i]--;
            }
            else if(ErrorReportCount[i] == 0)
            {
                Aux_MTC_TemperatureSafetyReportFlag[i] = 0;
            }
        }
//==============================================================================================
//			}
        Pid_Info->SampleTemp = (float)((long)((Temperature[i] + 0.05f) * 10))/10;
        Aux_MTC_Base::Set_TempPoint(i);
        Aux_MTC_Base::TEC_Phase = CalPWM;
    }
    break;

    case CalPWM:
    {
        UpdateAllSetTemp();
        if(MTC_CH_Status[i] == 0x10000)
            pwm_output = PID_Regulator(i);
        Aux_MTC_Base::TEC_Phase = OutputPWM;
    }
    break;

    case OutputPWM:
    {
        if(Aux_MTC_Base::T_Alert_Flag[i])
        {
			if(Aux_MTC_Base::Aux_MTC_Version == 1)
			{
				ControlPin::SetMeLow(PinGroup_PWM[Pin_TEC_DIR]);
			}
			else if(Aux_MTC_Base::Aux_MTC_Version == 2)
			{
				ControlPin::SetMeLow(PinGroup_PWM_417118[Pin_TEC_DIR]);
			}
            LPC4078PWM::PWM_MatchUpdate(md,ch,6e4,PWM_MATCH_UPDATE_NEXT_RST);
        }
        if(MTC_CH_Status[i] == 0x10000)
        {
            if(Pid_Info->SampleTemp - Pid_Info->SetTemp > COOLRANGE[i])
            {
				if(Aux_MTC_Base::Aux_MTC_Version == 1)
				{
					ControlPin::SetMeLow(PinGroup_PWM[Pin_TEC_DIR]);
				}
				else if(Aux_MTC_Base::Aux_MTC_Version == 2)
				{
					ControlPin::SetMeLow(PinGroup_PWM_417118[Pin_TEC_DIR]);
				}
                LPC4078PWM::PWM_MatchUpdate(md,ch,6e4,PWM_MATCH_UPDATE_NOW);
            }
            else if((Pid_Info->SampleTemp - Pid_Info->SetTemp > 1) && (Pid_Info->SetTemp < 14))
            {
				if(Aux_MTC_Base::Aux_MTC_Version == 1)
				{
					ControlPin::SetMeLow(PinGroup_PWM[Pin_TEC_DIR]);
				}
				else if(Aux_MTC_Base::Aux_MTC_Version == 2)
				{
					ControlPin::SetMeLow(PinGroup_PWM_417118[Pin_TEC_DIR]);
				}
                LPC4078PWM::PWM_MatchUpdate(md,ch,6e4,PWM_MATCH_UPDATE_NOW);
            }
            else if(Pid_Info->SetTemp - Pid_Info->SampleTemp > HEATRANGE[i] )
            {
                if(Pid_Info->SetTemp > 32.05)
                {
					if(Aux_MTC_Base::Aux_MTC_Version == 1)
					{
						ControlPin::SetMeHigh(PinGroup_PWM[Pin_TEC_DIR]);
					}
					else if(Aux_MTC_Base::Aux_MTC_Version == 2)
					{
						ControlPin::SetMeHigh(PinGroup_PWM_417118[Pin_TEC_DIR]);
					}
                    LPC4078PWM::PWM_MatchUpdate(md,ch,6e4,PWM_MATCH_UPDATE_NOW);
                }
                else
                    PWM_Calculation(pwm_output,i);
            }
            else
                PWM_Calculation(pwm_output,i);
        }
        Aux_MTC_Base::TEC_Phase = GetTemp;
		Aux_MTC_Base::FanLockCheckLocal(index); //Local Fanlock Check. Mu 11042020
				
        if(index == 7)
            index=0;
        else
            index++;
    }
    break;
    }
		
#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count_Time[COUNT_MTC] = CommonFunc::TimeDiff(OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT + 1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
    Time_Count::thread_time(COUNT_MTC);
#endif
}

/******************************************************************************
** Function name:        Check_T_Alert
** Descriptions:         check if there is a temperature alert
** parameters:           None
** Returned value:       None
** Created Date:         2018/04/04
** Revision Dates:
******************************************************************************/
void Aux_MTC_Base::Check_T_Alert(void)
{
    if(((LPC_GPIO1->PIN & 0x0080)) == 0)
    {
//		Aux_Chamber_TEC_Base_8::KEY_FLAG = 0;
//		HT16K33::LED_BLINK();
				if(Aux_MTC_Base::Aux_MTC_Version == 1)
				{
					ControlPin::SetMeLow(PinGroup_PWM[Pin_TEC_ENB_1]);
					ControlPin::SetMeLow(PinGroup_PWM[Pin_TEC_ENB_2]);
					ControlPin::SetMeLow(PinGroup_PWM[Pin_TEC_ENB_3]);
					ControlPin::SetMeLow(PinGroup_PWM[Pin_TEC_ENB_4]);
					ControlPin::SetMeLow(PinGroup_PWM[Pin_TEC_ENB_5]);
					ControlPin::SetMeLow(PinGroup_PWM[Pin_TEC_ENB_6]);
					ControlPin::SetMeLow(PinGroup_PWM[Pin_TEC_ENB_7]);
					ControlPin::SetMeLow(PinGroup_PWM[Pin_TEC_ENB_8]);
				}
				else if(Aux_MTC_Base::Aux_MTC_Version == 2)
				{
					ControlPin::SetMeLow(PinGroup_PWM_417118[Pin_TEC_ENB_1]);
					ControlPin::SetMeLow(PinGroup_PWM_417118[Pin_TEC_ENB_2]);
					ControlPin::SetMeLow(PinGroup_PWM_417118[Pin_TEC_ENB_3]);
					ControlPin::SetMeLow(PinGroup_PWM_417118[Pin_TEC_ENB_4]);
					ControlPin::SetMeLow(PinGroup_PWM_417118[Pin_TEC_ENB_5]);
					ControlPin::SetMeLow(PinGroup_PWM_417118[Pin_TEC_ENB_6]);
					ControlPin::SetMeLow(PinGroup_PWM_417118[Pin_TEC_ENB_7]);
					ControlPin::SetMeLow(PinGroup_PWM_417118[Pin_TEC_ENB_8]);					
				}
//			Aux_Chamber_TEC_Base_8::SET_ERR1();
//			Aux_Chamber_TEC_Base_8::SET_ERR2();
//			ControlPin::SetMeLow(Pin_ERR1,1);
//			ControlPin::SetMeLow(Pin_ERR2,1);
//			HT16K33::DispTemp_DoWork(Temperature);
    }
    else
    {
//		Aux_Chamber_TEC_Base_8::KEY_FLAG = 1;
//		Aux_Chamber_TEC_Base_8::CLR_Temp_Alert();
//		HT16K33::LED_NBLINK();
				if(Aux_MTC_Base::Aux_MTC_Version == 1)
				{
					ControlPin::SetMeHigh(PinGroup_PWM[Pin_TEC_ENB_1]);
					ControlPin::SetMeHigh(PinGroup_PWM[Pin_TEC_ENB_2]);
					ControlPin::SetMeHigh(PinGroup_PWM[Pin_TEC_ENB_3]);
					ControlPin::SetMeHigh(PinGroup_PWM[Pin_TEC_ENB_4]);
					ControlPin::SetMeHigh(PinGroup_PWM[Pin_TEC_ENB_5]);
					ControlPin::SetMeHigh(PinGroup_PWM[Pin_TEC_ENB_6]);
					ControlPin::SetMeHigh(PinGroup_PWM[Pin_TEC_ENB_7]);
					ControlPin::SetMeHigh(PinGroup_PWM[Pin_TEC_ENB_8]);
				}
				else if(Aux_MTC_Base::Aux_MTC_Version == 2)
				{
					ControlPin::SetMeHigh(PinGroup_PWM_417118[Pin_TEC_ENB_1]);
					ControlPin::SetMeHigh(PinGroup_PWM_417118[Pin_TEC_ENB_2]);
					ControlPin::SetMeHigh(PinGroup_PWM_417118[Pin_TEC_ENB_3]);
					ControlPin::SetMeHigh(PinGroup_PWM_417118[Pin_TEC_ENB_4]);
					ControlPin::SetMeHigh(PinGroup_PWM_417118[Pin_TEC_ENB_5]);
					ControlPin::SetMeHigh(PinGroup_PWM_417118[Pin_TEC_ENB_6]);
					ControlPin::SetMeHigh(PinGroup_PWM_417118[Pin_TEC_ENB_7]);
					ControlPin::SetMeHigh(PinGroup_PWM_417118[Pin_TEC_ENB_8]);					
				}
//		Aux_Chamber_TEC_Base_8::CLR_ERR1();
//		Aux_Chamber_TEC_Base_8::CLR_ERR2();
//		ControlPin::SetMeHigh(Pin_ERR1,1);
//		ControlPin::SetMeHigh(Pin_ERR2,1);
    }
}

void Aux_MTC_Base::UpdateAllSetTemp(void)
{
    for (int i = 0; i < MTC_MAX_CHANNELNO; i++)
    {
        DWIN_UartParse::UpdateValue( ( 0x00F0 + 2*i ), Aux_MTC_Base::Temperature_set[i]*10);
    }
}

void Aux_MTC_Base::TemperatureCheck(unsigned char uc_IVunitBufIndex, unsigned char uc_IVch, unsigned char uc_AuxType, unsigned char MTC_ChnIdx)
{
    CHAMBER_TEC_PID_INFO * Pid_Info = &Aux_MTC_Base::MTC_Pid_Info[MTC_ChnIdx];
    if(MTC_CH_Status[MTC_ChnIdx] == 0x00010000)
    {
        if( (Aux_MTC_Base::Temperature[MTC_ChnIdx] <= Pid_Info->SetTemp + 0.5f) &&
                (Aux_MTC_Base::Temperature[MTC_ChnIdx] >= Pid_Info->SetTemp - 0.5f) )
        {
            Aux_MTC_TemperatureSafetyCheckFlag[MTC_ChnIdx] = 0;
        }

        unsigned long long TimeTick;
        MsTime *MTC_TimeBuffer = &MTCSafetyCheckTimeBuffer[MTC_ChnIdx];
        TimeTick = CommonFunc::GetTimeTicks(MTC_TimeBuffer);
        if(TimeTick >= ( (long long)MTCSafetyCheckTimeValue[MTC_ChnIdx] ) * 600000)
        {
            DWIN_UartParse::MTCReportErr(0x0FC0+MTC_ChnIdx*2,0x0002);
            ReportMTCTempUnsafe(uc_IVunitBufIndex, uc_IVch, uc_AuxType, MTC_ChnIdx);
            TurnOffMTCChn(MTC_ChnIdx);
            Aux_MTC_TemperatureSafetyCheckFlag[MTC_ChnIdx] = 0;
            Aux_MTC_TemperatureSafetyReportFlag[MTC_ChnIdx] = 1;
            ErrorReportCount[MTC_ChnIdx] = 32;
        }
    }
    else
    {
        Aux_MTC_Base::MTCSafetyCheckTimeBuffer[MTC_ChnIdx] = CommonFunc::GetSystemTime();//OS_Time;
    }
}

unsigned char Aux_MTC_Base::ReportMTCTempUnsafe(unsigned char uc_IVunitBufIndex, unsigned char uc_IVch, unsigned char uc_AuxType, unsigned char MTC_ChnIdx)
{
    unsigned char TempAuxTypeIdex = Aux_DataManager::Aux_LocateAuxTypeArrayIndex(uc_AuxType);
    if(TempAuxTypeIdex == 0xFF)//辅助限制
        return 1;
    CANBusParse::Aux_RptMTCTempUnsafe(uc_IVunitBufIndex, uc_IVch, MTC_ChnIdx, uc_AuxType, Temperature[MTC_ChnIdx], MTCTempSafetyGoto[MTC_ChnIdx]);
    return 1;
}

void Aux_MTC_Base::TurnOnMTCChn( unsigned char TECChannelNo )
{
//	ControlPin::SetMeHigh(PinGroup_PWM[Pin_FCTL_A1 + 2* TECChannelNo]);
//	ControlPin::SetMeHigh(PinGroup_PWM[Pin_FCTL_B1 + 2* TECChannelNo]);//先开风扇
    if(MTC_CH_Status[TECChannelNo] == 0)
    {
				if(Aux_MTC_Base::Aux_MTC_Version == 1)
					ControlPin::SetMeHigh(PinGroup_PWM[Pin_TEC_ENB_1 + TECChannelNo]);//再开脉冲
				else if(Aux_MTC_Base::Aux_MTC_Version == 2)
					ControlPin::SetMeHigh(PinGroup_PWM_417118[Pin_TEC_ENB_1 + TECChannelNo]);//再开脉冲					
        MTC_CH_Status[TECChannelNo] = 0x10000;//状态标志位设置为0x10000
        DWIN_UartParse::UpdateValue(0x80+TECChannelNo*2,MTC_CH_Status[TECChannelNo]);
				if(Aux_MTC_Base::Aux_MTC_Version == 2)
					ControlPin::SetMeHigh(PinGroup_PWM_417118[Pin_FAN_CTRL_1 + TECChannelNo]);
    }
}

void Aux_MTC_Base::TurnOffMTCChn( unsigned char TECChannelNo )
{
//	ControlPin::SetMeLow(PinGroup_PWM[Pin_FCTL_A1 + 2* TECChannelNo]);
//	ControlPin::SetMeLow(PinGroup_PWM[Pin_FCTL_B1 + 2* TECChannelNo]);//先关风扇
    if(MTC_CH_Status[TECChannelNo] == 0x10000)
    {
				if(Aux_MTC_Base::Aux_MTC_Version == 1)
					ControlPin::SetMeLow(PinGroup_PWM[Pin_TEC_ENB_1 + TECChannelNo]);
				else if(Aux_MTC_Base::Aux_MTC_Version == 2)
					ControlPin::SetMeLow(PinGroup_PWM_417118[Pin_TEC_ENB_1 + TECChannelNo]);					
        MTC_CH_Status[TECChannelNo] = 0;
        DWIN_UartParse::UpdateValue(0x80+TECChannelNo*2,MTC_CH_Status[TECChannelNo]);
				if(Aux_MTC_Base::Aux_MTC_Version == 2)
					ControlPin::SetMeLow(PinGroup_PWM_417118[Pin_FAN_CTRL_1 + TECChannelNo]);
    }
}

void Aux_MTC_Base::FanLockCheckLocal(unsigned char MTC_ChnIdx)
{
// 	static unsigned char Fan_Err_Screen_Iter[MTC_MAX_CHANNELNO];
						PinLevel = 1; 	
	//unsigned int FanFaultFlag=0;
	if(MTC_CH_Status[MTC_ChnIdx] == 0x10000) //If this channel is running or not
	{
//		if(TestInput[MTC_ChnIdx] == 1 || Fan_Fault_All_Flag == 1)
		if(Aux_MTC_Base::Aux_MTC_Version == 1)
		{
			if(ControlPin::ReadMe(PinGroup_PWM[Pin_FAN_LOCK_1+MTC_ChnIdx])== 0)	
					PinLevel = 0;
		}
		else if(Aux_MTC_Base::Aux_MTC_Version == 2)
		{
			if(ControlPin::ReadMe(PinGroup_PWM_417118[Pin_FAN_LOCK_1+MTC_ChnIdx])== 0)
				PinLevel = 0;
		}

		if(PinLevel == 0)
		{
			MTC_Fan_Fault_Counter[MTC_ChnIdx] ++;      //Fan fault detection 12/13/2019
			if(MTC_Fan_Fault_Counter[MTC_ChnIdx] >= 550)
			{
			//	FanFaultFlag=1;
			//ReportMTCFanUnsafe(uc_IVunitBufIndex, uc_IVch, uc_AuxType, MTC_ChnIdx);		//报unsafe
				if(!Fan_Err_Screen_Iter[MTC_ChnIdx]) { Fan_Err_Screen_Iter[MTC_ChnIdx] = 36; }
				DWIN_UartParse::MTCReportErr(0x0FC0+MTC_ChnIdx*2,0x0003);
				TurnOffMTCChn(MTC_ChnIdx);             //Turn off that channel
				ARBUS_DELAY(23000);  //延时大1ms
			}
		}
		else
		{
			if(MTC_Fan_Fault_Counter[MTC_ChnIdx] >= 3)
				MTC_Fan_Fault_Counter[MTC_ChnIdx] = MTC_Fan_Fault_Counter[MTC_ChnIdx] - 3;
			else
				MTC_Fan_Fault_Counter[MTC_ChnIdx] = 0;
		}
	}
	// Report this 36 times to ensure the front screen updates
	if(Fan_Err_Screen_Iter[MTC_ChnIdx] > 1)
	{
		DWIN_UartParse::MTCReportErr(0x0FC0+MTC_ChnIdx*2,0x0003);
		--Fan_Err_Screen_Iter[MTC_ChnIdx];
	}
}

void Aux_MTC_Base::FanLockCheck(unsigned char uc_IVunitBufIndex, unsigned char uc_IVch,unsigned char uc_AuxType, unsigned char MTC_ChnIdx)
{
						PinLevel = 1; 	
	
	if(MTC_CH_Status[MTC_ChnIdx] == 0x10000)  //If this channel is running
	{
//		if(TestInput[MTC_ChnIdx] == 1 || Fan_Fault_All_Flag == 1)
		if(Aux_MTC_Base::Aux_MTC_Version == 1)
		{
			if(ControlPin::ReadMe(PinGroup_PWM[Pin_FAN_LOCK_1+MTC_ChnIdx])== 0)	
					PinLevel = 0;
		}
		else if(Aux_MTC_Base::Aux_MTC_Version == 2)
		{
			if(ControlPin::ReadMe(PinGroup_PWM_417118[Pin_FAN_LOCK_1+MTC_ChnIdx])== 0)
				PinLevel = 0;
		}
			if(PinLevel == 0)
			{

					MTC_Fan_Fault_Counter[MTC_ChnIdx] ++;      //Fan fault detection 12/13/2019
					if(MTC_Fan_Fault_Counter[MTC_ChnIdx] >= 500)
				{
					DWIN_UartParse::MTCReportErr(0x0FC0+MTC_ChnIdx*2,0x0003);
					ReportMTCFanUnsafe(uc_IVunitBufIndex, uc_IVch, uc_AuxType, MTC_ChnIdx);		//报unsafe
					ARBUS_DELAY(6000);  //延时大约0.6ms
					TurnOffMTCChn(MTC_ChnIdx);             //Turn off that channel
					ARBUS_DELAY(23000);  //延时大约1ms
				}
			}
			else
			{
				if(MTC_Fan_Fault_Counter[MTC_ChnIdx] >= 3)
					MTC_Fan_Fault_Counter[MTC_ChnIdx] = MTC_Fan_Fault_Counter[MTC_ChnIdx] - 3;
				else
					MTC_Fan_Fault_Counter[MTC_ChnIdx] = 0;
			}
	}
}

unsigned char Aux_MTC_Base::ReportMTCFanUnsafe(unsigned char uc_IVunitBufIndex, unsigned char uc_IVch, unsigned char uc_AuxType, unsigned char MTC_ChnIdx)
{
    unsigned char TempAuxTypeIdex = Aux_DataManager::Aux_LocateAuxTypeArrayIndex(uc_AuxType);
    if(TempAuxTypeIdex == 0xFF)//辅助限制
        return 1;
    CANBusParse::Aux_RptMTCFanUnsafe(uc_IVunitBufIndex, uc_IVch, MTC_ChnIdx, uc_AuxType);
    return 1;
}

void Aux_MTC_Base::TemperatureLimitCheck( unsigned char TECChannelNo )
{
    if(((Aux_MTC_TemperatureHighLimitCheckFlag[TECChannelNo] == 1)&&
            (Aux_MTC_Base::Temperature[TECChannelNo] >= Aux_MTC_TemperatureHighLimit[TECChannelNo]))||
            ((Aux_MTC_TemperatureLowLimitCheckFlag[TECChannelNo] == 1)&&
             (Aux_MTC_Base::Temperature[TECChannelNo] <= Aux_MTC_TemperatureLowLimit[TECChannelNo]))) {
        DWIN_UartParse::MTCReportErr(0x0FC0+TECChannelNo*2,0x0001);
        TurnOffMTCChn(TECChannelNo);
    }
}


