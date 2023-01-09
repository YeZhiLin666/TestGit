#include "includes.h"

unsigned char   CommonFunc::uc_Support_AUX=0;
//unsigned char 	CommonFunc::uc_F2UC[4];

/******************************************************************************
** Function name:        Float_2_UC
**
** Descriptions:         �������ֲ���ֽ�
**
** parameters:           f_Operand������
**
** Returned value:       None
**
** Created Date:
**
** Revision Dates:

******************************************************************************/
//void CommonFunc::Float_2_UC(float f_Operand)
//{
//// �������ֲ���ֽ�
//	union UW_F un_Temp;

//	un_Temp.f_member = f_Operand;
//	/*uc_F2UC[0] = un_Temp.ul_member & 0x00FF;		// give high 8 bits to [0] second 8 to [1], third 8 to [2] low 8 bits to [3]
//	uc_F2UC[1] = (un_Temp.ul_member >> 8) & 0x00FF;
//	uc_F2UC[2] = (un_Temp.ul_member >> 16) & 0x00FF;
//	uc_F2UC[3] = (un_Temp.ul_member >> 24) & 0x00FF;*/
//
//	uc_F2UC[0] = un_Temp.uc_member[0];
//	uc_F2UC[1] = un_Temp.uc_member[1];
//	uc_F2UC[2] = un_Temp.uc_member[2];
//	uc_F2UC[3] = un_Temp.uc_member[3];
//}


/******************************************************************************
** Function name:        Word_2_UC
**
** Descriptions:         �ֲַ���ֽ�
**
** parameters:           uw32_Operand������
**
** Returned value:       None
**
** Created Date:
**
** Revision Dates:

******************************************************************************/
//void CommonFunc::Word_2_UC(unsigned long uw32_Operand)
//{
//// �������ֲ���ֽ�
//	union UW_F un_Temp;

//	un_Temp.ul_member = uw32_Operand;
//	uc_F2UC[0] = un_Temp.ul_member & 0x0000FFFF;		// give high 8 bits to [0] second 8 to [1], third 8 to [2] low 8 bits to [3]
//	uc_F2UC[0] = un_Temp.uc_member[0];
//	uc_F2UC[1] = un_Temp.uc_member[1];

//}


/******************************************************************************
** Function name:        DWord_2_UC
**
** Descriptions:         ˫�ֲַ���ֽ�
**
** parameters:           uw32_Operand������
**
** Returned value:       None
**
** Created Date:
**
** Revision Dates:

******************************************************************************/
//void CommonFunc::DWord_2_UC(unsigned long uw32_Operand)
//{
//// �������ֲ���ֽ�
//	union UW_F un_Temp;

//	un_Temp.ul_member = uw32_Operand;
//	uc_F2UC[0] = un_Temp.uc_member[0];   // give high 8 bits to [0] second 8 to [1], third 8 to [2] low 8 bits to [3]
//	uc_F2UC[1] = un_Temp.uc_member[1];
//	uc_F2UC[2] = un_Temp.uc_member[2];
//	uc_F2UC[3] = un_Temp.uc_member[3];
//}

/******************************************************************************
** Function name:        UC_2_Word
**
** Descriptions:         ����ת����WORD
**
** parameters:           None
**
** Returned value:       un_Temp.ul_member
**
** Created Date:
**
** Revision Dates:

******************************************************************************/
// UWord16 CommonFunc::UC_2_Word(void)		// the UWord16 array is in uw_F2UW[0]~uw_F2UW[1]
// {
// 	union UW_F un_Temp;
// 	un_Temp.ul_member = uc_F2UC[1];
// 	un_Temp.ul_member = un_Temp.ul_member << 8;
// 	un_Temp.ul_member |= uc_F2UC[0];
// 	return un_Temp.ul_member;
// }

/******************************************************************************
** Function name:        UC_2_DWord
**
** Descriptions:         ����ת����˫WORD
**
** parameters:           None
**
** Returned value:       un_Temp.ul_member
**
** Created Date:
**
** Revision Dates:

******************************************************************************/
// UWord32 CommonFunc::UC_2_DWord(void)		// the UWord32 array is in uw_F2UW[0]~uw_F2UW[3]
// {
// 	union UW_F un_Temp;
// 	un_Temp.ul_member = uc_F2UC[3];
// 	un_Temp.ul_member = un_Temp.ul_member << 8;
// 	un_Temp.ul_member |= uc_F2UC[2];
// 	un_Temp.ul_member = un_Temp.ul_member << 8;
// 	un_Temp.ul_member |= uc_F2UC[1];
// 	un_Temp.ul_member = un_Temp.ul_member << 8;
// 	un_Temp.ul_member |= uc_F2UC[0];
// 	return un_Temp.ul_member;
// }

