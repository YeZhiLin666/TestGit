/*****************************************************************************
// ~{3LPrND<~~}      :TIMERS.cpp     ~{VwR*SCSZ6(J1FwS2<~G}6/!#~}
// ~{ND<~ChJv~}      :~{N*~}LPC4078 MCU board~{7~Nq~}
// ~{1`P4HUFZ~}      :2012.07.11
*****************************************************************************/

//~{0Q~}Timer0~{:M~}RTC ~{SP9X6TJ15D~} ~{W*RF6/~}OS_ServeProcess~{@oH%~}

#include "includes.h"

// COMMON     ~{3#SC?b!"9+92:j6(ReND<~<P~}:
#include "../Data_Manipulation/Datatypes.h"
// extern U32 uw32_TestSumTime;

// Private Methods
My_Func_Ptr_Void TIMERSFunc::Timer1Handler;
My_Func_Ptr_Void TIMERSFunc::Timer3Handler;
const unsigned long long TimerBase[] = {LPC_TIM0_BASE, LPC_TIM1_BASE,LPC_TIM2_BASE,LPC_TIM3_BASE};
const unsigned int Timer_PCONP[] = {PCTIM0,PCTIM1,PCTIM2,PCTIM3};
unsigned char m_TrigerTest = 0xff;
void TIMER0_IRQHandler(void)
{   //main timer,for OStime and ADC Sample
    LPC_TIM0->IR = 1;			// clear interrupt flag//

    OS_ServeProcess::Timer0HandlerISR();

    if(LPC_TIM0->TC > LPC_TIM0->MR0)   //20150324,hepeiqing,in case of another timeout
        LPC_TIM0->TC = 0;
}

void TIMER1_IRQHandler(void)
{   //for Internal Resistance Measuring
    LPC_TIM1->IR = 1;			// clear interrupt flag //

#if THREAD_TASK_TIME_STATISTIC == 1
	if(LPC_TIM1->TC > LPC_TIM1->MR0)
        LPC_TIM1->TC = 0;
	timer_overflow_count++;
	#else
    if(TIMERSFunc::Timer1Handler != 0)
        TIMERSFunc::Timer1Handler();
	#endif
}

void TIMER2_IRQHandler(void)
{   //for TCP-NET module
    LPC_TIM2->IR = 1;			// clear interrupt flag //

    timer_tick();
// 	++ uw32_TestSumTime;
}

void TIMER3_IRQHandler(void)
{
    //  ~{6(J1~}/~{<FJ}Fw~}3~{VP6O4&@m#,Mj3I~}Xms~{6(J1~}
    LPC_TIM3->IR = 1;			// clear interrupt flag   //
	
    LPC_TIM3->MR0 = 0xFFFFFF;   //  20200602 hp

#if  USE_AS_AUXMCU == 1
    Aux_DDS_ACIM_ACR_408888_SampleManage::SampleISR();
#endif

    if(TIMERSFunc::Timer3Handler != 0)
        TIMERSFunc::Timer3Handler();

    if(LPC_TIM3->TC > LPC_TIM3->MR0)
        LPC_TIM3->TC = 0;
}


// <<<End>>>Private Methods


//public:

//    My_Func_Ptr_Void TIMERSFunc::Timer1Handler;

