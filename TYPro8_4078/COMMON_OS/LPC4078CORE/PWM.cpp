#include "includes.h"
void LPC4078PWM::PWM0_IRQHandler(void)
{
    //int pwmId = PWM_HD0;
    //LPC_PWM_TypeDef* pPwm = (pwmId== PWM_HD0)? LPC_PWM0: LPC_PWM1;
    LPC_PWM_TypeDef* pPwm =LPC_PWM0;
    if(pPwm->IR & 0x01)
    {
        pPwm->IR |= 0x1;     //MR0 Reset
    }
    else if(pPwm->IR & (0x01<<1))
    {
        pPwm->IR |= (0x01<<1);//MR1 Reset
    }
}
void LPC4078PWM::PWM1_IRQHandler(void)
{
    //int pwmId = PWM_HD0;
    //LPC_PWM_TypeDef* pPwm = (pwmId== PWM_HD0)? LPC_PWM0: LPC_PWM1;
    LPC_PWM_TypeDef* pPwm =LPC_PWM1;
    if(pPwm->IR & 0x01)
    {
        pPwm->IR |= 0x1;     //MR0 Reset
    }
    else if(pPwm->IR & (0x01<<1))
    {
        pPwm->IR |= (0x01<<1);//MR1 Reset
    }
}
//add by junjie 20140718
void LPC4078PWM::Init_MyPWM(unsigned char pwmId, unsigned char PWM_Channel, PWM_TIMERCFG_Type pwm_timertype,PWM_MATCH_VALUE pwm_matchvalue)
{
    //	LPC_IOCON->P3_17 |= 2;   //2401 MCU上用的 放到Base里面初始化

    if(pwmId == PWM_HD0)
    {
        IRQFunc::install_irq( PWM0_IRQn, HIGHEST_PRIORITY + 14);
    }
    else if(pwmId == PWM_HD1)
    {
        IRQFunc::install_irq( PWM1_IRQn, HIGHEST_PRIORITY + 15);
    }

    PWM_Init(pwmId,PWM_MODE_TIMER,(void*)&pwm_timertype);
    PWM_ResetCounter(pwmId);
    // MR0
    PWM_MatchUpdate(pwmId,0,pwm_matchvalue.MR0_MatchValue,1);
    PWM_MATCHCFG_Type  pwm_cofig;
    pwm_cofig.MatchChannel = 0;
    pwm_cofig.IntOnMatch = DISABLE;  //中断
    pwm_cofig.ResetOnMatch = ENABLE;
    pwm_cofig.StopOnMatch = DISABLE;
    PWM_ConfigMatch(pwmId, &pwm_cofig); //MR0匹配时产生中断和复位
    // MR1
    PWM_MatchUpdate(pwmId,PWM_Channel,pwm_matchvalue.MR1_MatchValue,1);
    pwm_cofig.MatchChannel = PWM_Channel;
    pwm_cofig.IntOnMatch = DISABLE;
    pwm_cofig.ResetOnMatch = DISABLE;
    pwm_cofig.StopOnMatch = DISABLE;
    PWM_ConfigMatch(pwmId, &pwm_cofig);	//MR1匹配时产生中断

    PWM_ChannelCmd(pwmId,PWM_Channel,ENABLE);  //pwm 通道使能
    PWM_Cmd(pwmId,ENABLE);//pwm 使能
    PWM_CounterCmd(pwmId,ENABLE);

}
//测试修改PWM周期
// void LPC4078PWM::ChangePWMPeriod(unsigned char pwmId, unsigned char PWM_Channel,unsigned long MR0_MatchValue,unsigned long MR1_MatchValue)
// {
// 	// 		static int cnt = 0, duty = 100;
// 	// 		cnt++;
// 	// 		if(cnt%10000 == 0)
// 	// 		{
// 	// 			if(cnt >= 10000*30)
// 	// 			{
// 	// 				cnt = 0;
// 	// 				duty = 10;
// 	// 			}
// 	// 			duty =(int)duty*1.1f;
// 	// 		  LPC4078PWM::PWM_MatchUpdate(0,0,duty,1);
// 	// 		  LPC4078PWM::PWM_MatchUpdate(0,2,duty/2,1);
// 	// 		}
// 	LPC4078PWM::PWM_MatchUpdate(pwmId,0,MR0_MatchValue,1);
// 	LPC4078PWM::PWM_MatchUpdate(pwmId,PWM_Channel,MR1_MatchValue,1);
// }

