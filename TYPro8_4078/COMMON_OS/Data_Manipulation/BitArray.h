/*****************************************************************************
// �����ļ�      :BitArray.h     ��Ҫ���ڸ����Զ���Bit���鶨�塢ά����
// �ļ�����      :ΪMCU board����
// ��д����      :2013.1.16
*****************************************************************************/

#pragma  once

static  unsigned long long u64Variable1,u64Variable2,SysFileTime;

#define SetBit_32Bit(m_U32BitArray,BitIdx)         m_U32BitArray |= 0x01<<BitIdx;
#define ClrBit_32Bit(m_U32BitArray,BitIdx)         m_U32BitArray &= ~(0x01<<BitIdx);
#define CheckBit_32Bit(m_U32BitArray,BitIdx)       (m_U32BitArray & (0x01<<BitIdx))

#define MSTIME2FILETIME(InSecond,m100Us,FileTime)                             \
{                                                                             \
	u64Variable1 = InSecond;                                                  \
	u64Variable1 *= 10000000;                                                 \
	u64Variable2 = m100Us;                                                    \
	u64Variable2 *= 1000;                                                     \
	FileTime = u64Variable1;                                                  \
	FileTime += u64Variable2;                                                 \
}
#define FILETIME2MSTIME(InSecond,m100Us,FileTime)                             \
{                                                                             \
	InSecond = FileTime/10000000;                                             \
	u64Variable1= (FileTime%10000000);                                        \
	m100Us   = u64Variable1/1000;                                             \
}

