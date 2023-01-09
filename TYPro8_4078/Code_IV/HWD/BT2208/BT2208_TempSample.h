#ifndef _BT2208_TempSample_H_
#define _BT2208_TempSample_H_



class BT2208_TempSample
{

public:
	static void AD_Init(unsigned char ADCH_No);
	static float         Temp_rd(unsigned char ADCH_Number);	          

};

#endif
