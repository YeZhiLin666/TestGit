/*****************************************************************************
// 程序文件      :PWM.h     内部PWM输出
// 文件描述      :为MCU board服务
// 编写日期      :2014.03.12  模块没有测试
*****************************************************************************/

#ifndef _PWM_H_
#define _PWM_H_

#define MAXINTERNALPWM 8    //以后要放到大数据体

/** Timer/Counter 0 power/clock control bit */
#define	CLKPWR_PCONP_PCTIM0		((unsigned long)(1<<1))
/* Timer/Counter 1 power/clock control bit */
#define	CLKPWR_PCONP_PCTIM1		((unsigned long)(1<<2))
/* --------------------- BIT DEFINITIONS -------------------------------------- */
/**********************************************************************
* IR register definitions
**********************************************************************/
/** Interrupt flag for PWM match channel for 6 channel */
#define PWM_IR_PWMMRn(n)    	((unsigned long)((n<4)?(1<<n):(1<<(n+4))))
/** Interrupt flag for capture input */
#define PWM_IR_PWMCAPn(n)		((unsigned long)(1<<(n+4)))
/**  IR register mask */
#define PWM_IR_BITMASK			((unsigned long)(0x0000073F))

/**********************************************************************
* TCR register definitions
**********************************************************************/
/** TCR register mask */
#define PWM_TCR_BITMASK				((unsigned long)(0x0000000B))
#define PWM_TCR_COUNTER_ENABLE      ((unsigned long)(1<<0)) /*!< PWM Counter Enable */
#define PWM_TCR_COUNTER_RESET       ((unsigned long)(1<<1)) /*!< PWM Counter Reset */
#define PWM_TCR_PWM_ENABLE          ((unsigned long)(1<<3)) /*!< PWM Enable */

/**********************************************************************
* CTCR register definitions
**********************************************************************/
/** CTCR register mask */
#define PWM_CTCR_BITMASK			((unsigned long)(0x0000000F))
/** PWM Counter-Timer Mode */
#define PWM_CTCR_MODE(n)        	((unsigned long)(n&0x03))
/** PWM Capture input select */
#define PWM_CTCR_SELECT_INPUT(n)	((unsigned long)((n&0x03)<<2))

/**********************************************************************
* MCR register definitions
**********************************************************************/
/** MCR register mask */
#define PWM_MCR_BITMASK				((unsigned long)(0x001FFFFF))
/** generate a PWM interrupt when a MATCHn occurs */
#define PWM_MCR_INT_ON_MATCH(n)     ((unsigned long)(1<<(((n&0x7)<<1)+(n&0x07))))
/** reset the PWM when a MATCHn occurs */
#define PWM_MCR_RESET_ON_MATCH(n)   ((unsigned long)(1<<(((n&0x7)<<1)+(n&0x07)+1)))
/** stop the PWM when a MATCHn occurs */
#define PWM_MCR_STOP_ON_MATCH(n)    ((unsigned long)(1<<(((n&0x7)<<1)+(n&0x07)+2)))

/**********************************************************************
* CCR register definitions
**********************************************************************/
/** CCR register mask */
#define PWM_CCR_BITMASK				((unsigned long)(0x0000003F))
/** PCAPn is rising edge sensitive */
#define PWM_CCR_CAP_RISING(n) 	 	((unsigned long)(1<<(((n&0x2)<<1)+(n&0x1))))
/** PCAPn is falling edge sensitive */
#define PWM_CCR_CAP_FALLING(n) 		((unsigned long)(1<<(((n&0x2)<<1)+(n&0x1)+1)))
/** PWM interrupt is generated on a PCAP event */
#define PWM_CCR_INT_ON_CAP(n)  		((unsigned long)(1<<(((n&0x2)<<1)+(n&0x1)+2)))

/**********************************************************************
* PCR register definitions
**********************************************************************/
/** PCR register mask */
#define PWM_PCR_BITMASK			(unsigned long)0x00007E7C
/** PWM output n is a single edge controlled output */
#define PWM_PCR_PWMSELn(n)   	((unsigned long)(((n&0x7)<2) ? 0 : (1<<n)))
/** enable PWM output n */
#define PWM_PCR_PWMENAn(n)   	((unsigned long)(((n&0x7)<1) ? 0 : (1<<(n+8))))

