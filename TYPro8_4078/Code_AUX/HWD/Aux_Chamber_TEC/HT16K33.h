#ifndef _HT16K33_H_
#define _HT16K33_H_


#define I2CMAX_TIMEOUT	       0x000FFFFF

/*****************Temperature LED Display using HT16K33***************************
**       Added by C.Zhong 03_14_2016
**********************************************************************************/


class HT16K33
{
public:
    static unsigned long  I2Ctimeout;
    static void Init_HT16K33(void);
    static void LED_BLINK(void);
    static void LED_NBLINK(void);
    static void DispTemp(float temp);
    static unsigned char I2CEngine(unsigned char I2Cx);
    static void I2C_IRQProcess(unsigned char I2Cx);
    static unsigned char Digit[3];

private:
    static unsigned char Num[16];
    static bool ReturnVal;
    static void Init_NumTab(void);
};

#endif