//init~{6(J1Fw~}
//input ~{6(J1Fw~}id ~{:M~}	~{6(J1Fw<d8t~} 18M~{N*;y1>5%N;~}
//output false or ture
unsigned char TIMERSFunc::Init_Timer(UWord32 TimerInterval ,UChar8 timer_num)
{
    //~{6(J1Fw3uJ<;/?IQ!~}0~{#,~}1~{#,~}2~{#,~}3~{#,6(J1Fw3uJ<;/~}
    //Initialize timer, set timer interval, reset timer, install timer interrupt handler
    // Returned value:	true or false, if the interrupt handler can't be installed, return false.
    unsigned char uc_success = FALSE;
    //timer_counter0 = 0;
    //timer_counter1 = 0;
    //timer_counter2 = 0;
    //timer_counter3 = 0;
// 	switch(timer_num)
// 	{
// 	case 0:
// 		LPC_SC->PCONP |= PCTIM0;   // Turn On Timer0 Power //
// 		LPC_TIM0->TC = 0;
// 		LPC_TIM0->PC = 0;
// 		LPC_TIM0->PR = 0;
// 		LPC_TIM0->MR0 = TimerInterval; //????~{!@~}????~{!@~}??
// 		LPC_TIM0->MCR = 3;				// Interrupt and Reset on MR0 //
// 		if( IRQFunc::install_irq( TIMER0_IRQn, HIGHEST_PRIORITY + 6) == FALSE )
// 			uc_success = TRUE;
    /////////////////////////////////~{W"Rb~}Timer0~{TZ~}os~{VP~}///////////////////////////////////
// 		break;
// 	case 1:
// 		LPC_SC->PCONP |= PCTIM1;   // Turn On Timer1 Power //
// 		LPC_TIM1->TC = 0;
// 		LPC_TIM1->PC = 0;
// 		LPC_TIM1->PR = 0;
// 		LPC_TIM1->MR0 = TimerInterval; //????~{!@~}????~{!@~}??
// 		LPC_TIM1->MCR = 3;				// Interrupt and Reset on MR0//
// 		if( IRQFunc::install_irq( TIMER1_IRQn, HIGHEST_PRIORITY + 7) == FALSE )
// 			uc_success = TRUE;
// 		break;
// 	case 2:
// 		LPC_SC->PCONP |= PCTIM2;   // Turn On Timer2 Power //
// 		LPC_TIM2->TC = 0;
// 		LPC_TIM2->PC = 0;
// 		LPC_TIM2->PR = 0;
// 		LPC_TIM2->MR0 = TimerInterval; //????~{!@~}????~{!@~}??
// 		LPC_TIM2->MCR = 3;				// Interrupt and Reset on MR0 //
// 		if( IRQFunc::install_irq( TIMER2_IRQn, HIGHEST_PRIORITY + 8) == FALSE )
// 			uc_success = TRUE;
// 		break;
// 	case 3:
// 		LPC_SC->PCONP |= PCTIM3;   // Turn On Timer3 Power //
// 		LPC_TIM3->TC = 0;
// 		LPC_TIM3->PC = 0;
// 		LPC_TIM3->PR = 0;
// 		LPC_TIM3->MR0 = TimerInterval; //????~{!@~}????~{!@~}??
// 		LPC_TIM3->MCR = 3;				// Interrupt and Reset on MR0 //
// 		if ( IRQFunc::install_irq( TIMER3_IRQn, HIGHEST_PRIORITY + 9) == FALSE )
// 			uc_success = TRUE;
// 		break;
// 	default: ;  break;
    if(timer_num<=3)
    {
        LPC_SC->PCONP |= Timer_PCONP[timer_num];
        LPC_TIM_TypeDef * p_tim = (LPC_TIM_TypeDef *)TimerBase[timer_num];
        p_tim->TC = 0;
        p_tim->PC = 0;

#if THREAD_TASK_TIME_STATISTIC == 1
		if(timer_num == 1)
			p_tim->PR = 60;
		else
			p_tim->PR = 0;
		#else
		p_tim->PR = 0;
		#endif

        p_tim->MR0 = TimerInterval; //????~{!@~}????~{!@~}??
        p_tim->MCR = 3;				// Interrupt and Reset on MR0 //
        if ( IRQFunc::install_irq( (IRQn)(TIMER0_IRQn+timer_num), HIGHEST_PRIORITY+6+timer_num) == FALSE )
            uc_success = TRUE;
    }

    //TIMERSFunc::Enable_Timer( timer_num );
// 	if ( i == FALSE )
// 	{
// 		return (FALSE);
    return (uc_success);
}

void TIMERSFunc::Reset_Timer(UChar8 timer_num)
{

    Arbin_LPC_TIM_TypeDef * p_tim = (Arbin_LPC_TIM_TypeDef *)TimerBase[timer_num];

    p_tim->TCR.BitReg.CRST = 1;
    p_tim->TCR.BitReg.CRST = 0;
}

//reset~{6(J1Fw~}
//input ~{6(J1Fw~}id
//output N/A
void TIMERSFunc::Disable_Timer(UChar8 timer_num)
{   //   ~{6(J1Fw84N;?IQ!~}0~{#,~}1~{#,~}2~{#,~}3~{#,6(J1Fw84N;~}
    switch(timer_num)
    {
    case 0:
        Arbin_TIM0->TCR.BitReg.CEN = 0;
        break;
    case 1:
        Arbin_TIM1->TCR.BitReg.CEN = 0;
        break;
    case 2:
        Arbin_TIM2->TCR.BitReg.CEN = 0;
        break;
    case 3:
        Arbin_TIM3->TCR.BitReg.CEN = 0;
        break;
    default:
        ;
        break;
    }
}

