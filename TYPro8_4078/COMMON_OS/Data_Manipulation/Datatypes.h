

#ifndef _DATA_TYPES_

#define _DATA_TYPES_


//宏函数的定义
#define MYNEGATIVE(a)       ((a) > (0) ? (-(a)) : (a))
#define MYABS(a)            ((a) < (0) ? (-(a)) : (a))
#define MYHIGHPRIORIT(a,b)  (MYMIN(a,b))

#define MYMIN(a,b)          ((a) <= (b) ? (a) : (b))
#define MYMAX(a,b)          ((a) >= (b) ? (a) : (b))
//#define MYSWAPB(Word)     ((unsigned short)((Word) << 8) | ((Word) >> 8))

#define MYBITTRUE(a,b)      ((a[b/8] >> (b%8)) & 0X01)
#define MYBITMOVEL(a)       (0X00000001<<a)
#define MYBITMOVER(a)       (0X80000000>>a)

#define MYIVUNITINDEX(a)    (a = ((++a) < (MAX_IVMCU_PER_SYS) ? (a) : (0)))

#define GetSystemTimeCarried() (OS_ServeProcess::OS_Time_Carried)
#define GetSystemTimeInSecond() (CommonFunc::TimeDiffInSecond(0))

#define IsFiniteNumber(value) (fpclassify(value) == FP_ZERO || fpclassify(value) == FP_NORMAL)
enum
{
    CHN_TYPE_A = 0xc1,
    CHN_TYPE_B
};
//Generic word types for ITU compatibility

typedef unsigned int	UInt16;
typedef unsigned long	UInt32;

typedef  char                      Char8;              //例子:  C_len
typedef  short		               Word16;          //例子:  W16_PIDPWR_LastRead
typedef  long			           Word32;         //例子:   l_Temp
typedef  long	long               Word64;      //例子:    w64_TimeOffset

typedef  unsigned char             UChar8;           //例子:  uc_len
typedef  unsigned short        	   UWord16;       //例子:  uw16_ISet
typedef  unsigned long	           UWord32;      //例子:   uw32_TimeOffset
typedef  unsigned long	long int   UWord64;      //例子:   uw64_TimeOffset
//typedef unsigned short BOOL;
//float  例子: f_Temp

typedef short			Word16;
typedef int			    Int16;
typedef long			Int32;
typedef long			Word32;

////////
typedef unsigned char		uint8;  /*  8 bits */
typedef unsigned short int	uint16; /* 16 bits */
typedef unsigned long int	uint32; /* 32 bits */
typedef unsigned long long  uint64;  /* 64 bits */ //12.01.2009 add

typedef signed char			int8;   /*  8 bits */
typedef signed short int	int16;  /* 16 bits */
typedef signed long int		int32;  /* 32 bits */
typedef signed long long    int64;  /* 64 bits */ //12.01.2009 add

//使用sizeof(long long);


#ifndef WIN32
//#ifndef bool
//typedef unsigned char bool;
//#endif

typedef unsigned char BYTE;
typedef unsigned long UINT ;
typedef unsigned short WORD;
//typedef unsigned short BOOL;
typedef unsigned long DWORD;
typedef unsigned char byte;
#endif // #ifndef WIN32


//bool and null
#ifndef FALSE
#define FALSE      (0)
#define false	   0
#endif

#ifndef TRUE
#define TRUE      (1)
#define true	  1
#endif

#ifndef NULL
#define NULL    ((void *)0)
#define null	   0
#endif

//max overrun
#define MAX_32 			(Word32)0x7fffffffL
#define MIN_32 			(Word32)0x80000000L

#define MAX_16 			(Word16)0x7fff
#define MIN_16 			(Word16)0x8000

#define GP_MOD    1
#define Address_MOD 2
#define CS38_MOD    3
////////data type defintion////////
//port struct
typedef struct
{
    unsigned char P1;
    unsigned char P2;        //p(P1).[P2]
    //example    P0[20]   P1 = 0    P2 = 20
} G_Port;

//device struct
typedef struct
{
    unsigned short Type;
    //GPIO
    G_Port GPIO_PIN;
    //Address
    unsigned long Address;
    unsigned long Value;
    //138
    G_Port st_CS38[3];				// save the ports of 138 inputs, A~C : [0]~[2]
    G_Port st_CS38EN;				// save the port of 138's Enable line, Low active.
    UInt16 CS_Value;				// value for CS_38 array. bit 0~2 is the value of port st_CS38[0]~[2]

} D_Port;

union UNL
{
    unsigned char uc_member[4];
    long ul_member;
};

union UNS
{
    unsigned char uc_member[2];
    unsigned int us_member;
};


union UW_F
{
    float f_member;
    unsigned long ul_member;
    unsigned char uc_member[4];
};

#ifndef __MSTIME__
#define __MSTIME__
#pragma pack(1)
typedef struct
{
    unsigned long      Second;
    unsigned short     Us100;
} MsTime;
#pragma pack()
#endif
class CommonFunc
{
public:
    static unsigned char            uc_Support_AUX;
    //static unsigned char			FRAM_uw8ReadBuf[1025];
//	static unsigned char 			uc_F2UC[4];

public:
    //static float GetFloat_RomReadBuf(UWord16 uw16_start);
// 	static UWord16 UC_2_Word(void);		// the UWord16 array is in uw_F2UW[0]~uw_F2UW[1]
// 	static UWord32 UC_2_DWord(void);		// the UWord32 array is in uw_F2UW[0]~uw_F2UW[3]
// 	static float UC_2_Float(void);		// the UWord16 array is in uw_F2UW[2]

    //       static void Float_2_UC(float f_Operand);
    //       static void Word_2_UC(unsigned long uw32_Operand);
    //	   static void DWord_2_UC(unsigned long uw32_Operand);

    static signed long   Unipolar2Bipolar24BitCode(unsigned long Value);
    static unsigned long Bipolar2Unipolar24BitCode(signed long sValue);

//     static unsigned char IsFiniteNumber(float x);
private:
public:
    static void 	SetSystemTimeCarried(void);
    static MsTime TimeDiff(MsTime myTime);
    static unsigned long TimeDiffInSecond(unsigned long myTime);
    static void GetTimeTicksUpdate(MsTime * myTimeP);
    static unsigned long long GetTimeTicks(void * myTimeP);
    static void SetSystemTime(MsTime SetSystemTime);
    static MsTime	GetSystemTime(void);
    static void add_MsTime(void* baseTime,void* addTime);
    static double  TimeCompare(MsTime StartTime, MsTime EndTime);
    static signed long long TimeDiffInMS(MsTime myTime);
		static unsigned char CRC8_Normal(unsigned char *message, unsigned short length);
		static MsTime TimeDiffAndUpdateMyTime(MsTime *myTime);
};



#endif