/**********************************************************************
* LER register definitions
**********************************************************************/
/** LER register mask*/
#define PWM_LER_BITMASK				((unsigned long)(0x0000007F))
/** PWM MATCHn register update control */
#define PWM_LER_EN_MATCHn_LATCH(n)   ((unsigned long)((n<7) ? (1<<n) : 0))

// #define  PWM_PRESCALEVALUE  600 //6000,频率由100HZ提高到1000HZ。 

typedef enum {
    PWM_CHANNEL_SINGLE_EDGE,	/*!< PWM Channel Single edge mode */
    PWM_CHANNEL_DUAL_EDGE		/*!< PWM Channel Dual edge mode */
} PWM_CHANNEL_EDGE_OPT;
typedef enum {
    PWM_COUNTER_RISING = 1,		/*!< Rising edge mode */
    PWM_COUNTER_FALLING = 2,	/*!< Falling edge mode */
    PWM_COUNTER_ANY = 3			/*!< Both rising and falling mode */
} PWM_COUNTER_EDGE_OPT;

typedef enum {
    PWM_MATCH_UPDATE_NOW = 0,			/**< PWM Match Channel Update Now */
    PWM_MATCH_UPDATE_NEXT_RST			/**< PWM Match Channel Update on next
										PWM Counter resetting */
} PWM_MATCH_UPDATE_OPT;

typedef enum
{
    RESET = 0,
    SET = !RESET
} FlagStatus, IntStatus, SetState;
typedef enum {
    PWM_COUNTER_PCAP1_0 = 0,		/*!< PWM Counter input selected is PCAP1.0 pin */
    PWM_COUNTER_PCAP1_1			/*!< PWM counter input selected is CAP1.1 pin */
} PWM_COUNTER_INPUTSEL_OPT;

typedef enum
{
    DISABLE = 0,
    ENABLE = !DISABLE
} FunctionalState;
typedef enum
{
    PWM_HD0 = 0,
    PWM_HD1 = 1
} en_PWM_unitId;
typedef enum
{
    PWM_CH1 = 1,
    PWM_CH2 = 2,
    PWM_CH3 = 3,
    PWM_CH4 = 4,
    PWM_CH5 = 5,
    PWM_CH6 = 6,
} en_PWM_CHANNEL;

typedef enum
{
    PWM_TIMER_PRESCALE_TICKVAL = 0,			/*!< Prescale in absolute value */
    PWM_TIMER_PRESCALE_USVAL				/*!< Prescale in microsecond value */
} PWM_TIMER_PRESCALE_OPT;

typedef struct {

    unsigned char PrescaleOption;		/**< Prescale option, should be:
										- PWM_TIMER_PRESCALE_TICKVAL: Prescale in absolute value
										- PWM_TIMER_PRESCALE_USVAL: Prescale in microsecond value
										*/
    unsigned char Reserved[3];
    unsigned long PrescaleValue;		/**< Prescale value, 32-bit long, should be matched
										with PrescaleOption
										*/
} PWM_TIMERCFG_Type;
typedef struct {
    unsigned long MR0_MatchValue;
    unsigned long MR1_MatchValue;
}	PWM_MATCH_VALUE;
typedef struct {

    unsigned char CounterOption;		/**< Counter Option, should be:
										- PWM_COUNTER_RISING: Rising Edge
										- PWM_COUNTER_FALLING: Falling Edge
										- PWM_COUNTER_ANY: Both rising and falling mode
										*/
    unsigned char CountInputSelect;	/**< Counter input select, should be:
									- PWM_COUNTER_PCAP1_0: PWM Counter input selected is PCAP1.0 pin
									- PWM_COUNTER_PCAP1_1: PWM Counter input selected is PCAP1.1 pin
									*/
    unsigned char Reserved[2];
} PWM_COUNTERCFG_Type;
typedef struct {
    unsigned char MatchChannel;	/**< Match channel, should be in range
								from 0..6 */
    unsigned char IntOnMatch;		/**< Interrupt On match, should be:
									- ENABLE: Enable this function.
									- DISABLE: Disable this function.
									*/
    unsigned char  StopOnMatch;	/**< Stop On match, should be:
								- ENABLE: Enable this function.
								- DISABLE: Disable this function.
								*/
    unsigned char ResetOnMatch;	/**< Reset On match, should be:
								- ENABLE: Enable this function.
								- DISABLE: Disable this function.
								*/
} PWM_MATCHCFG_Type;
typedef enum {
    PWM_MODE_TIMER = 0,		/*!< PWM using Timer mode */
    PWM_MODE_COUNTER,		/*!< PWM using Counter mode */
} PWM_TC_MODE_OPT;

