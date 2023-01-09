

//把Timer0和RTC 有关对时的 转移动os_2468里去

#include "../../COMMON/includes.h"

// Private Methods

	void Timer0HandlerISR(void) __irq//timer0 ISR
	{
		T0IR = 1;	
        OS_ServeProcess::Timer0HandlerISR();
		VICVectAddr = 0;		// Acknowledge Interrupt //
	}
	
	void Timer1HandlerISR(void) __irq//timer1 ISR
	{
		T1IR = 1;		
		unsigned char IR_ChannelNo=IR::IRChannelNo;
		IR::ProcessIRTimer1(IR_ChannelNo);


		VICVectAddr = 0;		// Acknowledge Interrupt //
	}


	void Timer2HandlerISR(void) __irq//timer2 ISR
	{
		T2IR = 1;	
	    timer_tick();
		VICVectAddr = 0;		// Acknowledge Interrupt //
	}


	void Timer3HandlerISR(void) __irq//timer3 ISR
	{
   		T3IR = 1;	

		VICVectAddr = 0;		// Acknowledge Interrupt //
	}


// <<<End>>>Private Methods


//public:

	//init定时器
	//input 定时器id 和	定时器间隔 18M为基本单位
	//output false or ture
	UWord32 TIMERSFunc::Init_Timer(UWord32 TimerInterval ,UChar8 timer_num)
	{
		//定时器初始化可选0，1，2，3，定时器初始化
		//Initialize timer, set timer interval, reset timer, install timer interrupt handler
		// Returned value:	true or false, if the interrupt handler can't be installed, return false.
		UWord32 i = FALSE;
		//timer_counter0 = 0;
		//timer_counter1 = 0;
		//timer_counter2 = 0;
		//timer_counter3 = 0;
		switch(timer_num)
		{
		case 0: 
			T0MR0 = TimerInterval; //匹配比较时间
			T0MCR = 3;				// Interrupt and Reset on MR0 //
			if( IRQFunc::install_irq( TIMER0_INT, (void *)Timer0HandlerISR, HIGHEST_PRIORITY +6) == FALSE )    //VICAddress
				i = TRUE;
			/////////////////////////////////注意Timer0在os中///////////////////////////////////
			break;
		case 1:  
			T1MR0 = TimerInterval; //匹配比较时间
			T1MCR = 3;				// Interrupt and Reset on MR0//
			if( IRQFunc::install_irq( TIMER1_INT, (void *)Timer1HandlerISR, HIGHEST_PRIORITY+7) == FALSE )    //VICAddress
				i = TRUE;
			break;
		case 2:  
			PCONP |= (1<<22);
			T2MR0 = TimerInterval; //匹配比较时间
			T2MCR = 3;				// Interrupt and Reset on MR0 //
			if( IRQFunc::install_irq( TIMER2_INT, (void *)Timer2HandlerISR,  HIGHEST_PRIORITY+8) == FALSE )    //VICAddress
				i = TRUE;
			break;
		case 3:  
			PCONP |= (1<<23);
			T3MR0 = TimerInterval; //匹配比较时间
			T3MCR = 3;				// Interrupt and Reset on MR0 //
			if ( IRQFunc::install_irq( TIMER3_INT, (void *)Timer3HandlerISR,  HIGHEST_PRIORITY+9) == FALSE )    //VICAddress
				i = TRUE;
			break;
		default: ;  break;
		}

		//timer_2468::Enable_Timer( timer_num );   
		if ( i == FALSE )
		{
			return (FALSE);
		}
		else
		{
			return (TRUE);
		}
	}

	//reset定时器
	//input 定时器id
	//output N/A
	void TIMERSFunc::Reset_Timer(UChar8 timer_num)
	{
		//   定时器复位可选0，1，2，3，定时器复位
		UWord32 regVal = 0;

		switch(timer_num)
		{
		case 0:  regVal = T0TCR; 
			T0TCR = regVal | 0x02;  break;
		case 1:  regVal = T1TCR; 
			T1TCR = regVal | 0x02;  break;
		case 2:  regVal = T2TCR;
			T2TCR = regVal | 0x02;  break;
		case 3:  regVal = T3TCR; 
			T3TCR = regVal | 0x02; break;
		default: ;  break;
		}
		//return;
	}

	//Disable定时器
	//input 定时器id
	//output N/A
	void TIMERSFunc::Disable_Timer(UChar8 timer_num)
	{
		//   定时器关闭可选0，1，2，3，定时器关闭
		switch(timer_num)
		{
		case 0: T0TCR = 0;  break;
		case 1: T1TCR = 0;  break;
		case 2: T2TCR = 0;  break;
		case 3: T3TCR = 0;  break;
		default: ;  break;
		}
		// return;
	}

	//Enable定时器
	//input 定时器id
	//output N/A
	void TIMERSFunc::Enable_Timer(UChar8 timer_num)
	{
		//   定时器使能可选0，1，2，3，定时器开启
		switch(timer_num)
		{
		case 0: T0TCR = 1;  break;
		case 1: T1TCR = 1;  break;
		case 2: T2TCR = 1;  break;
		case 3: T3TCR = 1;  break;
		default: ;  break;
		}
		//return;
	}

	void TIMERSFunc::DELAY_SYS( UWord32  delayInMs )
	{
	    //  延时1毫秒大约为24000次。延时1秒大约为1000*24000 次。
	    UWord32 UW_i=0,UW_j=0;
	    UW_i=UW_j;
		// setup timer #1 for delay 1ms  主频120M  执行24000 次  //
	    // setup timer #1 for delay 1ms  主频72M   执行12000 次  //
	    for( UW_i=0; UW_i <= delayInMs; UW_i++)
		{
	       UW_j= UW_i;
		}
	}

/*********************************************************************************
**                            End Of File
*********************************************************************************/

