#ifndef _IBT_TempSample_H_
#define _IBT_TempSample_H_



class IBT_TempSample
{

public:
	static void   AD_Init(unsigned char ADCH_No);
	static float  Temp_rd(unsigned char ADCH_Number);	          

};

#endif
