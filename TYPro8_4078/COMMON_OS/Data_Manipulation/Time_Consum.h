#pragma once

//-------- <<< Use Configuration Wizard in Context Menu >>> -----------------
//
// <h>App Configuration
// =====================
//

#define TESTPIN_P2_8             0
#define TESTPIN_P0_31            1
#define TESTPIN_P2_4             2

//   <o>TEST PIN Selection   <0=> P2.8     <1=> P0.31     <2=> P2.4
//   <i> Default:    TESTPIN_P2_8
#define _TEST_PIN_SELECTION_              0

extern unsigned char m_TrigerTest;



#if(_TEST_PIN_SELECTION_ == TESTPIN_P2_8)
#define   INITTEST_PIN                         \
{                                    \
	LPC_IOCON->P2_8 |=  0x00;        \
	LPC_GPIO2->MASK |= (0<<0x08);     \
	LPC_GPIO2->DIR  |= (1<<0x08);     \
}
#define TEST_H  LPC_GPIO2->SET |=  (1<<0x8);
#define TEST_L  LPC_GPIO2->CLR |=  (1<<0x8);

#define TEST_H_IRQ                             \
	unsigned long IRQ1,IRQ2;               \
	IRQFunc::DisableIRQ(&IRQ1,&IRQ2);      \
	LPC_GPIO2->SET |=  (1<<0x8);
#define TEST_L_IRQ                             \
	LPC_GPIO2->CLR |=  (1<<0x8);           \
	IRQFunc::EnableIRQ(IRQ1,IRQ2);
#elif(_TEST_PIN_SELECTION_ == TESTPIN_P0_31)
#define   INITTEST_PIN                         \
{                                      \
	LPC_IOCON->P0_31 |=  0x00;         \
	LPC_GPIO0->MASK  |= (0<<31);       \
	LPC_GPIO0->DIR   |= (1<<31);       \
}
#define TEST_H  LPC_GPIO0->SET |=  (1<<31);
#define TEST_L  LPC_GPIO0->CLR |=  (1<<31);

#define TEST_H_IRQ                             \
	unsigned long IRQ1,IRQ2;              \
	IRQFunc::DisableIRQ(&IRQ1,&IRQ2);      \
	LPC_GPIO0->SET =  (1<<31);

#define TEST_L_IRQ                             \
	LPC_GPIO0->CLR =  (1<<31);            \
	IRQFunc::EnableIRQ(IRQ1,IRQ2);
#elif(_TEST_PIN_SELECTION_ == TESTPIN_P2_4)
#define   INITTEST_PIN                         \
{                                    \
	LPC_IOCON->P2_4  |=  0x00;       \
	LPC_GPIO2->MASK   |= (0<<0x04);   \
	LPC_GPIO2->DIR    |= (1<<0x04);   \
}
#define TEST_H  LPC_GPIO2->SET |=  (1<<0x4);
#define TEST_L  LPC_GPIO2->CLR |=  (1<<0x4);

#define TEST_H_IRQ                             \
	unsigned long IRQ1,IRQ2;               \
	IRQFunc::DisableIRQ(&IRQ1,&IRQ2);      \
	LPC_GPIO2->SET |=  (1<<0x4);
#define TEST_L_IRQ                             \
	LPC_GPIO2->CLR |=  (1<<0x4);           \
	IRQFunc::EnableIRQ(IRQ1,IRQ2);
#endif

enum
{
    TT_WHOLETASK = 0,
    TT_STEP,
    TT_SCHEDULE,
    TT_SCHPROSESS,
    TT_SCHMSG,
    TT_SAMPLE,
    TT_ETHERNET,
    TT_THIRDPARTY,
    TT_FRAM,
    TT_1CHSTEP,
    TT_1CHSCH,
    TT_1CHSAMP,
    TT_STEPPROCESS = 12,
    TT_STEPMSG,
    TT_STEPLIMIT,
    TT_STEPTRANS,
    TT_STEPDAC,
    TT_STEPTRAN,

    TT_STEPROUT_1 = 18,
    TT_STEPROUT_2,
    TT_STEPROUT_3,
    TT_STEPROUT_4,
    TT_STEPROUT_5,
    TT_STEPROUT_6,
    TT_STEPROUT_7,
    TT_STEPROUT_8,
    TT_STEPROUT_9,
    TT_STEPROUT_10,

    TT_SCHROUT_1 =28,
    TT_SCHROUT_2,
    TT_SCHROUT_3,
    TT_SCHROUT_4,
    TT_SCHROUT_5,
    TT_SCHROUT_6,
    TT_SCHROUT_7,
    TT_SCHROUT_8,
    TT_SCHROUT_9,
    TT_SCHROUT_10,

    TT_SAMPLEPROCESS = 38,
    TT_SAMPOUT_1,
    TT_SAMPOUT_2,
    TT_SAMPOUT_3,
    TT_SAMPOUT_4,
    TT_SAMPOUT_5,
    TT_SAMPOUT_6,
    TT_SAMPOUT_7,
    TT_SAMPOUT_8,
    TT_SAMPOUT_9,
    TT_SAMPOUT_10,
};

#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)