/******************************************************************************
** Function name:        UC_2_DWord
**
** Descriptions:         ����ת���ɸ�������
**
** parameters:           None
**
** Returned value:       un_Temp.f_member
**
** Created Date:
**
** Revision Dates:

******************************************************************************/
// float CommonFunc::UC_2_Float(void)		// the UWord16 array is in uw_F2UW[2]
// {
// 	//  �ֽ���ϳɸ�����
// 	union UW_F un_Temp;
// 	un_Temp.ul_member = uc_F2UC[3];
// 	un_Temp.ul_member = un_Temp.ul_member << 8;
// 	un_Temp.ul_member |= uc_F2UC[2];
// 	un_Temp.ul_member = un_Temp.ul_member << 8;
// 	un_Temp.ul_member |= uc_F2UC[1];
// 	un_Temp.ul_member = un_Temp.ul_member << 8;
// 	un_Temp.ul_member |= uc_F2UC[0];
// 	return un_Temp.f_member;
// }

signed long CommonFunc::Unipolar2Bipolar24BitCode(unsigned long Value)
{
    signed long sValue = (signed long)Value;
    sValue -= 0x800000;

    return sValue;
}

unsigned long CommonFunc::Bipolar2Unipolar24BitCode(signed long sValue)
{
    sValue = sValue << 8;
    sValue = sValue >> 8;
    unsigned long Value = (unsigned long)(sValue + 0x800000);

    return Value;
}

// unsigned char CommonFunc::IsFiniteNumber(float x)
// {
//     unsigned char uc_IsNum = 0;
//     (x <= FLT_MAX && x >= -FLT_MAX)? uc_IsNum|=0X01:uc_IsNum=0;

//     x == x? uc_IsNum|=0X02:uc_IsNum=0;
//     if(uc_IsNum<3)
//         uc_IsNum = 0;
//     return uc_IsNum;
// }
void CommonFunc::SetSystemTimeCarried(void)
{
    OS_ServeProcess::OS_Time.Us100 = 0;			// Reset System Time Tick section.
    OS_ServeProcess::OS_Time.Second ++;
    OS_ServeProcess::OS_Time_Carried = !OS_ServeProcess::OS_Time_Carried;
}
void CommonFunc::SetSystemTime(MsTime SetSystemTime)
{
    bool bCarried = GetSystemTimeCarried();
    OS_ServeProcess::OS_Time = SetSystemTime;
    if(bCarried != GetSystemTimeCarried())//recompare is nesessary, because interrupt may change OS_Time.
        OS_ServeProcess::OS_Time = SetSystemTime;
}
MsTime CommonFunc::GetSystemTime(void)
{
    bool bCarried = GetSystemTimeCarried();
    MsTime SystemTime = OS_ServeProcess::OS_Time;
    if(bCarried != GetSystemTimeCarried())//recompare is nesessary, because interrupt may change OS_Time.
        SystemTime = OS_ServeProcess::OS_Time;
    return SystemTime;
}
MsTime CommonFunc::TimeDiff(MsTime myTime)
{
    Int16 temp;
    MsTime SystemTime = GetSystemTime();
    temp = SystemTime.Us100 - myTime.Us100;
    MsTime timediff = {(SystemTime.Second - myTime.Second), 0};
    if(temp < 0)
    {
        temp = temp + 10000;
        timediff.Second --;
    }
    timediff.Us100 = temp;
    return timediff;
}
unsigned long CommonFunc::TimeDiffInSecond(unsigned long myTime)
{
    MsTime SystemTime = GetSystemTime();
		if(myTime > SystemTime.Second)
		{
			return 0;
		}	
    return (SystemTime.Second - myTime);
}
void CommonFunc::GetTimeTicksUpdate(MsTime * myTimeP)//ɾ
{
    MsTime SystemTime = GetSystemTime();
    *myTimeP = SystemTime;
}
unsigned long long CommonFunc::GetTimeTicks(void * myTimeP)
{
    MsTime timediff;
    Int16 temp;
    unsigned long long TimeTicks;  //��100usΪ��λ��ʱ�䵥λ��
    MsTime * myTime = (MsTime *) myTimeP;

    MsTime OStime = GetSystemTime();
    if(myTime->Second > OStime.Second)
    {
        memcpy(myTimeP, &OStime, sizeof(MsTime));
        return 0;
    }
    else if((myTime->Second == OStime.Second)&&(myTime->Us100 >= OStime.Us100))
    {
        memcpy(myTimeP, &OStime, sizeof(MsTime));
        return 0;   //add by hepeiqing,20150818,in case of a wrong time calculation.
    }

    temp = OStime.Us100 - myTime->Us100;
    timediff.Second = OStime.Second - myTime->Second;
    if(temp < 0)
    {
        temp = temp + 10000;
        timediff.Second --;
    }
    timediff.Us100 = temp;

    TimeTicks  = timediff.Second;
    TimeTicks *= 10000;
    TimeTicks += timediff.Us100;

    return TimeTicks;
}

