#ifndef _TESLA_TempSample_H_
#define _TESLA_TempSample_H_



class TESLA_TempSample
{

public:
	static void   AD_Init(unsigned char ADCH_No);
	static float  Temp_rd(unsigned char CH_Number);	          

};

#endif
