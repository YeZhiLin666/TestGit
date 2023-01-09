#ifndef _MSTAT21044_TempSample_H_
#define _MSTAT21044_TempSample_H_



class MSTAT21044_TempSample
{
public:
    static void  AD_Init(unsigned char ADCH_No);
    static float Temp_rd(unsigned char ADCH_Number);

};

#endif
