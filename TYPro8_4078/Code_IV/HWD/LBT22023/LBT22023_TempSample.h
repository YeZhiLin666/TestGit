#ifndef _LBT22024_TempSample_H_
#define _LBT22024_TempSample_H_



class LBT22024_TempSample
{

public:
    static void     AD_Init(unsigned char ADCH_No);
    static float         Temp_rd(unsigned char ADCH_Number);

};

#endif
