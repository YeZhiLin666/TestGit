
#include "includes.h"

bool              Aux_Chamber_TEC_Base::Aux_Chamber_TEC_Board = false;  //  true;  changed by DKQ

UInt16            Aux_Chamber_TEC_Base::KEY_Priority = PRIORITY4;
UInt16            Aux_Chamber_TEC_Base::KEY_TaskID = AUX_CHAMBER_TEC_KEY_TASK;

UInt16            Aux_Chamber_TEC_Base::Chamber_TEC_Priority = PRIORITY4;
UInt16            Aux_Chamber_TEC_Base::Chamber_TEC_TaskID = AUX_CHAMBER_TEC_CONTROL_TASK;

unsigned short Aux_Chamber_TEC_Base::TEC_Phase;
float Aux_Chamber_TEC_Base::Temperature;
float Aux_Chamber_TEC_Base::Temperature1 = 0;
unsigned char Aux_Chamber_TEC_Base::TC_count;
CHAMBER_TEC_PID_INFO Aux_Chamber_TEC_Base::Chamber_Pid_Info;
unsigned char Aux_Chamber_TEC_Base::T_Alert_Flag1 = 0;
unsigned char Aux_Chamber_TEC_Base::T_Alert_Flag2 = 0;
unsigned char Aux_Chamber_TEC_Base::KEY_FLAG = 1;
float Aux_Chamber_TEC_Base::PW_INC_LMT = 0.8;
float Aux_Chamber_TEC_Base::PW_DEC_LMT = 1.1236;
unsigned char Aux_Chamber_TEC_Base::TempUp = 0;
unsigned char Aux_Chamber_TEC_Base::TempRec[3];
unsigned char Aux_Chamber_TEC_Base::COOLRANGE = 5;
unsigned char Aux_Chamber_TEC_Base::HEATRANGE = 5;


const St_Port Pin_TEC_PWM = {2, 3, eCPTYPE_MCUGPIO, PWM1, 0};   //P2.3 TEC PWM Signal
const St_Port Pin_TEC_DIR = {2, 2, eCPTYPE_MCUGPIO, GPIO_O, 1}; //P2.2 PWM Direction
const St_Port Pin_TEC_ENB = {2, 1, eCPTYPE_MCUGPIO, GPIO_O, 0}; //P2.1 PWM Enable
const St_Port Pin_ERR1    = {1, 5, eCPTYPE_MCUGPIO, GPIO_O, 0}; //P1.5 ERR1
const St_Port Pin_ERR2    = {1, 6, eCPTYPE_MCUGPIO, GPIO_O, 0}; //P1.6 ERR2
const St_Port Pin_KEYUP   = {1, 2, eCPTYPE_MCUGPIO, GPIO_I, 0}; //P1.5 ERR1
const St_Port Pin_KEYDN   = {1, 3, eCPTYPE_MCUGPIO, GPIO_I, 0}; //P1.6 ERR2
const St_Port Pin_T_ALERT = {2, 0, eCPTYPE_MCUGPIO, GPIO_I, 0}; //P2.0 T_ALERT

void Aux_Chamber_TEC_Base::KEY_Init(void)
{
    //** KETUP & KEYDN
    ControlPin::SetIObyCP(Pin_KEYUP);
    ControlPin::SetIObyCP(Pin_KEYDN);
    //** T_ALERT
    ControlPin::SetIObyCP(Pin_T_ALERT);
    //**ERR 1 & 2
    ControlPin::SetIObyCP(Pin_ERR1);
    ControlPin::SetMeHigh(Pin_ERR1);
    ControlPin::SetIObyCP(Pin_ERR2);
    ControlPin::SetMeHigh(Pin_ERR2);

    I2CBusNew::I2CInit(I2C0,100);
    I2CBusNew::I2CInit(I2C2,100);
    HT16K33::Init_HT16K33();
    TMP75C_TEC::Init_TMP75C(0); //changed by zc 04.05.2017
    Aux_Chamber_TEC_Base::Temperature = TMP75C_TEC::Get_Temp();
}

/******************************************************************************
                                  TEC_Control
******************************************************************************/

