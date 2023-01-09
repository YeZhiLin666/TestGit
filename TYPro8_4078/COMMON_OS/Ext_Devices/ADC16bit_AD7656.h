#pragma once

class Adc16bit_AD7656
{
private:
    static unsigned char  CodeType;

public:
    static St_Port        Pin_ConverA;
    static St_Port        Pin_ConverB;
    static St_Port        Pin_ConverC;
    static St_Port        Pin_Busy;
    static St_Port        Pin_Rst;
    static St_Port        Pin_CS;

    static unsigned long  Address;

    static void Init(unsigned char ConvCodeType);

    static void ReadA(unsigned long *Data1,unsigned long *Data2);
  	static void ReadB(unsigned long *Data1,unsigned long *Data2);
    static void ReadC(unsigned long *Data1,unsigned long *Data2);

// 	static void SetAllConvert(void);
// 	static void ClrAllConvert(void);
// 	static void ConverA(void);
// 	static void ConverB(void);
// 	static void ConverC(void);
// 	static void ReadData(unsigned long *Data1,unsigned long *Data2);
    static void Reset(void);
};

