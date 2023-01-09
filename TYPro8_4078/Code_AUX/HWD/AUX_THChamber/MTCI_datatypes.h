/*****************************************************************************
// 程序文件      :MTCI_DATATYPES.C  中间处理函数，变量数据类型等程序头文件
// 文件描述      :为LPC2378 AUX-MPU board服务
// 编写日期      :2008.02.29
*****************************************************************************/

#ifndef _MTCI_DATATYPES_H
#define _MTCI_DATATYPES_H


/*
#ifndef FALSE
#define FALSE   (0)
#define false	   0
#endif

#ifndef TRUE
#define TRUE    (1)
#define true	  1
#endif
// Generic word types for ITU compatibility
typedef  char                     Char8;              //例子:  C_len
typedef  unsigned char            UChar8;           //例子:  uc_len
typedef  short		               Word16;          //例子:  W16_PIDPWR_LastRead
typedef  unsigned short        	   UWord16;       //例子:  uw16_ISet
typedef  long			           Word32;         //例子:   l_Temp
typedef  unsigned long	           UWord32;      //例子:   uw32_TimeOffset
typedef  long	long               Word64;      //例子:    w64_TimeOffset
typedef  unsigned long	long int   UWord64;      //例子:   uw64_TimeOffset
//typedef unsigned short BOOL;
//float  例子: f_Temp

typedef  unsigned char	       bool;
typedef unsigned char		uint8;  // 8 bits  //
typedef unsigned short int	uint16; // 16 bits //
typedef unsigned long int	uint32; // 32 bits //

typedef signed char			int8;   //  8 bits //
typedef signed short int	int16;  // 16 bits //
typedef signed long int		int32;  // 32 bits //
typedef signed long long    int64;  // 64 bits // //12.01.2009 add

#define EPSINON  0.00001f


#define MAX_32 			(Word32)0x7fffffffL
#define MIN_32 			(Word32)0x80000000L

#define MAX_16 			(Word16)0x7fff
#define MIN_16 			(Word16)0x8000


union UW_F
{
float f_member;
unsigned long ul_member;
};*/

union  F_data {
    float     f_d;
    unsigned char uc_d[4];
};

union  Int_data {
    unsigned long ul_member;
    unsigned char uc_d[4];
};

/*
typedef struct
{
Word16 ProportionalGain;
Word16 ProportionalGainScale;
Word16 IntegralGain;
Word16 IntegralGainScale;
Word16 DerivativeGain;
Word16 DerivativeGainScale;
Word16 PositivePIDLimit;
Word16 NegativePIDLimit;
Word16 IntegralPortionK_1;
Word16 InputErrorK_1;
}mc_sPIDparams;

typedef struct
{
Word16 ProportionalGain;
Word16 ProportionalGainScale;
Word16 IntegralGain;
Word16 IntegralGainScale;
Word16 PositivePILimit;
Word16 NegativePILimit;
Word16 IntegralPortionK_1;
}mc_sPIparams;
*/

// Functions Declarartion

//Word16 MC1_controllerPItype1(Word16 DesiredValue,Word16 MeasuredValue,mc_sPIparams *pParams);
//void delay(UWord32 uw32_i);
//void Float_2_UC(float f_Operand);
//float UC_2_Float(void);		// the UWord16 array is in uw_F2UW[2]
//UWord32 UC_2_DWord(void);		// the UWord32 array is in uw_F2UW[0]~uw_F2UW[3]
//UWord16 UC_2_Word(void);		// the UWord16 array is in uw_F2UW[0]~uw_F2UW[1]

//unsigned char HowLongToNow(UWord16 uw16_TSNo, UWord32 uw32_TS);
//float myabs(float f_in);
//unsigned char HowLongToNow_Pausing(UWord16  uw16_SCH_ExN, UWord16 uw16_TSNo, UWord32 uw32_TS);

class MTCI_datatypes
{
public:

    static void ctrl_Value2String_Hex(short int * pValue, unsigned char* string);
	static unsigned int ctrl_Value2String_Dot1(float Value,unsigned char* string);
    static unsigned int ctrl_Value2String_Dot(short int* pValue,unsigned char* string);
    static void ctrl_Value2String_BeforeZero(short int* pValue, unsigned char* string);

    static void ctrl_String2Value_Hex(unsigned char* string, short int* pValue);
    static unsigned char* ctrl_String2Value_Signed(unsigned char* string, short int* pValue);

    static unsigned short int CRC16(unsigned char *puchMsg, unsigned int usDataLen);
    //unsigned short int CRC16_XOR(unsigned char *puchMsg, unsigned int usDataLen);
    //unsigned short int CRC16_ToView(unsigned char *puchMsg, unsigned int usDataLen);
    static unsigned short int CRC16_Counter(unsigned char *puchMsg, unsigned int usDataLen);

    static unsigned int CheckSum(unsigned char *puchMsg, unsigned int usDataLen);
    static unsigned char CheckXorSum(unsigned char *puchMsg, unsigned int usDataLen);

    static short int Convert_FloatToInt(unsigned char* Source);
    static void Convert_IntToFloat(unsigned char* Source,short int w16_Value);

    static unsigned char  CalcCheckSum_Byte(unsigned char *puchMsg, unsigned int usDataLen);
    static bool ctrl_strncmp(unsigned char* Src, unsigned char* Dest, unsigned int Len);
    static unsigned char Convert_ASCIIToHEX(unsigned char uc_ASCII);
};

#endif

