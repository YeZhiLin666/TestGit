#ifndef __DDS_AD9834_H
#define __DDS_AD9834_H


class DDS_AD9834
{
public:

//    static  St_Port   Pin_SCLK;
//    static  St_Port   Pin_SDATA;
    static  St_Port   FSYNC;
    static void Init(void);
    static void WriteToAD9834Word(unsigned short Buf,unsigned char sspId);
    static void DDS_Output(void);
    static void SetFreq(float freq);  //04/10/2018

};

#endif