// void LPC4078PWM::PWM_PinConfig(unsigned char pwmId, unsigned char PWM_Channel, unsigned char PinselOption)
// {
//
// }
// unsigned int LPC4078PWM::PWM_GetIntStatus(unsigned char pwmId, unsigned long IntFlag)
// {
// 	LPC_PWM_TypeDef* pPwm = (pwmId== PWM_HD0)? LPC_PWM0: LPC_PWM1;

// 	return ((pPwm->IR & IntFlag) ? SET : RESET);
// }
// void LPC4078PWM::PWM_ClearIntPending(unsigned char pwmId, unsigned long IntFlag)
// {
// 	LPC_PWM_TypeDef* pPwm =(pwmId== PWM_HD0)? LPC_PWM0: LPC_PWM1;

// 	pPwm->IR = IntFlag;
// }
// void LPC4078PWM::PWM_ConfigStructInit(unsigned char PWMTimerCounterMode, void *PWM_InitStruct)
// {
// 	PWM_TIMERCFG_Type *pTimeCfg;
// 	PWM_COUNTERCFG_Type *pCounterCfg;

// 	pTimeCfg = (PWM_TIMERCFG_Type *) PWM_InitStruct;
// 	pCounterCfg = (PWM_COUNTERCFG_Type *) PWM_InitStruct;

// 	if (PWMTimerCounterMode == PWM_MODE_TIMER )
// 	{
// 		pTimeCfg->PrescaleOption = PWM_TIMER_PRESCALE_USVAL;
// 		pTimeCfg->PrescaleValue = 1;
// 	}
// 	else if (PWMTimerCounterMode == PWM_MODE_COUNTER)
// 	{
// 		pCounterCfg->CountInputSelect = PWM_COUNTER_PCAP1_0;
// 		pCounterCfg->CounterOption = PWM_COUNTER_RISING;
// 	}
// }
void LPC4078PWM::PWM_Init(unsigned char pwmId, unsigned long PWMTimerCounterMode, void *PWM_ConfigStruct)
{
    LPC_PWM_TypeDef* pPwm = (pwmId== PWM_HD0)? LPC_PWM0: LPC_PWM1;

    PWM_TIMERCFG_Type *pTimeCfg;
    PWM_COUNTERCFG_Type *pCounterCfg;
    uint64_t clkdlycnt;

    pTimeCfg = (PWM_TIMERCFG_Type *)PWM_ConfigStruct;
    pCounterCfg = (PWM_COUNTERCFG_Type *)PWM_ConfigStruct;

    if(pwmId == PWM_HD0)
    {
        CLKPWR_ConfigPPWR (PWM_HD0, ENABLE);
    }
    else if(pwmId == PWM_HD1)
    {
        CLKPWR_ConfigPPWR (PWM_HD1, ENABLE);
    }
// 	else
// 	{
// 		//Trap the error
// 		while(1);
// 		//return;
// 	}

    // Get peripheral clock of PWM1
    clkdlycnt = 1;

    // Clear all interrupts pending
    pPwm->IR = 0xFF & ((uint32_t)(0x0000073F));
    pPwm->TCR = 0x00;
    pPwm->CTCR = 0x00;
    pPwm->MCR = 0x00;
    pPwm->CCR = 0x00;
    pPwm->PCR = 0x00;
    pPwm->LER = 0x00;

    if (PWMTimerCounterMode == PWM_MODE_TIMER)
    {
        /* Absolute prescale value */
        if (pTimeCfg->PrescaleOption == PWM_TIMER_PRESCALE_TICKVAL)
        {
            pPwm->PR   = pTimeCfg->PrescaleValue - 1;
        }
        /* uSecond prescale value */
        else
        {
            clkdlycnt = (clkdlycnt * pTimeCfg->PrescaleValue) / 1000000;
            pPwm->PR = ((uint32_t) clkdlycnt) - 1;
        }

    }
    else if (PWMTimerCounterMode == PWM_MODE_COUNTER)
    {

        pPwm->CTCR |= (0x03&pCounterCfg->CounterOption) \
                      | ((0x03&pCounterCfg->CountInputSelect)<<2);
    }
}
// void LPC4078PWM::PWM_DeInit (unsigned char pwmId)
// {
// 	LPC_PWM_TypeDef* pPwm = (pwmId== PWM_HD0)? LPC_PWM0: LPC_PWM1;