//Enable~{6(J1Fw~}
//input ~{6(J1Fw~}id
//output N/A
void TIMERSFunc::Enable_Timer(UChar8 timer_num)
{   //   ~{6(J1FwJ9D\?IQ!~}0~{#,~}1~{#,~}2~{#,~}3~{#,6(J1Fw?*Ft~}
// 	switch(timer_num)
// 	{
// 	case 0:
// 		Arbin_TIM0->TCR.BitReg.CEN  = 1;
// 		Arbin_TIM0->TCR.BitReg.CRST = 0;
// 		break;
// 	case 1:
// 		Arbin_TIM1->TCR.BitReg.CEN  = 1;
// 		Arbin_TIM1->TCR.BitReg.CRST = 0;
// 		break;
// 	case 2:
// 		Arbin_TIM2->TCR.BitReg.CEN  = 1;
// 		Arbin_TIM2->TCR.BitReg.CRST = 0;
// 		break;
// 	case 3:
// 		Arbin_TIM3->TCR.BitReg.CEN  = 1;
// 		Arbin_TIM3->TCR.BitReg.CRST = 0;
// 		break;
// 	default: ;  break;
    Arbin_LPC_TIM_TypeDef * p_tim;
    if(timer_num<2)
    {
        p_tim = (Arbin_LPC_TIM_TypeDef *)(LPC_APB0_BASE + 0x04000 + 0x04000*timer_num);
    }
    else
    {
        unsigned uc_remain = (timer_num%2);
        p_tim = (Arbin_LPC_TIM_TypeDef *)(Arbin_APB1_BASE + 0x10000 + 0x04000*uc_remain);
    }
    p_tim->TCR.BitReg.CEN  = 1;
    p_tim->TCR.BitReg.CRST = 0;
}

void TIMERSFunc::DELAY_SYS( UWord32  delayInMs ,unsigned char uc_EnableMs)
{
    //  ~{QSJ1~}1~{:ACk4sT<N*~}24000~{4N!#QSJ1~}1~{Ck4sT<N*~}1000*24000 ~{4N!#~}
    UWord32 UW_i=0,UW_j=0,UW_k=0;
    UW_i=UW_j;
    // setup timer #1 for delay 1ms  ~{VwF5~}120M  ~{V4PP~}24000 ~{4N~}  //
    // setup timer #1 for delay 1ms  ~{VwF5~}72M   ~{V4PP~}12000 ~{4N~}  //
    if(uc_EnableMs)
    {
        while(UW_k<delayInMs)
        {
            for( UW_i=0; UW_i <= 24000; UW_i++)
            {
                UW_j= UW_i;
            }
            UW_k ++;
        }
    }
    else
    {
        for( UW_i=0; UW_i <= delayInMs; UW_i++)
        {
            UW_j= UW_i;
        }
    }
}

//Timer3~{W(SCSZ~}PULSE~{9&D\~}
unsigned long TIMERSFunc::Timer3_GetTick_NoPre(float TimeInMs) //~{C;SPT$7VF5~}   //~{4+5]2NJ}N*:ACkJ}#,75;X@`PMN*~} ~{U{PM~}tick~{J}~};
{
    double dTime = TimeInMs;

    dTime *= 1000;  //~{:ACk~} ~{W*;;N*~} ~{N"Ck~}
    dTime *= 60;    //~{N"Ck~} ~{W*;;N*~} 1/60~{N"Ck~} ~{N*5%N;5D~}tick~{J}~}(tick~{7"IzF5BJN*~}60MHz)
    dTime --;       //~{<FJ}Fw4S~}0~{?*J<#,PhR*<uR;~}

    unsigned long Tick = dTime;
    return Tick;
}
unsigned long TIMERSFunc::Timer3_GetTick_Pre(float TimeInMs)   //~{SPT$7VF5~}     //~{4+5]2NJ}N*:ACkJ}#,75;X@`PMN*~} ~{U{PM~}tick~{J}~};
{
    double dTime = TimeInMs;

    dTime *= 1000;  //~{TZT$7VF5<D4fFwIhVCN*~}59~{5DG0LaOB#,O`51SZC?8v~}TICK~{7"IzF5BJN*~}1MHz~{#,<4~}1~{N"Ck5]TvR;4N~}
    dTime --;

    unsigned long Tick = dTime;
    return Tick;
}
void TIMERSFunc::Timer3_SetTick_NoPre(unsigned long Tick)      //~{=+T$7VF5VCN*~}0~{#,<FJ}FwVCN*~}Tick
{
    LPC_TIM3->MR0 = Tick;
    LPC_TIM3->PR  = 0;
}
void TIMERSFunc::Timer3_SetTick_Pre(unsigned long Tick)        //~{=+T$7VF5VCN*~}59,~{<FJ}FwVCN*~}Tick
{
    LPC_TIM3->PR  = 59;
    LPC_TIM3->MR0 = Tick;
}

/*********************************************************************************
**                            End Of File
*********************************************************************************/