/******************************************************************************
** Function name:        Init_PWM1
** Descriptions:         Initialize PWM1 for TEC
                       - Frequency: 1kHz
                       - PWM1[0] will Reset automatically
                       - PWM1[4] is single edge controlled
                       - P2.1 TEC_ENB
											 - P2.2 TEC_DIR
                       - P2.3 TEC_PWM, PWM1[4]
** parameters:           None
** Returned value:       None
** Created Date:         2016/03/08
** Revision Dates:
******************************************************************************/
void Aux_Chamber_TEC_Base::Init_PWM1(void)
{
    PWM_MATCHCFG_Type  pwm_cofig;
//** Open interruption service
    IRQFunc::install_irq(PWM1_IRQn, HIGHEST_PRIORITY + 15);
//** IO Setting
    ControlPin::SetIObyCP(Pin_TEC_PWM);
    ControlPin::SetIObyCP(Pin_TEC_ENB);
    ControlPin::SetIObyCP(Pin_TEC_DIR);
//** Power on
    LPC4078PWM::CLKPWR_ConfigPPWR(1, ENABLE);
//** Clear all interrupts pending
    LPC_PWM1->IR = 0xFF & ((uint32_t)(0x0000073F));
    LPC_PWM1->TCR = 0x00;
    LPC_PWM1->CTCR = 0x00;
    LPC_PWM1->MCR = 0x00;
    LPC_PWM1->CCR = 0x00;
    LPC_PWM1->PCR = 0x00; //PWM Control Register
    LPC_PWM1->LER = 0x00;
//** Configure PCR (PWM CONTROL REGISTER)
    LPC4078PWM::PWM_ChannelCmd(1,4,ENABLE); //pPwm->PCR |= 0x1000; //PWM[4] output is enabled.
    LPC4078PWM::PWM_ChannelConfig(1,4,PWM_CHANNEL_SINGLE_EDGE);//pPwm->PCR &= (~ 0x0010); // PWM[4] is single edge controlled
//** Configure MR0 and LER
    LPC4078PWM::PWM_MatchUpdate(1,0,6e4,PWM_MATCH_UPDATE_NOW);//pPwm->MR0 = 0xEA60; pPwm->LER = 0x01;
    pwm_cofig.MatchChannel = 0;
//** Configure PWM Match Control Register
    pwm_cofig.IntOnMatch = DISABLE;
//  pwm_cofig.IntOnMatch = ENABLE;
    pwm_cofig.ResetOnMatch = ENABLE;
    pwm_cofig.StopOnMatch = DISABLE;
    LPC4078PWM::PWM_ConfigMatch(1, &pwm_cofig); //pPwm->MCR = 0x03;
//** Configure MR1 and LER
    LPC4078PWM::PWM_MatchUpdate(1,4,0,PWM_MATCH_UPDATE_NOW);//pPwm->MR4 = 0x7530; pPwm->LER = 0x01; //UPDATE NOW, 50% DUTY CYCLE
    pwm_cofig.MatchChannel = 4;
    pwm_cofig.IntOnMatch = DISABLE;
    pwm_cofig.ResetOnMatch = DISABLE;
    pwm_cofig.StopOnMatch = DISABLE;
    LPC4078PWM::PWM_ConfigMatch(1, &pwm_cofig);

    //** Reset Counter
    LPC4078PWM::PWM_ResetCounter(1);
    //** Counter Enable
    LPC4078PWM::PWM_CounterCmd(1,ENABLE);
    //** PWM Enable
    LPC4078PWM::PWM_Cmd(1,ENABLE);
}