// 	// Disable PWM control (timer, counter and PWM)
// 	pPwm->TCR = 0x00;

// 	CLKPWR_ConfigPPWR (((unsigned long)(1<<6)), DISABLE);
// }
void LPC4078PWM::PWM_Cmd(unsigned char pwmId, unsigned char NewState)
{
    LPC_PWM_TypeDef* pPwm = (pwmId== PWM_HD0)? LPC_PWM0: LPC_PWM1;

    if (NewState == ENABLE)
    {
        pPwm->TCR	|=  ((unsigned long)(1<<3));
    }
    else
    {
        pPwm->TCR &= (~((unsigned long)(1<<3))) & ((unsigned long)(0x0000000B));
    }
}
void LPC4078PWM::PWM_CounterCmd(unsigned char pwmId, unsigned char NewState)
{
    LPC_PWM_TypeDef* pPwm = (pwmId== PWM_HD0)? LPC_PWM0: LPC_PWM1;

    if (NewState == ENABLE)
    {
        pPwm->TCR	|=  ((unsigned long)(1<<0));
    }
    else
    {
        pPwm->TCR &= (~((unsigned long)(1<<0))) & ((unsigned long)(0x0000000B));
    }
}
void LPC4078PWM::PWM_ResetCounter(unsigned char pwmId)
{
    LPC_PWM_TypeDef* pPwm = (pwmId== PWM_HD0)? LPC_PWM0: LPC_PWM1;

    pPwm->TCR |= ((unsigned long)(1<<1));
    pPwm->TCR &= ~(0x1<<1);
    //pPwm->TCR &= (~((unsigned long)(1<<0))) & ((unsigned long)(0x0000000B));
}
void LPC4078PWM::PWM_ConfigMatch(unsigned char pwmId, PWM_MATCHCFG_Type *PWM_MatchConfigStruct)
{
    // 配置匹配通道
    LPC_PWM_TypeDef* pPwm = (pwmId== PWM_HD0)? LPC_PWM0: LPC_PWM1;

    //interrupt on MRn
    if (PWM_MatchConfigStruct->IntOnMatch == ENABLE)
    {
        pPwm->MCR |= PWM_MCR_INT_ON_MATCH(PWM_MatchConfigStruct->MatchChannel);
    }
    else
    {
        pPwm->MCR &= (~ PWM_MCR_INT_ON_MATCH(PWM_MatchConfigStruct->MatchChannel)) \
                     & PWM_MCR_BITMASK;
    }

    //reset on MRn
    if (PWM_MatchConfigStruct->ResetOnMatch == ENABLE)
    {
        pPwm->MCR |= PWM_MCR_RESET_ON_MATCH(PWM_MatchConfigStruct->MatchChannel);
    }
    else
    {
        pPwm->MCR &= (~ PWM_MCR_RESET_ON_MATCH(PWM_MatchConfigStruct->MatchChannel)) \
                     & PWM_MCR_BITMASK;
    }

    //stop on MRn
    if (PWM_MatchConfigStruct->StopOnMatch == ENABLE)
    {
        pPwm->MCR |= PWM_MCR_STOP_ON_MATCH(PWM_MatchConfigStruct->MatchChannel);
    }
    else
    {
        pPwm->MCR &= (~ PWM_MCR_STOP_ON_MATCH(PWM_MatchConfigStruct->MatchChannel)) \
                     & PWM_MCR_BITMASK;
    }
}
// void LPC4078PWM::PWM_ConfigCapture(unsigned char pwmId, PWM_CAPTURECFG_Type *PWM_CaptureConfigStruct)
// {
// 配置捕获通道
// 	LPC_PWM_TypeDef* pPwm = (pwmId== PWM_HD0)? LPC_PWM0: LPC_PWM1;