void CommonFunc::add_MsTime(void* baseTime,void* addTime)
{
    MsTime *tmpBasTime = (MsTime *)baseTime;
    MsTime *tmpAddTime = (MsTime *)addTime;
    if((tmpBasTime->Us100+tmpAddTime->Us100)>9999)
    {
        tmpBasTime->Second = tmpBasTime->Second + tmpAddTime->Second+1;
        tmpBasTime->Us100 = (tmpBasTime->Us100+tmpAddTime->Us100)-10000;
    }
    else
    {
        tmpBasTime->Second = tmpBasTime->Second + tmpAddTime->Second;
        tmpBasTime->Us100 = tmpBasTime->Us100+tmpAddTime->Us100;
    }
}
double  CommonFunc::TimeCompare(MsTime StartTime, MsTime EndTime)
{
    MsTime timediff;
    Int16 temp;
    double TimeInSecond;

    if(StartTime.Second > EndTime.Second)
        return 0;
    else if((StartTime.Second == EndTime.Second)&&(StartTime.Us100 >= EndTime.Us100))
        return 0;

    temp = EndTime.Us100 - StartTime.Us100;
    timediff.Second = EndTime.Second - StartTime.Second;
    if(temp < 0)
    {
        temp = temp + 10000;
        timediff.Second --;
    }
    timediff.Us100 = temp;
    // ���Ƕ��ڸ���������ֵ  NaN ��˵�ܻ�õ� false!
    TimeInSecond  = timediff.Us100;
    TimeInSecond /= 10000;
    TimeInSecond += timediff.Second;


    return TimeInSecond;
}

MsTime CommonFunc::TimeDiffAndUpdateMyTime(MsTime *myTime)
{
    Int16 temp;
    MsTime SystemTime = GetSystemTime();
    temp = SystemTime.Us100 - myTime->Us100;
    MsTime timediff = {(SystemTime.Second - myTime->Second), 0};
    if(temp < 0)
    {
        temp = temp + 10000;
        timediff.Second --;
    }
    timediff.Us100 = temp;
    *myTime = SystemTime;
    return timediff;
}

signed long long CommonFunc::TimeDiffInMS(MsTime myTime)
{
    MsTime SystemTime = GetSystemTime();
    signed long temp = SystemTime.Us100;
    temp = temp - myTime.Us100; 
	temp /= 10;
    signed long long tempMS = SystemTime.Second; 
    tempMS = tempMS - myTime.Second;     
    tempMS *= 1000; //convert second to mili second;
    tempMS += temp;

    return tempMS;
}


//CRCУ�����ͣ�CRC8   x8+x2+x+1
//����ģ�� NAME��CRC-8/ITU   x8+x2+x+1
//��� WIDTH��8
//����ʽ POLY��Hex����07
//��ʼֵ INIT��Hex����00
//������ֵ XOROUT��Hex����00
//���������ݷ�ת��REFIN��       ��������ݷ�ת��REFOUT��
unsigned char CommonFunc::CRC8_Normal(unsigned char *message, unsigned short length)
{
    uint8 crc = 0;
    uint8 i = 0;
    unsigned short len = length;
    if(len <= 0)
        return 0;
    while(len--)
    {
        crc ^= *message++;
        // ������������8λ����Ҫ����8�� //
        for (i=8; i>0; --i)
        {   // ��λ�ȴ��ķ�ʽ��XOR�����ݵĸ�λ��ʼ�����Ǿͽ���˳�����
            if (crc & 0x80)  // �ж����λ�Ƿ�Ϊ1 //
            {
                // ���λΪ1������Ҫ���������һλ��Ȼ����0x31��� //
                // 0x07(����ʽ��x8+x2+x+1��100000111)�����λ����Ҫ���ֱ��ȥ�� //
                crc = (crc << 1) ^ 0x07;
            }
            else
            {
                // ���λΪ0ʱ������Ҫ�����������������һλ //
                crc = (crc << 1);
            }
        }
    }
    return crc;
}


/*********************************************************************************
**                            End Of File
*********************************************************************************/
