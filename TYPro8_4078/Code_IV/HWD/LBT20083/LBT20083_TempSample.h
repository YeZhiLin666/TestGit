#ifndef _LBT20083_TempSample_H_
#define _LBT20083_TempSample_H_



class LBT20083_TempSample
{

public:
    static void   AD_Init(unsigned char ADCH_No);
    static float         Temp_rd(unsigned char ADCH_Number);

};

#endif
