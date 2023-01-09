#ifndef _TMP75_TEC_H_
#define _TMP75_TEC_H_

//#define H_Temp 0x2800  //40 degree
//#define L_Temp 0x2600  //38 degree

class TMP75C_TEC
{
public:
    static void Set_HighTemp(long Temp, unsigned int TECChannelNo); //Change by ZC 04.04.2018
    static void Set_LowTemp(long Temp, unsigned int TECChannelNo); //Change by ZC 04.04.2018
    static void Init_TMP75C(unsigned int TECChannelNo); //Change by ZC 04.04.2018
    static float Get_Temp(void);
    static float Get_MTC_Temp(unsigned int TECChannelNo);
    static float H_Temp;
    static float L_Temp;

//    static void Read_HighTemp(void)
//    static void Read_LowTemp(void);
//	private:  //comment out by ZC 04.04.2018
    static float f_Read_Temp;
    static unsigned short Read_16;
    static float Local[8];
};

#endif
