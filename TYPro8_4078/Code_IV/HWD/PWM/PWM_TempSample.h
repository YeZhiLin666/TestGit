#ifndef _PWM_TempSample_H_
#define _PWM_TempSample_H_



class PWM_TempSample
{

public:
    static void   AD_Init(unsigned char ADCH_No);
    static float  Temp_rd(unsigned char ADCH_Number);

};

#endif
