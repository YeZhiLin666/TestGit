#ifndef _LBT2401_TempSample_H_
#define _LBT2401_TempSample_H_



class LBT2401_TempSample
{

public:
	static void  AD_Init(unsigned char ADCH_No);
	static float Temp_rd(unsigned char ADCH_Number);	          

};

#endif