/******************************************************************************
** Function name:        Init_PIDPara
** Descriptions:         Read temperature from TMP75C
** parameters:           None
** Returned value:       f_Read_Temp
** Created Date:         2016/03/08
** Revision Dates:
******************************************************************************/
//void Aux_Chamber_TEC_Base::Init_PIDPara(CHAMBER_TEC_PID_INFO* Pid_Info)
void Aux_Chamber_TEC_Base::Init_PIDPara()
{
    CHAMBER_TEC_PID_INFO * Pid_Info = &Aux_Chamber_TEC_Base::Chamber_Pid_Info;
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

/******************************************************************************
** Function name:        Set_TempPoint
** Descriptions:         Set desired value for temperature
** parameters:           Pid_Info
** Returned value:       None
** Created Date:         2016/03/08
** Revision Dates:
******************************************************************************/
void Aux_Chamber_TEC_Base::Set_TempPoint()
{
    CHAMBER_TEC_PID_INFO * Pid_Info = &Aux_Chamber_TEC_Base::Chamber_Pid_Info;
    Pid_Info->SetTemp = TMP_KEYFunc::TempTemp;
    FM25L256Func::writePage(FRAM_TEMP_REC,&TempRec[0],3);
    if(TMP_KEYFunc::TempTemp > Aux_Chamber_TEC_Base::Temperature)
        Aux_Chamber_TEC_Base::TempUp = 1;
    else
        Aux_Chamber_TEC_Base::TempUp = 0;
//** 10~18 degree
    if(TMP_KEYFunc::TempTemp < 18.05)
    {
        COOLRANGE = 1;
        HEATRANGE = 5;
        if(TempUp == 1) // ÉýÎÂ passed
        {
            Pid_Info->Kp = 0.6; //TEC 2nd Generation
            Pid_Info->Ki = 1.5e-5;
            Pid_Info->Kd = 6.0e-6; //New [JFR]
        }
        else //½µÎÂ passed
        {
            Pid_Info->Kp = 0.4; //TEC 2nd Generation
            Pid_Info->Ki = 1.5e-5;
            Pid_Info->Kd = 6.0e-6; //New [JFR]
        }
    }

//** 18~32 degree
    else if(TMP_KEYFunc::TempTemp < 32.05)
    {
        COOLRANGE = 5;
        HEATRANGE = 5;
        if(TempUp == 1) //ÉýÎÂ passed
        {

            Pid_Info->Kp = 0.8; //TEC 2nd Generation
            Pid_Info->Ki = 1.5e-5;
            Pid_Info->Kd = 6.0e-6; //New [JFR]

        }
        else //½µÎÂ passed
        {
            Pid_Info->Kp = 0.6; //TEC 2nd Generation
            Pid_Info->Ki = 1.5e-5;
            Pid_Info->Kd = 6.0e-6; //New [JFR]
        }
    }
//** 32~48 degree
    else if(TMP_KEYFunc::TempTemp < 48.05)//
    {
        COOLRANGE = 5;
        HEATRANGE = 5;
        if(TempUp == 1) //ÉýÎÂ passed
        {
            Pid_Info->Kp = 0.7; //TEC 2nd Generation
            Pid_Info->Ki = 1.5e-5;
            Pid_Info->Kd = 6.0e-6; //New [JFR]
        }
        else //½µÎÂ passed
        {
            Pid_Info->Kp = 0.7; //TEC 2nd Generation
            Pid_Info->Ki = 1.5e-5;
            Pid_Info->Kd = 6.0e-6; //New [JFR]
        }
    }
//** 48~60 degree
    else
    {
        COOLRANGE = 5;
        HEATRANGE = 1;
        if(TempUp == 1) //ÉýÎÂ
        {
            Pid_Info->Kp = 0.65; //TEC 2nd Generation
            Pid_Info->Ki = 1.5e-5;
            Pid_Info->Kd = 6.0e-6; //New [JFR]
        }
        else //½µÎÂ
        {
            Pid_Info->Kp = 0.9; //TEC 2nd Generation
            Pid_Info->Ki = 1.5e-5;
            Pid_Info->Kd = 6.0e-6; //New [JFR]
        }
    }
}

/******************************************************************************
** Function name:        Init_Chamber_TEC
** Descriptions:         Init PWM, PID parameters and enable H bridge
** parameters:           Pid_Info
** Returned value:       None
** Created Date:         2016/03/08
** Revision Dates:
******************************************************************************/
void Aux_Chamber_TEC_Base::Init_Chamber_TEC()
{
    CHAMBER_TEC_PID_INFO * Pid_Info = &Aux_Chamber_TEC_Base::Chamber_Pid_Info;
//	Aux_Chamber_TEC_Base::Init_PWM1();
    Aux_Chamber_TEC_Base::TEC_Phase = GetTemp;
//  Aux_Chamber_TEC_Base::Init_PIDPara(Pid_Info);
//	Pid_Info->SetTemp = 24.0f;
    FM25L256Func::readPage(FRAM_TEMP_REC,&TempRec[0],3);
    Pid_Info->SetTemp = TempRec[0]*10.0+TempRec[1]*1.0+TempRec[2]*0.1;
    if((Pid_Info->SetTemp > 60.0f) || (Pid_Info->SetTemp < 10.0f))
        Pid_Info->SetTemp = 24.0f;
    TMP_KEYFunc::TempTemp = Pid_Info->SetTemp;
    Aux_Chamber_TEC_Base::Set_TempPoint();
    Pid_Info->OutMax = 0.99;
    Pid_Info->OutMin = -0.99;

    ControlPin::SetMeHigh(Pin_TEC_ENB);
}

void Aux_Chamber_TEC_Base::Init(void)
{
    Aux_Chamber_TEC_Base::Init_PWM1();
    Aux_Chamber_TEC_Base::Init_PIDPara();
    Aux_Chamber_TEC_Base::Init_Chamber_TEC();
}

/******************************************************************************
** Function name:        PID_Regulator
** Descriptions:         Traditional PID
** parameters:           Pid_Info
** Returned value:       Pid_Info->Output
** Created Date:         2016/03/08
** Revision Dates:
******************************************************************************/
float Aux_Chamber_TEC_Base::PID_Regulator()
{
    CHAMBER_TEC_PID_INFO * Pid_Info = &Aux_Chamber_TEC_Base::Chamber_Pid_Info;
    Pid_Info->NewErr = Pid_Info->SetTemp - Pid_Info->SampleTemp; // e(k)
    if((Pid_Info->NewErr < 0.05) && (Pid_Info->NewErr > -0.05))
        Pid_Info->NewErr = 0;
    Pid_Info->Up = Pid_Info->NewErr - Pid_Info->LastErr; // Delta_e(k) = e(k) - e(k-1)
//** Delta_u(k) = Kp * Delta_e(k) + Ki * e(k) + Kd * (Delta_e(k) - Delta_e(k-1))
    Pid_Info->OutPreSat = Pid_Info->Kp * Pid_Info->Up + Pid_Info->Ki * Pid_Info->NewErr + Pid_Info->Kd * (Pid_Info->Up - Pid_Info->Up1);
//** If Kd = 0
//	  Pid_Info->OutPreSat = Pid_Info->Kp * Pid_Info->Up + Pid_Info->Ki * Pid_Info->NewErr;

//	if(Pid_Info->OutPreSat > 0.1f)
//		Pid_Info->OutPreSat = 0.1;
//	else if(Pid_Info->OutPreSat < -0.1f)
//		Pid_Info->OutPreSat = -0.1;

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
** parameters:           pwm_output
** Returned value:       None
** Created Date:         2016/03/08
** Revision Dates:
******************************************************************************/
void Aux_Chamber_TEC_Base::PWM_Calculation(float pwm_output)
{
    if(pwm_output >= -0.1f)
    {
        ControlPin::SetMeHigh(Pin_TEC_DIR);
        if(pwm_output <= 0)
            pwm_output = 0;
        LPC4078PWM::PWM_MatchUpdate(1,4,PW_INC_LMT*pwm_output*6e4,PWM_MATCH_UPDATE_NEXT_RST);
    }
    else
    {
        ControlPin::SetMeLow(Pin_TEC_DIR);
        LPC4078PWM::PWM_MatchUpdate(1,4,-PW_DEC_LMT*(pwm_output+0.1)*6e4,PWM_MATCH_UPDATE_NEXT_RST);
    }
}

/******************************************************************************
** Function name:        Chamber_TEC_DoWork
** Descriptions:         Control TEC
** parameters:           Pid_Info
** Returned value:       None
** Created Date:         2016/03/08
** Revision Dates:
******************************************************************************/
void Aux_Chamber_TEC_Base::Chamber_TEC_DoWork()
{
#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count[COUNT_TEC]++;
    CommonFunc::GetTimeTicksUpdate(&OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
	Time_Count::thread_time_init(COUNT_TEC);
#endif	
	
    static float pwm_output;
    CHAMBER_TEC_PID_INFO * Pid_Info = &Aux_Chamber_TEC_Base::Chamber_Pid_Info;

    switch(TEC_Phase)
    {
    case GetTemp:
//			Pid_Info->SampleTemp = ReadTemp(0x90);
        Aux_Chamber_TEC_Base::Check_T_Alert();
        Aux_Chamber_TEC_Base::Temperature = TMP75C_TEC::Get_Temp();
        Pid_Info->SampleTemp = (float)((long)((Temperature + 0.05f) * 10))/10;
        Aux_Chamber_TEC_Base::TEC_Phase = CalPWM;
        break;
    case CalPWM:
        pwm_output = PID_Regulator();
        Aux_Chamber_TEC_Base::TEC_Phase = OutputPWM;
        break;
    case OutputPWM:
        if(Pid_Info->SampleTemp - Pid_Info->SetTemp > COOLRANGE)
        {
            ControlPin::SetMeLow(Pin_TEC_DIR);
            LPC4078PWM::PWM_MatchUpdate(1,4,6e4,PWM_MATCH_UPDATE_NOW);
        }
        else if((Pid_Info->SampleTemp - Pid_Info->SetTemp > 1) && (Pid_Info->SetTemp < 14))
        {
            ControlPin::SetMeLow(Pin_TEC_DIR);
            LPC4078PWM::PWM_MatchUpdate(1,4,6e4,PWM_MATCH_UPDATE_NOW);
        }
        else if(Pid_Info->SetTemp - Pid_Info->SampleTemp > HEATRANGE )
        {
            if(Pid_Info->SetTemp > 32.05)
            {
                ControlPin::SetMeHigh(Pin_TEC_DIR);
                LPC4078PWM::PWM_MatchUpdate(1,4,6e4,PWM_MATCH_UPDATE_NOW);
            }
            else
                PWM_Calculation(pwm_output);
        }
        else
            PWM_Calculation(pwm_output);
        Aux_Chamber_TEC_Base::TEC_Phase = GetTemp;
        break;
    }
		
#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count_Time[COUNT_TEC] = CommonFunc::TimeDiff(OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
	Time_Count::thread_time(COUNT_TEC);
#endif
}


//void Aux_Chamber_TEC_Base::DoWork(void)
//{
//	Aux_Chamber_TEC_Base::Chamber_TEC_DoWork(&Chamber_Pid_Info);
//}


void Aux_Chamber_TEC_Base::KEY_DoWork(void)
{
    unsigned int digit_i;
    Aux_Chamber_TEC_Base::Update_Temp();
    if(Aux_Chamber_TEC_Base::KEY_FLAG == 1)
    {
        if( TMP_KEYFunc::KEYUP() | TMP_KEYFunc::KEYDN() )
        {
            TMP_KEYFunc::Display_count = 900;
        }
        if(TMP_KEYFunc::Display_count > 1)
        {
            HT16K33::DispTemp(TMP_KEYFunc::TempTemp);
            TMP_KEYFunc::Display_count--;
        }
        else if(TMP_KEYFunc::Display_count == 1)
        {
//			HT16K33::DispTemp(Temperature1);
            for(digit_i = 0; digit_i < 3; digit_i++)
            {
                TempRec[digit_i] = HT16K33::Digit[digit_i];
            }
            Aux_Chamber_TEC_Base::Set_TempPoint();
            TMP_KEYFunc::Display_count--;
        }
        else if(TMP_KEYFunc::Display_count == 0)
            HT16K33::DispTemp(Temperature1);
    }
    else
    {
        HT16K33::DispTemp(Temperature1);
        Aux_Chamber_TEC_Base::TEC_Phase = GetTemp;
    }
}

//void Aux_Chamber_TEC_Base::Check_T_Alert(void)
//{
//	if(((LPC_GPIO2->PIN & 0x0001)) == 0)
//		Aux_Chamber_TEC_Base::T_Alert_Flag2 = 1;
//	else
//		Aux_Chamber_TEC_Base::T_Alert_Flag2 = 0;
//
//	if(Aux_Chamber_TEC_Base::T_Alert_Flag1 != Aux_Chamber_TEC_Base::T_Alert_Flag2)
//	{
//		if(Aux_Chamber_TEC_Base::T_Alert_Flag2 == 1)
//		{
//			Aux_Chamber_TEC_Base::KEY_FLAG = 0;
////			Aux_Chamber_TEC_Base::Temp_Alert();
//			HT16K33::LED_BLINK();
//			ControlPin::SetMeLow(Pin_TEC_ENB,1);
////			Aux_Chamber_TEC_Base::SET_ERR1();
////			Aux_Chamber_TEC_Base::SET_ERR2();
////			ControlPin::SetMeLow(Pin_ERR1,1);
////			ControlPin::SetMeLow(Pin_ERR2,1);
////			HT16K33::DispTemp_DoWork(Temperature);
//		}
//		else
//		{
//			Aux_Chamber_TEC_Base::KEY_FLAG = 1;
////			Aux_Chamber_TEC_Base::CLR_Temp_Alert();
//			HT16K33::LED_NBLINK();
//			ControlPin::SetMeHigh(Pin_TEC_ENB,1);
////			Aux_Chamber_TEC_Base::CLR_ERR1();
////			Aux_Chamber_TEC_Base::CLR_ERR2();
////			ControlPin::SetMeHigh(Pin_ERR1,1);
////			ControlPin::SetMeHigh(Pin_ERR2,1);
//		}
//	}
//	Aux_Chamber_TEC_Base::T_Alert_Flag1 = Aux_Chamber_TEC_Base::T_Alert_Flag2;
//}

void Aux_Chamber_TEC_Base::Check_T_Alert(void)
{
    if(((LPC_GPIO2->PIN & 0x0001)) == 0)
    {
        Aux_Chamber_TEC_Base::KEY_FLAG = 0;
        HT16K33::LED_BLINK();
        ControlPin::SetMeLow(Pin_TEC_ENB);
//			Aux_Chamber_TEC_Base::SET_ERR1();
//			Aux_Chamber_TEC_Base::SET_ERR2();
//			ControlPin::SetMeLow(Pin_ERR1,1);
//			ControlPin::SetMeLow(Pin_ERR2,1);
//			HT16K33::DispTemp_DoWork(Temperature);
    }
    else
    {
        Aux_Chamber_TEC_Base::KEY_FLAG = 1;
//		Aux_Chamber_TEC_Base::CLR_Temp_Alert();
        HT16K33::LED_NBLINK();
        ControlPin::SetMeHigh(Pin_TEC_ENB);
//		Aux_Chamber_TEC_Base::CLR_ERR1();
//		Aux_Chamber_TEC_Base::CLR_ERR2();
//		ControlPin::SetMeHigh(Pin_ERR1,1);
//		ControlPin::SetMeHigh(Pin_ERR2,1);
    }
}

void Aux_Chamber_TEC_Base::Update_Temp(void)
{
    if(Aux_Chamber_TEC_Base::Temperature != Aux_Chamber_TEC_Base::Temperature1)
        Aux_Chamber_TEC_Base::TC_count++;
    else
        Aux_Chamber_TEC_Base::TC_count = 0;
    if(Aux_Chamber_TEC_Base::TC_count >= 200)
    {
        Aux_Chamber_TEC_Base::Temperature1 = Aux_Chamber_TEC_Base::Temperature;
        Aux_Chamber_TEC_Base::TC_count = 0;
    }
}

//void Aux_Chamber_TEC_Base::SET_ERR1(void)
//{
//	ControlPin::SetMeLow(Pin_ERR1,1);
//}
//void Aux_Chamber_TEC_Base::SET_ERR2(void)
//{
//	ControlPin::SetMeLow(Pin_ERR2,1);
//}
//void Aux_Chamber_TEC_Base::CLR_ERR1(void)
//{
//	ControlPin::SetMeHigh(Pin_ERR1,1);
//}
//void Aux_Chamber_TEC_Base::CLR_ERR2(void)
//{
//	ControlPin::SetMeHigh(Pin_ERR2,1);
//}

