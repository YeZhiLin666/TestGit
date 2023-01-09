#ifndef _LBT2108_TempSample_H_
#define _LBT2108_TempSample_H_

class LBT2108_TempSample
{
public:
    static void  AD_Init(unsigned char ADCH_No);
    static float Temp_rd(unsigned char ADCH_Number);

};

#endif
