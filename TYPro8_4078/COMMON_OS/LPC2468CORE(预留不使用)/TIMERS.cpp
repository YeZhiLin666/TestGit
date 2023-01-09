

//��Timer0��RTC �йض�ʱ�� ת�ƶ�os_2468��ȥ

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

	//init��ʱ��
	//input ��ʱ��id ��	��ʱ����� 18MΪ������λ
	//output false or ture
	UWord32 TIMERSFunc::Init_Timer(UWord32 TimerInterval ,UChar8 timer_num)
	{
		//��ʱ����ʼ����ѡ0��1��2��3����ʱ����ʼ��
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
			T0MR0 = TimerInterval; //ƥ��Ƚ�ʱ��
			T0MCR = 3;				// Interrupt and Reset on MR0 //
			if( IRQFunc::install_irq( TIMER0_INT, (void *)Timer0HandlerISR, HIGHEST_PRIORITY +6) == FALSE )    //VICAddress
				i = TRUE;
			/////////////////////////////////ע��Timer0��os��///////////////////////////////////
			break;
		case 1:  
			T1MR0 = TimerInterval; //ƥ��Ƚ�ʱ��
			T1MCR = 3;				// Interrupt and Reset on MR0//
			if( IRQFunc::install_irq( TIMER1_INT, (void *)Timer1HandlerISR, HIGHEST_PRIORITY+7) == FALSE )    //VICAddress
				i = TRUE;
			break;
		case 2:  
			PCONP |= (1<<22);
			T2MR0 = TimerInterval; //ƥ��Ƚ�ʱ��
			T2MCR = 3;				// Interrupt and Reset on MR0 //
			if( IRQFunc::install_irq( TIMER2_INT, (void *)Timer2HandlerISR,  HIGHEST_PRIORITY+8) == FALSE )    //VICAddress
				i = TRUE;
			break;
		case 3:  
			PCONP |= (1<<23);
			T3MR0 = TimerInterval; //ƥ��Ƚ�ʱ��
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

	//reset��ʱ��
	//input ��ʱ��id
	//output N/A
	void TIMERSFunc::Reset_Timer(UChar8 timer_num)
	{
		//   ��ʱ����λ��ѡ0��1��2��3����ʱ����λ
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

	//Disable��ʱ��
	//input ��ʱ��id
	//output N/A
	void TIMERSFunc::Disable_Timer(UChar8 timer_num)
	{
		//   ��ʱ���رտ�ѡ0��1��2��3����ʱ���ر�
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

	//Enable��ʱ��
	//input ��ʱ��id
	//output N/A
	void TIMERSFunc::Enable_Timer(UChar8 timer_num)
	{
		//   ��ʱ��ʹ�ܿ�ѡ0��1��2��3����ʱ������
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
	    //  ��ʱ1�����ԼΪ24000�Ρ���ʱ1���ԼΪ1000*24000 �Ρ�
	    UWord32 UW_i=0,UW_j=0;
	    UW_i=UW_j;
		// setup timer #1 for delay 1ms  ��Ƶ120M  ִ��24000 ��  //
	    // setup timer #1 for delay 1ms  ��Ƶ72M   ִ��12000 ��  //
	    for( UW_i=0; UW_i <= delayInMs; UW_i++)
		{
	       UW_j= UW_i;
		}
	}

/*********************************************************************************
**                            End Of File
*********************************************************************************/