// 	if (PWM_CaptureConfigStruct->RisingEdge == ENABLE)
// 	{
// 		pPwm->CCR |= PWM_CCR_CAP_RISING(PWM_CaptureConfigStruct->CaptureChannel);
// 	}
// 	else
// 	{
// 		pPwm->CCR &= (~ PWM_CCR_CAP_RISING(PWM_CaptureConfigStruct->CaptureChannel)) \
// 																		& PWM_CCR_BITMASK;
// 	}

// 	if (PWM_CaptureConfigStruct->FallingEdge == ENABLE)
// 	{
// 		pPwm->CCR |= PWM_CCR_CAP_FALLING(PWM_CaptureConfigStruct->CaptureChannel);
// 	}
// 	else
// 	{
// 		pPwm->CCR &= (~ PWM_CCR_CAP_FALLING(PWM_CaptureConfigStruct->CaptureChannel)) \
// 																		& PWM_CCR_BITMASK;
// 	}

// 	if (PWM_CaptureConfigStruct->IntOnCaption == ENABLE)
// 	{
// 		pPwm->CCR |= PWM_CCR_INT_ON_CAP(PWM_CaptureConfigStruct->CaptureChannel);
// 	}
// 	else
// 	{
// 		pPwm->CCR &= (~ PWM_CCR_INT_ON_CAP(PWM_CaptureConfigStruct->CaptureChannel)) \
// 																		& PWM_CCR_BITMASK;
// 	}
// }
// unsigned long LPC4078PWM::PWM_GetCaptureValue(unsigned char pwmId, unsigned char CaptureChannel)
// {
// 	LPC_PWM_TypeDef* pPwm = (pwmId== PWM_HD0)? LPC_PWM0: LPC_PWM1;

// 	switch (CaptureChannel)
// 	{
// 		case 0:
// 			return pPwm->CR0;

// 		case 1:
// 			return pPwm->CR1;

// 		default:
// 			return (0);
// 	}
// }
void LPC4078PWM::PWM_MatchUpdate(unsigned char pwmId, unsigned char MatchChannel, unsigned long MatchValue, unsigned char UpdateType)
{
    LPC_PWM_TypeDef* pPwm = (pwmId== PWM_HD0)? LPC_PWM0: LPC_PWM1;

    switch (MatchChannel)
    {
    case 0:
        pPwm->MR0 = MatchValue;
        break;

    case 1:
        pPwm->MR1 = MatchValue;
        break;

    case 2:
        pPwm->MR2 = MatchValue;
        break;

    case 3:
        pPwm->MR3 = MatchValue;
        break;

    case 4:
        pPwm->MR4 = MatchValue;
        break;

    case 5:
        pPwm->MR5 = MatchValue;
        break;

    case 6:
        pPwm->MR6 = MatchValue;
        break;
    }

    // Write Latch register
    pPwm->LER |= PWM_LER_EN_MATCHn_LATCH(MatchChannel);

    // In case of update now
    if (UpdateType == PWM_MATCH_UPDATE_NOW)
    {
        pPwm->TCR |= PWM_TCR_COUNTER_RESET;
        pPwm->TCR &= (~PWM_TCR_COUNTER_RESET) & PWM_TCR_BITMASK;
    }
}
void LPC4078PWM::PWM_ChannelConfig(unsigned char pwmId, unsigned char PWMChannel, unsigned char ModeOption)
{
    LPC_PWM_TypeDef* pPwm = (pwmId== PWM_HD0)? LPC_PWM0: LPC_PWM1;

    // Single edge mode
    if (ModeOption == PWM_CHANNEL_SINGLE_EDGE)
    {
        pPwm->PCR &= (~ PWM_PCR_PWMSELn(PWMChannel)) & PWM_PCR_BITMASK;
    }
    // Double edge mode
    else if (PWM_CHANNEL_DUAL_EDGE)
    {
        pPwm->PCR |= PWM_PCR_PWMSELn(PWMChannel);
    }
}
void LPC4078PWM::PWM_ChannelCmd(unsigned char pwmId, unsigned char PWMChannel, unsigned char NewState)
{
    LPC_PWM_TypeDef* pPwm = (pwmId== PWM_HD0)? LPC_PWM0: LPC_PWM1;

    if (NewState == ENABLE)
    {
        pPwm->PCR |= PWM_PCR_PWMENAn(PWMChannel);
    }
    else
    {
        pPwm->PCR &= (~ PWM_PCR_PWMENAn(PWMChannel)) & PWM_PCR_BITMASK;
    }
}
LPC_PWM_TypeDef* LPC4078PWM::PWM_GetPointer (unsigned char pwmId)
{
    LPC_PWM_TypeDef* pPwm;

    switch (pwmId)
    {
    case PWM_HD0:
        pPwm = LPC_PWM0;
        break;

    case PWM_HD1:
        pPwm = LPC_PWM1;
        break;

    default:
        pPwm = NULL;
        break;
    }

    return pPwm;
}

