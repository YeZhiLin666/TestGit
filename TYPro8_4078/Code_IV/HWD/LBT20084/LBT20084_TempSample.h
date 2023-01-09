#ifndef _LBT20084_TempSample_H_
#define _LBT20084_TempSample_H_

class LBT20084_TempSample
{
public:
    static void  AD_Init(unsigned char ADCH_No);
    static float Temp_rd(unsigned char ADCH_Number);

};

#endif