typedef struct {
    unsigned char CaptureChannel;	/**< Capture channel, should be in range
									from 0..1 */
    unsigned char RisingEdge;		/**< caption rising edge, should be:
									- ENABLE: Enable rising edge.
									- DISABLE: Disable this function.
									*/
    unsigned char FallingEdge;		/**< caption falling edge, should be:
									- ENABLE: Enable falling edge.
									- DISABLE: Disable this function.
									*/
    unsigned char IntOnCaption;	/**< Interrupt On caption, should be:
								- ENABLE: Enable interrupt function.
								- DISABLE: Disable this function.
								*/
} PWM_CAPTURECFG_Type;



class LPC4078PWM
{
public:	  
    static void PWM0_IRQHandler(void);
    static void PWM1_IRQHandler(void);
    static void Init_MyPWM(unsigned char pwmId, unsigned char PWM_Channel, PWM_TIMERCFG_Type pwm_timertype,PWM_MATCH_VALUE pwm_matchvalue);
// 	static void ChangePWMPeriod(unsigned char pwmId, unsigned char PWM_Channel,unsigned long MR0_MatchValue,unsigned long MR1_MatchValue);
    static void SetPWM(unsigned char PWM_HDX,unsigned char PWM_Channel,unsigned long MR0_MatchValue,unsigned long MR1_MatchValue,unsigned short Frequency);
// 	static void SetPWM2(unsigned char PwmChIdx, unsigned long Frequency, float fDutyCycle);

    static void PWM_MatchUpdate(unsigned char pwmId, unsigned char MatchChannel, unsigned long MatchValue, unsigned char UpdateType);
    static LPC_PWM_TypeDef* PWM_GetPointer (unsigned char pwmId);
    static void CLKPWR_ConfigPPWR  (unsigned char pwmID, unsigned char NewState);
    static void PWM_ChannelConfig(unsigned char pwmId, unsigned char PWMChannel, unsigned char ModeOption);
    static void PWM_ChannelCmd(unsigned char pwmId, unsigned char PWMChannel, unsigned char NewState);
    static void PWM_CounterCmd(unsigned char pwmId, unsigned char NewState);
    static void PWM_ResetCounter(unsigned char pwmId);
    static void PWM_Cmd(unsigned char pwmId, unsigned char NewState);

    static void PWM_ConfigMatch(unsigned char pwmId, PWM_MATCHCFG_Type *PWM_MatchConfigStruct);//Moved from private to public by ZC 09/29/2016

private:
    //static void PWM_PinConfig(unsigned char pwmId, unsigned char PWM_Channel, unsigned char PinselOption);
    //static unsigned int PWM_GetIntStatus(unsigned char pwmId, unsigned long IntFlag);
    //static void PWM_ClearIntPending(unsigned char pwmId, unsigned long IntFlag);
    //static void PWM_ConfigStructInit(unsigned char PWMTimerCounterMode, void *PWM_InitStruct);
    static void PWM_Init(unsigned char pwmId, unsigned long PWMTimerCounterMode, void *PWM_ConfigStruct);
    //static void PWM_DeInit (unsigned char pwmId);
    //static void PWM_ConfigMatch(unsigned char pwmId, PWM_MATCHCFG_Type *PWM_MatchConfigStruct);
    //static void PWM_ConfigCapture(unsigned char pwmId, PWM_CAPTURECFG_Type *PWM_CaptureConfigStruct);
    //static unsigned long PWM_GetCaptureValue(unsigned char pwmId, unsigned char CaptureChannel);

};

#endif