void LPC4078PWM::CLKPWR_ConfigPPWR (unsigned char pwmID, unsigned char NewState)
{
    if (NewState == ENABLE)
    {
        if(pwmID == PWM_HD0)
            LPC_SC->PCONP |= PCPWM0;
        if(pwmID == PWM_HD1)
            LPC_SC->PCONP |= PCPWM1;

    }
    else if (NewState == DISABLE)
    {
        if(pwmID == PWM_HD0)
            LPC_SC->PCONP &= ~PCPWM0;
        if(pwmID == PWM_HD1)
            LPC_SC->PCONP &= ~PCPWM1;
    }
}
void LPC4078PWM::SetPWM(unsigned char PWM_HDX,unsigned char PWM_Channel,unsigned long MR0_MatchValue,unsigned long MR1_MatchValue,unsigned short Frequency)
{
    ///////风扇调速  初始化时设定为60%
    // 可选择使用普通脉宽调制模块HD0或者HD1.
    PWM_TIMERCFG_Type pwm_timertype;
    PWM_MATCH_VALUE pwm_matchvalue;
    pwm_timertype.PrescaleOption = PWM_TIMER_PRESCALE_TICKVAL;
    pwm_timertype.PrescaleValue = 600000/Frequency;
    pwm_matchvalue.MR0_MatchValue=MR0_MatchValue;
    pwm_matchvalue.MR1_MatchValue=MR1_MatchValue;    //0～100之间选择任意值(整数）
    //LPC4078PWM::Init_MyPWM(PWM_HD1,MY_PWMFANCHANNEL,pwm_timertype,pwm_matchvalue); //根据实际条件配置PWM参数（单边） dirui  20140723
    LPC4078PWM::Init_MyPWM(PWM_HDX,PWM_Channel,pwm_timertype,pwm_matchvalue);
}

// void LPC4078PWM::SetPWM2(unsigned char PwmChIdx, unsigned long Frequency, float fDutyCycle)
// {//MaxFrequency == 1MHz,    MaxDutyCycle == 100%,
// 	// 固定使用普通脉宽调制模块1.
// 	unsigned long SumValue,DutyValue;

// 	SumValue = 10000000 / Frequency;
// 	DutyValue = SumValue * fDutyCycle;

// 	PWM_TIMERCFG_Type pwm_timertype;
// 	PWM_MATCH_VALUE pwm_matchvalue;
// 	pwm_timertype.PrescaleOption = PWM_TIMER_PRESCALE_TICKVAL;
// 	pwm_timertype.PrescaleValue = 6;   //counter as 10MHz
// 	pwm_matchvalue.MR0_MatchValue=SumValue;
// 	pwm_matchvalue.MR1_MatchValue=DutyValue;    //0～100之间选择任意值(整数）
// 	LPC4078PWM::Init_MyPWM(PWM_HD1,PwmChIdx,pwm_timertype,pwm_matchvalue); //根据实际条件配置PWM参数（单边） dirui  20140723
// }
