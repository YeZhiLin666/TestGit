/*****************************************************************************
// �����ļ�      :wdt.h   LPC4078  WDTģ�鹦�ܳ���
// �ļ�����      :ΪLPC4078 IV-MPU board����
// ��д����      :2009.12.01
*****************************************************************************/

#ifndef __WDT_H
#define __WDT_H

#define WDEN		0x00000001
#define WDRESET		0x00000002
#define WDTOF		0x00000004
#define WDINT		0x00000008


//Hence the minimum Watchdog interval is (Twdclk * 256 * 4)
//and the maximum Watchdog interval is(Twdclk * 2^24 * 4) in multiples of (Twdclk * 4).
//Selectable time period from 1,024 watchdog clocks (Twdclk * 256 * 4) to over 67
//million watchdog clocks (Twdclk * 2^24 * 4) in increments of 4 watchdog clocks.

//This WDCLK oscillator has a typical frequency of 500 kHz.
//The Watchdog Timer has a dedicated oscillator that provides a 500 kHz clock to the
//Watchdog Timer that is always running if the Watchdog Timer is enabled.

//(Twdclk * 256 * 4)=(1/500000)*256*4=2us*256*4=2.048ms
//(Twdclk * 2^24 * 4)=(1/500000)*16777216*4=2us*16777216*4=134.217728s

//Writing values below 0xFF will cause 0x0000FF to be loaded intothe WDTC.
//#define WDT_FEED_VALUE	 0x0000FF  //   //��С 2.048ms
//#define WDT_FEED_VALUE	 0xFFFFFF  //   //��� 134.217728s
//#define WDT_FEED_VALUE	 0x01E65F  //   //��ͨ 1s =[X/255=1/0.002048  X=124511=0X01E65F]
//#define WDT_FEED_VALUE	     0x097FDB  //   //��ͨ 5s =[X/255=5/0.002048  X=622558=0x097FDE]
#define WDT_FEED_VALUE         0x0B71B0 //   //��ͨ 6s =750000   [X/255=6/0.002048  X=747070=0xB663E]

#define WDT_FEED_VALUE_67s         0x800000 //   //��ͨ 67s   [X/255=67/0.002048]

/*	unsigned char  Watchdog_Init( void );
void Watchdog_Feed( void );*/


class  WDTFunc
{
    //Attribute
public:
// 	static UWord32       uw32_wdt_counter;             //���Ź�������
// 	static UWord64       uw64_Watchdog;
// 	static unsigned char uc_Watchdog_Timer;
// 	static unsigned char uc_Watchdog_Valid;
// 	static unsigned char uc_WatchdogNo;

    //private:

    //protected:


    //Operation
public:
    static unsigned char  Watchdog_Init( void );
    static void Watchdog_Feed( void );
    static void IVCH_CheckWatchDog(void);
    static void IVCH_AnswerWatchdog(unsigned char uc_Live);  //���濴�Ź�ʱ�����

    static void IVCH_WatchdogCmdProcess(void);		// processing the watchdog command
    static void ResponseWatchdog(void);
    static void ResetMcu(void);                    //Ethernetparse call
// 	static void Watchdog_SetLongTime( unsigned long int uw32_Time );

    //private:

protected:
    //	void WDTHandler(void) __irq;

};

#endif /* end __WDT_H */


/*****************************************************************************
**                            End Of File
******************************************************************************/

